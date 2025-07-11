#include "stdafx.h"

#include "gamepersistent.h"
#include "..\XR_3DA\mesh.h"
#include "..\XR_3DA\Console.h"
#include "gamemtllib.h"
#include "..\XR_3DA\skeletoncustom.h"
#include "Profiler.h"
#include "MainMenu.h"
#include "ui/UICursor.h"
#include "game_base_space.h"
#include "level.h"
#include "ParticlesObject.h"
#include "Actor.h"
#include "WeaponHUD.h"
#include "stalker_animation_data_storage.h"
#include "stalker_velocity_holder.h"

#include "..\XR_3DA\CameraManager.h"

#ifndef MASTER_GOLD
#	include "custommonster.h"
#endif // MASTER_GOLD

#include "ai_debug.h"
#include "xr_level_controller.h"
#include "UI/UIGameTutorial.h"
#include "HolderCustom.h"
#include "game_sv_single.h"
#include "xrServer.h"
#include "StringTable.h"
#include "..\XR_3DA\x_ray.h"

static pvoid	ode_alloc(size_t size)
{
	return xr_malloc(size);
}
static pvoid	ode_realloc(pvoid ptr, size_t oldsize, size_t newsize)
{
	return xr_realloc(ptr, newsize);
}
static void		ode_free(pvoid ptr, size_t size)
{
	return xr_free(ptr);
}

CGamePersistent::CGamePersistent( )
{
	m_game_params.m_e_game_type = GAME_ANY;
	ambient_sound_next_time = 0;
	ambient_effect_next_time = 0;
	ambient_effect_stop_time = 0;
	ambient_particles = 0;

	m_pUI_core = NULL;
	m_pMainMenu = NULL;
	m_intro = NULL;
	m_intro_event.bind(this, &CGamePersistent::start_logo_intro);

#ifdef DEBUG
	m_frame_counter = 0;
	m_last_stats_frame = u32(-2);
#endif // def DEBUG

	dSetAllocHandler(ode_alloc);
	dSetReallocHandler(ode_realloc);
	dSetFreeHandler(ode_free);

	BOOL bDemoMode = (0 != strstr(Core.Params, "-demomode "));
	if (bDemoMode)
	{
		string256	fname;
		pcstr		name = strstr(Core.Params, "-demomode ") + 10;
		sscanf(name, "%s", fname);
		R_ASSERT2(fname[0], "Missing filename for 'demomode'");
		Msg("- playing in demo mode '%s'", fname);
		pDemoFile = FS.r_open(fname);
		Device.seqFrame.Add(this);
		eDemoStart = Engine.Event.Handler_Attach("GAME:demo", this);
		uTime2Change = 0;
	}
	else
	{
		pDemoFile = NULL;
		eDemoStart = NULL;
	}

	CWeaponHUD::CreateSharedContainer( );

	eQuickLoad = Engine.Event.Handler_Attach("Game:QuickLoad", this);
}

CGamePersistent::~CGamePersistent( )
{
	CWeaponHUD::DestroySharedContainer( );
	FS.r_close(pDemoFile);
	Device.seqFrame.Remove(this);
	Engine.Event.Handler_Detach(eDemoStart, this);
	Engine.Event.Handler_Detach(eQuickLoad, this);
}

void CGamePersistent::RegisterModel(IRenderVisual* V)
{
	// Check types
	switch (V->Type)
	{
		case MT_SKELETON_ANIM:
		case MT_SKELETON_RIGID:
		{
			u16 def_idx = GMLib.GetMaterialIdx("default_object");
			R_ASSERT2(GMLib.GetMaterialByIdx(def_idx)->Flags.is(SGameMtl::flDynamic), "'default_object' - must be dynamic");
			CKinematics* K = smart_cast<CKinematics*>(V); VERIFY(K);
			s32 cnt = K->LL_BoneCount( );
			for (u16 k = 0; k < cnt; k++)
			{
				CBoneData& bd = K->LL_GetData(k);
				if (*(bd.game_mtl_name))
				{
					bd.game_mtl_idx = GMLib.GetMaterialIdx(*bd.game_mtl_name);
					R_ASSERT2(GMLib.GetMaterialByIdx(bd.game_mtl_idx)->Flags.is(SGameMtl::flDynamic), "Required dynamic game material");
				}
				else
				{
					bd.game_mtl_idx = def_idx;
				}
			}
		}
		break;
	}
}

