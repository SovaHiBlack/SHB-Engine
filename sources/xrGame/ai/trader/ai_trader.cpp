////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_trader.cpp
//	Created 	: 13.05.2002
//  Modified 	: 13.05.2002
//	Author		: Jim
//	Description : AI Behaviour for monster "Trader"
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "ai_trader.h"
#include "../../Trade.h"
#include "../../script_entity_action.h"
#include "../../script_game_object.h"
#include "../../Inventory.h"
#include "../../xrserver_objects_alife_monsters.h"
#include "../../Artefact.h"
#include "../../xrserver.h"
#include "../../RelationRegistry.h"
#include "../../object_broker.h"
#include "../../sound_player.h"
#include "../../level.h"
#include "../../script_callback_ex.h"
#include "../../GameObject_space.h"
#include "../../clsid_game.h"
#include "trader_animation.h"

CAI_Trader::CAI_Trader( )
{
	AnimMan = xr_new<CTraderAnimation>(this);
}

CAI_Trader::~CAI_Trader( )
{
	xr_delete(m_sound_player);
	xr_delete(AnimMan);
}

void CAI_Trader::Load(pcstr section)
{
	//	setEnabled						(FALSE);
	inherited::Load(section);

	//fHealth							= pSettings->r_float	(section,"Health");
	SetfHealth(pSettings->r_float(section, "Health"));

	f32 max_weight = pSettings->r_float(section, "max_item_mass");
	inventory( ).SetMaxWeight(max_weight * 1000);
	//	inventory().SetMaxRuck(1000000);
	inventory( ).CalcTotalWeight( );
}

void CAI_Trader::reinit( )
{
	CScriptEntity::reinit( );
	CEntityAlive::reinit( );
	CInventoryOwner::reinit( );
	sound( ).reinit( );
	animation( ).reinit( );

	m_busy_now = false;
}

void CAI_Trader::reload(pcstr section)
{
	CEntityAlive::reload(section);
	CInventoryOwner::reload(section);
	sound( ).reload(section);
}

bool CAI_Trader::bfAssignSound(CScriptEntityAction* tpEntityAction)
{
	if (!CScriptEntity::bfAssignSound(tpEntityAction))
	{
//m_cur_head_anim_type	= MonsterSpace::eHeadAnimNone;
		return					(false);
	}

	//CScriptSoundAction	&l_tAction	= tpEntityAction->m_tSoundAction;
	//m_cur_head_anim_type = l_tAction.m_tHeadAnimType;

	return				(true);
}

//////////////////////////////////////////////////////////////////////////
// Look At Actor
//////////////////////////////////////////////////////////////////////////
void CAI_Trader::BoneCallback(CBoneInstance* B)
{
	CAI_Trader* this_class = static_cast<CAI_Trader*>(B->Callback_Param);

	this_class->LookAtActor(B);
}

void CAI_Trader::LookAtActor(CBoneInstance* B)
{
	fVector3 dir;
	dir.sub(Level( ).CurrentEntity( )->Position( ), Position( ));

	f32 yaw;
	f32 pitch;
	dir.getHP(yaw, pitch);

	f32 h;
	f32 p;
	f32 b;
	XFORM( ).getHPB(h, p, b);
	f32 cur_yaw = h;
	f32 dy = _abs(angle_normalize_signed(yaw - cur_yaw));

	if (angle_normalize_signed(yaw - cur_yaw) > 0) dy *= -1.f;

	fMatrix4x4 M;
	M.setHPB(0.f, -dy, 0.f);
	B->mTransform.mulB_43(M);
}

//////////////////////////////////////////////////////////////////////////

