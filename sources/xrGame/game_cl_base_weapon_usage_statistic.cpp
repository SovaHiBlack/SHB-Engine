#include "stdafx.h"

#include "game_cl_base.h"
#include "Level.h"
#include "Weapon.h"
#include "alife_space.h"
#include "hit.h"
#include "Actor.h"
#include "Server.h"
#include "HUDManager.h"//
#include "game_cl_base_weapon_usage_statistic.h"

SBulletData::SBulletData(CSharedString FName, CSharedString WName, SBullet* pBullet)
{
	FirerName = FName;
	WeaponName = WName;
	Bullet = *pBullet;
	HitRefCount = 0;
	HitResponds = 0;
	Removed = false;
}

void SHitData::net_save(CNetPacket* P)
{
	P->w_vec3(Pos0);
	P->w_vec3(Pos1);
	P->w_stringZ(TargetName);
	P->w_stringZ(BoneName);
	P->w_s16(BoneID);
	P->w_u8(Deadly ? 1 : 0);
}

void SHitData::net_load(CNetPacket* P)
{
	P->r_vec3(Pos0);
	P->r_vec3(Pos1);
	P->r_stringZ(TargetName);
	P->r_stringZ(BoneName);
	P->r_s16(BoneID);
	Deadly = !!P->r_u8( );
	Completed = true;
}

SWeaponStatistic::SWeaponStatistic(const char* Name)
{
	WName = Name;
	InvName = NULL;
	NumBought = 0;

	m_dwRoundsFired = m_dwRoundsFired_d = 0;
	m_dwBulletsFired = m_dwBulletsFired_d = 0;
	m_dwHitsScored = m_dwHitsScored_d = 0;
	m_dwKillsScored = m_dwKillsScored_d = 0;

	m_Hits.clear( );
	m_dwNumCompleted = 0;
	ZeroMemory(m_Basket, sizeof(m_Basket));
}

SWeaponStatistic::~SWeaponStatistic( )
{
	m_Hits.clear_and_free( );
	ZeroMemory(m_Basket, sizeof(m_Basket));
}

void SWeaponStatistic::net_save(CNetPacket* P)
{
	m_dwRoundsFired_d = m_dwRoundsFired - m_dwRoundsFired_d;
	P->w_u32(m_dwRoundsFired_d);
	m_dwRoundsFired_d = m_dwRoundsFired;
	P->w_u32(m_dwBulletsFired_d);
	m_dwBulletsFired_d = 0;
	P->w_u32(m_dwHitsScored_d);
	m_dwHitsScored_d = 0;
	P->w_u32(m_dwKillsScored_d);
	m_dwKillsScored_d = 0;

	P->w_u32(m_dwNumCompleted);
	u32 i = 0;
	while (i < m_Hits.size( ))
	{
		SHitData& Hit = m_Hits[i];
		if (Hit.Completed)
		{
			Hit.net_save(P);
			//-------------
			Hit = m_Hits.back( );
			m_Hits.pop_back( );
			//-------------
			m_dwNumCompleted--;
		}
		else
		{
			i++;
		}
	}
}

void SWeaponStatistic::net_load(CNetPacket* P)
{
	m_dwRoundsFired += P->r_u32( );
	m_dwBulletsFired += P->r_u32( );
	m_dwHitsScored += P->r_u32( );
	m_dwKillsScored += P->r_u32( );
	u32 HitsSize = P->r_u32( );
	for (u32 i = 0; i < HitsSize; i++)
	{
		SHitData NewHit;
		NewHit.net_load(P);
		m_Hits.push_back(NewHit);
//		m_dwNumCompleted++;
	}
}

SPlayerStatistic::SPlayerStatistic(const char* Name)
{
	PName = Name;
	m_dwTotalShots = 0;
	m_dwTotalShots_d = 0;
	m_dwCurrentTeam = 0;

	ZeroMemory(m_dwTotalAliveTime, sizeof(m_dwTotalAliveTime));
	ZeroMemory(m_dwTotalMoneyRound, sizeof(m_dwTotalMoneyRound));
	ZeroMemory(m_dwNumRespawned, sizeof(m_dwNumRespawned));
	ZeroMemory(m_dwArtefacts, sizeof(m_dwArtefacts));
	ZeroMemory(m_dwSpecialKills, sizeof(m_dwSpecialKills));
}

SPlayerStatistic::~SPlayerStatistic( )
{
	aWeaponStats.clear_and_free( );
}