extern void clean_game_globals( );
extern void init_game_globals( );

void CGamePersistent::OnAppStart( )
{
	// load game materials
	GMLib.Load( );
	init_game_globals( );
	__super::OnAppStart( );
	m_pUI_core = xr_new<ui_core>( );
	m_pMainMenu = xr_new<CMainMenu>( );
}

void CGamePersistent::OnAppEnd( )
{
	if (m_pMainMenu->IsActive( ))
	{
		m_pMainMenu->Activate(false);
	}

	xr_delete(m_pMainMenu);
	xr_delete(m_pUI_core);

	__super::OnAppEnd( );

	clean_game_globals( );

	GMLib.Unload( );
}

void CGamePersistent::Start(pcstr op)
{
	__super::Start(op);
	m_intro_event.bind(this, &CGamePersistent::start_game_intro);
}

void CGamePersistent::Disconnect( )
{
	CWeaponHUD::CleanSharedContainer( );

	// destroy ambient particles
	CParticlesObject::Destroy(ambient_particles);

	__super::Disconnect( );
	// stop all played emitters
	::Sound->stop_emitters( );
	m_game_params.m_e_game_type = GAME_ANY;
}

void CGamePersistent::OnGameStart( )
{
	__super::OnGameStart( );
	UpdateGameType( );
}

void CGamePersistent::UpdateGameType( )
{
	__super::UpdateGameType( );

	if (!xr_strcmp(m_game_params.m_game_type, "single"))
	{
		m_game_params.m_e_game_type = GAME_SINGLE;
	}
	else
	{
		FATAL("Unsupported game type [%s]", m_game_params.m_game_type);
	}

	g_current_keygroup = _sp;
}

void CGamePersistent::OnGameEnd( )
{
	__super::OnGameEnd( );

	xr_delete(g_stalker_animation_data_storage);
	xr_delete(g_stalker_velocity_holder);

	CWeaponHUD::CleanSharedContainer( );
}

void CGamePersistent::WeathersUpdate( )
{
	if (g_pGameLevel)
	{
		CActor* actor = smart_cast<CActor*>(Level( ).CurrentViewEntity( ));
		BOOL bIndoor = TRUE;
		if (actor)
		{
			bIndoor = actor->renderable_ROS( )->get_luminocity_hemi( ) < 0.05f;
		}

		s32 data_set = (Random.randF( ) < (1.0f - Environment( ).CurrentEnv.weight)) ? 0 : 1;
		CEnvDescriptor* _env = Environment( ).Current[data_set]; VERIFY(_env);
		CEnvAmbient* env_amb = _env->env_ambient;
		if (env_amb)
		{
			// start sound
			if (Device.dwTimeGlobal > ambient_sound_next_time)
			{
				ref_sound* snd = env_amb->get_rnd_sound( );
				ambient_sound_next_time = Device.dwTimeGlobal + env_amb->get_rnd_sound_time( );
				if (snd)
				{
					fVector3 pos;
					f32 angle = ::Random.randF(PI_MUL_2);
					pos.x = _cos(angle);
					pos.y = 0.0f;
					pos.z = _sin(angle);
					pos.normalize( ).mul(env_amb->get_rnd_sound_dist( )).add(Device.vCameraPosition);
					pos.y += 10.0f;
					snd->play_at_pos(0, pos);
				}
			}

			// start effect
			if ((FALSE == bIndoor) && (0 == ambient_particles) && Device.dwTimeGlobal > ambient_effect_next_time)
			{
				CEnvAmbient::SEffect* eff = env_amb->get_rnd_effect( );
				if (eff)
				{
					Environment( ).wind_gust_factor = eff->wind_gust_factor;
					ambient_effect_next_time = Device.dwTimeGlobal + env_amb->get_rnd_effect_time( );
					ambient_effect_stop_time = Device.dwTimeGlobal + eff->life_time;
					ambient_particles = CParticlesObject::Create(eff->particles.c_str( ), FALSE, false);
					fVector3 pos;
					pos.add(Device.vCameraPosition, eff->offset);
					ambient_particles->play_at_pos(pos);
					if (eff->sound._handle( ))
					{
						eff->sound.play_at_pos(0, pos);
					}
				}
			}
		}
		// stop if time exceed or indoor
		if (bIndoor || Device.dwTimeGlobal >= ambient_effect_stop_time)
		{
			if (ambient_particles)
			{
				ambient_particles->Stop( );
			}

			Environment( ).wind_gust_factor = 0.0f;
		}
		// if particles not playing - destroy
		if (ambient_particles && !ambient_particles->IsPlaying( ))
		{
			CParticlesObject::Destroy(ambient_particles);
		}
	}
}

