#include "stdafx.h"

#include "..\XR_3DA\Console.h"
#include "..\XR_3DA\ConsoleCommand.h"
#include "..\XR_3DA\customhud.h"
#include "..\XR_3DA\DemoRecord.h"
#include "..\XR_3DA\DemoPlay.h"
#include "xrMessages.h"
#include "xrserver.h"
#include "level.h"
#include "script_debugger.h"
#include "ai_debug.h"
#include "alife_simulator.h"
#include "game_cl_base.h"
#include "game_cl_single.h"
#include "game_sv_single.h"
#include "Hit.h"
#include "PHDestroyable.h"
#include "Actor.h"
//#include "Actor_flags.h"
#include "CustomZone.h"
#include "script_engine.h"
#include "script_engine_space.h"
#include "script_process.h"
#include "xrServer_Objects.h"
#include "ui/UIMainIngameWnd.h"//
#include "PhysicsGamePars.h"
#include "phworld.h"
#include "StringTable.h"
#include "AutosaveManager.h"
#include "AISpace.h"
#include "ai/monsters/BaseMonster/base_monster.h"
#include "date_time.h"
#include "mt_config.h"
#include "ZoneEffector.h"
#include "GameTask.h"
#include "MainMenu.h"
#include "saved_game_wrapper.h"
#include "level_graph.h"
#include "..\XR_3DA\resourcemanager.h"
#include "doug_lea_memory_allocator.h"
#include "CameraLook.h"

#ifdef DEBUG
#	include "PHDebug.h"
#	include "ui/UIDebugFonts.h"//
#	include "game_graph.h"
#endif // DEBUG

#include "hudmanager.h"
#include "ui/UIGameCustom.h"
#include "MapManager.h"
#include "alife_graph_registry.h"
#include "GamePersistent.h"
#include "AttachableItem.h"
#include "AttachmentOwner.h"

string_path g_last_saved_game;

extern void show_smart_cast_stats( );
extern void clear_smart_cast_stats( );
extern void release_smart_cast_stats( );

extern u64		g_qwStartGameTime;
extern u64		g_qwEStartGameTime;

ENGINE_API
extern f32		psHUD_FOV;
extern f32		psSqueezeVelocity;
extern s32		psLUA_GCSTEP;

extern s32		x_m_x;
extern s32		x_m_z;
extern BOOL		net_cl_inputguaranteed;
extern BOOL		net_sv_control_hit;
extern s32		g_dwInputUpdateDelta;

#ifdef DEBUG
extern	BOOL	g_ShowAnimationInfo;
#endif // DEBUG

extern	BOOL	g_bShowHitSectors;
extern	BOOL	g_bDebugDumpPhysicsStep;
extern	ESingleGameDifficulty g_SingleGameDifficulty;
extern	BOOL	g_show_wnd_rect;
extern	BOOL	g_show_wnd_rect2;
//-----------------------------------------------------------
extern f32		g_fTimeFactor;

BOOL	g_bCheckTime = FALSE;
s32		net_cl_inputupdaterate = 50;
flags32	g_mt_config = {mtLevelPath | mtDetailPath | mtObjectHandler | mtSoundPlayer | mtAiVision | mtBullets | mtLUA_GC | mtLevelSounds | mtALife};

#ifdef DEBUG
flags32	dbg_net_Draw_Flags = {0};
#endif

#ifdef DEBUG
BOOL	g_bDebugNode = FALSE;
u32		g_dwDebugNodeSource = 0;
u32		g_dwDebugNodeDest = 0;
extern	BOOL	g_bDrawBulletHit;

f32	debug_on_frame_gather_stats_frequency = 0.0f;
#endif

#ifdef DEBUG 
extern pstr	dbg_stalker_death_anim;
extern BOOL		b_death_anim_velocity;
#endif

s32 g_AI_inactive_time = 0;
flags32 g_uCommonFlags;
enum E_COMMON_FLAGS
{
	flAiUseTorchDynamicLights = 1
};

#ifndef PURE_ALLOC
#	ifndef USE_MEMORY_MONITOR
#		define SEVERAL_ALLOCATORS
#	endif // USE_MEMORY_MONITOR
#endif // PURE_ALLOC

#ifdef SEVERAL_ALLOCATORS
ENGINE_API 	u32 engine_lua_memory_usage( );
extern		u32 game_lua_memory_usage( );
#endif // SEVERAL_ALLOCATORS

class CCC_MemStats : public IConsoleCommand
{
public:
	CCC_MemStats(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		Memory.mem_compact( );
		u32		_process_heap = mem_usage_impl(nullptr, nullptr);

#ifdef SEVERAL_ALLOCATORS
		u32		_game_lua = game_lua_memory_usage( );
		u32		_engine_lua = engine_lua_memory_usage( );
		u32		_render = ::Render->memory_usage( );
#endif // SEVERAL_ALLOCATORS

		s32		_eco_strings = (s32)g_pStringContainer->stat_economy( );
		s32		_eco_smem = (s32)g_pSharedMemoryContainer->stat_economy( );
		u32		m_base = 0;
		u32		c_base = 0;
		u32		m_lmaps = 0;
		u32		c_lmaps = 0;

		if (Device.Resources)
		{
			Device.Resources->_GetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);
		}

		log_vminfo( );

		Msg("* [ D3D ]: textures[%d K]", (m_base + m_lmaps) / 1024);

#ifndef SEVERAL_ALLOCATORS
		Msg("* [x-ray]: process heap[%d K]", _process_heap / 1024);
#else // SEVERAL_ALLOCATORS
		Msg("* [x-ray]: process heap[%d K], game lua[%d K], engine lua[%d K], render[%d K]", _process_heap / 1024, _game_lua / 1024, _engine_lua / 1024, _render / 1024);
#endif // SEVERAL_ALLOCATORS

		Msg("* [x-ray]: economy: strings[%d K], smem[%d K]", _eco_strings / 1024, _eco_smem);

#ifdef DEBUG
		Msg("* [x-ray]: file mapping: memory[%d K], count[%d]", g_file_mapped_memory / 1024, g_file_mapped_count);
		dump_file_mappings( );
#endif // DEBUG

	}
};

// console commands
class CCC_GameDifficulty : public CCC_Token
{
public:
	CCC_GameDifficulty(pcstr N) : CCC_Token(N, (u32*)&g_SingleGameDifficulty, difficulty_type_token)
	{ }

	virtual void Execute(pcstr args)
	{
		CCC_Token::Execute(args);
		if (g_pGameLevel && Level( ).game)
		{
			game_cl_Single* game = smart_cast<game_cl_Single*>(Level( ).game);
			VERIFY(game);
			game->OnDifficultyChanged( );
		}
	}

	virtual void Info(TInfo& I)
	{
		strcpy_s(I, "game difficulty");
	}
};

