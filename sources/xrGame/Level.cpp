#include "stdafx.h"

#include <functional>// ������ ������

#include "..\XR_3DA\DemoRecord.h"
#include "..\XR_3DA\DemoPlay.h"
#include "..\XR_3DA\environment.h"
#include "..\XR_3DA\IGame_Persistent.h"
#include "ParticlesObject.h"
#include "Level.h"
#include "xrServer.h"
#include "net_queue.h"
#include "game_cl_base.h"
#include "EntityAlive.h"
#include "hudmanager.h"
#include "AISpace.h"
#include "ai_debug.h"
#include "PHdynamicdata.h"
#include "Physics.h"
#include "ShootingObject.h"
#include "BulletManager.h"
#include "script_process.h"
#include "script_engine.h"
#include "script_engine_space.h"
#include "TeamBaseZone.h"
#include "InfoPortion.h"
#include "patrol_path_storage.h"
#include "date_time.h"
#include "space_restriction_manager.h"
#include "seniority_hierarchy_holder.h"
#include "SpaceRestrictor.h"
#include "ClientSpawnManager.h"
#include "AutosaveManager.h"
#include "ClimableObject.h"
#include "level_graph.h"
#include "mt_config.h"
#include "phcommander.h"
#include "MapManager.h"
#include "..\XR_3DA\CameraManager.h"
#include "level_sounds.h"
#include "car.h"
#include "TradeParameters.h"
#include "game_cl_base_weapon_usage_statistic.h"
#include "clsid_game.h"
#include "MainMenu.h"
#include "..\XR_3DA\Console.h"

#ifdef DEBUG
#	include "LevelDebug.h"
#	include "ai/stalker/Stalker.h"
#	include "DebugRenderer.h"
#	include "physicobject.h"
#endif

extern BOOL	g_bDebugDumpPhysicsStep;