void CGamePersistent::start_logo_intro( )
{
	// ��������� ����� ���� ��������
//	/*
//#if 1//def DEBUG
//	if (0!=strstr(Core.Params,"-nointro")){
	m_intro_event = 0;
	Console->Show( );
	Console->Execute("main_menu on");
	return;
	//	}
	//#endif
	//	if (Device.dwPrecacheFrame==0)
	//	{
	//		m_intro_event.bind		(this,&CGamePersistent::update_logo_intro);
	//		if (0==xr_strlen(m_game_params.m_game_or_spawn) && NULL==g_pGameLevel)
	//		{
	//			VERIFY				(NULL==m_intro);
	//			m_intro				= xr_new<CUISequencer>();
	//			m_intro->Start		("intro_logo");
	//			Console->Hide		();
	//		}
	//	}
	//*/
}

void CGamePersistent::update_logo_intro( )
{
	if (m_intro && (false == m_intro->IsActive( )))
	{
		m_intro_event = 0;
		xr_delete(m_intro);
		Console->Execute("main_menu on");
	}
}

void CGamePersistent::start_game_intro( )
{
	// ��������� ����� ���� ��������
///*
//#if 1//def DEBUG
//	if (0!=strstr(Core.Params,"-nointro")){
	m_intro_event = 0;
	return;
	//	}
	//#endif
	//	if (g_pGameLevel && g_pGameLevel->bReady && Device.dwPrecacheFrame<=2){
	//		m_intro_event.bind		(this,&CGamePersistent::update_game_intro);
	//		if (0==stricmp(m_game_params.m_new_or_load,"new")){
	//			VERIFY				(NULL==m_intro);
	//			m_intro				= xr_new<CUISequencer>();
	//			m_intro->Start		("intro_game");
	//			Log("Intro start",Device.dwFrame);
	//		}
	//	}
	//	*/
}

void CGamePersistent::update_game_intro( )
{
	if (m_intro && (false == m_intro->IsActive( )))
	{
		xr_delete(m_intro);
		m_intro_event = 0;
	}
}

extern CUISequencer* g_tutorial;
extern CUISequencer* g_tutorial2;