#ifdef DEBUG
class CCC_ALifePath : public IConsoleCommand
{
public:
	CCC_ALifePath(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		if (!ai( ).get_level_graph( ))
		{
			Msg("! there is no graph!");
		}
		else
		{
			s32 id1 = -1;
			s32 id2 = -1;
			sscanf(args, "%d %d", &id1, &id2);
			if ((-1 != id1) && (-1 != id2))
			{
				if (_max(id1, id2) > (s32)ai( ).game_graph( ).header( ).vertex_count( ) - 1)
				{
					Msg("! there are only %d vertexes!", ai( ).game_graph( ).header( ).vertex_count( ));
				}
				else if (_min(id1, id2) < 0)
				{
					Msg("! invalid vertex number (%d)!", _min(id1, id2));
				}
			}
			else
			{
				Msg("! not enough parameters!");
			}
		}
	}
};
#endif // DEBUG

class CCC_ALifeTimeFactor : public IConsoleCommand
{
public:
	CCC_ALifeTimeFactor(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		f32 id1 = 0.0f;
		sscanf(args, "%f", &id1);
		if (id1 < EPSILON_3)
		{
			Msg("Invalid time factor! (%.4f)", id1);
		}
		else
		{
			if (!OnServer( ))
			{
				return;
			}

			Level( ).Server->game->SetGameTimeFactor(id1);
		}
	}
};

class CCC_ALifeSwitchDistance : public IConsoleCommand
{
public:
	CCC_ALifeSwitchDistance(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		if (ai( ).get_alife( ))
		{
			f32 id1 = 0.0f;
			sscanf(args, "%f", &id1);
			if (id1 < 2.0f)
			{
				Msg("Invalid online distance! (%.4f)", id1);
			}
			else
			{
				CNetPacket P;
				P.w_begin(M_SWITCH_DISTANCE);
				P.w_float(id1);
				Level( ).Send(P, net_flags(TRUE, TRUE));
			}
		}
		else
		{
			Log("!Not a single player game!");
		}
	}
};

class CCC_ALifeProcessTime : public IConsoleCommand
{
public:
	CCC_ALifeProcessTime(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		if (ai( ).get_alife( ))
		{
			game_sv_Single* tpGame = smart_cast<game_sv_Single*>(Level( ).Server->game);
			VERIFY(tpGame);
			s32 id1 = 0;
			sscanf(args, "%d", &id1);
			if (id1 < 1)
			{
				Msg("Invalid process time! (%d)", id1);
			}
			else
			{
				tpGame->alife( ).set_process_time(id1);
			}
		}
		else
		{
			Log("!Not a single player game!");
		}
	}
};

class CCC_ALifeObjectsPerUpdate : public IConsoleCommand
{
public:
	CCC_ALifeObjectsPerUpdate(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		if (ai( ).get_alife( ))
		{
			game_sv_Single* tpGame = smart_cast<game_sv_Single*>(Level( ).Server->game);
			VERIFY(tpGame);
			s32 id1 = 0;
			sscanf(args, "%d", &id1);
			tpGame->alife( ).objects_per_update(id1);
		}
		else
		{
			Log("!Not a single player game!");
		}
	}
};

class CCC_ALifeSwitchFactor : public IConsoleCommand
{
public:
	CCC_ALifeSwitchFactor(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		if (ai( ).get_alife( ))
		{
			game_sv_Single* tpGame = smart_cast<game_sv_Single*>(Level( ).Server->game);
			VERIFY(tpGame);
			f32 id1 = 0.0f;
			sscanf(args, "%f", &id1);
			clamp(id1, 0.1f, 1.0f);
			tpGame->alife( ).set_switch_factor(id1);
		}
		else
		{
			Log("!Not a single player game!");
		}
	}
};

#ifndef MASTER_GOLD
class CCC_TimeFactor : public IConsoleCommand
{
public:
	CCC_TimeFactor(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		f32 time_factor = (f32)atof(args);
		clamp(time_factor, 0.001f, 1000.0f);
		Device.time_factor(time_factor);
	}
};
#endif // MASTER_GOLD

//-----------------------------------------------------------------------
class CCC_DemoRecord : public IConsoleCommand
{
public:
	CCC_DemoRecord(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		if (!g_pGameLevel)
		{	// level not loaded
			Log("Demo Record is disabled when level is not loaded.");
			return;
		}

		Console->Hide( );

		if (MainMenu( )->IsActive( ))
		{
			MainMenu( )->Activate(false);
		}

		string_path fn_;
		strconcat(sizeof(fn_), fn_, args, ".xrdemo");
		string_path fn;
		FS.update_path(fn, "$game_saves$", fn_);

		g_pGameLevel->Cameras( ).AddCamEffector(xr_new<CDemoRecord>(fn));
	}
};

class CCC_DemoPlay : public IConsoleCommand
{
public:
	CCC_DemoPlay(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		if (0 == g_pGameLevel)
		{
			Msg("! There are no level(s) started");
		}
		else
		{
			Console->Hide( );
			string_path fn;
			u32 loops = 0;
			pstr comma = strchr(const_cast<pstr>(args), ',');
			if (comma)
			{
				loops = atoi(comma + 1);
				*comma = 0;	//. :)
			}

			strconcat(sizeof(fn), fn, args, ".xrdemo");
			FS.update_path(fn, "$game_saves$", fn);
			g_pGameLevel->Cameras( ).AddCamEffector(xr_new<CDemoPlay>(fn, 1.0f, loops));
		}
	}
};

bool valid_file_name(pcstr file_name)
{
	pcstr I = file_name;
	pcstr E = file_name + xr_strlen(file_name);
	for (; I != E; ++I)
	{
		if (!strchr("/\\:*?\"<>|", *I))
		{
			continue;
		}

		return false;
	}

	return true;
}

class CCC_ALifeSave : public IConsoleCommand
{
public:
	CCC_ALifeSave(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		if (!g_actor || !Actor( )->g_Alive( ))
		{
			Msg("cannot make saved game because actor is dead :(");
			return;
		}

		string_path S;
		string_path S1;
		S[0] = 0;
		strcpy_s(S, args);

#ifdef DEBUG
		CTimer timer;
		timer.Start( );
#endif
		if (!xr_strlen(S))
		{
			strconcat(sizeof(S), S, Core.UserName, "_", "quicksave");
			CNetPacket net_packet;
			net_packet.w_begin(M_SAVE_GAME);
			net_packet.w_stringZ(S);
			net_packet.w_u8(0);
			Level( ).Send(net_packet, net_flags(TRUE));
		}
		else
		{
			if (!valid_file_name(S))
			{
				Msg("invalid file name");
				return;
			}

			CNetPacket net_packet;
			net_packet.w_begin(M_SAVE_GAME);
			net_packet.w_stringZ(S);
			net_packet.w_u8(1);
			Level( ).Send(net_packet, net_flags(TRUE));
		}

#ifdef DEBUG
		Msg("Game save overhead  : %f milliseconds", timer.GetElapsed_sec( ) * 1000.0f);
#endif

		SDrawStaticStruct* _s = HUD( ).GetUI( )->UIGame( )->AddCustomStatic("game_saved", true);
		_s->m_endTime = Device.fTimeGlobal + 3.0f;// 3sec
		string_path save_name;
		strconcat(sizeof(save_name), save_name, *CStringTable( ).translate("st_game_saved"), ": ", S);
		_s->wnd( )->SetText(save_name);

		strcat(S, ".dds");
		FS.update_path(S1, "$game_saves$", S);

#ifdef DEBUG
		timer.Start( );
#endif

		MainMenu( )->Screenshot(IRender_interface::SM_FOR_GAMESAVE, S1);

#ifdef DEBUG
		Msg("Screenshot overhead : %f milliseconds", timer.GetElapsed_sec( ) * 1000.0f);
#endif

	}
};

