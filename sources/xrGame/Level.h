// Level.h: interface for the CLevel class.
#pragma once

#include "..\XR_3DA\igame_level.h"
#include "../xrNetServer/net_client.h"
#include "script_export_space.h"
#include "..\XR_3DA\StatGraph.h"
#include "xrMessages.h"
#include "alife_space.h"
#include "..\xrCore\xrDebug.h"
#include "xrServer.h"
#include "..\XR_3DA\feel_touch.h"

class	CHUDManager;
class	CParticlesObject;
class	xrServer;
class	game_cl_GameState;
class	CNETQueueEvent;
class	CSE_Abstract;
class	CSpaceRestrictionManager;
class	CSeniorityHierarchyHolder;
class	CClientSpawnManager;
class	CGameObject;
class	CAutosaveManager;
class	CPHCommander;
class	CLevelDebug;
class	CLevelSoundManager;

#ifdef DEBUG
class	CDebugRenderer;
#endif

extern f32 g_fov;

const s32 maxRP = 64;
const s32 maxTeams = 32;

class CBulletManager;
class CMapManager;

class GlobalFeelTouch : public Feel::Touch
{
public:
	GlobalFeelTouch( );
	virtual					~GlobalFeelTouch( );

	void			update( );
	bool			is_object_denied(const CObject* O);
};

class CLevel : public IGame_Level, public IPureClient
{
private:
	string_path					m_sDemoName;

	//------------- Demo Play	---------------------------------------
	BOOL						m_bDemoPlayMode;
	BOOL						m_bDemoPlayByFrame;

	xr_string					m_sDemoFileName;
	long						m_lDemoOfs;

	enum	DEMO_CHUNK
	{
		DATA_FRAME = u32(0),
		DATA_CLIENT_PACKET,
		DATA_SERVER_PACKET,

		DATA_DUMMY = u32(-1)
	};

	struct DemoFrameTime
	{
		f32									fTimeDelta;
		f32									fTimeGlobal;
		u32										dwTimeDelta;
		u32										dwTimeGlobal;
		u32										dwTimeServer;
		u32										dwTimeServer_Delta;
	};

	struct DemoHeaderStruct
	{
		u8			bServerClient;
		char		Head[31];
		shared_str		ServerOptions;
	};

	DemoHeaderStruct							m_sDemoHeader;

	struct DemoDataStruct
	{
		u32			m_dwDataType;
		u32			m_dwFrame;
		u32			m_dwTimeReceive;
		union
		{
			CNetPacket	Packet;
			DemoFrameTime	FrameTime;
		};
	};

	DEF_DEQUE(DemoDeque, DemoDataStruct);
	DemoDeque					m_aDemoData;
	void						Demo_Load(pcstr DemoName);
	void						Demo_Load_toFrame(pcstr FileName, DWORD toFrame, long& ofs);
	BOOL						m_bDemoStarted;
	u32							m_dwLastDemoFrame;
	void						Demo_Update( );

	//------------- Demo Store -----------------------------------------
	BOOL						m_bDemoSaveMode;

	xrCriticalSection			DemoCS;
	u32							m_dwStoredDemoDataSize;
	u8* m_pStoredDemoData;

	void						Demo_PrepareToStore( );
	void						Demo_StoreData(pvoid data, u32 size, DEMO_CHUNK DataType);
	void						Demo_DumpData( );
	void						Demo_Clear( );

	crashhandler* m_pOldCrashHandler;
	bool						m_we_used_old_crach_handler;

	u32							m_dwCurDemoFrame;
	void						Demo_StartFrame( );
	void						Demo_EndFrame( );

public:
	void						Demo_StoreServerData(pvoid data, u32 size);

	void						CallOldCrashHandler( );
	void						WriteStoredDemo( );

	BOOL						IsDemoPlay( )
	{
		return (!m_bDemoSaveMode && m_bDemoPlayMode);
	}
	BOOL						IsDemoSave( )
	{
		return (m_bDemoSaveMode && !m_bDemoPlayMode);
	}

	bool						IsServerDemo( )
	{
		return (m_sDemoHeader.bServerClient != 0);
	}
	bool						IsClientDemo( )
	{
		return (m_sDemoHeader.bServerClient == 0);
	}

	virtual	CNetPacket* net_msg_Retreive( );

private:
	void						ClearAllObjects( );

private:
#ifdef DEBUG
	bool						m_bSynchronization;
#endif

protected:
	typedef IGame_Level			inherited;

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
	shared_str					m_name;
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

	virtual void				OnMessage(pvoid data, u32 size);
	virtual void				OnConnectRejected( );

private:
	BOOL						m_bNeed_CrPr;
	u32							m_dwNumSteps;
	bool						m_bIn_CrPr;