CPHWorld* ph_world = 0;
f32		g_cl_lvInterp = 0.0f;
u32			lvInterpSteps = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLevel::CLevel( ) : IPureClient(Device.GetTimerGlobal( ))
{
	g_bDebugEvents = strstr(Core.Params, "-debug_ge") ? TRUE : FALSE;

	Server = NULL;

	game = NULL;
	//	game						= xr_new<game_cl_GameState>();
	game_events = xr_new<CNETQueueEvent>( );

	game_configured = FALSE;
	m_bGameConfigStarted = FALSE;

	eChangeRP = Engine.Event.Handler_Attach("LEVEL:ChangeRP", this);
	eDemoPlay = Engine.Event.Handler_Attach("LEVEL:PlayDEMO", this);
	eChangeTrack = Engine.Event.Handler_Attach("LEVEL:PlayMusic", this);
	eEnvironment = Engine.Event.Handler_Attach("LEVEL:Environment", this);

	eEntitySpawn = Engine.Event.Handler_Attach("LEVEL:spawn", this);

	m_pBulletManager = xr_new<CBulletManager>( );

	m_map_manager = xr_new<CMapManager>( );

	//----------------------------------------------------
	m_bNeed_CrPr = false;
	m_bIn_CrPr = false;
	m_dwNumSteps = 0;
	m_dwDeltaUpdate = u32(fixed_step * 1000);
	m_dwLastNetUpdateTime = 0;

	physics_step_time_callback = (PhysicsStepTimeCallback*)&PhisStepsCallback;
	m_seniority_hierarchy_holder = xr_new<CSeniorityHierarchyHolder>( );

	m_level_sound_manager = xr_new<CLevelSoundManager>( );
	m_space_restriction_manager = xr_new<CSpaceRestrictionManager>( );
	m_client_spawn_manager = xr_new<CClientSpawnManager>( );
	m_autosave_manager = xr_new<CAutosaveManager>( );

#ifdef DEBUG
	m_debug_renderer = xr_new<CDebugRenderer>( );
	m_level_debug = xr_new<CLevelDebug>( );
#endif

	m_ph_commander = xr_new<CPHCommander>( );
	m_ph_commander_scripts = xr_new<CPHCommander>( );

#ifdef DEBUG
	m_bSynchronization = false;
#endif	

	//---------------------------------------------------------
	pStatGraphR = NULL;
	pStatGraphS = NULL;
	//---------------------------------------------------------
	pObjects4CrPr.clear( );
	pActors4CrPr.clear( );
	//---------------------------------------------------------
	pCurrentControlEntity = NULL;

	//---------------------------------------------------------
	m_dwCL_PingLastSendTime = 0;
	m_dwCL_PingDeltaSend = 1000;
	m_dwRealPing = 0;

	//---------------------------------------------------------	
	m_sDemoName[0] = 0;
	m_bDemoSaveMode = FALSE;
	m_dwStoredDemoDataSize = 0;
	m_pStoredDemoData = NULL;
	m_pOldCrashHandler = NULL;
	m_we_used_old_crach_handler = false;

	//	if ( !strstr( Core.Params, "-tdemo " ) && !strstr(Core.Params,"-tdemof "))
	//	{
	//		Demo_PrepareToStore();
	//	};
		//---------------------------------------------------------
	//	m_bDemoPlayMode = FALSE;
	//	m_aDemoData.clear();
	//	m_bDemoStarted	= FALSE;

	Msg("%s", Core.Params);
	/*
	if (strstr(Core.Params,"-tdemo ") || strstr(Core.Params,"-tdemof ")) {
		string1024				f_name;
		if (strstr(Core.Params,"-tdemo "))
		{
			sscanf					(strstr(Core.Params,"-tdemo ")+7,"%[^ ] ",f_name);
			m_bDemoPlayByFrame = FALSE;

			Demo_Load	(f_name);
		}
		else
		{
			sscanf					(strstr(Core.Params,"-tdemof ")+8,"%[^ ] ",f_name);
			m_bDemoPlayByFrame = TRUE;

			m_lDemoOfs = 0;
			Demo_Load_toFrame(f_name, 100, m_lDemoOfs);
		};
	}
	*/
	//---------------------------------------------------------	
}

extern CAISpace* g_ai_space;

CLevel::~CLevel( )
{
	//	g_pGameLevel		= NULL;
	Msg("- Destroying level");

	Engine.Event.Handler_Detach(eEntitySpawn, this);

	Engine.Event.Handler_Detach(eEnvironment, this);
	Engine.Event.Handler_Detach(eChangeTrack, this);
	Engine.Event.Handler_Detach(eDemoPlay, this);
	Engine.Event.Handler_Detach(eChangeRP, this);

	if (ph_world)
	{
		ph_world->Destroy( );
		xr_delete(ph_world);
	}

	// destroy PSs
	for (POIt p_it = m_StaticParticles.begin( ); m_StaticParticles.end( ) != p_it; ++p_it)
	{
		CParticlesObject::Destroy(*p_it);
	}

	m_StaticParticles.clear( );

	// Unload sounds
	// unload prefetched sounds
	sound_registry.clear( );

	// unload static sounds
	for (u32 i = 0; i < static_Sounds.size( ); ++i)
	{
		static_Sounds[i]->destroy( );
		xr_delete(static_Sounds[i]);
	}

	static_Sounds.clear( );

	xr_delete(m_level_sound_manager);
	xr_delete(m_space_restriction_manager);
	xr_delete(m_seniority_hierarchy_holder);
	xr_delete(m_client_spawn_manager);
	xr_delete(m_autosave_manager);

#ifdef DEBUG
	xr_delete(m_debug_renderer);
#endif

	ai( ).script_engine( ).remove_script_process(ScriptEngine::eScriptProcessorLevel);

	xr_delete(game);
	xr_delete(game_events);

	//destroy bullet manager
	xr_delete(m_pBulletManager);
	//-----------------------------------------------------------
	xr_delete(pStatGraphR);
	xr_delete(pStatGraphS);

	//-----------------------------------------------------------
	xr_delete(m_ph_commander);
	xr_delete(m_ph_commander_scripts);
	//-----------------------------------------------------------
	pObjects4CrPr.clear( );
	pActors4CrPr.clear( );

	ai( ).unload( );
	//-----------------------------------------------------------	
#ifdef DEBUG
	xr_delete(m_level_debug);
#endif

	//-----------------------------------------------------------
	xr_delete(m_map_manager);

	//-----------------------------------------------------------
	Demo_Clear( );
	m_aDemoData.clear( );

	// here we clean default trade params
	// because they should be new for each saved/loaded game
	// and I didn't find better place to put this code in
	CTradeParameters::clean( );

	if (m_we_used_old_crach_handler)
	{
		Debug.set_crashhandler(m_pOldCrashHandler);
	}
}