class CCC_ALifeLoadFrom : public IConsoleCommand
{
public:
	CCC_ALifeLoadFrom(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		if (!ai( ).get_alife( ))
		{
			Log("! ALife simulator has not been started yet");
			return;
		}

		string256 saved_game;
		saved_game[0] = 0;
		strcpy_s(saved_game, args);
		if (!xr_strlen(saved_game))
		{
			Log("! Specify file name!");
			return;
		}

		if (!CSavedGameWrapper::saved_game_exist(saved_game))
		{
			Msg("! Cannot find saved game %s", saved_game);
			return;
		}

		if (!CSavedGameWrapper::valid_saved_game(saved_game))
		{
			Msg("! Cannot load saved game %s, version mismatch or saved game is corrupted", saved_game);
			return;
		}

		if (MainMenu( )->IsActive( ))
		{
			MainMenu( )->Activate(false);
		}

		if (Device.Paused( ))
		{
			Device.Pause(FALSE, TRUE, TRUE, "CCC_ALifeLoadFrom");
		}

		CNetPacket net_packet;
		net_packet.w_begin(M_LOAD_GAME);
		net_packet.w_stringZ(saved_game);
		Level( ).Send(net_packet, net_flags(TRUE));
	}
};

class CCC_LoadLastSave : public IConsoleCommand
{
public:
	CCC_LoadLastSave(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		if (args && *args)
		{
			strcpy_s(g_last_saved_game, args);
			return;
		}

		if (!*g_last_saved_game)
		{
			Msg("! cannot load last saved game since it hasn't been specified");
			return;
		}

		string512 command;
		if (ai( ).get_alife( ))
		{
			strconcat(sizeof(command), command, "load ", g_last_saved_game);
			Console->Execute(command);
			return;
		}

		strconcat(sizeof(command), command, "start server(", g_last_saved_game, "/single/alife/load)");
		Console->Execute(command);
	}

	virtual void Save(IWriter* F)
	{
		if (!*g_last_saved_game)
		{
			return;
		}

		F->w_printf("%s %s\r\n", cName, g_last_saved_game);
	}
};

class CCC_FlushLog : public IConsoleCommand
{
public:
	CCC_FlushLog(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr /*args*/)
	{
		FlushLog( );
		Msg("* Log file has been saved successfully!");
	}
};

class CCC_ClearLog : public IConsoleCommand
{
public:
	CCC_ClearLog(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr)
	{
		LogFile->clear_not_free( );
		FlushLog( );
		Msg("* Log file has been cleaned successfully!");
	}
};

class CCC_FloatBlock : public CCC_Float
{
public:
	CCC_FloatBlock(pcstr N, f32* V, f32 _min = 0.0f, f32 _max = 1.0f) : CCC_Float(N, V, _min, _max)
	{ }

	virtual void Execute(pcstr args)
	{
		CCC_Float::Execute(args);
	}
};

class CCC_Net_CL_InputUpdateRate : public CCC_Integer
{
protected:
	s32* value_blin;

public:
	CCC_Net_CL_InputUpdateRate(pcstr N, s32* V, s32 _min = 0, s32 _max = 999) : CCC_Integer(N, V, _min, _max), value_blin(V)
	{ }

	virtual void Execute(pcstr args)
	{
		CCC_Integer::Execute(args);
		if ((*value_blin > 0) && g_pGameLevel)
		{
			g_dwInputUpdateDelta = 1000 / (*value_blin);
		}
	}
};

#ifndef MASTER_GOLD
class CCC_Script : public IConsoleCommand
{
public:
	CCC_Script(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		string256 S;
		S[0] = 0;
		sscanf(args, "%s", S);
		if (!xr_strlen(S))
		{
			Log("* Specify script name!");
		}
		else
		{
			// rescan pathes
			FS_Path* P = FS.get_path("$game_scripts$");
			P->m_Flags.set(FS_Path::flNeedRescan, TRUE);
			FS.rescan_pathes( );
			// run script
			if (ai( ).script_engine( ).script_process(ScriptEngine::eScriptProcessorLevel))
			{
				ai( ).script_engine( ).script_process(ScriptEngine::eScriptProcessorLevel)->add_script(S, false, true);
			}
		}
	}
};

class CCC_ScriptCommand : public IConsoleCommand
{
public:
	CCC_ScriptCommand(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		if (!xr_strlen(args))
		{
			Log("* Specify string to run!");
		}
		else
		{

#if 1
			if (ai( ).script_engine( ).script_process(ScriptEngine::eScriptProcessorLevel))
			{
				ai( ).script_engine( ).script_process(ScriptEngine::eScriptProcessorLevel)->add_script(args, true, true);
			}
#else
			string4096 S;
			shared_str m_script_name = "console command";
			sprintf_s(S, "%s\n", args);
			s32 l_iErrorCode = luaL_loadbuffer(ai( ).script_engine( ).lua( ), S, xr_strlen(S), "@console_command");
			if (!l_iErrorCode)
			{
				l_iErrorCode = lua_pcall(ai( ).script_engine( ).lua( ), 0, 0, 0);
				if (l_iErrorCode)
				{
					ai( ).script_engine( ).print_output(ai( ).script_engine( ).lua( ), *m_script_name, l_iErrorCode);
					return;
				}
			}
			else
			{
				ai( ).script_engine( ).print_output(ai( ).script_engine( ).lua( ), *m_script_name, l_iErrorCode);
				return;
			}
#endif

		}
	}
};
#endif // MASTER_GOLD

#ifdef DEBUG
class CCC_DrawGameGraphAll : public IConsoleCommand
{
public:
	CCC_DrawGameGraphAll(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		if (!ai( ).get_level_graph( ))
		{
			return;
		}

		ai( ).level_graph( ).setup_current_level(-1);
	}
};

class CCC_DrawGameGraphCurrent : public IConsoleCommand
{
public:
	CCC_DrawGameGraphCurrent(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		if (!ai( ).get_level_graph( ))
		{
			return;
		}

		ai( ).level_graph( ).setup_current_level(ai( ).level_graph( ).level_id( ));
	}
};