	DEF_VECTOR(OBJECTS_LIST, CGameObject*);

	OBJECTS_LIST				pObjects4CrPr;
	OBJECTS_LIST				pActors4CrPr;

	CObject* pCurrentControlEntity;
	xrServer::EConnect			m_connect_server_err;

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

	BOOL						Connect2Server(pcstr options);

private:
	bool						m_bConnectResultReceived;
	bool						m_bConnectResult;
	xr_string					m_sConnectResult;

public:
	void						OnGameSpyChallenge(CNetPacket* P) //KRodin: удалить, если не вызывается!
	{
		Msg("!!Called OnGameSpyChallenge!");
	}
	void						OnBuildVersionChallenge( );
	void						OnConnectResult(CNetPacket* P);

public:
	//////////////////////////////////////////////
	// static particles
	DEFINE_VECTOR(CParticlesObject*, POVec, POIt);
	POVec						m_StaticParticles;

	game_cl_GameState* game;
	BOOL						m_bGameConfigStarted;
	BOOL						game_configured;
	CNETQueueEvent* game_events;
	xr_deque<CSE_Abstract*>		game_spawn_queue;
	xrServer* Server;
	GlobalFeelTouch				m_feel_deny;

private:
	// preload sounds registry
	DEFINE_MAP(shared_str, ref_sound, SoundRegistryMap, SoundRegistryMapIt);
	SoundRegistryMap			sound_registry;

public:
	void						PrefetchSound(pcstr name);

protected:
	BOOL						net_start_result_total;
	BOOL						connected_to_server;

	bool	xr_stdcall			net_start1( );
	bool	xr_stdcall			net_start2( );
	bool	xr_stdcall			net_start3( );
	bool	xr_stdcall			net_start4( );
	bool	xr_stdcall			net_start5( );
	bool	xr_stdcall			net_start6( );

	bool	xr_stdcall			net_start_client1( );
	bool	xr_stdcall			net_start_client2( );
	bool	xr_stdcall			net_start_client3( );
	bool	xr_stdcall			net_start_client4( );
	bool	xr_stdcall			net_start_client5( );
	bool	xr_stdcall			net_start_client6( );

	bool	xr_stdcall			net_start_finalizer( );

	void						net_OnChangeSelfName(CNetPacket* P);

public:
	// sounds
	xr_vector<ref_sound*>		static_Sounds;

	// startup options
	shared_str					m_caServerOptions;
	shared_str					m_caClientOptions;

	// Starting/Loading
	virtual BOOL				net_Start(pcstr op_server, pcstr op_client);
	virtual void				net_Load(pcstr name);
	virtual void				net_Save(pcstr name);
	virtual void				net_Stop( );
	virtual BOOL				net_Start_client(pcstr name);
	virtual void				net_Update( );

	virtual BOOL				Load_GameSpecific_Before( );
	virtual BOOL				Load_GameSpecific_After( );
	virtual void				Load_GameSpecific_CFORM(CDB::TRI* T, u32 count);

	// Events
	virtual void				OnEvent(EVENT E, u64 P1, u64 P2);
	virtual void				OnFrame(void);
	virtual void				OnRender( );
	void						cl_Process_Event(u16 dest, u16 type, CNetPacket& P);
	void						cl_Process_Spawn(CNetPacket& P);
	void						ProcessGameEvents( );
	void						ProcessGameSpawns( );

	// Input
	virtual	void				IR_OnKeyboardPress(s32 btn);
	virtual void				IR_OnKeyboardRelease(s32 btn);
	virtual void				IR_OnKeyboardHold(s32 btn);
	virtual void				IR_OnMousePress(s32 btn);
	virtual void				IR_OnMouseRelease(s32 btn);
	virtual void				IR_OnMouseHold(s32 btn);
	virtual void				IR_OnMouseMove(s32, s32);
	virtual void				IR_OnMouseStop(s32, s32);
	virtual void				IR_OnMouseWheel(s32 direction);
	virtual void				IR_OnActivate(void);

	s32					get_RPID(pcstr name);

	// Game
	void						InitializeClientGame(CNetPacket& P);
	void						ClientReceive( );
	void						ClientSend( );
	void						ClientSave( );
	u32					Objects_net_Save(CNetPacket* _Packet, u32 start, u32 count);
	virtual void				Send(CNetPacket& P, u32 dwFlags = DPNSEND_GUARANTEED, u32 dwTimeout = 0);

	void						g_cl_Spawn(pcstr name, u8 rp, u16 flags, fVector3 pos);	// only ask server
	void						g_sv_Spawn(CSE_Abstract* E);					// server reply/command spawning

