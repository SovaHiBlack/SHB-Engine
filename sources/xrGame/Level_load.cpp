#include "stdafx.h"
#include "HUDmanager.h"
#include "LevelGameDef.h"
#include "AISpace.h"
#include "ParticlesObject.h"
#include "script_process.h"
#include "script_engine.h"
#include "script_engine_space.h"
#include "level.h"
#include "game_cl_base.h"
#include "..\XR_3DA\x_ray.h"
#include "gamemtllib.h"
#include "PhysicsCommon.h"
#include "level_sounds.h"
#include "GamePersistent.h"

BOOL CLevel::Load_GameSpecific_Before( )
{
	// AI space
	g_pGamePersistent->LoadTitle("st_loading_ai_objects");
	string_path							fn_game;

	if (!ai( ).get_alife( ) && FS.exist(fn_game, "$level$", "level.ai"))
	{
		ai( ).load(net_SessionName( ));
	}

	if (!ai( ).get_alife( ) && ai( ).get_game_graph( ) && FS.exist(fn_game, "$level$", "level.game"))
	{
		IReader* stream = FS.r_open(fn_game);
		ai( ).patrol_path_storage_raw(*stream);
		FS.r_close(stream);
	}

	return TRUE;
}

BOOL CLevel::Load_GameSpecific_After( )
{
	// loading static particles
	string_path		fn_game;
	if (FS.exist(fn_game, "$level$", "level.ps_static"))
	{
		IReader* F = FS.r_open(fn_game);
		CParticlesObject* pStaticParticles;
		u32				chunk = 0;
		string256		ref_name;
		fMatrix4x4			transform;
		fVector3			zero_vel = {0.0f,0.0f,0.0f};
		for (IReader* OBJ = F->open_chunk_iterator(chunk); OBJ; OBJ = F->open_chunk_iterator(chunk, OBJ))
		{
			OBJ->r_stringZ(ref_name, sizeof(ref_name));
			OBJ->r(&transform, sizeof(fMatrix4x4));
			transform.c.y += 0.01f;
			pStaticParticles = CParticlesObject::Create(ref_name, FALSE, false);
			pStaticParticles->UpdateParent(transform, zero_vel);
			pStaticParticles->Play( );
			m_StaticParticles.push_back(pStaticParticles);
		}

		FS.r_close(F);
	}

	// loading static sounds
	VERIFY(m_level_sound_manager);
	m_level_sound_manager->Load( );

	// loading sound environment
	if (FS.exist(fn_game, "$level$", "level.snd_env"))
	{
		IReader* F = FS.r_open(fn_game);
		::Sound->set_geometry_env(F);
		FS.r_close(F);
	}

	// loading SOM
	if (FS.exist(fn_game, "$level$", "level.som"))
	{
		IReader* F = FS.r_open(fn_game);
		::Sound->set_geometry_som(F);
		FS.r_close(F);
	}

	// loading random (around player) sounds
	if (pSettings->section_exist("sounds_random"))
	{
		CIniFile::Sect& S = pSettings->r_section("sounds_random");
		Sounds_Random.reserve(S.Data.size( ));
		for (CIniFile::SectCIt I = S.Data.begin( ); S.Data.end( ) != I; ++I)
		{
			Sounds_Random.push_back(ref_sound( ));
			Sound->create(Sounds_Random.back( ), *I->first, st_Effect, sg_SourceType);
		}

		Sounds_Random_dwNextTime = Device.TimerAsync( ) + 50000;
		Sounds_Random_Enabled = FALSE;
	}

	// loading scripts
	ai( ).script_engine( ).remove_script_process(ScriptEngine::eScriptProcessorLevel);

	if (pLevel->section_exist("level_scripts") && pLevel->line_exist("level_scripts", "script"))
	{
		ai( ).script_engine( ).add_script_process(ScriptEngine::eScriptProcessorLevel, xr_new<CScriptProcess>("level", pLevel->r_string("level_scripts", "script")));
	}
	else
	{
		ai( ).script_engine( ).add_script_process(ScriptEngine::eScriptProcessorLevel, xr_new<CScriptProcess>("level", ""));
	}

	return TRUE;
}

struct translation_pair
{
	u32			m_id;
	u16			m_index;

	IC			translation_pair(u32 id, u16 index)
	{
		m_id = id;
		m_index = index;
	}

	IC	bool	operator==	(const u16& id) const
	{
		return	(m_id == id);
	}

	IC	bool	operator<	(const translation_pair& pair) const
	{
		return	(m_id < pair.m_id);
	}

	IC	bool	operator<	(const u16& id) const
	{
		return	(m_id < id);
	}
};

void CLevel::Load_GameSpecific_CFORM(CDB::TRI* tris, u32 count)
{
	typedef xr_vector<translation_pair>	ID_INDEX_PAIRS;
	ID_INDEX_PAIRS						translator;
	translator.reserve(GMLib.CountMaterial( ));
	u16									default_id = (u16)GMLib.GetMaterialIdx("default");
	translator.push_back(translation_pair(u32(-1), default_id));

	u16									index = 0, static_mtl_count = 1;
	s32 max_ID = 0;
	s32 max_static_ID = 0;
	for (GameMtlIt I = GMLib.FirstMaterial( ); GMLib.LastMaterial( ) != I; ++I, ++index)
	{
		if (!(*I)->Flags.test(SGameMtl::flDynamic))
		{
			++static_mtl_count;
			translator.push_back(translation_pair((*I)->GetID( ), index));
			if ((*I)->GetID( ) > max_static_ID)
			{
				max_static_ID = (*I)->GetID( );
			}
		}

		if ((*I)->GetID( ) > max_ID)
		{
			max_ID = (*I)->GetID( );
		}
	}

	// Msg("* Material remapping ID: [Max:%d, StaticMax:%d]",max_ID,max_static_ID);
	VERIFY(max_static_ID < 0xFFFF);

	if (static_mtl_count < 128)
	{
		CDB::TRI* I = tris;
		CDB::TRI* E = tris + count;
		for (; I != E; ++I)
		{
			ID_INDEX_PAIRS::iterator	i = std::find(translator.begin( ), translator.end( ), (u16)(*I).material);
			if (i != translator.end( ))
			{
				(*I).material = (*i).m_index;
				SGameMtl* mtl = GMLib.GetMaterialByIdx((*i).m_index);
				(*I).suppress_shadows = mtl->Flags.is(SGameMtl::flSuppressShadows);
				(*I).suppress_wm = mtl->Flags.is(SGameMtl::flSuppressWallmarks);
				continue;
			}

			Debug.fatal(DEBUG_INFO, "Game material '%d' not found", (*I).material);
		}

		return;
	}

	std::sort(translator.begin( ), translator.end( ));
	{
		CDB::TRI* I = tris;
		CDB::TRI* E = tris + count;
		for (; I != E; ++I)
		{
			ID_INDEX_PAIRS::iterator	i = std::lower_bound(translator.begin( ), translator.end( ), (u16)(*I).material);
			if ((i != translator.end( )) && ((*i).m_id == (*I).material))
			{
				(*I).material = (*i).m_index;
				SGameMtl* mtl = GMLib.GetMaterialByIdx((*i).m_index);
				(*I).suppress_shadows = mtl->Flags.is(SGameMtl::flSuppressShadows);
				(*I).suppress_wm = mtl->Flags.is(SGameMtl::flSuppressWallmarks);
				continue;
			}

			Debug.fatal(DEBUG_INFO, "Game material '%d' not found", (*I).material);
		}
	}
}