class CCC_DrawGameGraphLevel : public IConsoleCommand
{
public:
	CCC_DrawGameGraphLevel(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		if (!ai( ).get_level_graph( ))
		{
			return;
		}

		string256 S;
		S[0] = 0;
		sscanf(args, "%s", S);

		if (!*S)
		{
			ai( ).level_graph( ).setup_current_level(-1);
			return;
		}

		const GameGraph::SLevel* level = ai( ).game_graph( ).header( ).level(S, true);
		if (!level)
		{
			Msg("! There is no level %s in the game graph", S);
			return;
		}

		ai( ).level_graph( ).setup_current_level(level->id( ));
	}
};

class CCC_ScriptDbg : public IConsoleCommand
{
public:
	CCC_ScriptDbg(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		if (strstr(cName, "script_debug_break") == cName)
		{
			CScriptDebugger* d = ai( ).script_engine( ).debugger( );
			if (d)
			{
				if (d->Active( ))
				{
					d->initiateDebugBreak( );
				}
				else
				{
					Msg("Script debugger not active.");
				}
			}
			else
			{
				Msg("Script debugger not present.");
			}
		}
		else if (strstr(cName, "script_debug_stop") == cName)
		{
			ai( ).script_engine( ).stopDebugger( );
		}
		else if (strstr(cName, "script_debug_restart") == cName)
		{
			ai( ).script_engine( ).restartDebugger( );
		}
	}

	virtual void Info(TInfo& I)
	{
		if (strstr(cName, "script_debug_break") == cName)
		{
			strcpy_s(I, "initiate script debugger [DebugBreak] command");
		}
		else if (strstr(cName, "script_debug_stop") == cName)
		{
			strcpy_s(I, "stop script debugger activity");
		}
		else if (strstr(cName, "script_debug_restart") == cName)
		{
			strcpy_s(I, "restarts script debugger or start if no script debugger presents");
		}
	}
};

class CCC_DumpInfos : public IConsoleCommand
{
public:
	CCC_DumpInfos(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		CActor* A = smart_cast<CActor*>(Level( ).CurrentEntity( ));
		if (A)
		{
			A->DumpInfo( );
		}
	}

	virtual void Info(TInfo& I)
	{
		strcpy_s(I, "dumps all infoportions that actor have");
	}
};

class CCC_DumpMap : public IConsoleCommand
{
public:
	CCC_DumpMap(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		Level( ).MapManager( ).Dump( );
	}

	virtual void Info(TInfo& I)
	{
		strcpy_s(I, "dumps all currentmap locations");
	}
};

class CCC_DumpCreatures : public IConsoleCommand
{
public:
	CCC_DumpCreatures(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		typedef CSafeMapIterator<ALife::_OBJECT_ID, CSE_ALifeDynamicObject>::_REGISTRY::const_iterator const_iterator;

		const_iterator I = ai( ).alife( ).graph( ).level( ).objects( ).begin( );
		const_iterator E = ai( ).alife( ).graph( ).level( ).objects( ).end( );
		for (; I != E; ++I)
		{
			CSE_ALifeCreatureAbstract* obj = smart_cast<CSE_ALifeCreatureAbstract*>(I->second);
			if (obj)
			{
				Msg("\"%s\",", obj->name_replace( ));
			}
		}
	}

	virtual void Info(TInfo& I)
	{
		strcpy_s(I, "dumps all creature names");
	}
};

class CCC_DebugFonts : public IConsoleCommand
{
public:
	CCC_DebugFonts(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		HUD( ).GetUI( )->StartStopMenu(xr_new<CUIDebugFonts>( ), true);
	}
};

class CCC_DebugNode : public IConsoleCommand
{
public:
	CCC_DebugNode(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		string128 param1, param2;
		_GetItem(args, 0, param1, ' ');
		_GetItem(args, 1, param2, ' ');

		u32 value1;
		u32 value2;

		sscanf(param1, "%u", &value1);
		sscanf(param2, "%u", &value2);

		if ((value1 > 0) && (value2 > 0))
		{
			g_bDebugNode = TRUE;
			g_dwDebugNodeSource = value1;
			g_dwDebugNodeDest = value2;
		}
		else
		{
			g_bDebugNode = FALSE;
		}
	}
};

class CCC_ShowMonsterInfo : public IConsoleCommand
{
public:
	CCC_ShowMonsterInfo(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		string128 param1;
		string128 param2;
		_GetItem(args, 0, param1, ' ');
		_GetItem(args, 1, param2, ' ');

		CObject* obj = Level( ).Objects.FindObjectByName(param1);
		CBaseMonster* monster = smart_cast<CBaseMonster*>(obj);
		if (!monster)
		{
			return;
		}

		u32 value2;

		sscanf(param2, "%u", &value2);
		monster->set_show_debug_info(u8(value2));
	}
};

class CCC_DbgPhTrackObj : public IConsoleCommand
{
public:
	CCC_DbgPhTrackObj(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		ph_dbg_draw_mask1.set(ph_m1_DbgTrackObject, TRUE);
		PH_DBG_SetTrackObject(args);
	}
};
#endif

class CCC_PHIterations : public CCC_Integer
{
public:
	CCC_PHIterations(pcstr N) : CCC_Integer(N, &phIterations, 15, 50)
	{ }

	virtual void Execute(pcstr args)
	{
		CCC_Integer::Execute(args);
		dWorldSetQuickStepNumIterations(NULL, phIterations);
	}
};

#ifdef DEBUG
class CCC_PHGravity : public IConsoleCommand
{
public:
	CCC_PHGravity(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		if (!ph_world)
		{
			return;
		}

		ph_world->SetGravity(f32(atof(args)));
	}

	virtual void Status(TStatus& S)
	{
		if (ph_world)
		{
			sprintf_s(S, "%3.5f", ph_world->Gravity( ));
		}
		else
		{
			sprintf_s(S, "%3.5f", default_world_gravity);
		}

		while (xr_strlen(S) && ('0' == S[xr_strlen(S) - 1]))
		{
			S[xr_strlen(S) - 1] = 0;
		}
	}
};
#endif // DEBUG

class CCC_PHFps : public IConsoleCommand
{
public:
	CCC_PHFps(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		f32 step_count = (f32)atof(args);
		clamp(step_count, 50.0f, 200.0f);
		CPHWorld::SetStep(1.0f / step_count);
	}
	virtual void Status(TStatus& S)
	{
		sprintf_s(S, "%3.5f", 1.0f / fixed_step);
	}
};

#ifdef DEBUG
extern void print_help(lua_State* L);

struct CCC_LuaHelp : public IConsoleCommand
{
	CCC_LuaHelp(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		print_help(ai( ).script_engine( ).lua( ));
	}
};

struct CCC_ShowSmartCastStats : public IConsoleCommand
{
	CCC_ShowSmartCastStats(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		show_smart_cast_stats( );
	}
};

struct CCC_ClearSmartCastStats : public IConsoleCommand
{
	CCC_ClearSmartCastStats(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		clear_smart_cast_stats( );
	}
};
#endif