	// Save/Load/State
	void						SLS_Default( );					// Default/Editor Load

	IC CSpaceRestrictionManager& space_restriction_manager( );
	IC CSeniorityHierarchyHolder& seniority_holder( );
	IC CClientSpawnManager& client_spawn_manager( );
	IC CAutosaveManager& autosave_manager( );

#ifdef DEBUG
	IC CDebugRenderer& debug_renderer( );
#endif
	void	__stdcall				script_gc( );			// GC-cycle

	IC CPHCommander& ph_commander( );
	IC CPHCommander& ph_commander_scripts( );

	// C/D
	CLevel( );
	virtual ~CLevel( );

	//названияе текущего уровня
	virtual shared_str			name( ) const;
	virtual void				GetLevelInfo(CServerInfo* si);

	//gets the time from the game simulation

	//возвращает время в милисекундах относительно начала игры
	ALife::_TIME_ID		GetGameTime( );
	//возвращает время для энвайронмента в милисекундах относительно начала игры
	ALife::_TIME_ID		GetEnvironmentGameTime( );
	//игровое время в отформатированном виде
	void				GetGameDateTime(u32& year, u32& month, u32& day, u32& hours, u32& mins, u32& secs, u32& milisecs);

	f32				GetGameTimeFactor( );
	void				SetGameTimeFactor(const f32 fTimeFactor);
	void				SetGameTimeFactor(ALife::_TIME_ID GameTime, const f32 fTimeFactor);
	void				SetEnvironmentGameTimeFactor(ALife::_TIME_ID GameTime, const f32 fTimeFactor);
//	void				SetGameTime				(ALife::_TIME_ID GameTime);

	// gets current daytime [0..23]
	u8					GetDayTime( );
	u32					GetGameDayTimeMS( );
	f32				GetGameDayTimeSec( );
	f32				GetEnvironmentGameDayTimeSec( );

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
	IC CBulletManager& BulletManager( )
	{
		return	*m_pBulletManager;
	}

	bool			IsServer( );
	bool			IsClient( );
	CSE_Abstract* spawn_item(pcstr section, const fVector3& position, u32 level_vertex_id, u16 parent_id, bool return_item = false);

protected:
	u32		m_dwCL_PingDeltaSend;
	u32		m_dwCL_PingLastSendTime;
	u32		m_dwRealPing;

public:
	virtual u32				GetRealPing( )
	{
		return m_dwRealPing;
	}

public:
	void			remove_objects( );
	virtual void			OnSessionTerminate(pcstr reason);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CLevel)
#undef script_type_list
#define script_type_list save_type_list(CLevel)

IC CLevel& Level( )
{
	return *((CLevel*)g_pGameLevel);
}
IC game_cl_GameState& Game( )
{
	return *Level( ).game;
}
u32					GameID( );


IC CHUDManager& HUD( )
{
	return *((CHUDManager*)Level( ).pHUD);
}

#ifdef DEBUG
IC CLevelDebug& DBG( )
{
	return *((CLevelDebug*)Level( ).m_level_debug);
}
#endif

IC CSpaceRestrictionManager& CLevel::space_restriction_manager( )
{
	VERIFY(m_space_restriction_manager);
	return				(*m_space_restriction_manager);
}

IC CSeniorityHierarchyHolder& CLevel::seniority_holder( )
{
	VERIFY(m_seniority_hierarchy_holder);
	return				(*m_seniority_hierarchy_holder);
}

IC CClientSpawnManager& CLevel::client_spawn_manager( )
{
	VERIFY(m_client_spawn_manager);
	return				(*m_client_spawn_manager);
}

IC CAutosaveManager& CLevel::autosave_manager( )
{
	VERIFY(m_autosave_manager);
	return				(*m_autosave_manager);
}

#ifdef DEBUG
IC CDebugRenderer& CLevel::debug_renderer( )
{
	VERIFY(m_debug_renderer);
	return				(*m_debug_renderer);
}
#endif

IC CPHCommander& CLevel::ph_commander( )
{
	VERIFY(m_ph_commander);
	return *m_ph_commander;
}
IC CPHCommander& CLevel::ph_commander_scripts( )
{
	VERIFY(m_ph_commander_scripts);
	return *m_ph_commander_scripts;
}
//by Mad Max 
IC bool					OnServer( )
{
	return Level( ).IsServer( );
}
IC bool					OnClient( )
{
	return Level( ).IsClient( );
}

class  CPHWorld;
extern CPHWorld* ph_world;
extern BOOL						g_bDebugEvents;
