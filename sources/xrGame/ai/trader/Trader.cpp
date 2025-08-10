////////////////////////////////////////////////////////////////////////////
//	Module 		: Trader.cpp
//	Description : AI Behaviour for monster "Trader"
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "Trader.h"
#include "TraderAnimation.h"
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
#include "../../Level.h"
#include "../../script_callback_ex.h"
#include "../../GameObject_space.h"
#include "../../clsid_game.h"

CTrader::CTrader( )
{
	AnimMan = xr_new<CTraderAnimation>(this);
}

CTrader::~CTrader( )
{
	xr_delete(m_sound_player);
	xr_delete(AnimMan);
}

void CTrader::Load(pcstr section)
{
	//	setEnabled						(FALSE);
	inherited::Load(section);

	//fHealth							= pSettings->r_float	(section,"Health");
	SetfHealth(pSettings->r_float(section, "Health"));

	f32 max_weight = pSettings->r_float(section, "max_item_mass");
	inventory( ).SetMaxWeight(max_weight * 1000);
	inventory( ).CalcTotalWeight( );
}

void CTrader::reinit( )
{
	CScriptEntity::reinit( );
	CEntityAlive::reinit( );
	CInventoryOwner::reinit( );
	sound( ).reinit( );
	animation( ).reinit( );

	m_busy_now = false;
}

void CTrader::reload(pcstr section)
{
	CEntityAlive::reload(section);
	CInventoryOwner::reload(section);
	sound( ).reload(section);
}

bool CTrader::bfAssignSound(CScriptEntityAction* tpEntityAction)
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
void CTrader::BoneCallback(CBoneInstance* B)
{
	CTrader* this_class = static_cast<CTrader*>(B->Callback_Param);

	this_class->LookAtActor(B);
}

void CTrader::LookAtActor(CBoneInstance* B)
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

BOOL CTrader::net_Spawn(CSE_Abstract* DC)
{
	CSE_Abstract* e = (CSE_Abstract*)(DC);
	CSE_ALifeTrader* l_tpTrader = smart_cast<CSE_ALifeTrader*>(e);
	R_ASSERT(l_tpTrader);

	//проспавнить PDA у InventoryOwner
	if (!CInventoryOwner::net_Spawn(DC))
		return				(FALSE);

	if (!inherited::net_Spawn(DC) || !CScriptEntity::net_Spawn(DC))
		return				(FALSE);

	setVisible(TRUE);
	setEnabled(TRUE);

	set_money(l_tpTrader->m_dwMoney, false);

	// Установка callback на кости
	CBoneInstance* bone_head = &smart_cast<CKinematics*>(Visual( ))->LL_GetBoneInstance(smart_cast<CKinematics*>(Visual( ))->LL_BoneID("bip01_head"));
	bone_head->set_callback(bctCustom, BoneCallback, this);

	shedule.t_min = 100;
	shedule.t_max = 2500; // This equaltiy is broken by Dima :-( // 30 * NET_Latency / 4;

	return					(TRUE);
}

void CTrader::net_Export(CNetPacket& P)
{
	R_ASSERT(Local( ));

	//	P.w_float						(inventory().TotalWeight());
	//	P.w_u32							(m_dwMoney);
}

void CTrader::net_Import(CNetPacket& P)
{
	R_ASSERT(Remote( ));

	f32							fDummy;
	P.r_float(fDummy);
	set_money(P.r_u32( ), false);

	setVisible(TRUE);
	setEnabled(TRUE);
}

void CTrader::OnEvent(CNetPacket& P, u16 type)
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

void CTrader::feel_touch_new(CObject* O)
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

void CTrader::DropItemSendMessage(CObject* O)
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

void CTrader::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	UpdateInventoryOwner(dt);

	if (GetScriptControl( )) ProcessScripts( );
	else Think( );
}

void CTrader::g_WeaponBones(s32& L, s32& R1, s32& R2)
{
	CKinematics* V = smart_cast<CKinematics*>(Visual( ));
	R1 = V->LL_BoneID("bip01_r_hand");
	R2 = V->LL_BoneID("bip01_r_finger2");
	L = V->LL_BoneID("bip01_l_finger1");
}

void CTrader::g_fireParams(const CHudItem* pHudItem, fVector3& P, fVector3& D)
{
	VERIFY(inventory( ).ActiveItem( ));
	if (g_Alive( ) && inventory( ).ActiveItem( ))
	{
		Center(P);
		D.setHP(0, 0);
		D.normalize_safe( );
	}
}

void CTrader::Think( )
{ }

void CTrader::Die(CObject* who)
{
	inherited::Die(who);
}

void CTrader::net_Destroy( )
{
	inherited::net_Destroy( );
	CScriptEntity::net_Destroy( );
}

void CTrader::UpdateCL( )
{
	inherited::UpdateCL( );
	sound( ).update(Device.fTimeDelta);


	if (!GetScriptControl( ) && !bfScriptAnimation( ))
		animation( ).update_frame( );
}

BOOL CTrader::UsedAI_Locations( )
{
	return					(TRUE);
}

void CTrader::OnStartTrade( )
{
	m_busy_now = true;
	callback(GameObject::eTradeStart)();
}

void CTrader::OnStopTrade( )
{
	m_busy_now = false;
	callback(GameObject::eTradeStop)();
}

////////////////////////////////////////////////////////////////////////////////////////////

bool CTrader::can_attach(const CInventoryItem* inventory_item) const
{
	return				(false);
}

bool CTrader::use_bolts( ) const
{
	return				(false);
}

void CTrader::spawn_supplies( )
{
	inherited::spawn_supplies( );
	CInventoryOwner::spawn_supplies( );
}

void CTrader::save(CNetPacket& output_packet)
{
	inherited::save(output_packet);
	CInventoryOwner::save(output_packet);
}

void CTrader::load(IReader& input_packet)
{
	inherited::load(input_packet);
	CInventoryOwner::load(input_packet);
}

//проверяет список артефактов в заказах
u32 CTrader::ArtefactPrice(CArtefact* pArtefact)
{
	return pArtefact->Cost( );
}

//продажа артефакта, с последуещим изменением списка заказов (true - если артефакт был в списке)
bool CTrader::BuyArtefact(CArtefact* pArtefact)
{
	VERIFY(pArtefact);
	return false;
}

ALife::ERelationType CTrader::tfGetRelationType(const CEntityAlive* tpEntityAlive) const
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

DLL_Pure* CTrader::_construct( )
{
	m_sound_player = xr_new<CSoundPlayer>(this);

	CEntityAlive::_construct( );
	CInventoryOwner::_construct( );
	CScriptEntity::_construct( );

	return						(this);
}

bool CTrader::AllowItemToTrade(CInventoryItem const* item, EItemPlace place) const
{
	if (!g_Alive( ))
		return					(true);

	if (item->object( ).CLS_ID == CLSID_DEVICE_PDA)
		return					(false);

	return						(CInventoryOwner::AllowItemToTrade(item, place));
}

void CTrader::dialog_sound_start(pcstr phrase)
{
	animation( ).external_sound_start(phrase);
}

void CTrader::dialog_sound_stop( )
{
	animation( ).external_sound_stop( );
}
using namespace luabind;

#pragma optimize("s",on)
void CTrader::script_register(lua_State* L)
{
	module(L)
		[
			class_<CTrader, CGameObject>("CTrader")
				.def(constructor<>( ))
		];
}