#ifndef MASTER_GOLD
struct CCC_JumpToLevel : public IConsoleCommand
{
	CCC_JumpToLevel(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		if (!ai( ).get_alife( ))
		{
			Msg("! ALife simulator is needed to perform specified command!");
			return;
		}

		string256 level;
		sscanf(args, "%s", level);

		GameGraph::LEVEL_MAP::const_iterator I = ai( ).game_graph( ).header( ).levels( ).begin( );
		GameGraph::LEVEL_MAP::const_iterator E = ai( ).game_graph( ).header( ).levels( ).end( );
		for (; I != E; ++I)
		{
			if (!xr_strcmp((*I).second.name( ), level))
			{
				ai( ).alife( ).jump_to_level(level);
				return;
			}
		}

		Msg("! There is no level \"%s\" in the game graph!", level);
	}
};
#endif // MASTER_GOLD

class CCC_MainMenu : public IConsoleCommand
{
public:
	CCC_MainMenu(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		bool bWhatToDo = true;
		if (0 == xr_strlen(args))
		{
			bWhatToDo = !MainMenu( )->IsActive( );
		}

		if (EQ(args, "on") || EQ(args, "1"))
		{
			bWhatToDo = true;
		}

		if (EQ(args, "off") || EQ(args, "0"))
		{
			bWhatToDo = false;
		}

		MainMenu( )->Activate(bWhatToDo);
	}
};

struct CCC_StartTimeSingle : public IConsoleCommand
{
	CCC_StartTimeSingle(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		u32 year = 1;
		u32 month = 1;
		u32 day = 1;
		u32 hours = 0;
		u32 mins = 0;
		u32 secs = 0;
		u32 milisecs = 0;
		sscanf(args, "%d.%d.%d %d:%d:%d.%d", &year, &month, &day, &hours, &mins, &secs, &milisecs);
		year = _max(year, 1);
		month = _max(month, 1);
		day = _max(day, 1);
		g_qwStartGameTime = generate_time(year, month, day, hours, mins, secs, milisecs);

		if (!g_pGameLevel)
		{
			return;
		}

		if (!Level( ).Server)
		{
			return;
		}

		if (!Level( ).Server->game)
		{
			return;
		}

		Level( ).Server->game->SetGameTimeFactor(g_qwStartGameTime, g_fTimeFactor);
	}

	virtual void Status(TStatus& S)
	{
		u32 year = 1;
		u32 month = 1;
		u32 day = 1;
		u32 hours = 0;
		u32 mins = 0;
		u32 secs = 0;
		u32 milisecs = 0;
		split_time(g_qwStartGameTime, year, month, day, hours, mins, secs, milisecs);
		sprintf_s(S, "%d.%d.%d %d:%d:%d.%d", year, month, day, hours, mins, secs, milisecs);
	}
};

struct CCC_TimeFactorSingle : public CCC_Float
{
	CCC_TimeFactorSingle(pcstr N, f32* V, f32 _min = 0.0f, f32 _max = 1.0f) : CCC_Float(N, V, _min, _max)
	{ }

	virtual void Execute(pcstr args)
	{
		CCC_Float::Execute(args);

		if (!g_pGameLevel)
		{
			return;
		}

		if (!Level( ).Server)
		{
			return;
		}

		if (!Level( ).Server->game)
		{
			return;
		}

		Level( ).Server->game->SetGameTimeFactor(g_fTimeFactor);
	}
};

#ifdef DEBUG
class CCC_RadioGroupMask2;
class CCC_RadioMask :public CCC_Mask
{
	CCC_RadioGroupMask2* group;

public:
	CCC_RadioMask(pcstr N, flags32* V, u32 M) : CCC_Mask(N, V, M)
	{
		group = NULL;
	}

	void SetGroup(CCC_RadioGroupMask2* G)
	{
		group = G;
	}

	virtual void Execute(pcstr args);

	IC void Set(BOOL V)
	{
		value->set(mask, V);
	}
};

class CCC_RadioGroupMask2
{
	CCC_RadioMask* mask0;
	CCC_RadioMask* mask1;

public:
	CCC_RadioGroupMask2(CCC_RadioMask* m0, CCC_RadioMask* m1)
	{
		mask0 = m0; mask1 = m1;
		mask0->SetGroup(this);
		mask1->SetGroup(this);
	}

	void Execute(CCC_RadioMask& m, pcstr args)
	{
		BOOL value = m.GetValue( );
		if (value)
		{
			mask0->Set(!value); mask1->Set(!value);
		}

		m.Set(value);
	}
};

void CCC_RadioMask::Execute(pcstr args)
{
	CCC_Mask::Execute(args);
	VERIFY2(group, "CCC_RadioMask: group not set");
	group->Execute(*this, args);
}

#define CMD_RADIOGROUPMASK2(p1,p2,p3,p4,p5,p6)\
{\
static CCC_RadioMask x##CCC_RadioMask1(p1,p2,p3);		Console->AddCommand(&x##CCC_RadioMask1);\
static CCC_RadioMask x##CCC_RadioMask2(p4,p5,p6);		Console->AddCommand(&x##CCC_RadioMask2);\
static CCC_RadioGroupMask2 x##CCC_RadioGroupMask2(&x##CCC_RadioMask1,&x##CCC_RadioMask2);\
}

struct CCC_DbgBullets : public CCC_Integer
{
	CCC_DbgBullets(pcstr N, s32* V, s32 _min = 0, s32 _max = 999) : CCC_Integer(N, V, _min, _max)
	{ }

	virtual void Execute(pcstr args)
	{
		extern FvectorVec g_hit[ ];
		g_hit[0].clear( );
		g_hit[1].clear( );
		g_hit[2].clear( );
		CCC_Integer::Execute(args);
	}
};

class CCC_TuneAttachableItem : public IConsoleCommand
{
public:
	CCC_TuneAttachableItem(pcstr N) :IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		if (CAttachableItem::m_dbgItem)
		{
			CAttachableItem::m_dbgItem = NULL;
			Msg("CCC_TuneAttachableItem switched to off");
			return;
		}

		CObject* obj = Level( ).CurrentViewEntity( );
		VERIFY(obj);
		CAttachmentOwner* owner = smart_cast<CAttachmentOwner*>(obj);
		shared_str ssss = args;
		CAttachableItem* itm = owner->attachedItem(ssss);
		if (itm)
		{
			CAttachableItem::m_dbgItem = itm;
			Msg("CCC_TuneAttachableItem switched to ON for [%s]", args);
		}
		else
		{
			Msg("CCC_TuneAttachableItem cannot find attached item [%s]", args);
		}
	}

	virtual void Info(TInfo& I)
	{
		sprintf_s(I, "allows to change bind rotation and position offsets for attached item, <section_name> given as arguments");
	}
};

class CCC_Crash : public IConsoleCommand
{
public:
	CCC_Crash(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr /*args*/)
	{
		VERIFY3(false, "This is a test crash", "Do not post it as a bug");
		s32* pointer = 0;
		*pointer = 0;
	}
};

