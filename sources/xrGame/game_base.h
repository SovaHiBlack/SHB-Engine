#pragma once

#include "game_base_space.h"
#include "script_export_space.h"
#include "ALife_space.h"

#pragma pack(push,1)
struct	game_PlayerState;//fw
class	CNetPacket;

struct		RPoint
{
	Fvector3	P;
	Fvector3 A;
	u32		TimeToUnfreeze;
	bool	Blocked;
	u32		BlockedByID;
	u32		BlockTime;
	RPoint( )
	{
		P.set(0.0f, 0.0f, 0.0f);
		A.set(0.0f, 0.0f, 0.0f);
		TimeToUnfreeze = 0;
		Blocked = false;
	}

	bool	operator ==		(const u32& ID)	const			{ return (Blocked && BlockedByID == ID);		}
	DECLARE_SCRIPT_REGISTER_FUNCTION_STRUCT
};

add_to_type_list(RPoint)
#undef script_type_list
#define script_type_list save_type_list(RPoint)

struct	game_PlayerState
{
	string64	name;
	unsigned char			team;
	
	signed short			m_iRivalKills;
	signed short			m_iSelfKills;
	signed short			m_iTeamKills;
	signed short			m_iKillsInRowCurr;
	signed short			m_iKillsInRowMax;
	signed short			m_iDeaths;
	int			money_for_round;

	float		experience_Real;
	float		experience_New;
	float		experience_D;
	unsigned char			rank;
	unsigned char			af_count;
	unsigned short			flags__;

	unsigned short			ping;

	unsigned short			GameID;

	unsigned short			lasthitter;
	unsigned short			lasthitweapon;
	signed char			skin;
	u32			RespawnTime;
	u32			DeathTime;
	signed short			money_delta;
	unsigned char			m_bCurrentVoteAgreed;
//	DEF_DEQUE	(OLD_GAME_ID, unsigned short);
	using OLD_GAME_ID = xr_deque<unsigned short>;
	using OLD_GAME_ID_it = OLD_GAME_ID::iterator;
	OLD_GAME_ID	mOldIDs;
	int			money_added;
	bool		m_bPayForSpawn;
	u32			m_online_time;

public:
					game_PlayerState		();
					~game_PlayerState		();
	virtual void	clear					();
			bool	testFlag				(unsigned short f) const;
			void	setFlag					(unsigned short f);
			void	resetFlag				(unsigned short f);
			const char* getName					(){return name;}
			void	setName					(const char* s){strcpy(name,s);}
			void	SetGameID				(unsigned short NewID);
			bool	HasOldID				(unsigned short ID);
			bool	IsSkip					() const {return testFlag(GAME_PLAYER_FLAG_SKIP);}
			
			signed short		frags					() const {return m_iRivalKills - m_iSelfKills - m_iTeamKills;}

	virtual void	net_Export				(CNetPacket& P, BOOL Full = FALSE);
	virtual void	net_Import				(CNetPacket& P);
	//---------------------------------------
	
//	DEF_VECTOR(PLAYER_ITEMS_LIST, unsigned short);
	using PLAYER_ITEMS_LIST = xr_vector<unsigned short>;
	using PLAYER_ITEMS_LIST_it = PLAYER_ITEMS_LIST::iterator;

	PLAYER_ITEMS_LIST	pItemList;

//	DEF_VECTOR(SPAWN_POINTS_LIST, signed short);
	using SPAWN_POINTS_LIST = xr_vector<signed short>;
	using SPAWN_POINTS_LIST_it = SPAWN_POINTS_LIST::iterator;

	SPAWN_POINTS_LIST	pSpawnPointsList;
	signed short					m_s16LastSRoint;

	int					LastBuyAcount;
	bool				m_bClearRun;
	DECLARE_SCRIPT_REGISTER_FUNCTION_STRUCT
};

add_to_type_list(game_PlayerState)
#undef script_type_list
#define script_type_list save_type_list(game_PlayerState)

struct game_TeamState
{
	int			score;
	unsigned short			num_targets;

	game_TeamState();
};
#pragma pack(pop)

class	game_GameState : public DLL_Pure
{
protected:
	int								m_type;
	unsigned short								m_phase;
	int								m_round;
	u32								m_start_time;

	u32								m_round_start_time;
	string64						m_round_start_time_str;

protected:
	virtual		void				switch_Phase			(u32 new_phase);
	virtual		void				OnSwitchPhase			(u32 old_phase, u32 new_phase)	{};	

public:
									game_GameState			();
	virtual							~game_GameState			()								{}
				u32					Type					() const						{return m_type;};
				unsigned short					Phase					() const						{return m_phase;};
				int					Round					() const						{return m_round;};
				u32					StartTime				() const						{return m_start_time;};
	virtual		void				Create					(CSharedString& options)				{};
	virtual		const char* type_name				() const						{return "base game";};
//for scripting enhancement
	static		CLASS_ID			getCLASS_ID				(const char* game_type_name, bool bServer);
	virtual		game_PlayerState*	createPlayerState()		{return xr_new<game_PlayerState>(); };

//moved from game_sv_base (time routines)
private:
	// scripts
	unsigned __int64								m_qwStartProcessorTime;
	unsigned __int64								m_qwStartGameTime;
	float							m_fTimeFactor;
	//-------------------------------------------------------
	unsigned __int64								m_qwEStartProcessorTime;
	unsigned __int64								m_qwEStartGameTime;
	float							m_fETimeFactor;
	//-------------------------------------------------------

public:
	virtual		ALife::_TIME_ID		GetGameTime				();	
	virtual		float				GetGameTimeFactor		();	
				void				SetGameTimeFactor		(ALife::_TIME_ID GameTime, const float fTimeFactor);
	virtual		void				SetGameTimeFactor		(const float fTimeFactor);
	

	virtual		ALife::_TIME_ID		GetEnvironmentGameTime	();
	virtual		float				GetEnvironmentGameTimeFactor		();
				void				SetEnvironmentGameTimeFactor		(ALife::_TIME_ID GameTime, const float fTimeFactor);
	virtual		void				SetEnvironmentGameTimeFactor		(const float fTimeFactor);

public:
	static void script_register(lua_State*);
};

add_to_type_list(game_GameState)
#undef script_type_list
#define script_type_list save_type_list(game_GameState)