BOOL CAI_Trader::net_Spawn(CSE_Abstract* DC)
{
	CSE_Abstract* e = (CSE_Abstract*)(DC);
	CSE_ALifeTrader* l_tpTrader = smart_cast<CSE_ALifeTrader*>(e);
	R_ASSERT(l_tpTrader);

	//����������� PDA � InventoryOwner
	if (!CInventoryOwner::net_Spawn(DC))
		return				(FALSE);

	if (!inherited::net_Spawn(DC) || !CScriptEntity::net_Spawn(DC))
		return				(FALSE);

	setVisible(TRUE);
	setEnabled(TRUE);

	set_money(l_tpTrader->m_dwMoney, false);

	// ��������� callback �� �����
	CBoneInstance* bone_head = &smart_cast<CKinematics*>(Visual( ))->LL_GetBoneInstance(smart_cast<CKinematics*>(Visual( ))->LL_BoneID("bip01_head"));
	bone_head->set_callback(bctCustom, BoneCallback, this);

	shedule.t_min = 100;
	shedule.t_max = 2500; // This equaltiy is broken by Dima :-( // 30 * NET_Latency / 4;

	return					(TRUE);
}

void CAI_Trader::net_Export(CNetPacket& P)
{
	R_ASSERT(Local( ));

	//	P.w_float						(inventory().TotalWeight());
	//	P.w_u32							(m_dwMoney);
}

void CAI_Trader::net_Import(CNetPacket& P)
{
	R_ASSERT(Remote( ));

	f32							fDummy;
	P.r_float(fDummy);
	set_money(P.r_u32( ), false);

	setVisible(TRUE);
	setEnabled(TRUE);
}

void CAI_Trader::OnEvent(CNetPacket& P, u16 type)
{
	inherited::OnEvent(P, type);
	CInventoryOwner::OnEvent(P, type);

	u16 id;
	CObject* Obj;

	switch (type)
	{
		case GE_TRADE_BUY:
		case GE_OWNERSHIP_TAKE:
			P.r_u16(id);
			Obj = Level( ).Objects.net_Find(id);
			if (inventory( ).CanTakeItem(smart_cast<CInventoryItem*>(Obj)))
			{
				Obj->H_SetParent(this);
				inventory( ).Take(smart_cast<CGameObject*>(Obj), false, false);
			}
			else
			{
				CNetPacket				P;
				u_EventGen(P, GE_OWNERSHIP_REJECT, ID( ));
				P.w_u16(u16(Obj->ID( )));
				u_EventSend(P);
			}
			break;
		case GE_TRADE_SELL:
		case GE_OWNERSHIP_REJECT:
		{
			P.r_u16(id);
			Obj = Level( ).Objects.net_Find(id);
			bool just_before_destroy = !P.r_eof( ) && P.r_u8( );
			Obj->SetTmpPreDestroy(just_before_destroy);
			if (inventory( ).DropItem(smart_cast<CGameObject*>(Obj)))
				Obj->H_SetParent(0, just_before_destroy);
		}break;
		case GE_TRANSFER_AMMO:
			break;
	}
}

void CAI_Trader::feel_touch_new(CObject* O)
{
	if (!g_Alive( ))		return;
	if (Remote( ))		return;

	// Now, test for game specific logical objects to minimize traffic
	CInventoryItem* I = smart_cast<CInventoryItem*>	(O);

	if (I && I->useful_for_NPC( ))
	{
		Msg("Taking item %s!", *I->object( ).cName( ));
		CNetPacket		P;
		u_EventGen(P, GE_OWNERSHIP_TAKE, ID( ));
		P.w_u16(u16(I->object( ).ID( )));
		u_EventSend(P);
	}
}

void CAI_Trader::DropItemSendMessage(CObject* O)
{
	if (!O || !O->H_Parent( ) || (this != O->H_Parent( )))
		return;

	Msg("Dropping item!");
	// We doesn't have similar weapon - pick up it
	CNetPacket				P;
	u_EventGen(P, GE_OWNERSHIP_REJECT, ID( ));
	P.w_u16(u16(O->ID( )));
	u_EventSend(P);
}

void CAI_Trader::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	UpdateInventoryOwner(dt);

	if (GetScriptControl( )) ProcessScripts( );
	else Think( );
}

void CAI_Trader::g_WeaponBones(s32& L, s32& R1, s32& R2)
{
	CKinematics* V = smart_cast<CKinematics*>(Visual( ));
	R1 = V->LL_BoneID("bip01_r_hand");
	R2 = V->LL_BoneID("bip01_r_finger2");
	L = V->LL_BoneID("bip01_l_finger1");
}