shared_str	CLevel::name( ) const
{
	return m_name;
}

void CLevel::GetLevelInfo(CServerInfo* si)
{
	Server->GetServerInfo(si);
}

void CLevel::PrefetchSound(pcstr name)
{
	// preprocess sound name
	string_path					tmp;
	strcpy_s(tmp, name);
	xr_strlwr(tmp);
	if (strext(tmp))
	{
		*strext(tmp) = 0;
	}

	shared_str	snd_name = tmp;
	// find in registry
	SoundRegistryMapIt it = sound_registry.find(snd_name);
	// if find failed - preload sound
	if (it == sound_registry.end( ))
	{
		sound_registry[snd_name].create(snd_name.c_str( ), st_Effect, sg_SourceType);
	}
}

// Game interface ////////////////////////////////////////////////////
s32	CLevel::get_RPID(pcstr /**name/**/)
{
	/*
	// Gain access to string
	pcstr	params = pLevel->r_string("respawn_point",name);
	if (0==params)	return -1;

	// Read data
	fVector4	pos;
	s32			team;
	sscanf		(params,"%f,%f,%f,%d,%f",&pos.x,&pos.y,&pos.z,&team,&pos.w); pos.y += 0.1f;

	// Search respawn point
	svector<fVector4,maxRP>	&rp = Level().get_team(team).RespawnPoints;
	for (s32 i=0; i<(s32)(rp.size()); ++i)
		if (pos.similar(rp[i],EPSILON_3))	return i;
	*/
	return -1;
}

BOOL		g_bDebugEvents = FALSE;

void CLevel::cl_Process_Event(u16 dest, u16 type, CNetPacket& P)
{
	//			Msg				("--- event[%d] for [%d]",type,dest);
	CObject* O = Objects.net_Find(dest);
	if (0 == O)
	{

#ifdef DEBUG
		Msg("* WARNING: c_EVENT[%d] to [%d]: unknown dest", type, dest);
#endif // DEBUG

		return;
	}

	CGameObject* GO = smart_cast<CGameObject*>(O);
	if (!GO)
	{
		Msg("! ERROR: c_EVENT[%d] : non-game-object", dest);
		return;
	}

	if (type != GE_DESTROY_REJECT)
	{
		if (type == GE_DESTROY)
			Game( ).OnDestroy(GO);
		GO->OnEvent(P, type);
	}
	else
	{ // handle GE_DESTROY_REJECT here
		u32				pos = P.r_tell( );
		u16				id = P.r_u16( );
		P.r_seek(pos);

		bool			ok = true;

		CObject* D = Objects.net_Find(id);
		if (0 == D)
		{
			Msg("! ERROR: c_EVENT[%d] : unknown dest", id);
			ok = false;
		}

		CGameObject* GD = smart_cast<CGameObject*>(D);
		if (!GD)
		{
			Msg("! ERROR: c_EVENT[%d] : non-game-object", id);
			ok = false;
		}

		GO->OnEvent(P, GE_OWNERSHIP_REJECT);
		if (ok)
		{
			Game( ).OnDestroy(GD);
			GD->OnEvent(P, GE_DESTROY);
		}
	}
}

