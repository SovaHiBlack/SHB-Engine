#pragma once

#include <depr/dplay8.h>// раньше не было

#include "game_base.h"
#include "alife_space.h"
#include "script_export_space.h"
#include "..\ENGINE\ClientID.h"

//enum ERoundEnd_Result
//{
//	eRoundEnd_Finish		= u32(0),
//	eRoundEnd_GameRestarted,
//	eRoundEnd_GameRestartedFast,
//	eRoundEnd_TimeLimit,
//	eRoundEnd_FragLimit,
//	eRoundEnd_ArtrefactLimit,
//	eRoundEnd_Force			= u32(-1)
//};

class CSE_Abstract;
class CServer;
// [OLES] Policy:
// it_*		- means ordinal number of player
// id_*		- means player-id
// eid_*	- means entity-id
class GameEventQueue;

class	game_sv_GameState	: public game_GameState
{
	typedef game_GameState inherited;

protected:
	CServer*						m_server;
	GameEventQueue*					m_event_queue;

	//Events
	virtual		void				OnEvent					(CNetPacket &tNetPacket, U16 type, u32 time, ClientID sender );

	virtual		void				ReadOptions				(CSharedString& options);
	virtual		void				ConsoleCommands_Create	();
	virtual		void				ConsoleCommands_Clear	();

//	DEF_DEQUE(MAP_ROTATION_LIST,	);
	using MAP_ROTATION_LIST = xr_deque<xr_string>;
	using MAP_ROTATION_LIST_it = MAP_ROTATION_LIST::iterator;

	//bool							m_bMapRotation;
	bool							m_bMapNeedRotation;
	bool							m_bMapSwitched;
	bool							m_bFastRestart;

	MAP_ROTATION_LIST				m_pMapRotation_List;

public:
#define		TEAM_COUNT 4

	bool							NewPlayerName_Exists	(void* pClient, const char* NewName);
	void							NewPlayerName_Generate	(void* pClient, char* NewPlayerName);
	void							NewPlayerName_Replace	(void* pClient, const char* NewPlayerName);

	BOOL							sv_force_sync;
	float							rpoints_MinDist [TEAM_COUNT];
	xr_vector<RPoint>				rpoints	[TEAM_COUNT];
	//DEF_VECTOR(RPRef, RPoint*);
	using RPRef = xr_vector<RPoint*>;
	using RPRef_it = RPRef::iterator;
	RPRef							rpointsBlocked;

	//ERoundEnd_Result				round_end_reason;
	
	virtual		void				SaveMapList				();
	//virtual		bool				HasMapRotation			() {return m_bMapRotation; };
	
	virtual		void				OnPlayerConnect			(ClientID id_who);
	virtual		void				OnPlayerDisconnect		(ClientID id_who, char* Name, U16 GameID);
	//virtual		void				OnPlayerReady			(ClientID id_who)							   {};
	//virtual		void				OnPlayerEnteredGame		(ClientID id_who)	{};
	virtual		void				OnPlayerConnectFinished	(ClientID id_who)	{};
	virtual		void				OnPlayerFire			(ClientID id_who, CNetPacket &P) {};
	virtual		void				OnPlayer_Sell_Item		(ClientID id_who, CNetPacket &P) {};
				//void				GenerateGameMessage		(CNetPacket &P);

	//virtual		void				OnRoundStart			();									// старт раунда
	//virtual		void				OnRoundEnd				();	//	round_end_reason			// конец раунда

	//virtual		void				MapRotation_AddMap		(const char* MapName);
	//virtual		void				MapRotation_ListMaps	();
	//virtual		bool				OnNextMap				()									{return false;}
//	virtual		void				OnPrevMap				()									{}
//	virtual		bool				SwitchToNextMap			()	{ return m_bMapNeedRotation; };
	
									game_sv_GameState		();
	virtual							~game_sv_GameState		();
	// Main accessors
	//virtual		game_PlayerState*	get_eid					(U16 id);
	virtual		void*				get_client				(U16 id); //if exist
	virtual		game_PlayerState*	get_it					(u32 it);
	virtual		game_PlayerState*	get_id					(ClientID id);
	
