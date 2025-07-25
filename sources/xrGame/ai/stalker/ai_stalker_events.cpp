////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_events.cpp
//	Created 	: 26.02.2003
//  Modified 	: 26.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Events handling for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Stalker.h"
#include "../../PDA.h"
#include "../../Inventory.h"
#include "../../xrmessages.h"
#include "../../ShootingObject.h"
#include "../../../xrNetServer/net_utils.h"
#include "../../level.h"
#include "../../ai_monster_space.h"

using namespace StalkerSpace;
using namespace MonsterSpace;

#define SILENCE

void CStalker::OnEvent		(CNetPacket& P, u16 type)
{
	inherited::OnEvent			(P,type);
	CInventoryOwner::OnEvent	(P,type);

	switch (type)
	{
		case GE_TRADE_BUY :
		case GE_OWNERSHIP_TAKE : {

			u16			id;
			P.r_u16		(id);
			CObject		*O = Level().Objects.net_Find	(id);

			R_ASSERT	(O);

#ifndef SILENCE
			Msg("Trying to take - %s (%d)", *O->cName(),O->ID());
#endif
			CGameObject	*_O = smart_cast<CGameObject*>(O);
			if (inventory().CanTakeItem(smart_cast<CInventoryItem*>(_O))) { //GetScriptControl()
				O->H_SetParent(this);
				inventory().Take(_O,true, false);
				if (!inventory().ActiveItem() && GetScriptControl() && smart_cast<CShootingObject*>(O))
					CObjectHandler::set_goal	(eObjectActionIdle,_O);

				on_after_take			(_O);
#ifndef SILENCE
				Msg("TAKE - %s (%d)", *O->cName(),O->ID());
#endif
			}
			else {
//				DropItemSendMessage(O);
				CNetPacket				P;
				u_EventGen				(P,GE_OWNERSHIP_REJECT,ID());
				P.w_u16					(u16(O->ID()));
				u_EventSend				(P);

#ifndef SILENCE
				Msg("TAKE - can't take! - Dropping for valid server information %s (%d)", *O->cName(),O->ID());
#endif
			}
			break;
		}
		case GE_TRADE_SELL :
		case GE_OWNERSHIP_REJECT : {
			u16 id;
			P.r_u16		(id);
			CObject		*O = Level().Objects.net_Find(id);

#pragma todo("Dima to Oles : how can this happen?")
			if (!O)
				break;

			bool just_before_destroy	= !P.r_eof() && P.r_u8();
			O->SetTmpPreDestroy				(just_before_destroy);

			if (inventory().DropItem(smart_cast<CGameObject*>(O)) && !O->getDestroy()) {
				O->H_SetParent	(0, just_before_destroy);
				feel_touch_deny	(O,2000);
			}

			break;
		}
	}
}

void CStalker::feel_touch_new				(CObject* O)
{
//	Msg					("FEEL_TOUCH::NEW : %s",*O->cName());
	if (!g_Alive())		return;
	if (Remote())		return;
	if ((O->spatial.type | STYPE_VISIBLEFORAI) != O->spatial.type) return;

	// Now, test for game specific logical objects to minimize traffic
	CInventoryItem		*I	= smart_cast<CInventoryItem*>	(O);

	if (!wounded() && !critically_wounded() && I && I->useful_for_NPC() && can_take(I)) {
#ifndef SILENCE
		Msg("Taking item %s (%d)!",*I->cName(),I->ID());
#endif
		CNetPacket		P;
		u_EventGen		(P,GE_OWNERSHIP_TAKE,ID());
		P.w_u16			(u16(I->object().ID()));
		u_EventSend		(P);
	}
}

void CStalker::DropItemSendMessage(CObject *O)
{
	if (!O || !O->H_Parent() || (this != O->H_Parent()))
		return;

#ifndef SILENCE
	Msg("Dropping item!");
#endif
	// We doesn't have similar weapon - pick up it
	CNetPacket				P;
	u_EventGen				(P,GE_OWNERSHIP_REJECT,ID());
	P.w_u16					(u16(O->ID()));
	u_EventSend				(P);
}

void CStalker::UpdateAvailableDialogs(CPhraseDialogManager* partner)
{
/*	m_AvailableDialogs.clear();
	m_CheckedDialogs.clear();

	if(CInventoryOwner::m_known_info_registry->registry().objects_ptr())
	{
		for(KNOWN_INFO_VECTOR::const_iterator it = CInventoryOwner::m_known_info_registry->registry().objects_ptr()->begin();
			CInventoryOwner::m_known_info_registry->registry().objects_ptr()->end() != it; ++it)
		{
			//���������� ������� ���������� � ������� �� ��������
			CInfoPortion info_portion;
			info_portion.Load((*it).id);

			for(u32 i = 0; i<info_portion.DialogNames().size(); i++)
				AddAvailableDialog(*info_portion.DialogNames()[i], partner);
		}
	}
*/
	CAI_PhraseDialogManager::UpdateAvailableDialogs(partner);
}