void CLevel::ProcessGameEvents( )
{
	// Game events

	CNetPacket			P;
	u32 svT = timeServer( ) - NET_Latency;

	/*
	if (!game_events->queue.empty())
		Msg("- d[%d],ts[%d] -- E[svT=%d],[evT=%d]",Device.dwTimeGlobal,timeServer(),svT,game_events->queue.begin()->timestamp);
	*/

	while (game_events->available(svT))
	{
		u16 ID, dest, type;
		game_events->get(ID, dest, type, P);

		switch (ID)
		{
			case M_SPAWN:
			{
				u16 dummy16;
				P.r_begin(dummy16);
				cl_Process_Spawn(P);
			}
			break;
			case M_EVENT:
			{
				cl_Process_Event(dest, type, P);
			}
			break;
			default:
			{
				VERIFY(0);
			}
			break;
		}
	}
}

void CLevel::OnFrame( )
{
	m_feel_deny.update( );

	psDeviceFlags.set(rsDisableObjectsAsCrows, false);

	// commit events from bullet manager from prev-frame
	Device.Statistic->TEST0.Begin( );
	BulletManager( ).CommitEvents( );
	Device.Statistic->TEST0.End( );

	Device.Statistic->netClient1.Begin( );

	ClientReceive( );

	Device.Statistic->netClient1.End( );

	ProcessGameEvents( );

	if (m_bNeed_CrPr)
	{
		make_NetCorrectionPrediction( );
	}

	MapManager( ).Update( );
	// Inherited update
	inherited::OnFrame( );

	g_pGamePersistent->Environment( ).SetGameTime(GetEnvironmentGameDayTimeSec( ), GetGameTimeFactor( ));

	ai( ).script_engine( ).script_process(ScriptEngine::eScriptProcessorLevel)->update( );

	m_ph_commander->update( );
	m_ph_commander_scripts->update( );
	//	autosave_manager().update			();

	//���������� ����� ����
	Device.Statistic->TEST0.Begin( );
	BulletManager( ).CommitRenderSet( );
	Device.Statistic->TEST0.End( );

	// update static sounds
	if (g_mt_config.test(mtLevelSounds))
	{
		Device.seqParallel.push_back(fastdelegate::FastDelegate0<>(m_level_sound_manager, &CLevelSoundManager::Update));
	}
	else
	{
		m_level_sound_manager->Update( );
	}

	// deffer LUA-GC-STEP
	if (g_mt_config.test(mtLUA_GC))
	{
		Device.seqParallel.push_back(fastdelegate::FastDelegate0<>(this, &CLevel::script_gc));
	}
	else
	{
		script_gc( );
	}

	//-----------------------------------------------------
	if (pStatGraphR)
	{
		static	f32 fRPC_Mult = 10.0f;
		static	f32 fRPS_Mult = 1.0f;

		pStatGraphR->AppendItem(f32(m_dwRPC) * fRPC_Mult, 0xffff0000, 1);
		pStatGraphR->AppendItem(f32(m_dwRPS) * fRPS_Mult, 0xff00ff00, 0);
	}
}

s32		psLUA_GCSTEP = 10;
void	CLevel::script_gc( )
{
	lua_gc(ai( ).script_engine( ).lua( ), LUA_GCSTEP, psLUA_GCSTEP);
}

#ifdef DEBUG
extern	flags32	dbg_net_Draw_Flags;
#endif

extern void draw_wnds_rects( );

