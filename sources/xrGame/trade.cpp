#include "stdafx.h"

#include "Trade.h"
#include "actor.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/trader/ai_trader.h"
#include "Artefact.h"
#include "inventory.h"
#include "xrmessages.h"
#include "character_info.h"
#include "relation_registry.h"
#include "level.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
#include "GameObject_space.h"
#include "trade_parameters.h"

class CInventoryOwner;

//////////////////////////////////////////////////////////////////////////////////////////
// CTrade class //////////////////////////////////////////////////////////////////////////
CTrade::CTrade(CInventoryOwner *p_io) 
{
	TradeState = false;
	m_dwLastTradeTime	= 0;
	pPartner.Set(TT_NONE,0,0);

	m_bNeedToUpdateArtefactTasks = false;

	// Заполнить pThis
	CAI_Trader *pTrader;
	CActor *pActor;
	CAI_Stalker *pStalker;

	// Определяем потомка этого экземпляра класса
	pTrader = smart_cast<CAI_Trader *>(p_io);
	if (pTrader) 
		pThis.Set(TT_TRADER, pTrader, p_io);
	else 
	{
		pActor = smart_cast<CActor *>(p_io);
		if (pActor) 
			pThis.Set(TT_ACTOR, pActor, p_io);
		else 
		{
			pStalker = smart_cast<CAI_Stalker *>(p_io);
			if (pStalker) pThis.Set(TT_STALKER, pStalker, p_io);
		}
	}
}

CTrade::~CTrade()
{
	
}

void CTrade::RemovePartner()
{
	pPartner.Set(TT_NONE,0,0);
}
//// предложение торговли
//void CTrade::Communicate() 
//{
//	// Вывести приветствие
////	Msg("--TRADE::----------------------------------------------");
////	Msg("--TRADE::          TRADE ACIVATED                      ");
////	Msg("--TRADE::----------------------------------------------");
////	Msg("--TRADE:: - Hello, my name is [%s]", *pThis.base->cName());
////	Msg("--TRADE::   Wanna trade with me?" );
//
//	if (pPartner.inv_owner->GetTrade()->OfferTrade(pThis)) { 
//		StartTrade();
//	}
//
//}
//
bool CTrade::SetPartner(CEntity *p)
{
	CAI_Trader *pTrader;
	CActor *pActor;
	CAI_Stalker *pStalker;

	pTrader = smart_cast<CAI_Trader *>(p);
	if (pTrader && (pTrader != pThis.base))  
		pPartner.Set(TT_TRADER, pTrader, pTrader);
	else 
	{
		pActor = smart_cast<CActor *>(p);
		if (pActor && (pActor != pThis.base)) 
			pPartner.Set(TT_ACTOR, pActor, pActor);
		else 
		{
			pStalker = smart_cast<CAI_Stalker *>(p);
			if (pStalker && (pStalker != pThis.base)) 
				pPartner.Set(TT_STALKER, pStalker, pStalker);
			else return false;
		}
	}
	return true;
}


//// Man предлагает торговать 
//// возвращает true, если данный trader готов торговать с man
//// т.е. принятие торговли
//bool CTrade::OfferTrade(SInventoryOwner man)
//{
//	StartTrade();
//	pPartner.Set(man.type,man.base,man.inv_owner); 
//	
//	string64	s;
//	switch (pPartner.type) 
//	{
//		case TT_TRADER: strcpy(s, "trader"); break;
//		case TT_STALKER: 
//		case TT_ACTOR: strcpy(s, "stalker"); break;
//	}
//	
//	
//	switch (pPartner.inv_owner->m_tRank) 
//	{
//		case ALife::eStalkerRankNone: strcpy(s,"NO_RANK"); break;
//		case ALife::eStalkerRankNovice: strcpy(s,"NOVICE"); break;
//		case ALife::eStalkerRankExperienced: strcpy(s,"EXPERIENCED"); break;
//		case ALife::eStalkerRankVeteran: strcpy(s,"VETERAN"); break;
//		case ALife::eStalkerRankMaster: strcpy(s,"MASTER"); break;
//		case ALife::eStalkerRankDummy: strcpy(s,"DUMMY"); break;
//	}
//
//	return true;
//}
//

void CTrade::StartTrade()
{
	TradeState = true;
	m_dwLastTradeTime =  Level().timeServer();
	m_bNeedToUpdateArtefactTasks = false;

//	if (pThis.type == TT_TRADER) smart_cast<CAI_Trader*>(pThis.base)->OnStartTrade();
}