void SPlayerStatistic::net_save(CNetPacket* P)
{
	P->w_u32(m_dwTotalShots_d); m_dwTotalShots_d = 0;
	P->w_u32(aWeaponStats.size( ));
	for (u32 i = 0; i < aWeaponStats.size( ); i++)
	{
		SWeaponStatistic& WS = aWeaponStats[i];
		P->w_stringZ(WS.WName);
		WS.net_save(P);
	}
}

void SPlayerStatistic::net_load(CNetPacket* P)
{
	m_dwTotalShots += P->r_u32( );
	u32 NumWeapons = P->r_u32( );
	for (u32 i = 0; i < NumWeapons; i++)
	{
		CSharedString WName;
		P->r_stringZ(WName);
		SWeaponStatistic& WS = *(FindPlayersWeapon(*WName));
		WS.net_load(P);
	}
}

SWeaponUsageStatistic::SWeaponUsageStatistic( )
{
	Clear( );
	m_dwUpdateTimeDelta = 60000;
	m_bCollectStatistic = false;
}

void SWeaponUsageStatistic::Clear( )
{
	ActiveBullets.clear( );
	aPlayersStatistic.clear( );
	m_Requests.clear( );
	m_dwLastRequestSenderID = 0;

	ZeroMemory(m_dwTotalPlayersAliveTime, sizeof(m_dwTotalPlayersAliveTime));
	ZeroMemory(m_dwTotalPlayersMoneyRound, sizeof(m_dwTotalPlayersMoneyRound));
	ZeroMemory(m_dwTotalNumRespawns, sizeof(m_dwTotalNumRespawns));

	m_dwLastUpdateTime = Level( ).timeServer( );
	mFileName[0] = 0;
}

SWeaponUsageStatistic::~SWeaponUsageStatistic( )
{
	ActiveBullets.clear_and_free( );
	aPlayersStatistic.clear_and_free( );
	m_Requests.clear_and_free( );
	m_dwLastRequestSenderID = 0;
}

bool SWeaponUsageStatistic::GetPlayer(const char* PlayerName, PlayersStatisticVec_it& pPlayerI)
{
	pPlayerI = std::find(aPlayersStatistic.begin( ), aPlayersStatistic.end( ), PlayerName);
	if (pPlayerI == aPlayersStatistic.end( ) || !((*pPlayerI) == PlayerName))
	{
		return false;
	}

	return true;
}

PlayersStatisticVec_it SWeaponUsageStatistic::FindPlayer(const char* PlayerName)
{
	PlayersStatisticVec_it pPlayerI;
	if (!GetPlayer(PlayerName, pPlayerI))
	{
		aPlayersStatistic.push_back(SPlayerStatistic(PlayerName));
		pPlayerI = aPlayersStatistic.end( ) - 1;
	}

	return pPlayerI;
}

void SWeaponUsageStatistic::ChangePlayerName(const char* from, const char* to)
{
	if (!CollectData( ))
	{
		return;
	}

	PlayersStatisticVec_it pPlayerI = FindPlayer(from);
	pPlayerI->PName = to;
}

WeaponsStatisticVec_it SPlayerStatistic::FindPlayersWeapon(const char* WeaponName)
{
	WeaponsStatisticVec_it pWeaponI = std::find(aWeaponStats.begin( ), aWeaponStats.end( ), WeaponName);
	if (pWeaponI == aWeaponStats.end( ) || !((*pWeaponI) == WeaponName))
	{
		aWeaponStats.push_back(SWeaponStatistic(WeaponName));
		pWeaponI = aWeaponStats.end( ) - 1;
		pWeaponI->InvName = pSettings->r_string_wb(WeaponName, "inv_name");
	}

	return pWeaponI;
}

bool SWeaponUsageStatistic::FindBullet(u32 BulletID, BulletsDataVec_it& Bullet_It)
{
	Bullet_It = std::find(ActiveBullets.begin( ), ActiveBullets.end( ), BulletID);
	if (Bullet_It == ActiveBullets.end( ) || (*Bullet_It) != BulletID)
	{
		return false;
	}

	return true;
}

bool SWeaponStatistic::FindHit(u32 BulletID, HitsDataVec_it& Hit_it)
{
	Hit_it = std::find(m_Hits.begin( ), m_Hits.end( ), BulletID);
	if (Hit_it == m_Hits.end( ) || (*Hit_it) != BulletID)
	{
		return false;
	}

	return true;
}