void CLevel::OnRender( )
{
	inherited::OnRender( );

	Game( ).OnRender( );
	//���������� ������ ����
	//Device.Statistic->TEST1.Begin();
	BulletManager( ).Render( );
	//Device.Statistic->TEST1.End();
	//���������� ��������c ������������
	HUD( ).RenderUI( );

	draw_wnds_rects( );

#ifdef DEBUG
	ph_world->OnRender( );
#endif

#ifdef DEBUG
	if (ai( ).get_level_graph( ))
	{
		ai( ).level_graph( ).render( );
	}

	CStalker* stalker = smart_cast<CStalker*>(Level( ).CurrentEntity( ));
	if (stalker)
	{
		stalker->OnRender( );
	}

	if (bDebug)
	{
		for (u32 I = 0; I < Level( ).Objects.o_count( ); I++)
		{
			CObject* _O = Level( ).Objects.o_get_by_iterator(I);

			CPhysicObject* physic_object = smart_cast<CPhysicObject*>(_O);
			if (physic_object)
			{
				physic_object->OnRender( );
			}

			CSpaceRestrictor* space_restrictor = smart_cast<CSpaceRestrictor*>(_O);
			if (space_restrictor)
			{
				space_restrictor->OnRender( );
			}

			CClimableObject* climable = smart_cast<CClimableObject*>	(_O);
			if (climable)
			{
				climable->OnRender( );
			}

			CTeamBaseZone* team_base_zone = smart_cast<CTeamBaseZone*>(_O);
			if (team_base_zone)
			{
				team_base_zone->OnRender( );
			}

			if (dbg_net_Draw_Flags.test(1 << 11)) //draw skeleton
			{
				CGameObject* pGO = smart_cast<CGameObject*>	(_O);
				if (pGO && pGO != Level( ).CurrentViewEntity( ) && !pGO->H_Parent( ))
				{
					if (pGO->Position( ).distance_to_sqr(Device.vCameraPosition) < 400.0f)
					{
						pGO->dbg_DrawSkeleton( );
					}
				}
			}
		}

		if (Server && Server->game)
		{
			Server->game->OnRender( );
		}

		ObjectSpace.dbgRender( );

		//---------------------------------------------------------------------
		HUD( ).Font( ).pFontSmall->OutSet(170.0f, 630.0f);
		HUD( ).Font( ).pFontSmall->SetHeight(16.0f);
		HUD( ).Font( ).pFontSmall->SetColor(0xffff0000);

		if (Server)
		{
			HUD( ).Font( ).pFontSmall->OutNext("Client Objects:      [%d]", Server->GetEntitiesNum( ));
		}

		HUD( ).Font( ).pFontSmall->OutNext("Server Objects:      [%d]", Objects.o_count( ));
		HUD( ).Font( ).pFontSmall->OutNext("Interpolation Steps: [%d]", Level( ).GetInterpolationSteps( ));
		HUD( ).Font( ).pFontSmall->SetHeight(8.0f);
		//---------------------------------------------------------------------
	}
#endif

#ifdef DEBUG
	if (bDebug)
	{
		DBG( ).draw_object_info( );
		DBG( ).draw_text( );
		DBG( ).draw_level_info( );
	}

	debug_renderer( ).render( );

	if (psAI_Flags.is(aiVision))
	{
		for (u32 I = 0; I < Level( ).Objects.o_count( ); I++)
		{
			CObject* object = Objects.o_get_by_iterator(I);
			CStalker* stalker = smart_cast<CStalker*>(object);
			if (!stalker)
			{
				continue;
			}

			stalker->dbg_draw_vision( );
		}
	}

	if (psAI_Flags.test(aiDrawVisibilityRays))
	{
		for (u32 I = 0; I < Level( ).Objects.o_count( ); I++)
		{
			CObject* object = Objects.o_get_by_iterator(I);
			CStalker* stalker = smart_cast<CStalker*>(object);
			if (!stalker)
			{
				continue;
			}

			stalker->dbg_draw_visibility_rays( );
		}
	}
#endif
}