void CTrade::StartTrade(CInventoryOwner* pInvOwner)
{
	SetPartner(smart_cast<CEntity*>(pInvOwner));
	StartTrade();
}
void CTrade::TradeCB(bool bStart)
{
	if(bStart){
		if (pThis.type == TT_TRADER) smart_cast<CAI_Trader*>(pThis.base)->OnStartTrade();
	}else
		if (pThis.type == TT_TRADER) smart_cast<CAI_Trader*>(pThis.base)->OnStopTrade();

}

void CTrade::OnPerformTrade(u32 money_get, u32 money_put)
{
	if (pThis.type == TT_TRADER) 
		smart_cast<CAI_Trader*>(pThis.base)->callback(GameObject::eTradePerformTradeOperation)(money_get, money_put);
}

void CTrade::StopTrade()
{
	TradeState = false;
	m_dwLastTradeTime = 0;
//	Msg("--TRADE:: [%s]: Trade stopped...",*pThis.base->cName());

	CAI_Trader* pTrader = NULL;
	if (pThis.type == TT_TRADER)
	{
		//pTrader = smart_cast<CAI_Trader*>(pThis.base);
		//pTrader->OnStopTrade();
	}
	else if (pPartner.type == TT_TRADER)
	{
		pTrader = smart_cast<CAI_Trader*>(pPartner.base);
	}

	RemovePartner();
}

void CTrade::UpdateTrade()
{
}

bool CTrade::CanTrade( )
{
	CEntity* pEntity;

	m_nearest.clear_not_free( );
	Level( ).ObjectSpace.GetNearest(m_nearest, pThis.base->Position( ), 2.f, NULL);
	if (!m_nearest.empty( ))
	{
		for (u32 i = 0, n = m_nearest.size( ); i < n; ++i)
		{
			// Может ли объект торговать
			pEntity = smart_cast<CEntity*>(m_nearest[i]);
			if (pEntity && !pEntity->g_Alive( )) return false;
			if (SetPartner(pEntity)) break;
		}
	}

	if (!pPartner.base) return false;

	// Объект рядом
	float dist = pPartner.base->Position( ).distance_to(pThis.base->Position( ));
	if (dist < 0.5f || dist > 4.5f)
	{
		RemovePartner( );
		return false;
	}

	// Объект смотрит на меня
	float yaw, pitch;
	float yaw2, pitch2;

	pThis.base->Direction( ).getHP(yaw, pitch);
	pPartner.base->Direction( ).getHP(yaw2, pitch2);
	yaw = angle_normalize(yaw);
	yaw2 = angle_normalize(yaw2);

	float Res = rad2deg(_abs(yaw - yaw2) < PI ? _abs(yaw - yaw2) :
						PI_MUL_2 - _abs(yaw - yaw2));
	if (Res < 165.f || Res > 195.f)
	{
		RemovePartner( );
		return false;
	}

	return true;
}

void CTrade::TransferItem(CInventoryItem* pItem, bool bBuying)
{
	// сумма сделки учитывая ценовой коэффициент
	// актер цену не говорит никогда, все делают за него
	u32 dwTransferMoney = GetItemPrice(pItem, bBuying);

	if (bBuying)
	{
		pPartner.inv_owner->on_before_sell(pItem);
		pThis.inv_owner->on_before_buy(pItem);
	}
	else
	{
		pThis.inv_owner->on_before_sell(pItem);
		pPartner.inv_owner->on_before_buy(pItem);
	}

	CGameObject* O1 = smart_cast<CGameObject*>(pPartner.inv_owner);
	CGameObject* O2 = smart_cast<CGameObject*>(pThis.inv_owner);

	if (!bBuying)
		swap(O1, O2);

	NET_Packet				P;
	O1->u_EventGen(P, GE_TRADE_SELL, O1->ID( ));
	P.w_u16(pItem->object( ).ID( ));
	O1->u_EventSend(P);

	if (bBuying)
		pPartner.inv_owner->set_money(pPartner.inv_owner->get_money( ) + dwTransferMoney, false);
	else
		pThis.inv_owner->set_money(pThis.inv_owner->get_money( ) + dwTransferMoney, false);

	// взять у партнера
	O2->u_EventGen(P, GE_TRADE_BUY, O2->ID( ));
	P.w_u16(pItem->object( ).ID( ));
	O2->u_EventSend(P);

	if (bBuying)
		pThis.inv_owner->set_money(pThis.inv_owner->get_money( ) - dwTransferMoney, false);
	else
		pPartner.inv_owner->set_money(pPartner.inv_owner->get_money( ) - dwTransferMoney, false);


	CAI_Trader* pTrader = NULL;

	if (pThis.type == TT_TRADER && bBuying)
	{
		CArtefact* pArtefact = smart_cast<CArtefact*>(pItem);
		if (pArtefact)
		{
			pTrader = smart_cast<CAI_Trader*>(pThis.base);
			m_bNeedToUpdateArtefactTasks |= pTrader->BuyArtefact(pArtefact);
		}
	}

	if ((pPartner.type == TT_ACTOR) || (pThis.type == TT_ACTOR))
	{
		bool bDir = (pThis.type != TT_ACTOR) && bBuying;
		Actor( )->callback(GameObject::eTradeSellBuyItem)(pItem->object( ).lua_game_object( ), bDir, dwTransferMoney);
	}
}

