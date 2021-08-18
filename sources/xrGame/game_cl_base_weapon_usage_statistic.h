#pragma once

#include "level_bullet_manager.h"
#include "game_base_kill_type.h"

struct BulletData
{
	CSharedString		FirerName;
	CSharedString		WeaponName;

	U16				HitRefCount;
	U16				HitResponds;

	bool			Removed;

	SBullet			Bullet;
	bool			operator	==		(u32 BulletID)	{return BulletID == Bullet.m_dwID;};
	bool			operator	!=		(u32 BulletID)	{return BulletID != Bullet.m_dwID;};

	BulletData		(CSharedString FName, CSharedString WName, SBullet* pBullet);
};

//DEF_VECTOR		(ABULLETS, BulletData);
using ABULLETS = xr_vector<BulletData>;
using ABULLETS_it = ABULLETS::iterator;

struct HitData
{
	Fvector3 Pos0;
	Fvector3 Pos1;
	
	S16		BoneID;
	CSharedString	BoneName;
	U16		TargetID;
	CSharedString	TargetName;
	u32		BulletID;
	bool	Deadly;

	bool	Completed;

	void			net_save			(CNetPacket* P);
	void			net_load			(CNetPacket* P);

	bool			operator	==		(u32 ID)	{return ID == BulletID;};
	bool			operator	!=		(u32 ID)	{return ID != BulletID;};
};

//DEF_VECTOR	(HITS_VEC, HitData);
using HITS_VEC = xr_vector<HitData>;
using HITS_VEC_it = HITS_VEC::iterator;

#define MAX_BASKET		34
struct Weapon_Statistic
{
	CSharedString		WName;
	CSharedString		InvName;
	u32				NumBought;
	//---------------------------
	u32				m_dwRoundsFired, m_dwRoundsFired_d;
	u32				m_dwBulletsFired, m_dwBulletsFired_d;
	u32				m_dwHitsScored, m_dwHitsScored_d;
	u32				m_dwKillsScored, m_dwKillsScored_d;
	//---------------------------
	u32				m_Basket[3][MAX_BASKET];

	u32				m_dwNumCompleted;
	HITS_VEC		m_Hits;

	Weapon_Statistic(const char* Name);
	~Weapon_Statistic();

	void			net_save			(CNetPacket* P);
	void			net_load			(CNetPacket* P);

	bool			FindHit				(u32 BulletID, HITS_VEC_it& Hit_it);
	bool			operator	==		(const char* name){int res = xr_strcmp(WName.c_str(), name);return	res	 == 0;}
};

//DEF_VECTOR		(WEAPON_STATS, Weapon_Statistic);
using WEAPON_STATS = xr_vector<Weapon_Statistic>;
using WEAPON_STATS_it = WEAPON_STATS::iterator;

struct Player_Statistic
{
	CSharedString		PName;
	
	u32				m_dwTotalShots;
	u32				m_dwTotalShots_d;
	//-----------------------------------------------
	u32				m_dwTotalAliveTime[3];
	int				m_dwTotalMoneyRound[3];
	u32				m_dwNumRespawned[3];
	U8				m_dwArtefacts[3];
	
	u32				m_dwSpecialKills	[3];//headshot, backstab, knifekill

	U8				m_dwCurrentTeam;

	WEAPON_STATS	aWeaponStats;
	//-----------------------------------------------
	u32				m_dwCurMoneyRoundDelta;

	Player_Statistic(const char* Name);
	~Player_Statistic();

	WEAPON_STATS_it	FindPlayersWeapon	(const char* WeaponName);

	void			net_save			(CNetPacket* P);
	void			net_load			(CNetPacket* P);

	bool			operator	==		(const char* name){int res = xr_strcmp(PName.c_str(), name);return	res	 == 0;}
};

//DEF_VECTOR	(PLAYERS_STATS, Player_Statistic);
using PLAYERS_STATS = xr_vector<Player_Statistic>;
using PLAYERS_STATS_it = PLAYERS_STATS::iterator;