void CLevel::OnEvent(EVENT E, u64 P1, u64 /**P2/**/)
{
	if (E == eEntitySpawn)
	{
		string128	Name;
		Name[0] = 0;
		sscanf(pcstr(P1), "%s", Name);
		Level( ).g_cl_Spawn(Name, 0xff, M_SPAWN_OBJECT_LOCAL, fVector3( ).set(0.0f, 0.0f, 0.0f));
	}
	else if (E == eChangeRP && P1)
	{
	}
	else if (E == eDemoPlay && P1)
	{
		pstr name = (pstr)P1;
		string_path RealName;
		strcpy_s(RealName, name);
		strcat(RealName, ".xrdemo");
		Cameras( ).AddCamEffector(xr_new<CDemoPlay>(RealName, 1.3f, 0));
	}
	else if (E == eChangeTrack && P1)
	{
		// s32 id = atoi((pstr)P1);
		// Environment->Music_Play(id);
	}
	else if (E == eEnvironment)
	{
		// s32 id=0; f32 s=1;
		// sscanf((pstr)P1,"%d,%f",&id,&s);
		// Environment->set_EnvMode(id,s);
	}
	else
	{
		return;
	}
}

void CLevel::AddObject_To_Objects4CrPr(CGameObject* pObj)
{
	if (!pObj)
	{
		return;
	}

	for (OBJECTS_LIST_it OIt = pObjects4CrPr.begin( ); OIt != pObjects4CrPr.end( ); OIt++)
	{
		if (*OIt == pObj)
		{
			return;
		}
	}

	pObjects4CrPr.push_back(pObj);
}

void CLevel::AddActor_To_Actors4CrPr(CGameObject* pActor)
{
	if (!pActor)
	{
		return;
	}

	if (pActor->CLS_ID != CLSID_OBJECT_ACTOR)
	{
		return;
	}

	for (OBJECTS_LIST_it AIt = pActors4CrPr.begin( ); AIt != pActors4CrPr.end( ); AIt++)
	{
		if (*AIt == pActor)
		{
			return;
		}
	}

	pActors4CrPr.push_back(pActor);
}

void CLevel::RemoveObject_From_4CrPr(CGameObject* pObj)
{
	if (!pObj)
	{
		return;
	}

	OBJECTS_LIST_it OIt = std::find(pObjects4CrPr.begin( ), pObjects4CrPr.end( ), pObj);
	if (OIt != pObjects4CrPr.end( ))
	{
		pObjects4CrPr.erase(OIt);
	}

	OBJECTS_LIST_it AIt = std::find(pActors4CrPr.begin( ), pActors4CrPr.end( ), pObj);
	if (AIt != pActors4CrPr.end( ))
	{
		pActors4CrPr.erase(AIt);
	}
}

