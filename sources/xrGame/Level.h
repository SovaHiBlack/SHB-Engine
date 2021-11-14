// Level.h: interface for the CLevel class.

#pragma once

#include "..\ENGINE\IGameLevel.h"
#include "..\ENGINE\NET_Client.h"
#include "script_export_space.h"
#include "..\ENGINE\StatGraph.h"//
#include "Messages.h"
#include "ALife_space.h"
#include "..\Core\Debug.h"
#include "Server.h"

class CHUDManager;
class CParticlesObject;
class CServer;
class game_cl_GameState;
class CNetQueueEvent;
class CSE_Abstract;
class CSpaceRestrictionManager;
class CSeniorityHierarchyHolder;
class CClientSpawnManager;
class CGameObject;
class CAutosaveManager;
class CPHCommander;
class CLevelDebug;
class CLevelSoundManager;

#ifdef DEBUG
class CDebugRenderer;
#endif

extern float g_fov;

class CBulletManager;
class CMapManager;

#include "..\ENGINE\feel_touch.h"

class GlobalFeelTouch : public Feel::Touch
{
public:
	GlobalFeelTouch( );
	virtual					~GlobalFeelTouch( );

	void			update( );
	bool			is_object_denied(CObject const* O);
};

class CLevel : public IGameLevel, public IPureClient
{
#include "Level_network_Demo.h"
	void						ClearAllObjects( );

private:
#ifdef DEBUG
	bool						m_bSynchronization;
#endif

protected:
	using inherited = IGameLevel;

	CLevelSoundManager* m_level_sound_manager;

	// movement restriction manager
	CSpaceRestrictionManager* m_space_restriction_manager;
	// seniority hierarchy holder
	CSeniorityHierarchyHolder* m_seniority_hierarchy_holder;
	// client spawn_manager
	CClientSpawnManager* m_client_spawn_manager;
	// autosave manager
	CAutosaveManager* m_autosave_manager;

#ifdef DEBUG
	// debug renderer
	CDebugRenderer* m_debug_renderer;
#endif

	CPHCommander* m_ph_commander;
	CPHCommander* m_ph_commander_scripts;

	// level name
	CSharedString					m_name;
	// Local events
	EVENT						eChangeRP;
	EVENT						eDemoPlay;
	EVENT						eChangeTrack;
	EVENT						eEnvironment;
	EVENT						eEntitySpawn;
	//---------------------------------------------
	CStatGraph* pStatGraphS;
	u32							m_dwSPC;	//SendedPacketsCount
	u32							m_dwSPS;	//SendedPacketsSize
	CStatGraph* pStatGraphR;
	u32							m_dwRPC;	//ReceivedPacketsCount
	u32							m_dwRPS;	//ReceivedPacketsSize
	//---------------------------------------------

public:
#ifdef DEBUG
	// level debugger
	CLevelDebug* m_level_debug;
#endif

public:
	////////////// network ////////////////////////
	u32							GetInterpolationSteps( );
	bool						InterpolationDisabled( );
	void						ReculcInterpolationSteps( );
	u32							GetNumCrSteps( ) const
	{
		return m_dwNumSteps;
	}
	void						SetNumCrSteps(u32 NumSteps);
	static void 				PhisStepsCallback(u32 Time0, u32 Time1);
	bool						In_NetCorrectionPrediction( )
	{
		return m_bIn_CrPr;
	}

	virtual void				OnMessage(void* data, u32 size);
	virtual void				OnConnectRejected( );

private:
	BOOL						m_bNeed_CrPr;
	u32							m_dwNumSteps;
	bool						m_bIn_CrPr;

	using GameObjectVec = xr_vector<CGameObject*>;
	using GameObjectVec_it = GameObjectVec::iterator;

	GameObjectVec				pObjects4CrPr;
	GameObjectVec				pActors4CrPr;

	CObject* pCurrentControlEntity;
	CServer::EConnect			m_connect_server_err;

public:
	void						AddObject_To_Objects4CrPr(CGameObject* pObj);
	void						AddActor_To_Actors4CrPr(CGameObject* pActor);

	void						RemoveObject_From_4CrPr(CGameObject* pObj);

	CObject* CurrentControlEntity( ) const
	{
		return pCurrentControlEntity;
	}
	void						SetControlEntity(CObject* O)
	{
		pCurrentControlEntity = O;
	}

private:
	void						make_NetCorrectionPrediction( );