struct Bullet_Check_Request
{
	u32	BulletID;
	S16 BoneID;
	bool Result;
	bool Processed;
	Bullet_Check_Request() : BulletID(0), Result(false) {};
	Bullet_Check_Request(u32 ID, S16 BID) : BulletID(ID), BoneID(BID), Result(false), Processed(false) {};
};

//DEF_VECTOR(BChR, Bullet_Check_Request);
using BChR = xr_vector<Bullet_Check_Request>;
using BChR_it = BChR::iterator;

struct Bullet_Check_Array
{
	u32	SenderID;

	BChR	Requests;
	U8		NumTrue;
	U8		NumFalse;

	bool			operator	==		(u32 ID){return	ID == SenderID;}
	bool			operator	!=		(u32 ID){return	ID != SenderID;}
	Bullet_Check_Array(u32 ID) : SenderID(ID) {Requests.clear(); NumTrue = 0; NumFalse = 0;};
	~Bullet_Check_Array () {Requests.clear_and_free();};
};

//DEF_VECTOR(BChA, Bullet_Check_Array);
using BChA = xr_vector<Bullet_Check_Array>;
using BChA_it = BChA::iterator;

struct WeaponUsageStatistic {

	bool			m_bCollectStatistic;
	bool			CollectData				() {return m_bCollectStatistic;};
	void			SetCollectData			(bool Collect);
	//-----------------------------------------------
	ABULLETS		ActiveBullets;
	//-----------------------------------------------
	PLAYERS_STATS	aPlayersStatistic;
	//-----------------------------------------------
	u32				m_dwTotalPlayersAliveTime[3];
	int				m_dwTotalPlayersMoneyRound[3];
	u32				m_dwTotalNumRespawns[3];
	//-----------------------------------------------
	u32				m_dwLastUpdateTime;
	u32				m_dwUpdateTimeDelta;
	//-----------------------------------------------
	WeaponUsageStatistic();
	~WeaponUsageStatistic();

	void				Clear			();

	PLAYERS_STATS_it					FindPlayer			(const char* PlayerName);
	bool								GetPlayer			(const char* PlayerName, PLAYERS_STATS_it& pPlayerI);
	void								ChangePlayerName	(const char* from, const char* to );

	bool								FindBullet			(u32 BulletID, ABULLETS_it& Bullet_it);
	void								RemoveBullet		(ABULLETS_it& Bullet_it);
	//-----------------------------------------------
	void				OnWeaponBought			(game_PlayerState* ps, const char* WeaponName);
	void				OnBullet_Fire			(SBullet* pBullet, const CCartridge& cartridge);
	void				OnBullet_Hit			(SBullet* pBullet, U16 TargetID, S16 element, Fvector3 HitLocation);
	void				OnBullet_Remove			(SBullet* pBullet);
	//-----------------------------------------------

	u32						m_dwLastRequestSenderID;

	BChA					m_Requests;

	void				OnBullet_Check_Request		(SHit* pHDS);
	void				OnBullet_Check_Result		(bool Result);
	//-----------------------------------------------
	void				Send_Check_Respond			();
	void				On_Check_Respond			(CNetPacket* P);

	void				OnPlayerKilled				(game_PlayerState* ps);
	void				OnPlayerSpawned				(game_PlayerState* ps);
	void				OnPlayerAddMoney			(game_PlayerState* ps, int MoneyAmount);
	void				OnPlayerBringArtefact		(game_PlayerState* ps);
	void				OnPlayerKillPlayer			(game_PlayerState* ps, KILL_TYPE KillType, SPECIAL_KILL_TYPE SpecialKillType);
	void				OnExplosionKill				(game_PlayerState* ps, const SHit& hit);
	//-----------------------------------------------
	void				Update						();
	void				OnUpdateRequest				(CNetPacket* P);
	void				OnUpdateRespond				(CNetPacket* P);
	//-----------------------------------------------
	string_path			mFileName;
};

struct Bullet_Check_Respond_True
{
	u32	BulletID;
	S16 BoneID;
};