void CLevel::make_NetCorrectionPrediction( )
{
	m_bNeed_CrPr = false;
	m_bIn_CrPr = true;
	u64 NumPhSteps = ph_world->m_steps_num;
	ph_world->m_steps_num -= m_dwNumSteps;
	if (g_bDebugDumpPhysicsStep && m_dwNumSteps > 10)
	{
		Msg("!!!TOO MANY PHYSICS STEPS FOR CORRECTION PREDICTION = %d !!!", m_dwNumSteps);
		m_dwNumSteps = 10;
	}
	//////////////////////////////////////////////////////////////////////////////////
	ph_world->Freeze( );

	//setting UpdateData and determining number of PH steps from last received update
	for (OBJECTS_LIST_it OIt = pObjects4CrPr.begin( ); OIt != pObjects4CrPr.end( ); OIt++)
	{
		CGameObject* pObj = *OIt;
		if (!pObj)
		{
			continue;
		}

		pObj->PH_B_CrPr( );
	}
	//////////////////////////////////////////////////////////////////////////////////
		//first prediction from "delivered" to "real current" position
		//making enought PH steps to calculate current objects position based on their updated state	

	for (u32 i = 0; i < m_dwNumSteps; i++)
	{
		ph_world->Step( );

		for (OBJECTS_LIST_it AIt = pActors4CrPr.begin( ); AIt != pActors4CrPr.end( ); AIt++)
		{
			CGameObject* pActor = *AIt;
			if (!pActor || pActor->CrPr_IsActivated( ))
			{
				continue;
			}

			pActor->PH_B_CrPr( );
		}
	}
	//////////////////////////////////////////////////////////////////////////////////
	for (OBJECTS_LIST_it OIt = pObjects4CrPr.begin( ); OIt != pObjects4CrPr.end( ); OIt++)
	{
		CGameObject* pObj = *OIt;
		if (!pObj)
		{
			continue;
		}

		pObj->PH_I_CrPr( );
	}
	//////////////////////////////////////////////////////////////////////////////////
	if (!InterpolationDisabled( ))
	{
		for (u32 i = 0; i < lvInterpSteps; i++)	//second prediction "real current" to "future" position
		{
			ph_world->Step( );

#ifdef DEBUG
			/*
						for	(OBJECTS_LIST_it OIt = pObjects4CrPr.begin(); OIt != pObjects4CrPr.end(); OIt++)
						{
							CGameObject* pObj = *OIt;
							if (!pObj) continue;
							pObj->PH_Ch_CrPr();
						};
			*/
#endif

		}
		//////////////////////////////////////////////////////////////////////////////////
		for (OBJECTS_LIST_it OIt = pObjects4CrPr.begin( ); OIt != pObjects4CrPr.end( ); OIt++)
		{
			CGameObject* pObj = *OIt;
			if (!pObj)
			{
				continue;
			}

			pObj->PH_A_CrPr( );
		}
	}

	ph_world->UnFreeze( );

	ph_world->m_steps_num = NumPhSteps;
	m_dwNumSteps = 0;
	m_bIn_CrPr = false;

	pObjects4CrPr.clear( );
	pActors4CrPr.clear( );
}

u32 CLevel::GetInterpolationSteps( )
{
	return lvInterpSteps;
}

void CLevel::UpdateDeltaUpd(u32 LastTime)
{
	u32 CurrentDelta = LastTime - m_dwLastNetUpdateTime;
	if (CurrentDelta < m_dwDeltaUpdate)
	{
		CurrentDelta = iFloor(f32(m_dwDeltaUpdate * 10 + CurrentDelta) / 11);
	}

	m_dwLastNetUpdateTime = LastTime;
	m_dwDeltaUpdate = CurrentDelta;

	if (0 == g_cl_lvInterp)
	{
		ReculcInterpolationSteps( );
	}
	else if (g_cl_lvInterp > 0)
	{
		lvInterpSteps = iCeil(g_cl_lvInterp / fixed_step);
	}
}

void CLevel::ReculcInterpolationSteps( )
{
	lvInterpSteps = iFloor(f32(m_dwDeltaUpdate) / (fixed_step * 1000));
	if (lvInterpSteps > 60)
	{
		lvInterpSteps = 60;
	}

	if (lvInterpSteps < 3)
	{
		lvInterpSteps = 3;
	}
}

bool CLevel::InterpolationDisabled( )
{
	return (g_cl_lvInterp < 0);
}

void CLevel::PhisStepsCallback(u32 Time0, u32 Time1)
{
	return;
}

void CLevel::SetNumCrSteps(u32 NumSteps)
{
	m_bNeed_CrPr = true;
	if (m_dwNumSteps > NumSteps)
	{
		return;
	}

	m_dwNumSteps = NumSteps;
	if (m_dwNumSteps > 1000000)
	{
		VERIFY(0);
	}
}

ALife::_TIME_ID CLevel::GetGameTime( )
{
	return			(game->GetGameTime( ));
}

ALife::_TIME_ID CLevel::GetEnvironmentGameTime( )
{
	return			(game->GetEnvironmentGameTime( ));
}