CInventory& CTrade::GetTradeInv(SInventoryOwner owner)
{
	R_ASSERT(TT_NONE != owner.type);

	//return ((TT_TRADER == owner.type) ? (*owner.inv_owner->m_trade_storage) : (owner.inv_owner->inventory()));
	return owner.inv_owner->inventory( );
}

CTrade* CTrade::GetPartnerTrade( )
{
	return pPartner.inv_owner->GetTrade( );
}

CInventory* CTrade::GetPartnerInventory( )
{
	return &GetTradeInv(pPartner);
}

CInventoryOwner* CTrade::GetPartner( )
{
	return pPartner.inv_owner;
}

u32	CTrade::GetItemPrice(PIItem pItem, bool b_buying)
{
	CArtefact* pArtefact = smart_cast<CArtefact*>(pItem);

	// computing base_cost
	float					base_cost;
	if (pArtefact && (pThis.type == TT_ACTOR) && (pPartner.type == TT_TRADER))
	{
		CAI_Trader* pTrader = smart_cast<CAI_Trader*>(pPartner.inv_owner);
		VERIFY(pTrader);
		base_cost = (float) pTrader->ArtefactPrice(pArtefact);
	}
	else
		base_cost = (float) pItem->Cost( );

	// computing condition factor
	// for "dead" weapon we use 10% from base cost, for "good" weapon we use full base cost
	float					condition_factor = powf(pItem->GetCondition( ) * 0.9f + .1f, 0.75f);

	// computing relation factor
	float					relation_factor;

	CHARACTER_GOODWILL		attitude = RELATION_REGISTRY( ).GetAttitude(pPartner.inv_owner, pThis.inv_owner);

	if (NO_GOODWILL == attitude)
		relation_factor = 0.f;
	else
		relation_factor = float(attitude + 1000.f) / 2000.f;

	clamp(relation_factor, 0.f, 1.f);

	const SInventoryOwner* _partner = 0;
	bool					buying = true;
	bool					is_actor = (pThis.type == TT_ACTOR) || (pPartner.type == TT_ACTOR);
	if (is_actor)
	{
//.		buying				= (pPartner.type == TT_ACTOR);
		buying = b_buying;
		_partner = &(buying ? pThis : pPartner);
	}
	else
	{
  // rare case
		_partner = &pPartner;
	}
//.	const SInventoryOwner	&partner = *_partner;

	// computing action factor
	const CTradeFactors* p_trade_factors;


	if (buying)
	{
		if (!pThis.inv_owner->trade_parameters( ).enabled(CTradeParameters::action_buy(0), pItem->object( ).cNameSect( ))) return 0;
		p_trade_factors = &pThis.inv_owner->trade_parameters( ).factors(CTradeParameters::action_buy(0), pItem->object( ).cNameSect( ));
	}
	else
	{
		if (!pThis.inv_owner->trade_parameters( ).enabled(CTradeParameters::action_sell(0), pItem->object( ).cNameSect( ))) return 0;
		p_trade_factors = &pThis.inv_owner->trade_parameters( ).factors(CTradeParameters::action_sell(0), pItem->object( ).cNameSect( ));
	}
	const CTradeFactors& trade_factors = *p_trade_factors;

	float					action_factor;
	if (trade_factors.friend_factor( ) <= trade_factors.enemy_factor( ))
		action_factor =
		trade_factors.friend_factor( ) +
		(
			trade_factors.enemy_factor( ) -
			trade_factors.friend_factor( )
			) *
		(1.f - relation_factor);
	else
		action_factor =
		trade_factors.enemy_factor( ) +
		(
			trade_factors.friend_factor( ) -
			trade_factors.enemy_factor( )
			) *
		relation_factor;

	clamp(
		action_factor,
		_min(trade_factors.enemy_factor( ), trade_factors.friend_factor( )),
		_max(trade_factors.enemy_factor( ), trade_factors.friend_factor( ))
	);

	// computing deficit_factor
#if 0
	float					deficit_factor = partner.inv_owner->deficit_factor(pItem->object( ).cNameSect( ));
#else
	float					deficit_factor = 1.f;
#endif

	// total price calculation
	u32						result =
		iFloor(
			base_cost *
			condition_factor *
			action_factor *
			deficit_factor
		);

	return					(result);
}