void SWeaponUsageStatistic::RemoveBullet(BulletsDataVec_it& Bullet_it)
{
	if (!Bullet_it->Removed || Bullet_it->HitRefCount != Bullet_it->HitResponds)
	{
		return;
	}

	//-------------------------------------------------------------
	PlayersStatisticVec_it PlayerIt = FindPlayer(*(Bullet_it->FirerName));
	WeaponsStatisticVec_it WeaponIt = PlayerIt->FindPlayersWeapon(*(Bullet_it->WeaponName));
	HitsDataVec_it HitIt;
	if (WeaponIt->FindHit(Bullet_it->Bullet.m_dwID, HitIt))
	{
		HitIt->Completed = true;
		WeaponIt->m_dwNumCompleted++;
	}
	//-------------------------------------------------------------
	*Bullet_it = ActiveBullets.back( );
	ActiveBullets.pop_back( );
}

void SWeaponUsageStatistic::OnWeaponBought(game_PlayerState* ps, const char* WeaponName)
{
	if (!CollectData( ))
	{
		return;
	}

	if (!ps)
	{
		return;
	}

	PlayersStatisticVec_it PlayerIt = FindPlayer(ps->getName( ));
	WeaponsStatisticVec_it WeaponIt = PlayerIt->FindPlayersWeapon(WeaponName);
	WeaponIt->NumBought++;
	//-----------------------------------------------
	int BasketPos = 0;
	if (ps->money_for_round > 500)
	{
		BasketPos = (ps->money_for_round - 1) / 1000 + 1;
	}

	WeaponIt->m_Basket[ps->team][BasketPos]++;
}

void SWeaponUsageStatistic::OnBullet_Fire(SBullet* pBullet, const CCartridge& cartridge)
{
	if (!CollectData( ))
	{
		return;
	}

	if (!pBullet || !pBullet->flags.allow_sendhit)
	{
		return;
	}

	CObject* object_weapon = Level( ).Objects.net_Find(pBullet->weapon_id);
	if (!object_weapon)
	{
		return;
	}

	CObject* object_parent = Level( ).Objects.net_Find(pBullet->parent_id);
	if (!object_parent)
	{
		return;
	}

	CActor* pActor = smart_cast<CActor*>(object_parent);
	if (!pActor)
	{
		return;
	}
	//-----------------------------------------------------------------------------------
	PlayersStatisticVec_it PlayerIt = FindPlayer(*object_parent->cName( ));
	pBullet->m_dwID = PlayerIt->m_dwTotalShots++;
	PlayerIt->m_dwTotalShots_d++;
	WeaponsStatisticVec_it WeaponIt = PlayerIt->FindPlayersWeapon(*object_weapon->cNameSect( ));
	WeaponIt->m_dwRoundsFired = (++WeaponIt->m_dwBulletsFired) / cartridge.m_buckShot;
	WeaponIt->m_dwBulletsFired_d++;
	//-----------------------------------------------------------------------------------
	ActiveBullets.push_back(SBulletData(object_parent->cName( ), object_weapon->cNameSect( ), pBullet));

//	Msg("! OnBullet Fire ID[%d]", pBullet->m_dwID);
}

void SWeaponUsageStatistic::OnBullet_Hit(SBullet* pBullet, U16 TargetID, S16 element, Fvector3 HitLocation)
{
	if (!pBullet || !pBullet->flags.allow_sendhit)
	{
		return;
	}

//	Msg("! OnBullet Hit ID[%d]", pBullet->m_dwID);
	BulletsDataVec_it BulletIt;
	if (!FindBullet(pBullet->m_dwID, BulletIt))
	{
		return;
	}

	//-----------------------------------------------------
	PlayersStatisticVec_it PlayerIt = FindPlayer(*(BulletIt->FirerName));
	WeaponsStatisticVec_it WeaponIt = PlayerIt->FindPlayersWeapon(*(BulletIt->WeaponName));
	if (!BulletIt->HitRefCount++)
	{
		WeaponIt->m_dwHitsScored++;
		WeaponIt->m_dwHitsScored_d++;
		//---------------------------
		CObject* pTarget = Level( ).Objects.net_Find(TargetID);
		if (!pTarget)
		{
			return;
		}

		CActor* pActor = smart_cast<CActor*>(pTarget);
		if (!pActor)
		{
			return;
		}

		//---------------------------
		SBulletData& BD = *BulletIt;
		SHitData NewHit;
		//---------------------------
		NewHit.Completed = false;
		NewHit.Deadly = false;
		NewHit.BoneID = element;
		NewHit.TargetID = TargetID;
		NewHit.BulletID = BD.Bullet.m_dwID;
		NewHit.Pos0 = BD.Bullet.pos;
		NewHit.Pos1 = HitLocation;
		NewHit.TargetName = pTarget->cName( );
		NewHit.BoneName = smart_cast<CKinematics*>(pTarget->Visual( ))->LL_BoneName_dbg(element);
		//---------------------------
		WeaponIt->m_Hits.push_back(NewHit);
	}
}