u8 CLevel::GetDayTime( )
{
	u32 dummy32;
	u32 hours;
	GetGameDateTime(dummy32, dummy32, dummy32, hours, dummy32, dummy32, dummy32);
	VERIFY(hours < 256);
	return	u8(hours);
}

f32 CLevel::GetGameDayTimeSec( )
{
	return	(f32(s64(GetGameTime( ) % (24 * 60 * 60 * 1000))) / 1000.0f);
}

u32 CLevel::GetGameDayTimeMS( )
{
	return	(u32(s64(GetGameTime( ) % (24 * 60 * 60 * 1000))));
}

f32 CLevel::GetEnvironmentGameDayTimeSec( )
{
	return	(f32(s64(GetEnvironmentGameTime( ) % (24 * 60 * 60 * 1000))) / 1000.0f);
}

void CLevel::GetGameDateTime(u32& year, u32& month, u32& day, u32& hours, u32& mins, u32& secs, u32& milisecs)
{
	split_time(GetGameTime( ), year, month, day, hours, mins, secs, milisecs);
}

f32 CLevel::GetGameTimeFactor( )
{
	return game->GetGameTimeFactor( );
}

void CLevel::SetGameTimeFactor(const f32 fTimeFactor)
{
	game->SetGameTimeFactor(fTimeFactor);
}

void CLevel::SetGameTimeFactor(ALife::_TIME_ID GameTime, const f32 fTimeFactor)
{
	game->SetGameTimeFactor(GameTime, fTimeFactor);
}

void CLevel::SetEnvironmentGameTimeFactor(ALife::_TIME_ID GameTime, const f32 fTimeFactor)
{
	game->SetEnvironmentGameTimeFactor(GameTime, fTimeFactor);
}

bool CLevel::IsServer( )
{
	if (IsDemoPlay( ))
	{
		return IsServerDemo( );
	}

	if (!Server)
	{
		return false;
	}

	return (Server->client_Count( ) != 0);
}

bool CLevel::IsClient( )
{
	if (IsDemoPlay( ))
	{
		return IsClientDemo( );
	}

	if (!Server)
	{
		return true;
	}

	return (Server->client_Count( ) == 0);
}

void CLevel::OnSessionTerminate(pcstr reason)
{
	MainMenu( )->OnSessionTerminate(reason);
}

u32	GameID( )
{
	return Game( ).Type( );
}

GlobalFeelTouch::GlobalFeelTouch( )
{ }

GlobalFeelTouch::~GlobalFeelTouch( )
{ }

struct delete_predicate_by_time : public std::binary_function<Feel::Touch::DenyTouch, DWORD, bool>
{
	bool operator () (Feel::Touch::DenyTouch const& left, DWORD const expire_time) const
	{
		if (left.Expire <= expire_time)
		{
			return true;
		}

		return false;
	}
};

struct objects_ptrs_equal : public std::binary_function<Feel::Touch::DenyTouch, CObject const*, bool>
{
	bool operator() (Feel::Touch::DenyTouch const& left, CObject const* const right) const
	{
		if (left.O == right)
		{
			return true;
		}

		return false;
	}
};

void GlobalFeelTouch::update( )
{
	//we ignore P and R arguments, we need just delete evaled denied objects...
	xr_vector<Feel::Touch::DenyTouch>::iterator new_end = std::remove_if(feel_touch_disable.begin( ), feel_touch_disable.end( ), std::bind2nd(delete_predicate_by_time( ), Device.dwTimeGlobal));
	feel_touch_disable.erase(new_end, feel_touch_disable.end( ));
}

bool GlobalFeelTouch::is_object_denied(const CObject* O)
{
	if (std::find_if(feel_touch_disable.begin( ), feel_touch_disable.end( ), std::bind2nd(objects_ptrs_equal( ), O)) == feel_touch_disable.end( ))
	{
		return false;
	}

	return true;
}