	u32							m_dwDeltaUpdate;
	u32							m_dwLastNetUpdateTime;
	void						UpdateDeltaUpd(u32 LastTime);

	BOOL						Connect2Server(const char* options);

	bool						m_bConnectResultReceived;
	bool						m_bConnectResult;
	xr_string					m_sConnectResult;

public:
	void						OnBuildVersionChallenge( );
	void						OnConnectResult(CNetPacket* P);

	//////////////////////////////////////////////
	// static particles
	using ParticlesObjectVec = xr_vector<CParticlesObject*>;
	using ParticlesObjectVec_it = ParticlesObjectVec::iterator;
	ParticlesObjectVec						m_StaticParticles;

	game_cl_GameState* game;
	bool						m_bGameConfigStarted;
	bool						game_configured;
	CNetQueueEvent* game_events;
	xr_deque<CSE_Abstract*>		game_spawn_queue;
	CServer* Server;
	GlobalFeelTouch				m_feel_deny;

private:
	// preload sounds registry
	using SoundRegistryMap = xr_map<CSharedString, ref_sound>;
	using SoundRegistryMap_it = SoundRegistryMap::iterator;
	SoundRegistryMap			sound_registry;

public:
	void						PrefetchSound(const char* name);

protected:
	BOOL						net_start_result_total;
	BOOL						connected_to_server;

	bool xr_stdcall			net_start1( );
	bool xr_stdcall			net_start2( );
	bool xr_stdcall			net_start3( );
	bool xr_stdcall			net_start4( );
	bool xr_stdcall			net_start5( );
	bool xr_stdcall			net_start6( );

	bool xr_stdcall			net_start_client1( );
	bool xr_stdcall			net_start_client2( );
	bool xr_stdcall			net_start_client3( );
	bool xr_stdcall			net_start_client4( );
	bool xr_stdcall			net_start_client5( );
	bool xr_stdcall			net_start_client6( );

	bool xr_stdcall			net_start_finalizer( );

	void						net_OnChangeSelfName(CNetPacket* P);

public:
	// sounds
	xr_vector<ref_sound*>		static_Sounds;

	// startup options
	CSharedString					m_caServerOptions;
	CSharedString					m_caClientOptions;

	// Starting/Loading
	virtual BOOL				net_Start(const char* op_server, const char* op_client);
	virtual void				net_Load(const char* name);
	virtual void				net_Save(const char* name);
	virtual void				net_Stop( );
	virtual BOOL				net_Start_client(const char* name);
	virtual void				net_Update( );

	virtual BOOL				Load_GameSpecific_Before( );
	virtual BOOL				Load_GameSpecific_After( );
	virtual void				Load_GameSpecific_CFORM(CDB::TRI* T, u32 count);

	// Events
	virtual void				OnEvent(EVENT E, U64 P1, U64 P2);
	virtual void				OnFrame( );
	virtual void				OnRender( );
	void						cl_Process_Event(unsigned short dest, unsigned short type, CNetPacket& P);
	void						cl_Process_Spawn(CNetPacket& P);
	void						ProcessGameEvents( );
	void						ProcessGameSpawns( );

	// Input
	virtual	void				IR_OnKeyboardPress(int dik);
	virtual void				IR_OnKeyboardRelease(int dik);
	virtual void				IR_OnKeyboardHold(int dik);
	virtual void				IR_OnMousePress(int btn);
	virtual void				IR_OnMouseRelease(int btn);
	virtual void				IR_OnMouseHold(int btn);
	virtual void				IR_OnMouseMove(int dx, int dy);
	virtual void				IR_OnMouseStop(int x, int y);
	virtual void				IR_OnMouseWheel(int direction);
	virtual void				IR_OnActivate( );

	// Game
	void						InitializeClientGame(CNetPacket& P);
	void						ClientReceive( );
	void						ClientSend( );
	void						ClientSave( );
	u32					Objects_net_Save(CNetPacket* _Packet, u32 start, u32 count);
	virtual void				Send(CNetPacket& P, u32 dwFlags = DPNSEND_GUARANTEED, u32 dwTimeout = 0);

	void						g_cl_Spawn(const char* name, unsigned char rp, unsigned short flags, Fvector3 pos);	// only ask server
	void						g_sv_Spawn(CSE_Abstract* E);					// server reply/command spawning

	// Save/Load/State
	void						SLS_Default( );					// Default/Editor Load

