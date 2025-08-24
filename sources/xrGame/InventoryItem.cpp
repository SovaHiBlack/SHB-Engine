////////////////////////////////////////////////////////////////////////////
//	Module 		: InventoryItem.cpp
//	Description : Inventory item
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InventoryItem_impl.h"
#include "Inventory.h"
#include "Physics.h"
#include "xrserver_objects_alife.h"
#include "xrserver_objects_alife_items.h"
#include "Level.h"
#include "game_cl_base.h"
#include "Actor.h"
#include "StringTable.h"
#include "..\XR_3DA\skeletoncustom.h"
#include "ai_object_location.h"
#include "object_broker.h"
#include "..\XR_3DA\IGamePersistent.h"

#ifdef DEBUG
#	include "DebugRenderer.h"
#endif

#define ITEM_REMOVE_TIME		30000
struct net_update_IItem
{
	u32					dwTimeStamp;
	SPHNetState			State;
};
struct net_updateData
{
	xr_deque<net_update_IItem>	NET_IItem;
	/// spline coeff /////////////////////
	f32			SCoeff[3][4];

#ifdef DEBUG
	DEF_VECTOR(VIS_POSITION, fVector3);
	VIS_POSITION	LastVisPos;
#endif

	fVector3			IStartPos;
	fQuaternion		IStartRot;

	fVector3			IRecPos;
	fQuaternion		IRecRot;

	fVector3			IEndPos;
	fQuaternion		IEndRot;

	SPHNetState		LastState;
	SPHNetState		RecalculatedState;

#ifdef DEBUG
	SPHNetState		CheckState;
#endif

	SPHNetState		PredictedState;

	u32				m_dwIStartTime;
	u32				m_dwIEndTime;
	u32				m_dwILastUpdateTime;
};

net_updateData* CInventoryItem::NetSync( )
{
	if (!m_net_updateData)
	{
		m_net_updateData = xr_new<net_updateData>( );
	}

	return m_net_updateData;
}

CInventoryItem::CInventoryItem( )
{
	m_net_updateData = NULL;
	m_slot = NO_ACTIVE_SLOT;
	m_flags.set(Fbelt, FALSE);
	m_flags.set(Fruck, TRUE);
	m_flags.set(FRuckDefault, TRUE);
	m_pCurrentInventory = NULL;

	SetDropManual(FALSE);

	m_flags.set(eIIF_CAN_TAKE, TRUE);
	m_flags.set(FCanTrade, TRUE);
	m_flags.set(FUsingCondition, FALSE);
	m_fCondition = 1.0f;

	m_name = m_nameShort = NULL;

	m_eItemPlace = eItemPlaceUndefined;
	m_Description = "";
}

CInventoryItem::~CInventoryItem( )
{
	delete_data(m_net_updateData);

	bool B_GOOD = (!m_pCurrentInventory || (std::find(m_pCurrentInventory->m_all.begin( ), m_pCurrentInventory->m_all.end( ), this) == m_pCurrentInventory->m_all.end( )));
	if (!B_GOOD)
	{
		CObject* p = object( ).H_Parent( );
		Msg("inventory ptr is [%s]", m_pCurrentInventory ? "not-null" : "null");
		if (p)
		{
			Msg("parent name is [%s]", p->cName( ).c_str( ));
		}

		Msg("! ERROR item_id[%d] H_Parent=[%s][%d] [%d]",
			object( ).ID( ),
			p ? p->cName( ).c_str( ) : "none",
			p ? p->ID( ) : -1,
			Device.dwFrame);
	}
}

void CInventoryItem::Load(pcstr section)
{
	CHitImmunity::LoadImmunities(pSettings->r_string(section, "immunities_sect"), pSettings);

	ISpatial* self = smart_cast<ISpatial*> (this);
	if (self)
	{
		self->spatial.type |= STYPE_VISIBLEFORAI;
	}

	m_name = CStringTable( ).translate(pSettings->r_string(section, "inv_name"));
	m_nameShort = CStringTable( ).translate(pSettings->r_string(section, "inv_name_short"));

	m_weight = pSettings->r_float(section, "inv_weight");
	R_ASSERT(m_weight >= 0.f);

	m_cost = pSettings->r_u32(section, "cost");

	m_slot = READ_IF_EXISTS(pSettings, r_u32, section, "slot", NO_ACTIVE_SLOT);

	// Description
	if (pSettings->line_exist(section, "description"))
	{
		m_Description = CStringTable( ).translate(pSettings->r_string(section, "description"));
	}

	m_flags.set(Fbelt, READ_IF_EXISTS(pSettings, r_bool, section, "belt", FALSE));
	m_flags.set(FRuckDefault, READ_IF_EXISTS(pSettings, r_bool, section, "default_to_ruck", TRUE));

	m_flags.set(eIIF_CAN_TAKE, READ_IF_EXISTS(pSettings, r_bool, section, "can_take", TRUE));
	m_flags.set(FCanTrade, READ_IF_EXISTS(pSettings, r_bool, section, "can_trade", TRUE));
	m_flags.set(FIsQuestItem, READ_IF_EXISTS(pSettings, r_bool, section, "quest_item", FALSE));

	//время убирания объекта с уровня
	m_dwItemRemoveTime = READ_IF_EXISTS(pSettings, r_u32, section, "item_remove_time", ITEM_REMOVE_TIME);

	m_flags.set(FAllowSprint, READ_IF_EXISTS(pSettings, r_bool, section, "sprint_allowed", TRUE));
	m_fControlInertionFactor = READ_IF_EXISTS(pSettings, r_float, section, "control_inertion_factor", 1.0f);
	m_icon_name = READ_IF_EXISTS(pSettings, r_string, section, "icon_name", NULL);
}