void CAI_Trader::g_fireParams(const CHudItem* pHudItem, fVector3& P, fVector3& D)
{
	VERIFY(inventory( ).ActiveItem( ));
	if (g_Alive( ) && inventory( ).ActiveItem( ))
	{
		Center(P);
		D.setHP(0, 0);
		D.normalize_safe( );
	}
}

void CAI_Trader::Think( )
{ }

void CAI_Trader::Die(CObject* who)
{
	inherited::Die(who);
}

void CAI_Trader::net_Destroy( )
{
	inherited::net_Destroy( );
	CScriptEntity::net_Destroy( );
}

void CAI_Trader::UpdateCL( )
{
	inherited::UpdateCL( );
	sound( ).update(Device.fTimeDelta);


	if (!GetScriptControl( ) && !bfScriptAnimation( ))
		animation( ).update_frame( );
}

BOOL CAI_Trader::UsedAI_Locations( )
{
	return					(TRUE);
}

void CAI_Trader::OnStartTrade( )
{
	m_busy_now = true;
	callback(GameObject::eTradeStart)();
}

void CAI_Trader::OnStopTrade( )
{
	m_busy_now = false;
	callback(GameObject::eTradeStop)();
}

////////////////////////////////////////////////////////////////////////////////////////////

bool CAI_Trader::can_attach(const CInventoryItem* inventory_item) const
{
	return				(false);
}

bool CAI_Trader::use_bolts( ) const
{
	return				(false);
}

void CAI_Trader::spawn_supplies( )
{
	inherited::spawn_supplies( );
	CInventoryOwner::spawn_supplies( );
}

void CAI_Trader::save(CNetPacket& output_packet)
{
	inherited::save(output_packet);
	CInventoryOwner::save(output_packet);
}

void CAI_Trader::load(IReader& input_packet)
{
	inherited::load(input_packet);
	CInventoryOwner::load(input_packet);
}

//��������� ������ ���������� � �������
u32 CAI_Trader::ArtefactPrice(CArtefact* pArtefact)
{
	return pArtefact->Cost( );
}

//������� ���������, � ����������� ���������� ������ ������� (true - ���� �������� ��� � ������)
bool CAI_Trader::BuyArtefact(CArtefact* pArtefact)
{
	VERIFY(pArtefact);
	return false;
}

ALife::ERelationType  CAI_Trader::tfGetRelationType(const CEntityAlive* tpEntityAlive) const
{
	const CInventoryOwner* pOtherIO = smart_cast<const CInventoryOwner*>(tpEntityAlive);

	ALife::ERelationType relation = ALife::eRelationTypeDummy;

	if (pOtherIO && !(const_cast<CEntityAlive*>(tpEntityAlive)->cast_base_monster( )))
		relation = SRelationRegistry( ).GetRelationType(static_cast<const CInventoryOwner*>(this), pOtherIO);

	if (ALife::eRelationTypeDummy != relation)
		return relation;
	else
		return inherited::tfGetRelationType(tpEntityAlive);
}

DLL_Pure* CAI_Trader::_construct( )
{
	m_sound_player = xr_new<CSoundPlayer>(this);

	CEntityAlive::_construct( );
	CInventoryOwner::_construct( );
	CScriptEntity::_construct( );

	return						(this);
}

bool CAI_Trader::AllowItemToTrade(CInventoryItem const* item, EItemPlace place) const
{
	if (!g_Alive( ))
		return					(true);

	if (item->object( ).CLS_ID == CLSID_DEVICE_PDA)
		return					(false);

	return						(CInventoryOwner::AllowItemToTrade(item, place));
}

void CAI_Trader::dialog_sound_start(pcstr phrase)
{
	animation( ).external_sound_start(phrase);
}

void CAI_Trader::dialog_sound_stop( )
{
	animation( ).external_sound_stop( );
}
using namespace luabind;

#pragma optimize("s",on)
void CAI_Trader::script_register(lua_State* L)
{
	module(L)
		[
			class_<CAI_Trader, CGameObject>("CAI_Trader")
				.def(constructor<>( ))
		];
}
