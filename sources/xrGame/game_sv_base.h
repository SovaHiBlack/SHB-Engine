#pragma once

#include <depr/dplay8.h>// раньше не было

#include "game_base.h"
#include "alife_space.h"
#include "script_export_space.h"
#include "..\xrNetServer\ClientID.h"

enum ERoundEnd_Result
{
	eRoundEnd_Finish = u32(0),
	eRoundEnd_GameRestarted,
	eRoundEnd_GameRestartedFast,
	eRoundEnd_TimeLimit,
	eRoundEnd_FragLimit,
	eRoundEnd_ArtrefactLimit,
	eRoundEnd_Force = u32(-1)
};

class CSE_Abstract;
class xrServer;
// [OLES] Policy:
// it_*		- means ordinal number of player
// id_*		- means player-id
// eid_*	- means entity-id
class GameEventQueue;

class	game_sv_GameState : public game_GameState
{
	typedef game_GameState inherited;

protected:
	xrServer* m_server;
	GameEventQueue* m_event_queue;

	//Events
	virtual		void				OnEvent(CNetPacket& tNetPacket, u16 type, u32 time, CClientID sender);

	virtual		void				ReadOptions(shared_str& options);
	virtual		void				ConsoleCommands_Create( );
	virtual		void				ConsoleCommands_Clear( );

	DEF_DEQUE(MAP_ROTATION_LIST, xr_string);
	bool							m_bMapRotation;
	bool							m_bMapNeedRotation;
	bool							m_bMapSwitched;
	bool							m_bFastRestart;

	MAP_ROTATION_LIST				m_pMapRotation_List;

public:
#define		TEAM_COUNT 4

	bool							NewPlayerName_Exists(pvoid pClient, pcstr NewName);
	void							NewPlayerName_Generate(pvoid pClient, pstr NewPlayerName);
	void							NewPlayerName_Replace(pvoid pClient, pcstr NewPlayerName);

	BOOL							sv_force_sync;
	f32								rpoints_MinDist[TEAM_COUNT];
	xr_vector<RPoint>				rpoints[TEAM_COUNT];
	DEF_VECTOR(RPRef, RPoint*);
	RPRef							rpointsBlocked;

	ERoundEnd_Result				round_end_reason;

	virtual		void				SaveMapList( );
	virtual		bool				HasMapRotation( )
	{
		return m_bMapRotation;
	}

public:
	virtual		void				OnPlayerConnect(CClientID id_who);
	virtual		void				OnPlayerDisconnect(CClientID id_who, pstr Name, u16 GameID);
	virtual		void				OnPlayerReady(CClientID id_who)
	{ }
	virtual		void				OnPlayerEnteredGame(CClientID id_who)
	{ }
	virtual		void				OnPlayerConnectFinished(CClientID id_who)
	{ }
	virtual		void				OnPlayerFire(CClientID id_who, CNetPacket& P)
	{ }
	virtual		void				OnPlayer_Sell_Item(CClientID id_who, CNetPacket& P)
	{ }
	void							GenerateGameMessage(CNetPacket& P);

	virtual		void				OnRoundStart( );									// старт раунда
	virtual		void				OnRoundEnd( );	//	round_end_reason			// конец раунда

	virtual		void				MapRotation_AddMap(pcstr MapName);
	virtual		void				MapRotation_ListMaps( );
	virtual		bool				OnNextMap( )
	{
		return false;
	}
	virtual		void				OnPrevMap( )
	{ }
	virtual		bool				SwitchToNextMap( )
	{
		return m_bMapNeedRotation;
	}

public:
									game_sv_GameState( );
	virtual							~game_sv_GameState( );

	// Main accessors
	virtual		game_PlayerState*	get_eid(u16 id);
	virtual		pvoid				get_client(u16 id); //if exist
	virtual		game_PlayerState*	get_it(u32 it);
	virtual		game_PlayerState*	get_id(CClientID id);

	virtual		pcstr				get_name_it(u32 it);
	virtual		pcstr				get_name_id(CClientID id);
	pcstr							get_player_name_id(CClientID id);
	virtual		u16					get_id_2_eid(CClientID id);
	virtual		CClientID			get_it_2_id(u32 it);
	virtual		u32					get_players_count( );
	CSE_Abstract*					get_entity_from_eid(u16 id);
	RPoint							getRP(u16 team_idx, u32 rp_idx);
	u32								getRPcount(u16 team_idx);