	//virtual		const char* get_name_it				(u32 it);
	//virtual		const char* get_name_id				(ClientID id);
	//const char* get_player_name_id		(ClientID id);
	//virtual		U16					get_id_2_eid			(ClientID id);
	virtual		ClientID			get_it_2_id				(u32 it);
	virtual		u32					get_players_count		();
				CSE_Abstract*		get_entity_from_eid		(U16 id);
				//RPoint				getRP					(U16 team_idx, u32 rp_idx);
				//u32					getRPcount				(U16 team_idx);
	// Signals
	virtual		void				signal_Syncronize		();
	//virtual		void				assign_RP				(CSE_Abstract* E, game_PlayerState* ps_who);
	//virtual		bool				IsPointFreezed			(RPoint* rp);
	//virtual		void				SetPointFreezed			(RPoint* rp);

#ifdef DEBUG
	virtual		void				OnRender				();
#endif
	
	virtual		void				OnSwitchPhase			(u32 old_phase, u32 new_phase);	
				//CSE_Abstract*		spawn_begin				(const char* N);
				//CSE_Abstract*		spawn_end				(CSE_Abstract* E, ClientID id);

	// Utilities
	//float							get_option_f			(const char* lst, const char* name, float def = 0.0f);
	int								get_option_i			(const char* lst, const char* name, int def = 0);
	//string64&						get_option_s			(const char* lst, const char* name, const char* def = 0);
	//virtual		u32					get_alive_count			(u32 team);
	//virtual		xr_vector<U16>*		get_children			(ClientID id_who);
	void							u_EventGen				(CNetPacket& P, U16 type, U16 dest	);
	void							u_EventSend				(CNetPacket& P, u32 dwFlags = DPNSEND_GUARANTEED);

	// Events
	virtual		BOOL				OnPreCreate				(CSE_Abstract* E)				{return TRUE;};
	virtual		void				OnCreate				(U16 id_who)					{};
	virtual		void				OnPostCreate			(U16 id_who)					{};
	virtual		BOOL				OnTouch					(U16 eid_who, U16 eid_target, BOOL bForced = FALSE)	= 0;			// TRUE=allow ownership, FALSE=denied
	virtual		void				OnDetach				(U16 eid_who, U16 eid_target)	= 0;
	virtual		void				OnDestroyObject			(U16 eid_who)							{};

	virtual		void				OnHit					(U16 id_hitter, U16 id_hitted, CNetPacket& P);	//кто-то получил Hit
	virtual		void				OnPlayerHitPlayer		(U16 id_hitter, U16 id_hitted, CNetPacket& P){}; //игрок получил Hit

	// Main
	virtual		void				Create					(CSharedString& options);
	virtual		void				Update					();
	virtual		void				net_Export_State		(CNetPacket& P, ClientID id_to);				// full state
	virtual		void				net_Export_Update		(CNetPacket& P, ClientID id_to, ClientID id);		// just incremental update for specific client
	virtual		void				net_Export_GameTime		(CNetPacket& P);						// update GameTime only for remote clients

	virtual		bool				change_level			(CNetPacket &net_packet, ClientID sender);
	virtual		void				save_game				(CNetPacket &net_packet, ClientID sender);
	virtual		bool				load_game				(CNetPacket &net_packet, ClientID sender);
	//virtual		void				reload_game				(CNetPacket &net_packet, ClientID sender);
	virtual		void				switch_distance			(CNetPacket &net_packet, ClientID sender);

				void				AddDelayedEvent			(CNetPacket &tNetPacket, U16 type, u32 time, ClientID sender );
				void				ProcessDelayedEvent		();
	//virtual		BOOL				isFriendlyFireEnabled	()	{return FALSE;};
	//virtual		BOOL				CanHaveFriendlyFire		()	= 0;
	virtual		void				teleport_object			(CNetPacket &packet, U16 id);
	virtual		void				add_restriction			(CNetPacket &packet, U16 id);
	virtual		void				remove_restriction		(CNetPacket &packet, U16 id);
	virtual		void				remove_all_restrictions	(CNetPacket &packet, U16 id);
	virtual		bool				custom_sls_default		() {return false;};
	virtual		void				sls_default				() {};
	virtual		CSharedString			level_name				(const CSharedString& server_options) const;
	virtual		void				on_death				(CSE_Abstract *e_dest, CSE_Abstract *e_src);

	//virtual		void				DumpOnlineStatistic		(){};

	public:
		static void script_register(lua_State*);
};

add_to_type_list(game_sv_GameState)
#undef script_type_list
#define script_type_list save_type_list(game_sv_GameState)