void CInventoryItem::ChangeCondition(f32 fDeltaCondition)
{
	m_fCondition += fDeltaCondition;
	clamp(m_fCondition, 0.0f, 1.0f);
}

void CInventoryItem::Hit(SHit* pHDS)
{
	if (!m_flags.test(FUsingCondition))
	{
		return;
	}

	f32 hit_power = pHDS->damage( );
	hit_power *= m_HitTypeK[pHDS->hit_type];

	ChangeCondition(-hit_power);
}

pcstr CInventoryItem::Name( )
{
	return *m_name;
}

pcstr CInventoryItem::NameShort( )
{
	return *m_nameShort;
}

bool CInventoryItem::Useful( ) const
{
	return CanTake( );
}

bool CInventoryItem::Activate( )
{
	return false;
}

void CInventoryItem::Deactivate( )
{ }

void CInventoryItem::OnH_B_Independent(bool just_before_destroy)
{
	UpdateXForm( );
	m_eItemPlace = eItemPlaceUndefined;
}

void CInventoryItem::OnH_A_Independent( )
{
	m_dwItemIndependencyTime = Level( ).timeServer( );
	m_eItemPlace = eItemPlaceUndefined;
	inherited::OnH_A_Independent( );
}

void CInventoryItem::OnH_B_Chield( )
{ }

void CInventoryItem::OnH_A_Chield( )
{
	inherited::OnH_A_Chield( );
}

#ifdef DEBUG
extern	flags32	dbg_net_Draw_Flags;
#endif

void CInventoryItem::UpdateCL( )
{

#ifdef DEBUG
	if (bDebug)
	{
		if (dbg_net_Draw_Flags.test(1 << 4))
		{
			Device.seqRender.Remove(this);
			Device.seqRender.Add(this);
		}
		else
		{
			Device.seqRender.Remove(this);
		}
	}
#endif

}

void CInventoryItem::OnEvent(CNetPacket& P, u16 type)
{
	switch (type)
	{
		case GE_ADDON_ATTACH:
		{
			u32 ItemID;
			P.r_u32(ItemID);
			CInventoryItem* ItemToAttach = smart_cast<CInventoryItem*>(Level( ).Objects.net_Find(ItemID));
			if (!ItemToAttach) break;
			Attach(ItemToAttach, true);
			CActor* pActor = smart_cast<CActor*>(object( ).H_Parent( ));
			if (pActor && pActor->inventory( ).ActiveItem( ) == this)
			{
				pActor->inventory( ).SetPrevActiveSlot(pActor->inventory( ).GetActiveSlot( ));
				pActor->inventory( ).Activate(NO_ACTIVE_SLOT);
			}
		}
		break;
		case GE_ADDON_DETACH:
		{
			string64 i_name;
			P.r_stringZ(i_name);
			Detach(i_name, true);
			CActor* pActor = smart_cast<CActor*>(object( ).H_Parent( ));
			if (pActor && pActor->inventory( ).ActiveItem( ) == this)
			{
				pActor->inventory( ).SetPrevActiveSlot(pActor->inventory( ).GetActiveSlot( ));
				pActor->inventory( ).Activate(NO_ACTIVE_SLOT);
			}
		}
		break;
		case GE_CHANGE_POS:
		{
			fVector3 p;
			P.r_vec3(p);
			CPHSynchronize* pSyncObj = NULL;
			pSyncObj = object( ).PHGetSyncItem(0);
			if (!pSyncObj)
			{
				return;
			}

			SPHNetState state;
			pSyncObj->get_State(state);
			state.position = p;
			state.previous_position = p;
			pSyncObj->set_State(state);
		}
		break;
	}
}