void SWeaponUsageStatistic::OnBullet_Remove(SBullet* pBullet)
{
	if (!pBullet || !pBullet->flags.allow_sendhit)
	{
		return;
	}

	BulletsDataVec_it BulletIt;
	if (!FindBullet(pBullet->m_dwID, BulletIt))
	{
		return;
	}

//	Msg("! Bullet Removed ID[%d]", BulletIt->Bullet.m_dwID);
	BulletIt->Removed = true;
	RemoveBullet(BulletIt);
}

void SWeaponUsageStatistic::OnBullet_Check_Request(SHit* pHDS)
{
	if (!pHDS || OnClient( ))
	{
		return;
	}

	S16 BoneID = pHDS->bone( );
	u32 BulletID = pHDS->BulletID;
	u32 SenderID = pHDS->SenderID;

	BulletsCheckArrayVec_it pSenderI = std::find(m_Requests.begin( ), m_Requests.end( ), SenderID);
	if (pSenderI == m_Requests.end( ) || (*pSenderI) != SenderID)
	{
		m_Requests.push_back(SBulletCheckArray(SenderID));
		pSenderI = m_Requests.end( ) - 1;
	}

	(*pSenderI).Requests.push_back(SBulletCheckRequest(BulletID, BoneID));
	m_dwLastRequestSenderID = SenderID;
}

void SWeaponUsageStatistic::OnBullet_Check_Result(bool Result)
{
	if (OnClient( ))
	{
		return;
	}

	if (m_dwLastRequestSenderID)
	{
		BulletsCheckArrayVec_it pSenderI = std::find(m_Requests.begin( ), m_Requests.end( ), m_dwLastRequestSenderID);
		if (pSenderI != m_Requests.end( ) && (*pSenderI) == m_dwLastRequestSenderID)
		{
			(*pSenderI).Requests.back( ).Result = Result;
			(*pSenderI).Requests.back( ).Processed = true;
			if (Result)
			{
				(*pSenderI).NumTrue++;
			}
			else
			{
				(*pSenderI).NumFalse++;
			}
		}
		else
		{
			Msg("! Warning can't Find Check!");
			R_ASSERT(0);
		}

		m_dwLastRequestSenderID = 0;
	}
}

void SWeaponUsageStatistic::Send_Check_Respond( )
{
	if (!OnServer( ))
	{
		return;
	}

	CNetPacket P;
	string1024 STrue, SFalse;
	for (u32 i = 0; i < m_Requests.size( ); i++)
	{
		SBulletCheckArray& BChA_Request = m_Requests[i];
		if (BChA_Request.Requests.empty( ))
		{
			continue;
		}

		SBulletCheckRespondTrue* pSTrue = (SBulletCheckRespondTrue*) STrue;
		u32* pSFalse = (u32*) SFalse;
		//-----------------------------------------------------
		u32 NumFalse = 0;
		u32 NumTrue = 0;
		u32 j = 0;
		while (j < BChA_Request.Requests.size( ))
		{
			SBulletCheckRequest& curBChR = BChA_Request.Requests[j];
			if (!curBChR.Processed)
			{
				j++;
				continue;
			}
			else
			{
				if (curBChR.Result)
				{
					pSTrue->BulletID = curBChR.BulletID;
					pSTrue->BoneID = curBChR.BoneID;
					pSTrue++;
					NumTrue++;
				}
				else
				{
					*(pSFalse++) = curBChR.BulletID;
					NumFalse++;
				}

				//-----------------------------------------------------
				*(BChA_Request.Requests.begin( ) + j) = BChA_Request.Requests.back( );
				BChA_Request.Requests.pop_back( );
			}
		}
		//-----------------------------------------------------
		P.w_begin(M_BULLET_CHECK_RESPOND);
//		Msg("%d-%d || %d-%d", NumFalse, BChA_Request.NumFalse, NumTrue, BChA_Request.NumTrue);
		P.w_u8(BChA_Request.NumFalse);		BChA_Request.NumFalse = 0;
		P.w_u8(BChA_Request.NumTrue);		BChA_Request.NumTrue = 0;

		if ((char*) pSFalse != (char*) SFalse)
		{
			P.w(SFalse, u32((char*) pSFalse - (char*) SFalse));
		}

		if ((char*) pSTrue != (char*) STrue)
		{
			P.w(STrue, u32((char*) pSTrue - (char*) STrue));
		}

		//-----------------------------------------------------
		ClientID ClID; ClID.set(BChA_Request.SenderID);
		if (Level( ).Server)
		{
			Level( ).Server->SendTo(ClID, P);
		}
	}
}