	inline CSpaceRestrictionManager& space_restriction_manager( );
	inline CSeniorityHierarchyHolder& seniority_holder( );
	inline CClientSpawnManager& client_spawn_manager( );
	inline CAutosaveManager& autosave_manager( );

#ifdef DEBUG
	inline CDebugRenderer& debug_renderer( );
#endif

	void	__stdcall				script_gc( );			// GC-cycle

	inline CPHCommander& ph_commander( );
	inline CPHCommander& ph_commander_scripts( );

	// C/D
	CLevel( );
	virtual ~CLevel( );

	//названияе текущего уровня
	virtual CSharedString			name( ) const;
	virtual void				GetLevelInfo(CServerInfo* si);

	//gets the time from the game simulation

	//возвращает время в милисекундах относительно начала игры
	ALife::_TIME_ID		GetGameTime( );
	//возвращает время для энвайронмента в милисекундах относительно начала игры
	ALife::_TIME_ID		GetEnvironmentGameTime( );
	//игровое время в отформатированном виде
	void				GetGameDateTime(u32& year, u32& month, u32& day, u32& hours, u32& mins, u32& secs, u32& milisecs);

	float				GetGameTimeFactor( );
	void				SetGameTimeFactor(const float fTimeFactor);
	void				SetGameTimeFactor(ALife::_TIME_ID GameTime, const float fTimeFactor);
	void				SetEnvironmentGameTimeFactor(ALife::_TIME_ID GameTime, const float fTimeFactor);

	// gets current daytime [0..23]
	unsigned char					GetDayTime( );
	u32					GetGameDayTimeMS( );
	float				GetGameDayTimeSec( );
	float				GetEnvironmentGameDayTimeSec( );

protected:
	CMapManager* m_map_manager;

public:
	CMapManager& MapManager( )
	{
		return *m_map_manager;
	}

	//работа с пулями
protected:
	CBulletManager* m_pBulletManager;

public:
	inline CBulletManager& BulletManager( )
	{
		return *m_pBulletManager;
	}

	bool			IsServer( );
	bool			IsClient( );
	CSE_Abstract* spawn_item(const char* section, const Fvector3& position, u32 level_vertex_id, unsigned short parent_id, bool return_item = false);

	void			remove_objects( );
	virtual void			OnSessionTerminate(const char* reason);

	static void script_register(lua_State*);
};

add_to_type_list(CLevel)
#undef script_type_list
#define script_type_list save_type_list(CLevel)

inline CLevel& Level( )
{
	return *((CLevel*) g_pGameLevel);
}

inline game_cl_GameState& Game( )
{
	return *Level( ).game;
}

u32					GameID( );

inline CHUDManager& HUD( )
{
	return *((CHUDManager*) Level( ).pHUD);
}

#ifdef DEBUG
inline CLevelDebug& DBG( )
{
	return *((CLevelDebug*) Level( ).m_level_debug);
}
#endif

inline CSpaceRestrictionManager& CLevel::space_restriction_manager( )
{
	VERIFY(m_space_restriction_manager);
	return *m_space_restriction_manager;
}

inline CSeniorityHierarchyHolder& CLevel::seniority_holder( )
{
	VERIFY(m_seniority_hierarchy_holder);
	return *m_seniority_hierarchy_holder;
}

inline CClientSpawnManager& CLevel::client_spawn_manager( )
{
	VERIFY(m_client_spawn_manager);
	return *m_client_spawn_manager;
}

inline CAutosaveManager& CLevel::autosave_manager( )
{
	VERIFY(m_autosave_manager);
	return *m_autosave_manager;
}

#ifdef DEBUG
inline CDebugRenderer& CLevel::debug_renderer( )
{
	VERIFY(m_debug_renderer);
	return *m_debug_renderer;
}
#endif // def DEBUG

inline CPHCommander& CLevel::ph_commander( )
{
	VERIFY(m_ph_commander);
	return *m_ph_commander;
}

inline CPHCommander& CLevel::ph_commander_scripts( )
{
	VERIFY(m_ph_commander_scripts);
	return *m_ph_commander_scripts;
}

inline bool					OnServer( )
{
	return Level( ).IsServer( );
}

inline bool					OnClient( )
{
	return Level( ).IsClient( );
}

class CPHWorld;
extern CPHWorld* ph_world;
extern bool						g_bDebugEvents;