//процесс отсоединения вещи заключается в спауне новой вещи 
//в инвентаре и установке соответствующих флагов в родительском
//объекте, поэтому функция должна быть переопределена
bool CInventoryItem::Detach(pcstr item_section_name, bool b_spawn_item)
{
	if (OnClient( ))
	{
		return true;
	}

	if (b_spawn_item)
	{
		CSE_Abstract* D = F_entity_Create(item_section_name);
		R_ASSERT(D);
		CSE_ALifeDynamicObject* l_tpALifeDynamicObject = smart_cast<CSE_ALifeDynamicObject*>(D);
		R_ASSERT(l_tpALifeDynamicObject);

		l_tpALifeDynamicObject->m_tNodeID = object( ).ai_location( ).level_vertex_id( );

		// Fill
		D->s_name = item_section_name;
		D->set_name_replace("");
		D->s_gameid = u8(GameID( ));
		D->s_RP = 0xff;
		D->ID = 0xffff;
		D->ID_Parent = u16(object( ).H_Parent( )->ID( ));
		D->ID_Phantom = 0xffff;
		D->o_Position = object( ).Position( );
		D->s_flags.assign(M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime = 0;
		// Send
		CNetPacket P;
		D->Spawn_Write(P, TRUE);
		Level( ).Send(P, net_flags(TRUE));
		// Destroy
		F_entity_Destroy(D);
	}

	return true;
}

/////////// network ///////////////////////////////
BOOL CInventoryItem::net_Spawn(CSE_Abstract* DC)
{
	m_flags.set(FInInterpolation, FALSE);
	m_flags.set(FInInterpolate, FALSE);
	m_flags.set(Fuseful_for_NPC, TRUE);
	CSE_Abstract* e = (CSE_Abstract*) (DC);
	CSE_ALifeObject* alife_object = smart_cast<CSE_ALifeObject*>(e);
	if (alife_object)
	{
		m_flags.set(Fuseful_for_NPC, alife_object->m_flags.test(CSE_ALifeObject::flUsefulForAI));
	}

	CSE_ALifeInventoryItem* pSE_InventoryItem = smart_cast<CSE_ALifeInventoryItem*>(e);
	if (!pSE_InventoryItem)
	{
		return TRUE;
	}

	//!!!
	m_fCondition = pSE_InventoryItem->m_fCondition;

	m_dwItemIndependencyTime = 0;

	return TRUE;
}

void CInventoryItem::net_Destroy( )
{
	//инвентарь которому мы принадлежали
//.	m_pCurrentInventory = NULL;
}

void CInventoryItem::save(CNetPacket& packet)
{
	packet.w_u8((u8) m_eItemPlace);
	packet.w_float(m_fCondition);

	if (object( ).H_Parent( ))
	{
		packet.w_u8(0);
		return;
	}

	u8 _num_items = (u8) object( ).PHGetSyncItemsNumber( );
	packet.w_u8(_num_items);
	object( ).PHSaveState(packet);
}

typedef CSE_ALifeInventoryItem::mask_num_items	mask_num_items;

void CInventoryItem::net_Import(CNetPacket& P)
{
	u8 NumItems = 0;
	NumItems = P.r_u8( );
	if (!NumItems)
	{
		return;
	}

	net_update_IItem N;
	N.State.force.set(0.0f, 0.0f, 0.0f);
	N.State.torque.set(0.0f, 0.0f, 0.0f);

	P.r_vec3(N.State.position);

	N.State.quaternion.x = P.r_float_q8(0.0f, 1.0f);
	N.State.quaternion.y = P.r_float_q8(0.0f, 1.0f);
	N.State.quaternion.z = P.r_float_q8(0.0f, 1.0f);
	N.State.quaternion.w = P.r_float_q8(0.0f, 1.0f);

	mask_num_items				num_items;
	num_items.common = NumItems;
	NumItems = num_items.num_items;

	N.State.enabled = num_items.mask & CSE_ALifeInventoryItem::inventory_item_state_enabled;
	if (!(num_items.mask & CSE_ALifeInventoryItem::inventory_item_angular_null))
	{
		N.State.angular_vel.x = P.r_float_q8(0.f, 10.f * PI_MUL_2);
		N.State.angular_vel.y = P.r_float_q8(0.f, 10.f * PI_MUL_2);
		N.State.angular_vel.z = P.r_float_q8(0.f, 10.f * PI_MUL_2);
	}
	else
	{
		N.State.angular_vel.set(0.0f, 0.0f, 0.0f);
	}

	if (!(num_items.mask & CSE_ALifeInventoryItem::inventory_item_linear_null))
	{
		N.State.linear_vel.x = P.r_float_q8(-32.0f, 32.0f);
		N.State.linear_vel.y = P.r_float_q8(-32.0f, 32.0f);
		N.State.linear_vel.z = P.r_float_q8(-32.0f, 32.0f);
	}
	else
	{
		N.State.linear_vel.set(0.0f, 0.0f, 0.0f);
	}
	////////////////////////////////////////////

	N.State.previous_position = N.State.position;
	N.State.previous_quaternion = N.State.quaternion;

	net_updateData* p = NetSync( );

	//if (!p->NET_IItem.empty() && (p->NET_IItem.back().dwTimeStamp>=N.dwTimeStamp))
	//	return;
	if (!p->NET_IItem.empty( ))
	{
		m_flags.set(FInInterpolate, TRUE);
	}

	Level( ).AddObject_To_Objects4CrPr(m_object);
	object( ).CrPr_SetActivated(false);
	object( ).CrPr_SetActivationStep(0);

	p->NET_IItem.push_back(N);
	while (p->NET_IItem.size( ) > 2)
	{
		p->NET_IItem.pop_front( );
	}
}

void CInventoryItem::net_Export(CNetPacket& P)
{
	P.w_u8(0);
}

void CInventoryItem::load(IReader& packet)
{
	m_eItemPlace = (EItemPlace) packet.r_u8( );
	m_fCondition = packet.r_float( );

	u8 tmp = packet.r_u8( );
	if (!tmp)
	{
		return;
	}

	if (!object( ).PPhysicsShell( ))
	{
		object( ).setup_physic_shell( );
		object( ).PPhysicsShell( )->Disable( );
	}

	object( ).PHLoadState(packet);
	object( ).PPhysicsShell( )->Disable( );
}

///////////////////////////////////////////////
void CInventoryItem::PH_B_CrPr( )
{
	net_updateData* p = NetSync( );
	//just set last update data for now
	if (object( ).CrPr_IsActivated( ))
	{
		return;
	}

	if (object( ).CrPr_GetActivationStep( ) > ph_world->m_steps_num)
	{
		return;
	}

	object( ).CrPr_SetActivated(true);

	///////////////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = object( ).PHGetSyncItem(0);
	if (!pSyncObj)
	{
		return;
	}
	///////////////////////////////////////////////
	pSyncObj->get_State(p->LastState);
	///////////////////////////////////////////////
	net_update_IItem N_I = p->NET_IItem.back( );

	pSyncObj->set_State(N_I.State);

	object( ).PHUnFreeze( );
	///////////////////////////////////////////////
	if (Level( ).InterpolationDisabled( ))
	{
		m_flags.set(FInInterpolation, FALSE);
	}
	///////////////////////////////////////////////
}

void CInventoryItem::PH_I_CrPr( )		// actions & operations between two phisic prediction steps
{
	net_updateData* p = NetSync( );
	//store recalculated data, then we able to restore it after small future prediction
	if (!object( ).CrPr_IsActivated( ))
	{
		return;
	}
	////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = object( ).PHGetSyncItem(0);
	if (!pSyncObj)
	{
		return;
	}
	////////////////////////////////////
	pSyncObj->get_State(p->RecalculatedState);
	///////////////////////////////////////////////
	fMatrix4x4 xformX;
	pSyncObj->cv2obj_Xfrom(p->RecalculatedState.quaternion, p->RecalculatedState.position, xformX);

	VERIFY2(_valid(xformX), *object( ).cName( ));
	pSyncObj->cv2obj_Xfrom(p->RecalculatedState.quaternion, p->RecalculatedState.position, xformX);

	p->IRecRot.set(xformX);
	p->IRecPos.set(xformX.c);
	VERIFY2(_valid(p->IRecPos), *object( ).cName( ));
}

#ifdef DEBUG
void CInventoryItem::PH_Ch_CrPr( )
{
	net_updateData* p = NetSync( );
	//restore recalculated data and get data for interpolation
	if (!object( ).CrPr_IsActivated( ))
	{
		return;
	}
	////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = object( ).PHGetSyncItem(0);
	if (!pSyncObj)
	{
		return;
	}
	////////////////////////////////////
	pSyncObj->get_State(p->CheckState);

	if (!object( ).H_Parent( ) && object( ).getVisible( ))
	{
		if (p->CheckState.enabled == false && p->RecalculatedState.enabled == true)
		{
			///////////////////////////////////////////////////////////////////
			pSyncObj->set_State(p->LastState);
			pSyncObj->set_State(p->RecalculatedState);//, N_A.State.enabled);

			object( ).PHUnFreeze( );
			///////////////////////////////////////////////////////////////////
			ph_world->Step( );
			///////////////////////////////////////////////////////////////////
			PH_Ch_CrPr( );
			////////////////////////////////////
		}
	}
}
#endif

void CInventoryItem::PH_A_CrPr( )
{
	net_updateData* p = NetSync( );
	//restore recalculated data and get data for interpolation
	if (!object( ).CrPr_IsActivated( ))
	{
		return;
	}
	////////////////////////////////////
	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = object( ).PHGetSyncItem(0);
	if (!pSyncObj)
	{
		return;
	}
	////////////////////////////////////
	pSyncObj->get_State(p->PredictedState);
	////////////////////////////////////
	pSyncObj->set_State(p->RecalculatedState);
	////////////////////////////////////

	if (!m_flags.test(FInInterpolate))
	{
		return;
	}
	////////////////////////////////////
	fMatrix4x4 xformX;
	pSyncObj->cv2obj_Xfrom(p->PredictedState.quaternion, p->PredictedState.position, xformX);

	VERIFY2(_valid(xformX), *object( ).cName( ));
	pSyncObj->cv2obj_Xfrom(p->PredictedState.quaternion, p->PredictedState.position, xformX);

	p->IEndRot.set(xformX);
	p->IEndPos.set(xformX.c);
	VERIFY2(_valid(p->IEndPos), *object( ).cName( ));
	/////////////////////////////////////////////////////////////////////////
	CalculateInterpolationParams( );
	///////////////////////////////////////////////////
}

extern f32		g_cl_lvInterp;

void CInventoryItem::CalculateInterpolationParams( )
{
	net_updateData* p = NetSync( );
	p->IStartPos.set(object( ).Position( ));
	p->IStartRot.set(object( ).XFORM( ));

	fVector3 P0;
	fVector3 P1;
	fVector3 P2;
	fVector3 P3;

	CPHSynchronize* pSyncObj = NULL;
	pSyncObj = object( ).PHGetSyncItem(0);

	fMatrix4x4 xformX0;
	fMatrix4x4 xformX1;

	if (m_flags.test(FInInterpolation))
	{
		u32 CurTime = Level( ).timeServer( );
		f32 factor = f32(CurTime - p->m_dwIStartTime) / (p->m_dwIEndTime - p->m_dwIStartTime);
		if (factor > 1.0f)
		{
			factor = 1.0f;
		}

		f32 c = factor;
		for (u32 k = 0; k < 3; k++)
		{
			P0[k] = c * (c * (c * p->SCoeff[k][0] + p->SCoeff[k][1]) + p->SCoeff[k][2]) + p->SCoeff[k][3];
			P1[k] = (c * c * p->SCoeff[k][0] * 3 + c * p->SCoeff[k][1] * 2 + p->SCoeff[k][2]) / 3; // сокрость из формулы в 3 раза превышает скорость при расчете коэффициентов !!!!
		}

		P0.set(p->IStartPos);
		P1.add(p->IStartPos);
	}
	else
	{
		P0 = p->IStartPos;

		if (p->LastState.linear_vel.x == 0 && p->LastState.linear_vel.y == 0 && p->LastState.linear_vel.z == 0)
		{
			pSyncObj->cv2obj_Xfrom(p->RecalculatedState.previous_quaternion, p->RecalculatedState.previous_position, xformX0);
			pSyncObj->cv2obj_Xfrom(p->RecalculatedState.quaternion, p->RecalculatedState.position, xformX1);
		}
		else
		{
			pSyncObj->cv2obj_Xfrom(p->LastState.previous_quaternion, p->LastState.previous_position, xformX0);
			pSyncObj->cv2obj_Xfrom(p->LastState.quaternion, p->LastState.position, xformX1);
		}

		P1.sub(xformX1.c, xformX0.c);
		P1.add(p->IStartPos);
	}

	P2.sub(p->PredictedState.position, p->PredictedState.linear_vel);
	pSyncObj->cv2obj_Xfrom(p->PredictedState.quaternion, P2, xformX0);
	P2.set(xformX0.c);

	pSyncObj->cv2obj_Xfrom(p->PredictedState.quaternion, p->PredictedState.position, xformX1);
	P3.set(xformX1.c);
	/////////////////////////////////////////////////////////////////////////////
	fVector3 TotalPath;
	TotalPath.sub(P3, P0);
	f32 TotalLen = TotalPath.magnitude( );

	SPHNetState	State0 = (p->NET_IItem.back( )).State;
	SPHNetState	State1 = p->PredictedState;

	f32 lV0 = State0.linear_vel.magnitude( );
	f32 lV1 = State1.linear_vel.magnitude( );

	u32		ConstTime = u32((fixed_step - ph_world->m_frame_time) * 1000) + Level( ).GetInterpolationSteps( ) * u32(fixed_step * 1000);

	p->m_dwIStartTime = p->m_dwILastUpdateTime;

	if ((lV0 + lV1) > 0.000001 && g_cl_lvInterp == 0.0f)
	{
		u32 CulcTime = iCeil(TotalLen * 2000.0f / (lV0 + lV1));
		p->m_dwIEndTime = p->m_dwIStartTime + _min(CulcTime, ConstTime);
	}
	else
	{
		p->m_dwIEndTime = p->m_dwIStartTime + ConstTime;
	}
	/////////////////////////////////////////////////////////////////////////////
	fVector3 V0;
	fVector3 V1;
	V0.sub(P1, P0);
	V1.sub(P3, P2);
	lV0 = V0.magnitude( );
	lV1 = V1.magnitude( );

	if (TotalLen != 0)
	{
		if (V0.x != 0 || V0.y != 0 || V0.z != 0)
		{
			if (lV0 > TotalLen / 3)
			{
				V0.normalize( );
				V0.mul(TotalLen / 3);
				P1.add(V0, P0);
			}
		}

		if (V1.x != 0 || V1.y != 0 || V1.z != 0)
		{
			if (lV1 > TotalLen / 3)
			{
				V1.normalize( );
				V1.mul(TotalLen / 3);
				P2.sub(P3, V1);
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////
	for (u32 i = 0; i < 3; i++)
	{
		p->SCoeff[i][0] = P3[i] - 3 * P2[i] + 3 * P1[i] - P0[i];
		p->SCoeff[i][1] = 3 * P2[i] - 6 * P1[i] + 3 * P0[i];
		p->SCoeff[i][2] = 3 * P1[i] - 3 * P0[i];
		p->SCoeff[i][3] = P0[i];
	}
	/////////////////////////////////////////////////////////////////////////////
	m_flags.set(FInInterpolation, TRUE);

	if (object( ).m_pPhysicsShell)
	{
		object( ).m_pPhysicsShell->NetInterpolationModeON( );
	}
}

void CInventoryItem::make_Interpolation( )
{
	net_updateData* p = NetSync( );
	p->m_dwILastUpdateTime = Level( ).timeServer( );

	if (!object( ).H_Parent( ) && object( ).getVisible( ) && object( ).m_pPhysicsShell && m_flags.test(FInInterpolation))
	{

		u32 CurTime = Level( ).timeServer( );
		if (CurTime >= p->m_dwIEndTime)
		{
			m_flags.set(FInInterpolation, FALSE);

			object( ).m_pPhysicsShell->NetInterpolationModeOFF( );

			CPHSynchronize* pSyncObj = NULL;
			pSyncObj = object( ).PHGetSyncItem(0);
			pSyncObj->set_State(p->PredictedState);
			fMatrix4x4 xformI;
			pSyncObj->cv2obj_Xfrom(p->PredictedState.quaternion, p->PredictedState.position, xformI);
			VERIFY2(_valid(object( ).renderable.xform), *object( ).cName( ));
			object( ).XFORM( ).set(xformI);
			VERIFY2(_valid(object( ).renderable.xform), *object( ).cName( ));
		}
		else
		{
			VERIFY(CurTime <= p->m_dwIEndTime);
			f32 factor = f32(CurTime - p->m_dwIStartTime) / (p->m_dwIEndTime - p->m_dwIStartTime);
			if (factor > 1.0f)
			{
				factor = 1.0f;
			}
			else if (factor < 0.0f)
			{
				factor = 0.0f;
			}

			fVector3 IPos;
			fQuaternion IRot;

			f32 c = factor;
			for (u32 k = 0; k < 3; k++)
			{
				IPos[k] = c * (c * (c * p->SCoeff[k][0] + p->SCoeff[k][1]) + p->SCoeff[k][2]) + p->SCoeff[k][3];
			}

			VERIFY2(_valid(IPos), *object( ).cName( ));
			VERIFY(factor >= 0.0f && factor <= 1.0f);
			IRot.slerp(p->IStartRot, p->IEndRot, factor);
			VERIFY2(_valid(IRot), *object( ).cName( ));
			object( ).XFORM( ).rotation(IRot);
			VERIFY2(_valid(object( ).renderable.xform), *object( ).cName( ));
			object( ).Position( ).set(IPos);
			VERIFY2(_valid(object( ).renderable.xform), *object( ).cName( ));
		}
	}
	else
	{
		m_flags.set(FInInterpolation, FALSE);
	}

#ifdef DEBUG
	fVector3 iPos = object( ).Position( );

	if (!object( ).H_Parent( ) && object( ).getVisible( ))
	{
		if (m_net_updateData)
		{
			m_net_updateData->LastVisPos.push_back(iPos);
		}
	}
#endif

}

void CInventoryItem::reload(pcstr section)
{
	inherited::reload(section);
	m_holder_range_modifier = READ_IF_EXISTS(pSettings, r_float, section, "holder_range_modifier", 1.f);
	m_holder_fov_modifier = READ_IF_EXISTS(pSettings, r_float, section, "holder_fov_modifier", 1.f);
}

void CInventoryItem::reinit( )
{
	m_pCurrentInventory = NULL;
	m_eItemPlace = eItemPlaceUndefined;
}

bool CInventoryItem::can_kill( ) const
{
	return false;
}

CInventoryItem* CInventoryItem::can_kill(CInventory* inventory) const
{
	return				(0);
}

const CInventoryItem* CInventoryItem::can_kill(const xr_vector<const CGameObject*>& items) const
{
	return				(0);
}

CInventoryItem* CInventoryItem::can_make_killing(const CInventory* inventory) const
{
	return				(0);
}

bool CInventoryItem::ready_to_kill( ) const
{
	return false;
}

void CInventoryItem::activate_physic_shell( )
{
	CEntityAlive* E = smart_cast<CEntityAlive*>(object( ).H_Parent( ));
	if (!E)
	{
		on_activate_physic_shell( );
		return;
	}

	UpdateXForm( );

	object( ).CPhysicsShellHolder::activate_physic_shell( );
}

void CInventoryItem::UpdateXForm( )
{
	if (0 == object( ).H_Parent( ))
	{
		return;
	}

	// Get access to entity and its visual
	CEntityAlive* E = smart_cast<CEntityAlive*>(object( ).H_Parent( ));
	if (!E)
	{
		return;
	}

	if (E->cast_base_monster( ))
	{
		return;
	}

	const CInventoryOwner* parent = smart_cast<const CInventoryOwner*>(E);
	if (parent && parent->use_simplified_visual( ))
	{
		return;
	}

	if (parent->attached(this))
	{
		return;
	}

	R_ASSERT(E);
	CKinematics* V = smart_cast<CKinematics*>	(E->Visual( ));
	VERIFY(V);

	// Get matrices
	s32 boneL;
	s32 boneR;
	s32 boneR2;
	E->g_WeaponBones(boneL, boneR, boneR2);

#pragma todo("TO ALL: serious performance problem")
	V->CalculateBones( );
	fMatrix4x4& mL = V->LL_GetTransform(u16(boneL));
	fMatrix4x4& mR = V->LL_GetTransform(u16(boneR));
	// Calculate
	fMatrix4x4			mRes;
	fVector3			R;
	fVector3 D;
	fVector3 N;
	D.sub(mL.c, mR.c);	D.normalize_safe( );

	if (fis_zero(D.magnitude( )))
	{
		mRes.set(E->XFORM( ));
		mRes.c.set(mR.c);
	}
	else
	{
		D.normalize( );
		R.crossproduct(mR.j, D);

		N.crossproduct(D, R);
		N.normalize( );

		mRes.set(R, N, D, mR.c);
		mRes.mulA_43(E->XFORM( ));
	}

	//	UpdatePosition	(mRes);
	object( ).Position( ).set(mRes.c);
}

#ifdef DEBUG
void CInventoryItem::OnRender( )
{
	if (bDebug && object( ).Visual( ))
	{
		if (!(dbg_net_Draw_Flags.is_any((1 << 4))))
		{
			return;
		}

		fVector3 bc;
		fVector3 bd;
		object( ).Visual( )->vis.box.get_CD(bc, bd);
		fMatrix4x4	M = object( ).XFORM( );
		M.c.add(bc);
		Level( ).debug_renderer( ).draw_obb(M, bd, color_rgba(0, 0, 255, 255));
		/*
				u32 Color;
				if (processing_enabled())
				{
					if (m_bInInterpolation)
						Color = color_rgba(0,255,255, 255);
					else
						Color = color_rgba(0,255,0, 255);
				}
				else
				{
					if (m_bInInterpolation)
						Color = color_rgba(255,0,255, 255);
					else
						Color = color_rgba(255, 0, 0, 255);
				}

		//		Level().debug_renderer().draw_obb			(M,bd,Color);
				f32 size = 0.01f;
				if (!H_Parent())
				{
					Level().debug_renderer().draw_aabb			(Position(), size, size, size, color_rgba(0, 255, 0, 255));

					fVector3 Pos1, Pos2;
					VIS_POSITION_it It = LastVisPos.begin();
					Pos1 = *It;
					for (; It != LastVisPos.end(); It++)
					{
						Pos2 = *It;
						Level().debug_renderer().draw_line(Fidentity, Pos1, Pos2, color_rgba(255, 255, 0, 255));
						Pos1 = Pos2;
					}

				}
				//---------------------------------------------------------
				if (OnClient() && !H_Parent() && m_bInInterpolation)
				{
					fMatrix4x4 xformI;

					xformI.rotation(IRecRot);
					xformI.c.set(IRecPos);
					Level().debug_renderer().draw_aabb			(IRecPos, size, size, size, color_rgba(255, 0, 255, 255));

					xformI.rotation(IEndRot);
					xformI.c.set(IEndPos);
					Level().debug_renderer().draw_obb			(xformI,bd,color_rgba(0, 255, 0, 255));

					///////////////////////////////////////////////////////////////////////////
					fVector3 point0 = IStartPos, point1;

					f32 c = 0;
					for (f32 i=0.1f; i<1.1f; i+= 0.1f)
					{
						c = i;// * 0.1f;
						for (u32 k=0; k<3; k++)
						{
							point1[k] = c*(c*(c*SCoeff[k][0]+SCoeff[k][1])+SCoeff[k][2])+SCoeff[k][3];
						};
						Level().debug_renderer().draw_line(Fidentity, point0, point1, color_rgba(0, 0, 255, 255));
						point0.set(point1);
					}
				}
				*/
	}
}
#endif

DLL_Pure* CInventoryItem::_construct( )
{
	m_object = smart_cast<CPhysicsShellHolder*>(this);
	VERIFY(m_object);
	return inherited::_construct( );
}

void CInventoryItem::modify_holder_params(f32& range, f32& fov) const
{
	range *= m_holder_range_modifier;
	fov *= m_holder_fov_modifier;
}

bool CInventoryItem::NeedToDestroyObject( )	const
{
	return (TimePassedAfterIndependant( ) > m_dwItemRemoveTime);
}

ALife::_TIME_ID CInventoryItem::TimePassedAfterIndependant( ) const
{
	if (!object( ).H_Parent( ) && m_dwItemIndependencyTime != 0)
	{
		return Level( ).timeServer( ) - m_dwItemIndependencyTime;
	}
	else
	{
		return 0;
	}
}

bool CInventoryItem::CanTrade( ) const
{
	bool res = true;
#pragma todo("Dima to Andy : why CInventoryItem::CanTrade can be called for the item, which doesn't have owner?")
	if (m_pCurrentInventory)
	{
		res = inventory_owner( ).AllowItemToTrade(this, m_eItemPlace);
	}

	return (res && m_flags.test(FCanTrade) && !IsQuestItem( ));
}

f32 CInventoryItem::GetKillMsgXPos( ) const
{
	return READ_IF_EXISTS(pSettings, r_float, m_object->cNameSect( ), "kill_msg_x", 0.0f);
}

f32 CInventoryItem::GetKillMsgYPos( ) const
{
	return READ_IF_EXISTS(pSettings, r_float, m_object->cNameSect( ), "kill_msg_y", 0.0f);
}

f32 CInventoryItem::GetKillMsgWidth( ) const
{
	return READ_IF_EXISTS(pSettings, r_float, m_object->cNameSect( ), "kill_msg_width", 0.0f);
}

f32 CInventoryItem::GetKillMsgHeight( ) const
{
	return READ_IF_EXISTS(pSettings, r_float, m_object->cNameSect( ), "kill_msg_height", 0.0f);
}

s32 CInventoryItem::GetXPos( ) const
{
	return pSettings->r_u32(m_object->cNameSect( ), "inv_grid_x");
}

s32 CInventoryItem::GetYPos( ) const
{
	return pSettings->r_u32(m_object->cNameSect( ), "inv_grid_y");
}

s32 CInventoryItem::GetGridWidth( ) const
{
	return pSettings->r_u32(m_object->cNameSect( ), "inv_grid_width");
}

s32 CInventoryItem::GetGridHeight( ) const
{
	return pSettings->r_u32(m_object->cNameSect( ), "inv_grid_height");
}

bool CInventoryItem::IsNecessaryItem(CInventoryItem* item)
{
	return IsNecessaryItem(item->object( ).cNameSect( ));
}

BOOL CInventoryItem::IsInvalid( ) const
{
	return (object( ).getDestroy( ) || GetDropManual( ));
}

u16 CInventoryItem::bone_count_to_synchronize( ) const
{
	return 0;
}