void SWeaponUsageStatistic::On_Check_Respond(CNetPacket* P)
{
	if (!P)
	{
		return;
	}

	U8 NumFalse = P->r_u8( );
	U8 NumTrue = P->r_u8( );

	U8 i;
	BulletsDataVec_it BulletIt;
	for (i = 0; i < NumFalse; i++)
	{
		u32 BulletID = P->r_u32( );
		if (!FindBullet(BulletID, BulletIt))
		{
			Msg("! Warning: No bullet found! ID[%d]", BulletID);
			continue;
		}

		BulletIt->HitResponds++;
		RemoveBullet(BulletIt);
	}

	for (i = 0; i < NumTrue; i++)
	{
		u32 BulletID = P->r_u32( );
		S16 BoneID = P->r_s16( );
		if (!FindBullet(BulletID, BulletIt))
		{
			Msg("! Warning: No bullet found! ID[%d]", BulletID);
			continue;
		}

		BulletIt->HitResponds++;

		//---------------------------------------------------------------
		PlayersStatisticVec_it PlayerIt = FindPlayer(*(BulletIt->FirerName));
		WeaponsStatisticVec_it WeaponIt = PlayerIt->FindPlayersWeapon(*(BulletIt->WeaponName));
		(*WeaponIt).m_dwKillsScored++;
		(*WeaponIt).m_dwKillsScored_d++;

		HitsDataVec_it HitIt;
		if (WeaponIt->FindHit(BulletID, HitIt))
		{
			SHitData& HData = *HitIt;
			HData.Deadly = true;
			HData.BoneID = BoneID;
			CObject* pObj = Level( ).Objects.net_Find(HData.TargetID);

			if (pObj)
			{
				HData.BoneName = smart_cast<CKinematics*>(pObj->Visual( ))->LL_BoneName_dbg(BoneID);
			}
		}
		//---------------------------------------------------------------
		RemoveBullet(BulletIt);
	}
}

void SWeaponUsageStatistic::OnPlayerBringArtefact(game_PlayerState* ps)
{
	if (!CollectData( ))
	{
		return;
	}

	if (!ps)
	{
		return;
	}

	SPlayerStatistic& PlayerStat = *(FindPlayer(ps->getName( )));

	PlayerStat.m_dwArtefacts[ps->team]++;
}

void SWeaponUsageStatistic::OnPlayerSpawned(game_PlayerState* ps)
{
	if (!CollectData( ))
	{
		return;
	}

	if (!ps)
	{
		return;
	}

	SPlayerStatistic& PlayerStat = *(FindPlayer(ps->getName( )));
	PlayerStat.m_dwNumRespawned[ps->team]++;
	PlayerStat.m_dwCurMoneyRoundDelta = 0;
	m_dwTotalNumRespawns[ps->team]++;
	PlayerStat.m_dwCurrentTeam = ps->team;
}

void SWeaponUsageStatistic::OnPlayerAddMoney(game_PlayerState* ps, int MoneyAmount)
{
	if (!CollectData( ))
	{
		return;
	}

	if (!ps || MoneyAmount <= 0)
	{
		return;
	}

	SPlayerStatistic& PlayerStat = *(FindPlayer(ps->getName( )));
	PlayerStat.m_dwCurMoneyRoundDelta += MoneyAmount;
}