	// Signals
	virtual		void				signal_Syncronize( );
	virtual		void				assign_RP(CSE_Abstract* E, game_PlayerState* ps_who);
	virtual		bool				IsPointFreezed(RPoint* rp);
	virtual		void				SetPointFreezed(RPoint* rp);

#ifdef DEBUG
	virtual		void				OnRender( );
#endif

	virtual		void				OnSwitchPhase(u32 old_phase, u32 new_phase);
	CSE_Abstract*					spawn_begin(pcstr N);
	CSE_Abstract*					spawn_end(CSE_Abstract* E, CClientID id);

	// Utilities
	f32								get_option_f(pcstr lst, pcstr name, f32 def = 0.0f);
	s32								get_option_i(pcstr lst, pcstr name, s32 def = 0);
	string64&						get_option_s(pcstr lst, pcstr name, pcstr def = 0);
	virtual		u32					get_alive_count(u32 team);
	virtual		xr_vector<u16>*		get_children(CClientID id_who);
	void							u_EventGen(CNetPacket& P, u16 type, u16 dest);
	void							u_EventSend(CNetPacket& P, u32 dwFlags = DPNSEND_GUARANTEED);

	// Events
	virtual		BOOL				OnPreCreate(CSE_Abstract* E)
	{
		return TRUE;
	}
	virtual		void				OnCreate(u16 id_who)
	{ }
	virtual		void				OnPostCreate(u16 id_who)
	{ }
	virtual		BOOL				OnTouch(u16 eid_who, u16 eid_target, BOOL bForced = FALSE) = 0;			// TRUE=allow ownership, FALSE=denied
	virtual		void				OnDetach(u16 eid_who, u16 eid_target) = 0;
	virtual		void				OnDestroyObject(u16 eid_who)
	{ }

	virtual		void				OnHit(u16 id_hitter, u16 id_hitted, CNetPacket& P);	//кто-то получил Hit
	virtual		void				OnPlayerHitPlayer(u16 id_hitter, u16 id_hitted, CNetPacket& P)
	{ } //игрок получил Hit

	// Main
	virtual		void				Create(shared_str& options);
	virtual		void				Update( );
	virtual		void				net_Export_State(CNetPacket& P, CClientID id_to);						// full state
	virtual		void				net_Export_Update(CNetPacket& P, CClientID id_to, CClientID id);		// just incremental update for specific client
	virtual		void				net_Export_GameTime(CNetPacket& P);										// update GameTime only for remote clients

	virtual		bool				change_level(CNetPacket& net_packet, CClientID sender);
	virtual		void				save_game(CNetPacket& net_packet, CClientID sender);
	virtual		bool				load_game(CNetPacket& net_packet, CClientID sender);
	virtual		void				reload_game(CNetPacket& net_packet, CClientID sender);
	virtual		void				switch_distance(CNetPacket& net_packet, CClientID sender);

	void							AddDelayedEvent(CNetPacket& tNetPacket, u16 type, u32 time, CClientID sender);
	void							ProcessDelayedEvent( );
	virtual		BOOL				isFriendlyFireEnabled( )
	{
		return FALSE;
	}
	virtual		BOOL				CanHaveFriendlyFire( ) = 0;
	virtual		void				teleport_object(CNetPacket& packet, u16 id);
	virtual		void				add_restriction(CNetPacket& packet, u16 id);
	virtual		void				remove_restriction(CNetPacket& packet, u16 id);
	virtual		void				remove_all_restrictions(CNetPacket& packet, u16 id);
	virtual		bool				custom_sls_default( )
	{
		return false;
	}
	virtual		void				sls_default( )
	{ }
	virtual		shared_str			level_name(const shared_str& server_options) const;
	virtual		void				on_death(CSE_Abstract* e_dest, CSE_Abstract* e_src);

	virtual		void				DumpOnlineStatistic( )
	{ }

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(game_sv_GameState)
#undef script_type_list
#define script_type_list save_type_list(game_sv_GameState)