class CCC_DumpModelBones : public IConsoleCommand
{
public:
	CCC_DumpModelBones(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr arguments)
	{
		if (!arguments || !*arguments)
		{
			Msg("! no arguments passed");
			return;
		}

		string_path name;
		string_path fn;

		if (0 == strext(arguments))
		{
			strconcat(sizeof(name), name, arguments, ".ogf");
		}
		else
		{
			strcpy_s(name, sizeof(name), arguments);
		}

		if (!FS.exist(arguments) && !FS.exist(fn, "$level$", name) && !FS.exist(fn, "$game_meshes$", name))
		{
			Msg("! Cannot find visual \"%s\"", arguments);
			return;
		}

		IRenderVisual* visual = Render->model_Create(arguments);
		CKinematics* kinematics = smart_cast<CKinematics*>(visual);
		if (!kinematics)
		{
			Render->model_Delete(visual);
			Msg("! Invalid visual type \"%s\" (not a CKinematics)", arguments);
			return;
		}

		Msg("bones for model \"%s\"", arguments);
		for (u16 i = 0, n = kinematics->LL_BoneCount( ); i < n; ++i)
		{
			Msg("%s", *kinematics->LL_GetData(i).name);
		}

		Render->model_Delete(visual);
	}
};

extern void show_animation_stats( );

class CCC_ShowAnimationStats : public IConsoleCommand
{
public:
	CCC_ShowAnimationStats(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr)
	{
		show_animation_stats( );
	}
};
#endif // DEBUG

class CCC_DumpObjects : public IConsoleCommand
{
public:
	CCC_DumpObjects(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr)
	{
		Level( ).Objects.dump_all_objects( );
	}
};