void SWeaponUsageStatistic::OnPlayerKillPlayer(game_PlayerState* ps, KILL_TYPE KillType, SPECIAL_KILL_TYPE SpecialKillType)
{
	if (!CollectData( ))
	{
		return;
	}

	if (!ps)
	{
		return;
	}

	SPlayerStatistic& PlayerStat = *(FindPlayer(ps->getName( )));

//.	m_dwSpecialKills[0];//headshot, backstab, knifekill
	switch (SpecialKillType)
	{
		case SKT_HEADSHOT:
		{
			PlayerStat.m_dwSpecialKills[0]++;
		}
		break;
		case SKT_BACKSTAB:
		{
			PlayerStat.m_dwSpecialKills[1]++;
		}
		break;
		case SKT_KNIFEKILL:
		{
			PlayerStat.m_dwSpecialKills[2]++;
		}
		break;
	}
}

void SWeaponUsageStatistic::OnExplosionKill(game_PlayerState* ps, const SHit& hit)
{
	if (!CollectData( ))
	{
		return;
	}

	if (!ps)
	{
		return;
	}

	if (!OnServer( ))
	{
		return;
	}

	CObject* killer = hit.who;
	if (!killer)
	{
		return;
	}

	U16 killer_id = hit.whoID;
	game_PlayerState* killerPS = Game( ).GetPlayerByGameID(killer_id);
	SPlayerStatistic& PlayerStatKiller = *(FindPlayer(killerPS->getName( )));

	CObject* weapon_object = Level( ).Objects.net_Find(hit.weaponID);
	WeaponsStatisticVec_it WeaponIt = PlayerStatKiller.FindPlayersWeapon(weapon_object->cNameSect( ).c_str( ));
	WeaponIt->m_dwHitsScored++;
	WeaponIt->m_dwKillsScored++;

	SHitData NewHit;
	//---------------------------
	NewHit.Completed = true;
	NewHit.Deadly = true;
	NewHit.BoneID = hit.boneID;
	NewHit.TargetID = ps->GameID;
	NewHit.BulletID = 0;
	NewHit.Pos0 = killer->Position( );
	NewHit.Pos1 = weapon_object->Position( );
	NewHit.TargetName = ps->getName( );
	NewHit.BoneName = 0;
	//---------------------------
	WeaponIt->m_Hits.push_back(NewHit);
}

void SWeaponUsageStatistic::OnPlayerKilled(game_PlayerState* ps)
{
	if (!CollectData( ))
	{
		return;
	}

	if (!ps)
	{
		return;
	}

	u32 dwAliveTime = ps->DeathTime - ps->RespawnTime;

	m_dwTotalPlayersAliveTime[ps->team] += dwAliveTime;

	SPlayerStatistic& PlayerStat = *(FindPlayer(ps->getName( )));
	PlayerStat.m_dwTotalAliveTime[ps->team] += dwAliveTime;

	PlayerStat.m_dwTotalMoneyRound[ps->team] += PlayerStat.m_dwCurMoneyRoundDelta;
	m_dwTotalPlayersMoneyRound[ps->team] += PlayerStat.m_dwCurMoneyRoundDelta;
}

void SWeaponUsageStatistic::Update( )
{
	if (!CollectData( ))
	{
		return;
	}

	if (Level( ).timeServer( ) < (m_dwLastUpdateTime + m_dwUpdateTimeDelta))
	{
		//---------------------------------------------
		m_dwLastUpdateTime = Level( ).timeServer( );
		//---------------------------------------------
		CNetPacket P;
		P.w_begin(M_STATISTIC_UPDATE);
		P.w_u32(Level( ).timeServer( ));
		Level( ).Send(P);
	}
}

void SWeaponUsageStatistic::OnUpdateRequest(CNetPacket*)
{
	if (aPlayersStatistic.empty( ))
	{
		return;
	}

	SPlayerStatistic& PS = aPlayersStatistic.front( );
	//-------------------------------------------------
	CNetPacket P;
	P.w_begin(M_STATISTIC_UPDATE_RESPOND);
	//-------------------------------------------------
	P.w_stringZ(PS.PName);
	PS.net_save(&P);
	//-------------------------------------------------
	Level( ).Send(P);
}

void SWeaponUsageStatistic::OnUpdateRespond(CNetPacket* P)
{
	if (!P)
	{
		return;
	}

	CSharedString PName;
	P->r_stringZ(PName);
	SPlayerStatistic& PS = *(FindPlayer(*PName));
	PS.net_load(P);
}

void SWeaponUsageStatistic::SetCollectData(bool Collect)
{
	if (Collect && !m_bCollectStatistic)
	{
		Clear( );
	}

	m_bCollectStatistic = Collect;
}