void CGamePersistent::OnFrame( )
{
	if (g_tutorial2)
	{
		g_tutorial2->Destroy( );
		xr_delete(g_tutorial2);
	}

	if (g_tutorial && !g_tutorial->IsActive( ))
	{
		xr_delete(g_tutorial);
	}

#ifdef DEBUG
	++m_frame_counter;
#endif // def DEBUG

	if (!m_intro_event.empty( ))
	{
		m_intro_event( );
	}

	if (!m_pMainMenu->IsActive( ))
	{
		m_pMainMenu->DestroyInternal(false);
	}

	if (!g_pGameLevel)
	{
		return;
	}

	if (!g_pGameLevel->bReady)
	{
		return;
	}

	if (Device.Paused( ))
	{

#ifndef MASTER_GOLD
		if (Level( ).CurrentViewEntity( ))
		{
			if (!g_actor || (g_actor->ID( ) != Level( ).CurrentViewEntity( )->ID( )))
			{
				CCustomMonster* custom_monster = smart_cast<CCustomMonster*>(Level( ).CurrentViewEntity( ));
				if (custom_monster) // can be spectator in multiplayer
				{
					custom_monster->UpdateCamera( );
				}
			}
			else
			{
				CCameraBase* C = NULL;
				if (g_actor)
				{
					if (!Actor( )->Holder( ))
					{
						C = Actor( )->cam_Active( );
					}
					else
					{
						C = Actor( )->Holder( )->Camera( );
					}

					Actor( )->Cameras( ).Update(C);
					Actor( )->Cameras( ).ApplyDevice(VIEWPORT_NEAR);
				}
			}
		}
#else // ndef MASTER_GOLD
		if (g_actor)
		{
			CCameraBase* C = NULL;
			if (!Actor( )->Holder( ))
			{
				C = Actor( )->cam_Active( );
			}
			else
			{
				C = Actor( )->Holder( )->Camera( );
			}

			Actor( )->Cameras( ).Update(C);
			Actor( )->Cameras( ).ApplyDevice(VIEWPORT_NEAR);
		}
#endif // ndef MASTER_GOLD

	}
	__super::OnFrame( );

	if (!Device.Paused( ))
	{
		Engine.Sheduler.Update( );
	}

	// update weathers ambient
	if (!Device.Paused( ))
	{
		WeathersUpdate( );
	}

	if (0 != pDemoFile)
	{
		if (Device.dwTimeGlobal > uTime2Change)
		{
			// Change level + play demo
			if (pDemoFile->elapsed( ) < 3)
			{	// cycle
				pDemoFile->seek(0);
			}

			// Read params
			string512 params;
			pDemoFile->r_string(params, sizeof(params));
			string256			o_server;
			string256			o_client;
			string256			o_demo;
			u32 o_time;
			sscanf(params, "%[^,],%[^,],%[^,],%d", o_server, o_client, o_demo, &o_time);

			// Start _new level + demo
			Engine.Event.Defer("KERNEL:disconnect");
			Engine.Event.Defer("KERNEL:start", size_t(xr_strdup(_Trim(o_server))), size_t(xr_strdup(_Trim(o_client))));
			Engine.Event.Defer("GAME:demo", size_t(xr_strdup(_Trim(o_demo))), u64(o_time));
			uTime2Change = 0xffffffff;	// Block changer until Event received
		}
	}

#ifdef DEBUG
	if ((m_last_stats_frame + 1) < m_frame_counter)
	{
		profiler( ).clear( );
	}
#endif // def DEBUG

}

void CGamePersistent::OnEvent(EVENT E, u64 P1, u64 P2)
{
	if (E == eQuickLoad)
	{
		if (Device.Paused( ))
		{
			Device.Pause(FALSE, TRUE, TRUE, "eQuickLoad");
		}

		pstr saved_name = (pstr) (P1);

		Level( ).remove_objects( );
		game_sv_Single* game = smart_cast<game_sv_Single*>(Level( ).Server->game);
		R_ASSERT(game);
		game->restart_simulator(saved_name);
		xr_free(saved_name);
		return;
	}
	else if (E == eDemoStart)
	{
		string256			cmd;
		pcstr				demo = pcstr(P1);
		sprintf_s(cmd, "demo_play %s", demo);
		Console->Execute(cmd);
		xr_free(demo);
		uTime2Change = Device.TimerAsync( ) + u32(P2) * 1000;
	}
}

void CGamePersistent::Statistics(CGameFont* F)
{

#ifdef DEBUG
	m_last_stats_frame = m_frame_counter;
	profiler( ).show_stats(F, !!psAI_Flags.test(aiStats));
#endif // def DEBUG

}

f32 CGamePersistent::MtlTransparent(u32 mtl_idx)
{
	return GMLib.GetMaterialByIdx((u16) mtl_idx)->fVisTransparencyFactor;
}

static BOOL bRestorePause = FALSE;
static BOOL bEntryFlag = TRUE;

void CGamePersistent::OnAppActivate( )
{
	Device.Pause(FALSE, !bRestorePause, TRUE, "CGP::OnAppActivate");

	bEntryFlag = TRUE;
}

void CGamePersistent::OnAppDeactivate( )
{
	if (!bEntryFlag)
	{
		return;
	}

	bRestorePause = Device.Paused( );
	Device.Pause(TRUE, TRUE, TRUE, "CGP::OnAppDeactivate");

	bEntryFlag = FALSE;
}

bool CGamePersistent::OnRenderPPUI_query( )
{
	return MainMenu( )->OnRenderPPUI_query( );
	// enable PP or not
}

void CGamePersistent::OnRenderPPUI_main( )
{
	// always
	MainMenu( )->OnRenderPPUI_main( );
}

void CGamePersistent::OnRenderPPUI_PP( )
{
	MainMenu( )->OnRenderPPUI_PP( );
}

void CGamePersistent::LoadTitle(pcstr str)
{
	string512			buff;
	sprintf_s(buff, "%s...", CStringTable( ).translate(str).c_str( ));
	pApp->LoadTitleInt(buff);
}