void CCC_RegisterCommands( )
{
	CMD1(CCC_MemStats, "stat_memory");
	// game
	psActorFlags.set(AF_ALWAYSRUN, true);
	CMD3(CCC_Mask, "g_always_run", &psActorFlags, AF_ALWAYSRUN);
	CMD1(CCC_GameDifficulty, "g_game_difficulty");

	CMD3(CCC_Mask, "g_backrun", &psActorFlags, AF_RUN_BACKWARD);

	// alife
#ifdef DEBUG
	CMD1(CCC_ALifePath, "al_path");		// build path
#endif // DEBUG

	CMD1(CCC_ALifeSave, "save");		// save game
	CMD1(CCC_ALifeLoadFrom, "load");		// load game from ...
	CMD1(CCC_LoadLastSave, "load_last_save");		// load last saved game from ...

	CMD1(CCC_FlushLog, "flush");		// flush log
	CMD1(CCC_ClearLog, "clear_log");

#ifndef MASTER_GOLD
	CMD1(CCC_ALifeTimeFactor, "al_time_factor");		// set time factor
	CMD1(CCC_ALifeSwitchDistance, "al_switch_distance");		// set switch distance
	CMD1(CCC_ALifeProcessTime, "al_process_time");		// set process time
	CMD1(CCC_ALifeObjectsPerUpdate, "al_objects_per_update");		// set process time
	CMD1(CCC_ALifeSwitchFactor, "al_switch_factor");		// set switch factor
#endif // MASTER_GOLD

	CMD3(CCC_Mask, "hud_weapon", &psHUD_Flags, HUD_WEAPON);
	CMD3(CCC_Mask, "hud_info", &psHUD_Flags, HUD_INFO);

#ifndef MASTER_GOLD
	CMD3(CCC_Mask, "hud_draw", &psHUD_Flags, HUD_DRAW);
#endif // MASTER_GOLD

	// hud
	psHUD_Flags.set(HUD_CROSSHAIR, true);
	psHUD_Flags.set(HUD_WEAPON, true);
	psHUD_Flags.set(HUD_DRAW, true);
	psHUD_Flags.set(HUD_INFO, true);

	CMD3(CCC_Mask, "hud_crosshair", &psHUD_Flags, HUD_CROSSHAIR);
	CMD3(CCC_Mask, "hud_crosshair_dist", &psHUD_Flags, HUD_CROSSHAIR_DIST);

#ifdef DEBUG
	CMD4(CCC_Float, "hud_fov", &psHUD_FOV, 0.1f, 1.0f);
	CMD4(CCC_Float, "fov", &g_fov, 5.0f, 180.0f);
#endif // DEBUG

	// Demo
	CMD1(CCC_DemoPlay, "demo_play");
	CMD1(CCC_DemoRecord, "demo_record");

#ifndef MASTER_GOLD
	// ai
	CMD3(CCC_Mask, "mt_ai_vision", &g_mt_config, mtAiVision);
	CMD3(CCC_Mask, "mt_level_path", &g_mt_config, mtLevelPath);
	CMD3(CCC_Mask, "mt_detail_path", &g_mt_config, mtDetailPath);
	CMD3(CCC_Mask, "mt_object_handler", &g_mt_config, mtObjectHandler);
	CMD3(CCC_Mask, "mt_sound_player", &g_mt_config, mtSoundPlayer);
	CMD3(CCC_Mask, "mt_bullets", &g_mt_config, mtBullets);
	CMD3(CCC_Mask, "mt_script_gc", &g_mt_config, mtLUA_GC);
	CMD3(CCC_Mask, "mt_level_sounds", &g_mt_config, mtLevelSounds);
	CMD3(CCC_Mask, "mt_alife", &g_mt_config, mtALife);
#endif // MASTER_GOLD

#ifdef DEBUG
	CMD4(CCC_Integer, "lua_gcstep", &psLUA_GCSTEP, 1, 1000);
	CMD3(CCC_Mask, "ai_debug", &psAI_Flags, aiDebug);
	CMD3(CCC_Mask, "ai_dbg_brain", &psAI_Flags, aiBrain);
	CMD3(CCC_Mask, "ai_dbg_motion", &psAI_Flags, aiMotion);
	CMD3(CCC_Mask, "ai_dbg_frustum", &psAI_Flags, aiFrustum);
	CMD3(CCC_Mask, "ai_dbg_funcs", &psAI_Flags, aiFuncs);
	CMD3(CCC_Mask, "ai_dbg_alife", &psAI_Flags, aiALife);
	CMD3(CCC_Mask, "ai_dbg_lua", &psAI_Flags, aiLua);
	CMD3(CCC_Mask, "ai_dbg_goap", &psAI_Flags, aiGOAP);
	CMD3(CCC_Mask, "ai_dbg_goap_script", &psAI_Flags, aiGOAPScript);
	CMD3(CCC_Mask, "ai_dbg_goap_object", &psAI_Flags, aiGOAPObject);
	CMD3(CCC_Mask, "ai_dbg_cover", &psAI_Flags, aiCover);
	CMD3(CCC_Mask, "ai_dbg_anim", &psAI_Flags, aiAnimation);
	CMD3(CCC_Mask, "ai_dbg_vision", &psAI_Flags, aiVision);
	CMD3(CCC_Mask, "ai_dbg_monster", &psAI_Flags, aiMonsterDebug);
	CMD3(CCC_Mask, "ai_dbg_stalker", &psAI_Flags, aiStalker);
	CMD3(CCC_Mask, "ai_stats", &psAI_Flags, aiStats);
	CMD3(CCC_Mask, "ai_dbg_destroy", &psAI_Flags, aiDestroy);
	CMD3(CCC_Mask, "ai_dbg_serialize", &psAI_Flags, aiSerialize);
	CMD3(CCC_Mask, "ai_dbg_dialogs", &psAI_Flags, aiDialogs);
	CMD3(CCC_Mask, "ai_dbg_infoportion", &psAI_Flags, aiInfoPortion);

	CMD3(CCC_Mask, "ai_draw_game_graph", &psAI_Flags, aiDrawGameGraph);
	CMD3(CCC_Mask, "ai_draw_game_graph_stalkers", &psAI_Flags, aiDrawGameGraphStalkers);
	CMD3(CCC_Mask, "ai_draw_game_graph_objects", &psAI_Flags, aiDrawGameGraphObjects);

	CMD3(CCC_Mask, "ai_nil_object_access", &psAI_Flags, aiNilObjectAccess);

	CMD3(CCC_Mask, "ai_draw_visibility_rays", &psAI_Flags, aiDrawVisibilityRays);
	CMD3(CCC_Mask, "ai_animation_stats", &psAI_Flags, aiAnimationStats);

	CMD1(CCC_DumpModelBones, "debug_dump_model_bones");

	CMD1(CCC_DrawGameGraphAll, "ai_draw_game_graph_all");
	CMD1(CCC_DrawGameGraphCurrent, "ai_draw_game_graph_current_level");
	CMD1(CCC_DrawGameGraphLevel, "ai_draw_game_graph_level");

	CMD4(CCC_Integer, "ai_dbg_inactive_time", &g_AI_inactive_time, 0, 1000000);

	CMD1(CCC_DebugNode, "ai_dbg_node");
	CMD1(CCC_ScriptDbg, "script_debug_break");
	CMD1(CCC_ScriptDbg, "script_debug_stop");
	CMD1(CCC_ScriptDbg, "script_debug_restart");

	CMD1(CCC_ShowMonsterInfo, "ai_monster_info");
	CMD1(CCC_DebugFonts, "debug_fonts");
	CMD1(CCC_TuneAttachableItem, "dbg_adjust_attachable_item");

	// adjust mode support
	CMD4(CCC_Integer, "hud_adjust_mode", &g_bHudAdjustMode, 0, 5);
	CMD4(CCC_Float, "hud_adjust_value", &g_fHudAdjustValue, 0.0f, 1.0f);

	CMD1(CCC_ShowAnimationStats, "ai_show_animation_stats");
#endif // DEBUG

#ifndef MASTER_GOLD
	CMD3(CCC_Mask, "ai_ignore_actor", &psAI_Flags, aiIgnoreActor);
#endif // MASTER_GOLD

	// Physics
	CMD1(CCC_PHFps, "ph_frequency");
	CMD1(CCC_PHIterations, "ph_iterations");

#ifdef DEBUG
	CMD1(CCC_PHGravity, "ph_gravity");
	CMD4(CCC_FloatBlock, "ph_timefactor", &phTimefactor, 0.0001f, 1000.0f);
	CMD4(CCC_FloatBlock, "ph_break_common_factor", &phBreakCommonFactor, 0.0f, 1000000000.0f);
	CMD4(CCC_FloatBlock, "ph_rigid_break_weapon_factor", &phRigidBreakWeaponFactor, 0.0f, 1000000000.0f);
	CMD4(CCC_Integer, "ph_tri_clear_disable_count", &ph_tri_clear_disable_count, 0, 255);
	CMD4(CCC_FloatBlock, "ph_tri_query_ex_aabb_rate", &ph_tri_query_ex_aabb_rate, 1.01f, 3.0f);
#endif // DEBUG

#ifndef MASTER_GOLD
	CMD1(CCC_JumpToLevel, "jump_to_level");
	CMD3(CCC_Mask, "g_god", &psActorFlags, AF_GODMODE);
	CMD3(CCC_Mask, "g_unlimitedammo", &psActorFlags, AF_UNLIMITEDAMMO);
	CMD1(CCC_Script, "run_script");
	CMD1(CCC_ScriptCommand, "run_string");
	CMD1(CCC_TimeFactor, "time_factor");
#endif // MASTER_GOLD

	CMD3(CCC_Mask, "g_autopickup", &psActorFlags, AF_AUTOPICKUP);

#ifdef DEBUG
	CMD1(CCC_LuaHelp, "lua_help");
	CMD1(CCC_ShowSmartCastStats, "show_smart_cast_stats");
	CMD1(CCC_ClearSmartCastStats, "clear_smart_cast_stats");

	CMD3(CCC_Mask, "dbg_draw_actor_alive", &dbg_net_Draw_Flags, (1 << 0));
	CMD3(CCC_Mask, "dbg_draw_actor_dead", &dbg_net_Draw_Flags, (1 << 1));
	CMD3(CCC_Mask, "dbg_draw_customzone", &dbg_net_Draw_Flags, (1 << 2));
	CMD3(CCC_Mask, "dbg_draw_teamzone", &dbg_net_Draw_Flags, (1 << 3));
	CMD3(CCC_Mask, "dbg_draw_invitem", &dbg_net_Draw_Flags, (1 << 4));
	CMD3(CCC_Mask, "dbg_draw_actor_phys", &dbg_net_Draw_Flags, (1 << 5));
	CMD3(CCC_Mask, "dbg_draw_customdetector", &dbg_net_Draw_Flags, (1 << 6));
	CMD3(CCC_Mask, "dbg_destroy", &dbg_net_Draw_Flags, (1 << 7));
	CMD3(CCC_Mask, "dbg_draw_autopickupbox", &dbg_net_Draw_Flags, (1 << 8));
	CMD3(CCC_Mask, "dbg_draw_rp", &dbg_net_Draw_Flags, (1 << 9));
	CMD3(CCC_Mask, "dbg_draw_climbable", &dbg_net_Draw_Flags, (1 << 10));
	CMD3(CCC_Mask, "dbg_draw_skeleton", &dbg_net_Draw_Flags, (1 << 11));

	CMD3(CCC_Mask, "dbg_draw_ph_contacts", &ph_dbg_draw_mask, phDbgDrawContacts);
	CMD3(CCC_Mask, "dbg_draw_ph_enabled_aabbs", &ph_dbg_draw_mask, phDbgDrawEnabledAABBS);
	CMD3(CCC_Mask, "dbg_draw_ph_intersected_tries", &ph_dbg_draw_mask, phDBgDrawIntersectedTries);
	CMD3(CCC_Mask, "dbg_draw_ph_saved_tries", &ph_dbg_draw_mask, phDbgDrawSavedTries);
	CMD3(CCC_Mask, "dbg_draw_ph_tri_trace", &ph_dbg_draw_mask, phDbgDrawTriTrace);
	CMD3(CCC_Mask, "dbg_draw_ph_positive_tries", &ph_dbg_draw_mask, phDBgDrawPositiveTries);
	CMD3(CCC_Mask, "dbg_draw_ph_negative_tries", &ph_dbg_draw_mask, phDBgDrawNegativeTries);
	CMD3(CCC_Mask, "dbg_draw_ph_tri_test_aabb", &ph_dbg_draw_mask, phDbgDrawTriTestAABB);
	CMD3(CCC_Mask, "dbg_draw_ph_tries_changes_sign", &ph_dbg_draw_mask, phDBgDrawTriesChangesSign);
	CMD3(CCC_Mask, "dbg_draw_ph_tri_point", &ph_dbg_draw_mask, phDbgDrawTriPoint);
	CMD3(CCC_Mask, "dbg_draw_ph_explosion_position", &ph_dbg_draw_mask, phDbgDrawExplosionPos);
	CMD3(CCC_Mask, "dbg_draw_ph_statistics", &ph_dbg_draw_mask, phDbgDrawObjectStatistics);
	CMD3(CCC_Mask, "dbg_draw_ph_mass_centres", &ph_dbg_draw_mask, phDbgDrawMassCenters);
	CMD3(CCC_Mask, "dbg_draw_ph_death_boxes", &ph_dbg_draw_mask, phDbgDrawDeathActivationBox);
	CMD3(CCC_Mask, "dbg_draw_ph_hit_app_pos", &ph_dbg_draw_mask, phHitApplicationPoints);
	CMD3(CCC_Mask, "dbg_draw_ph_cashed_tries_stats", &ph_dbg_draw_mask, phDbgDrawCashedTriesStat);
	CMD3(CCC_Mask, "dbg_draw_ph_car_dynamics", &ph_dbg_draw_mask, phDbgDrawCarDynamics);
	CMD3(CCC_Mask, "dbg_draw_ph_car_plots", &ph_dbg_draw_mask, phDbgDrawCarPlots);
	CMD3(CCC_Mask, "dbg_ph_ladder", &ph_dbg_draw_mask, phDbgLadder);
	CMD3(CCC_Mask, "dbg_draw_ph_explosions", &ph_dbg_draw_mask, phDbgDrawExplosions);
	CMD3(CCC_Mask, "dbg_draw_car_plots_all_trans", &ph_dbg_draw_mask, phDbgDrawCarAllTrnsm);
	CMD3(CCC_Mask, "dbg_draw_ph_zbuffer_disable", &ph_dbg_draw_mask, phDbgDrawZDisable);
	CMD3(CCC_Mask, "dbg_ph_obj_collision_damage", &ph_dbg_draw_mask, phDbgDispObjCollisionDammage);
	CMD_RADIOGROUPMASK2("dbg_ph_ai_always_phmove", &ph_dbg_draw_mask, phDbgAlwaysUseAiPhMove, "dbg_ph_ai_never_phmove", &ph_dbg_draw_mask, phDbgNeverUseAiPhMove);
	CMD3(CCC_Mask, "dbg_ph_ik", &ph_dbg_draw_mask, phDbgIK);
	CMD3(CCC_Mask, "dbg_ph_ik_off", &ph_dbg_draw_mask1, phDbgIKOff);
	CMD3(CCC_Mask, "dbg_draw_ph_ik_goal", &ph_dbg_draw_mask, phDbgDrawIKGoal);
	CMD3(CCC_Mask, "dbg_ph_ik_limits", &ph_dbg_draw_mask, phDbgIKLimits);
	CMD3(CCC_Mask, "dbg_ph_character_control", &ph_dbg_draw_mask, phDbgCharacterControl);
	CMD3(CCC_Mask, "dbg_draw_ph_ray_motions", &ph_dbg_draw_mask, phDbgDrawRayMotions);
	CMD4(CCC_Float, "dbg_ph_vel_collid_damage_to_display", &dbg_vel_collid_damage_to_display, 0.0f, 1000.0f);
	CMD4(CCC_DbgBullets, "dbg_draw_bullet_hit", &g_bDrawBulletHit, 0, 1);
	CMD1(CCC_DbgPhTrackObj, "dbg_track_obj");
	CMD3(CCC_Mask, "dbg_ph_actor_restriction", &ph_dbg_draw_mask1, ph_m1_DbgActorRestriction);
	CMD3(CCC_Mask, "dbg_draw_ph_hit_anims", &ph_dbg_draw_mask1, phDbgHitAnims);
	CMD3(CCC_Mask, "dbg_draw_ph_ik_limits", &ph_dbg_draw_mask1, phDbgDrawIKLimits);
#endif

#ifdef DEBUG
	CMD4(CCC_Integer, "string_table_error_msg", &CStringTable::m_bWriteErrorsToLog, 0, 1);

	CMD1(CCC_DumpInfos, "dump_infos");
	CMD1(CCC_DumpMap, "dump_map");
	CMD1(CCC_DumpCreatures, "dump_creatures");
#endif

	CMD3(CCC_Mask, "cl_dynamiccrosshair", &psHUD_Flags, HUD_CROSSHAIR_DYNAMIC);
	CMD1(CCC_MainMenu, "main_menu");

#ifndef MASTER_GOLD
	CMD1(CCC_StartTimeSingle, "start_time_single");
	CMD4(CCC_TimeFactorSingle, "time_factor_single", &g_fTimeFactor, 0.0f, flt_max);
#endif // MASTER_GOLD

	g_uCommonFlags.zero( );
	g_uCommonFlags.set(flAiUseTorchDynamicLights, TRUE);

	CMD3(CCC_Mask, "ai_use_torch_dynamic_lights", &g_uCommonFlags, flAiUseTorchDynamicLights);

#ifndef MASTER_GOLD
	CMD4(CCC_Vector3, "psp_cam_offset", &CCameraLook2::m_cam_offset, fVector3( ).set(-1000.0f, -1000.0f, -1000.0f), fVector3( ).set(1000.0f, 1000.0f, 1000.0f));
#endif // MASTER_GOLD

#ifdef DEBUG
	CMD1(CCC_DumpObjects, "dump_all_objects");
	CMD3(CCC_String, "stalker_death_anim", dbg_stalker_death_anim, 32);
	CMD4(CCC_Integer, "death_anim_velocity", &b_death_anim_velocity, FALSE, TRUE);
	CMD4(CCC_Integer, "show_wnd_rect", &g_show_wnd_rect, 0, 1);
	CMD4(CCC_Integer, "show_wnd_rect_all", &g_show_wnd_rect2, 0, 1);
	CMD1(CCC_Crash, "crash");
	CMD4(CCC_Integer, "dbg_show_ani_info", &g_ShowAnimationInfo, 0, 1);
	CMD4(CCC_Integer, "dbg_dump_physics_step", &g_bDebugDumpPhysicsStep, 0, 1);
#endif

	* g_last_saved_game = 0;
}
