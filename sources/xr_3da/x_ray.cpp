#include "stdafx.h"
#include "igame_level.h"
#include "igame_persistent.h"

#include "Input.h"
#include "Console.h"
#include "ConsoleCommand.h"
#include "x_ray.h"
#include "std_classes.h"
#include "GameFont.h"
#include "resource.h"
#include "LightAnimLibrary.h"
#include "ispatial.h"
#include <process.h>

//---------------------------------------------------------------------
ENGINE_API CIniFile* pGameIni = NULL;
BOOL	g_bIntroFinished = FALSE;
s32		max_load_stage = 0;

// computing build id
CORE_API	pcstr	build_date;
CORE_API	u32		build_id;

#define NO_MULTI_INSTANCES

static pstr month_id[12] =
{
	"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};

static s32 days_in_month[12] =
{
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static s32 start_day = 1;	// 1
static s32 start_month = 5;	// May
static s32 start_year = 2022;	// 2022

void compute_build_id( )
{
	build_date = __DATE__;

	s32					days;
	s32					months = 0;
	s32					years;
	string16			month;
	string256			buffer;
	strcpy_s(buffer, __DATE__);
	sscanf(buffer, "%s %d %d", month, &days, &years);

	for (s32 i = 0; i < 12; i++)
	{
		if (_stricmp(month_id[i], month))
		{
			continue;
		}

		months = i;
		break;
	}

	build_id = (years - start_year) * 365 + days - start_day;

	for (s32 i = 0; i < months; ++i)
	{
		build_id += days_in_month[i];
	}

	for (s32 i = 0; i < start_month - 1; ++i)
	{
		build_id -= days_in_month[i];
	}
}

struct _SoundProcessor : public pureFrame
{
	virtual void OnFrame( )
	{
		Device.Statistic->Sound.Begin( );
		::Sound->update(Device.vCameraPosition, Device.vCameraDirection, Device.vCameraTop);
		Device.Statistic->Sound.End( );
	}
}	SoundProcessor;

//////////////////////////////////////////////////////////////////////////
// global variables
ENGINE_API	CApplication* pApp = NULL;
static		HWND			logoWindow = NULL;

void			doBenchmark(pcstr name);
ENGINE_API	bool			g_bBenchmark = false;
string512		g_sBenchmarkName;


ENGINE_API	string512		g_sLaunchOnExit_params;
ENGINE_API	string512		g_sLaunchOnExit_app;
// -------------------------------------------
// startup point
void InitEngine( )
{
	Engine.Initialize( );
	while (!g_bIntroFinished)
	{
		Sleep(100);
	}

	Device.Initialize( );
}

void InitSettings( )
{
	string_path fname;

	FS.update_path(fname, "$game_config$", "system.ltx");
	pSettings = xr_new<CIniFile>(fname, TRUE);
	CHECK_OR_EXIT(!pSettings->sections( ).empty( ), make_string("Cannot find file %s.\nReinstalling application may fix this problem.", fname));

	FS.update_path(fname, "$game_config$", "game.ltx");
	pGameIni = xr_new<CIniFile>(fname, TRUE);
	CHECK_OR_EXIT(!pGameIni->sections( ).empty( ), make_string("Cannot find file %s.\nReinstalling application may fix this problem.", fname));
}

void InitConsole( )
{
	Console = xr_new<CConsole>( );
	Console->Initialize( );

	strcpy_s(Console->ConfigFile, "user.ltx");
	if (strstr(Core.Params, "-ltx "))
	{
		string64 c_name;
		sscanf(strstr(Core.Params, "-ltx ") + 5, "%[^ ] ", c_name);
		strcpy_s(Console->ConfigFile, c_name);
	}
}

void InitInput( )
{
	BOOL bCaptureInput = !strstr(Core.Params, "-i");

	pInput = xr_new<CInput>(bCaptureInput);
}

void destroyInput( )
{
	xr_delete(pInput);
}

void InitSound( )
{
	CSound_manager_interface::_create(u64(Device.m_hWnd));
}

void destroySound( )
{
	CSound_manager_interface::_destroy( );
}

void destroySettings( )
{
	xr_delete(pSettings);
	xr_delete(pGameIni);
}

void destroyConsole( )
{
	Console->Destroy( );
	xr_delete(Console);
}

void destroyEngine( )
{
	Device.Destroy( );
	Engine.Destroy( );
}

void execUserScript( )
{
	// Execute script
	Console->Execute("unbindall");
	Console->ExecuteScript(Console->ConfigFile);
}

void slowdownthread(pvoid)
{
	//	Sleep		(30*1000);
	for (;;)
	{
		if (Device.Statistic->fFPS < 30)
		{
			Sleep(1);
		}

		if (Device.mt_bMustExit)
		{
			return;
		}

		if (0 == pSettings)
		{
			return;
		}

		if (0 == Console)
		{
			return;
		}

		if (0 == pInput)
		{
			return;
		}

		if (0 == pApp)
		{
			return;
		}
	}
}

void CheckPrivilegySlowdown( )
{

#ifdef DEBUG
	if (strstr(Core.Params, "-slowdown"))
	{
		thread_spawn(slowdownthread, "slowdown", 0, 0);
	}

	if (strstr(Core.Params, "-slowdown2x"))
	{
		thread_spawn(slowdownthread, "slowdown", 0, 0);
		thread_spawn(slowdownthread, "slowdown", 0, 0);
	}
#endif // def DEBUG

}

void Startup( )
{
	execUserScript( );
	InitSound( );

	// ...command line for auto start
	{
		pcstr pStartup = strstr(Core.Params, "-start ");
		if (pStartup)
		{
			Console->Execute(pStartup + 1);
		}
	}
	{
		pcstr	pStartup = strstr(Core.Params, "-load ");
		if (pStartup)
		{
			Console->Execute(pStartup + 1);
		}
	}

	// Initialize APP
	ShowWindow(Device.m_hWnd, SW_SHOWNORMAL);
	Device.Create( );
	LALib.OnCreate( );
	pApp = xr_new<CApplication>( );
	g_pGamePersistent = (IGame_Persistent*)NEW_INSTANCE(CLSID_GAME_PERSISTANT);
	g_SpatialSpace = xr_new<ISpatial_DB>( );
	g_SpatialSpacePhysic = xr_new<ISpatial_DB>( );

	// Destroy LOGO
	DestroyWindow(logoWindow);
	logoWindow = NULL;

	// Main cycle
	Memory.mem_usage( );
	Device.Run( );

	// Destroy APP
	xr_delete(g_SpatialSpacePhysic);
	xr_delete(g_SpatialSpace);
	DEL_INSTANCE(g_pGamePersistent);
	xr_delete(pApp);
	Engine.Event.Dump( );

	// Destroying
	destroySound( );
	destroyInput( );

	if (!g_bBenchmark)
	{
		destroySettings( );
	}

	LALib.OnDestroy( );

	if (!g_bBenchmark)
	{
		destroyConsole( );
	}
	else
	{
		Console->Reset( );
	}

	destroyEngine( );
}

static BOOL CALLBACK logDlgProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		case WM_DESTROY:
		{
		}
		break;
		case WM_CLOSE:
		{
			DestroyWindow(hw);
		}
		break;
		case WM_COMMAND:
		{
			if (LOWORD(wp) == IDCANCEL)
			{
				DestroyWindow(hw);
			}
		}
		break;
		default:
		{
			return FALSE;
		}
	}

	return TRUE;
}

#define dwStickyKeysStructSize sizeof( STICKYKEYS )
#define dwFilterKeysStructSize sizeof( FILTERKEYS )
#define dwToggleKeysStructSize sizeof( TOGGLEKEYS )

struct damn_keys_filter
{
	BOOL bScreenSaverState;

	// Sticky & Filter & Toggle keys
	STICKYKEYS StickyKeysStruct;
	FILTERKEYS FilterKeysStruct;
	TOGGLEKEYS ToggleKeysStruct;

	DWORD dwStickyKeysFlags;
	DWORD dwFilterKeysFlags;
	DWORD dwToggleKeysFlags;

	damn_keys_filter( )
	{
		// Screen saver stuff
		bScreenSaverState = FALSE;

		// Saveing current state
		SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, (PVOID)&bScreenSaverState, 0);

		if (bScreenSaverState)
		{	// Disable screensaver
			SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, 0);
		}

		dwStickyKeysFlags = 0;
		dwFilterKeysFlags = 0;
		dwToggleKeysFlags = 0;

		ZeroMemory(&StickyKeysStruct, dwStickyKeysStructSize);
		ZeroMemory(&FilterKeysStruct, dwFilterKeysStructSize);
		ZeroMemory(&ToggleKeysStruct, dwToggleKeysStructSize);

		StickyKeysStruct.cbSize = dwStickyKeysStructSize;
		FilterKeysStruct.cbSize = dwFilterKeysStructSize;
		ToggleKeysStruct.cbSize = dwToggleKeysStructSize;

		// Saving current state
		SystemParametersInfo(SPI_GETSTICKYKEYS, dwStickyKeysStructSize, (PVOID)&StickyKeysStruct, 0);
		SystemParametersInfo(SPI_GETFILTERKEYS, dwFilterKeysStructSize, (PVOID)&FilterKeysStruct, 0);
		SystemParametersInfo(SPI_GETTOGGLEKEYS, dwToggleKeysStructSize, (PVOID)&ToggleKeysStruct, 0);

		if (StickyKeysStruct.dwFlags & SKF_AVAILABLE)
		{
			// Disable StickyKeys feature
			dwStickyKeysFlags = StickyKeysStruct.dwFlags;
			StickyKeysStruct.dwFlags = 0;
			SystemParametersInfo(SPI_SETSTICKYKEYS, dwStickyKeysStructSize, (PVOID)&StickyKeysStruct, 0);
		}

		if (FilterKeysStruct.dwFlags & FKF_AVAILABLE)
		{
			// Disable FilterKeys feature
			dwFilterKeysFlags = FilterKeysStruct.dwFlags;
			FilterKeysStruct.dwFlags = 0;
			SystemParametersInfo(SPI_SETFILTERKEYS, dwFilterKeysStructSize, (PVOID)&FilterKeysStruct, 0);
		}

		if (ToggleKeysStruct.dwFlags & TKF_AVAILABLE)
		{
			// Disable FilterKeys feature
			dwToggleKeysFlags = ToggleKeysStruct.dwFlags;
			ToggleKeysStruct.dwFlags = 0;
			SystemParametersInfo(SPI_SETTOGGLEKEYS, dwToggleKeysStructSize, (PVOID)&ToggleKeysStruct, 0);
		}
	}

	~damn_keys_filter( )
	{
		if (bScreenSaverState)
		{	// Restoring screen saver
			SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, 0);
		}

		if (dwStickyKeysFlags)
		{
			// Restore StickyKeys feature
			StickyKeysStruct.dwFlags = dwStickyKeysFlags;
			SystemParametersInfo(SPI_SETSTICKYKEYS, dwStickyKeysStructSize, (PVOID)&StickyKeysStruct, 0);
		}

		if (dwFilterKeysFlags)
		{
			// Restore FilterKeys feature
			FilterKeysStruct.dwFlags = dwFilterKeysFlags;
			SystemParametersInfo(SPI_SETFILTERKEYS, dwFilterKeysStructSize, (PVOID)&FilterKeysStruct, 0);
		}

		if (dwToggleKeysFlags)
		{
			// Restore FilterKeys feature
			ToggleKeysStruct.dwFlags = dwToggleKeysFlags;
			SystemParametersInfo(SPI_SETTOGGLEKEYS, dwToggleKeysStructSize, (PVOID)&ToggleKeysStruct, 0);
		}
	}
};

#undef dwStickyKeysStructSize
#undef dwFilterKeysStructSize
#undef dwToggleKeysStructSize

typedef void DUMMY_STUFF(pcvoid, const u32&, pvoid);
CORE_API DUMMY_STUFF* g_temporary_stuff;

#define TRIVIAL_ENCRYPTOR_DECODER
#include "trivial_encryptor.h"

//#define RUSSIAN_BUILD

s32 APIENTRY WinMain_impl(HINSTANCE hInstance,
						  HINSTANCE hPrevInstance,
						  pstr lpCmdLine,
						  s32       nCmdShow)
{
	// Check for another instance
#ifdef NO_MULTI_INSTANCES
#define STALKER_PRESENCE_MUTEX "STALKER-SoC"

	HANDLE hCheckPresenceMutex = INVALID_HANDLE_VALUE;
	hCheckPresenceMutex = OpenMutex(READ_CONTROL, FALSE, STALKER_PRESENCE_MUTEX);
	if (hCheckPresenceMutex == NULL)
	{
		// New mutex
		hCheckPresenceMutex = CreateMutex(NULL, FALSE, STALKER_PRESENCE_MUTEX);
		if (hCheckPresenceMutex == NULL)
		{	// Shit happens
			return 2;
		}
	}
	else
	{
		// Already running
		CloseHandle(hCheckPresenceMutex);
		return 1;
	}
#endif

	SetThreadAffinityMask(GetCurrentThread( ), 1);

	// Title window
	logoWindow = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_STARTUP), 0, logDlgProc);
	SetWindowPos(
		logoWindow,
#ifndef DEBUG
		HWND_TOPMOST,
#else
		HWND_NOTOPMOST,
#endif // NDEBUG
		0,
		0,
		0,
		0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW
	);

	// AVI
	g_bIntroFinished = TRUE;

	g_sLaunchOnExit_app[0] = NULL;
	g_sLaunchOnExit_params[0] = NULL;

	pcstr						fsgame_ltx_name = "-fsltx ";
	string_path					fsgame = "";
	if (strstr(lpCmdLine, fsgame_ltx_name))
	{
		s32						sz = xr_strlen(fsgame_ltx_name);
		sscanf(strstr(lpCmdLine, fsgame_ltx_name) + sz, "%[^ ] ", fsgame);
	}

	g_temporary_stuff = &trivial_encryptor::decode;

	compute_build_id( );
	Core._initialize("xray", NULL, TRUE, fsgame[0] ? fsgame : NULL);
	InitSettings( );

	{
		damn_keys_filter		filter;
		(void)filter;

		FPU::m24r( );
		InitEngine( );
		InitConsole( );

		pcstr benchName = "-batch_benchmark ";
		if (strstr(lpCmdLine, benchName))
		{
			s32 sz = xr_strlen(benchName);
			string64 b_name;
			sscanf(strstr(Core.Params, benchName) + sz, "%[^ ] ", b_name);
			doBenchmark(b_name);
			return 0;
		}

		if (strstr(Core.Params, "-r2a"))
		{
			Console->Execute("renderer renderer_r2a");
		}
		else if (strstr(Core.Params, "-r2"))
		{
			Console->Execute("renderer renderer_r2");
		}
		else
		{
			CCC_LoadCFG_custom* pTmp = xr_new<CCC_LoadCFG_custom>("renderer ");
			pTmp->Execute(Console->ConfigFile);
			xr_delete(pTmp);
		}

		InitInput( );
		Engine.External.Initialize( );
		Console->Execute("stat_memory");
		Startup( );
		Core._destroy( );

		pstr _args[3];
		// check for need to execute something external
		if (/*xr_strlen(g_sLaunchOnExit_params) && */xr_strlen(g_sLaunchOnExit_app))
		{
			string4096 ModuleFileName = "";
			GetModuleFileName(NULL, ModuleFileName, 4096);

			string4096 ModuleFilePath = "";
			pstr ModuleName = NULL;
			GetFullPathName(ModuleFileName, 4096, ModuleFilePath, &ModuleName);
			ModuleName[0] = 0;
			strcat(ModuleFilePath, g_sLaunchOnExit_app);
			_args[0] = g_sLaunchOnExit_app;
			_args[1] = g_sLaunchOnExit_params;
			_args[2] = NULL;

			_spawnv(_P_NOWAIT, _args[0], _args);//, _envvar);
		}

#ifdef NO_MULTI_INSTANCES
		// Delete application presence mutex
		CloseHandle(hCheckPresenceMutex);
#endif

	}
	// here damn_keys_filter class instanse will be destroyed

	return						0;
}

s32 stack_overflow_exception_filter(s32 exception_code)
{
	if (exception_code == EXCEPTION_STACK_OVERFLOW)
	{
		// Do not call _resetstkoflw here, because
		// at this point, the stack is not yet unwound.
		// Instead, signal that the handler (the __except block)
		// is to be executed.
		return EXCEPTION_EXECUTE_HANDLER;
	}
	else
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
}

s32 APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 pstr lpCmdLine,
					 s32       nCmdShow)
{
	__try
	{
		Debug._initialize( );

		WinMain_impl(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	}
	__except (stack_overflow_exception_filter(GetExceptionCode( )))
	{
		_resetstkoflw( );
		FATAL("stack overflow");
	}

	return					(0);
}

pcstr _GetFontTexName(pcstr section)
{
	static char* tex_names[ ] = {"texture800","texture","texture1600"};
	s32 def_idx = 1;//default 1024x768
	s32 idx = def_idx;

#if 0
	u32 w = Device.dwWidth;

	if (w <= 800)
	{
		idx = 0;
	}
	else if (w <= 1280)
	{
		idx = 1;
	}
	else
	{
		idx = 2;
	}
#else
	u32 h = Device.dwHeight;

	if (h <= 600)
	{
		idx = 0;
	}
	else if (h <= 900)
	{
		idx = 1;
	}
	else
	{
		idx = 2;
	}
#endif

	while (idx >= 0)
	{
		if (pSettings->line_exist(section, tex_names[idx]))
		{
			return pSettings->r_string(section, tex_names[idx]);
		}

		--idx;
	}

	return pSettings->r_string(section, tex_names[def_idx]);
}

void _InitializeFont(CGameFont*& F, pcstr section, u32 flags)
{
	pcstr font_tex_name = _GetFontTexName(section);
	R_ASSERT(font_tex_name);

	if (!F)
	{
		F = xr_new<CGameFont>("font", font_tex_name, flags);
		Device.seqRender.Add(F, REG_PRIORITY_LOW - 1000);
	}
	else
	{
		F->Initialize("font", font_tex_name);
	}

	if (pSettings->line_exist(section, "size"))
	{
		f32 sz = pSettings->r_float(section, "size");
		if (flags & CGameFont::fsDeviceIndependent)
		{
			F->SetHeightI(sz);
		}
		else
		{
			F->SetHeight(sz);
		}
	}

	if (pSettings->line_exist(section, "interval"))
	{
		F->SetInterval(pSettings->r_fvector2(section, "interval"));
	}
}

CApplication::CApplication( )
{
	ll_dwReference = 0;

	// events
	eQuit = Engine.Event.Handler_Attach("KERNEL:quit", this);
	eStart = Engine.Event.Handler_Attach("KERNEL:start", this);
	eStartLoad = Engine.Event.Handler_Attach("KERNEL:load", this);
	eDisconnect = Engine.Event.Handler_Attach("KERNEL:disconnect", this);

	// levels
	Level_Current = 0;
	Level_Scan( );

	// Font
	pFontSystem = NULL;

	// Register us
	Device.seqFrame.Add(this, REG_PRIORITY_HIGH + 1000);

	if (psDeviceFlags.test(mtSound))
	{
		Device.seqFrameMT.Add(&SoundProcessor);
	}
	else
	{
		Device.seqFrame.Add(&SoundProcessor);
	}

	Console->Show( );

	// App Title
	app_title[0] = '\0';
}

CApplication::~CApplication( )
{
	Console->Hide( );

	// font
	Device.seqRender.Remove(pFontSystem);
	xr_delete(pFontSystem);

	Device.seqFrameMT.Remove(&SoundProcessor);
	Device.seqFrame.Remove(&SoundProcessor);
	Device.seqFrame.Remove(this);

	// events
	Engine.Event.Handler_Detach(eDisconnect, this);
	Engine.Event.Handler_Detach(eStartLoad, this);
	Engine.Event.Handler_Detach(eStart, this);
	Engine.Event.Handler_Detach(eQuit, this);
}

extern CRenderDevice Device;

void CApplication::OnEvent(EVENT E, u64 P1, u64 P2)
{
	if (E == eQuit)
	{
		PostQuitMessage(0);

		for (u32 i = 0; i < Levels.size( ); i++)
		{
			xr_free(Levels[i].folder);
			xr_free(Levels[i].name);
		}
	}
	else if (E == eStart)
	{
		pstr op_server = pstr(P1);
		pstr op_client = pstr(P2);
		R_ASSERT(0 == g_pGameLevel);
		R_ASSERT(0 != g_pGamePersistent);

		{
			Console->Execute("main_menu off");
			Console->Hide( );
			Device.Reset(false);
			//-----------------------------------------------------------
			g_pGamePersistent->PreStart(op_server);
			//-----------------------------------------------------------
			g_pGameLevel = (IGame_Level*)NEW_INSTANCE(CLSID_GAME_LEVEL);
			pApp->LoadBegin( );
			g_pGamePersistent->Start(op_server);
			g_pGameLevel->net_Start(op_server, op_client);
			pApp->LoadEnd( );
		}

		xr_free(op_server);
		xr_free(op_client);
	}
	else if (E == eDisconnect)
	{
		if (g_pGameLevel)
		{
			Console->Hide( );
			g_pGameLevel->net_Stop( );
			DEL_INSTANCE(g_pGameLevel);
			Console->Show( );

			if ((FALSE == Engine.Event.Peek("KERNEL:quit")) && (FALSE == Engine.Event.Peek("KERNEL:start")))
			{
				Console->Execute("main_menu off");
				Console->Execute("main_menu on");
			}
		}

		R_ASSERT(0 != g_pGamePersistent);
		g_pGamePersistent->Disconnect( );
	}
}

static	CTimer	phase_timer;
extern	ENGINE_API BOOL			g_appLoaded = FALSE;

void CApplication::LoadBegin( )
{
	ll_dwReference++;
	if (1 == ll_dwReference)
	{
		g_appLoaded = FALSE;

		_InitializeFont(pFontSystem, "ui_font_graffiti19_russian", 0);

		ll_hGeom.create(FVF::F_TL, RCache.Vertex.Buffer( ), RCache.QuadIB);
		sh_progress.create("hud\\default", "ui\\ui_load");
		ll_hGeom2.create(FVF::F_TL, RCache.Vertex.Buffer( ), NULL);

		phase_timer.Start( );
		load_stage = 0;
	}
}

void CApplication::LoadEnd( )
{
	ll_dwReference--;
	if (0 == ll_dwReference)
	{
		Msg("* phase time: %d ms", phase_timer.GetElapsed_ms( ));
		Msg("* phase cmem: %d K", Memory.mem_usage( ) / 1024);
		Console->Execute("stat_memory");
		g_appLoaded = TRUE;
	}
}

void CApplication::destroy_loading_shaders( )
{
	hLevelLogo.destroy( );
	sh_progress.destroy( );
	//.	::Sound->mute			(false);
}

u32 calc_progress_color(u32, u32, s32, s32);

void CApplication::LoadDraw( )
{
	if (g_appLoaded)
	{
		return;
	}

	Device.dwFrame += 1;

	if (!Device.Begin( ))
	{
		return;
	}

	load_draw_internal( );

	Device.End( );
}

void CApplication::LoadTitleInt(pcstr str)
{
	load_stage++;

	VERIFY(ll_dwReference);
	VERIFY(str && xr_strlen(str) < 256);
	strcpy_s(app_title, str);
	Msg("* phase time: %d ms", phase_timer.GetElapsed_ms( ));	phase_timer.Start( );
	Msg("* phase cmem: %d K", Memory.mem_usage( ) / 1024);
	//.	Console->Execute			("stat_memory");
	Log(app_title);

	if (g_pGamePersistent->GameType( ) == 1 && strstr(Core.Params, "alife"))
	{
		max_load_stage = 17;
	}
	else
	{
		max_load_stage = 14;
	}

	LoadDraw( );
}

// Sequential
void CApplication::OnFrame( )
{
	Engine.Event.OnFrame( );
	g_SpatialSpace->update( );
	g_SpatialSpacePhysic->update( );
	if (g_pGameLevel)
	{
		g_pGameLevel->SoundEvent_Dispatch( );
	}
}

void CApplication::Level_Append(pcstr folder)
{
	string_path	N1, N2, N3, N4;
	strconcat(sizeof(N1), N1, folder, "level");
	strconcat(sizeof(N2), N2, folder, "level.ltx");
	strconcat(sizeof(N3), N3, folder, "level.geom");
	strconcat(sizeof(N4), N4, folder, "level.cform");
	if (
		FS.exist("$game_levels$", N1) &&
		FS.exist("$game_levels$", N2) &&
		FS.exist("$game_levels$", N3) &&
		FS.exist("$game_levels$", N4)
		)
	{
		sLevelInfo LI;
		LI.folder = xr_strdup(folder);
		LI.name = 0;
		Levels.push_back(LI);
	}
}

void CApplication::Level_Scan( )
{
#pragma todo("container is created in stack!")
	xr_vector<char*>* folder = FS.file_list_open("$game_levels$", FS_ListFolders | FS_RootOnly);
	R_ASSERT(folder && folder->size( ));
	for (u32 i = 0; i < folder->size( ); i++)
	{
		Level_Append((*folder)[i]);
	}

	FS.file_list_close(folder);

#ifdef DEBUG
	folder = FS.file_list_open("$game_levels$", "$debug$\\", FS_ListFolders | FS_RootOnly);
	if (folder)
	{
		string_path	tmp_path;
		for (u32 i = 0; i < folder->size( ); i++)
		{
			strconcat(sizeof(tmp_path), tmp_path, "$debug$\\", (*folder)[i]);
			Level_Append(tmp_path);
		}

		FS.file_list_close(folder);
	}
#endif

}

void CApplication::Level_Set(u32 L)
{
	if (L >= Levels.size( ))
	{
		return;
	}
	Level_Current = L;
	FS.get_path("$level$")->_set(Levels[L].folder);

	string_path temp;
	string_path temp2;
	strconcat(sizeof(temp), temp, "intro\\intro_", Levels[L].folder);
	temp[xr_strlen(temp) - 1] = 0;
	if (FS.exist(temp2, "$game_textures$", temp, ".dds"))
	{
		hLevelLogo.create("font", temp);
	}
	else
	{
		hLevelLogo.create("font", "intro\\intro_no_start_picture");
	}
}

s32 CApplication::Level_ID(pcstr name)
{
	string256 buffer;
	strconcat(sizeof(buffer), buffer, name, "\\");
	for (u32 I = 0; I < Levels.size( ); I++)
	{
		if (0 == stricmp(buffer, Levels[I].folder))
		{
			return s32(I);
		}
	}

	return -1;
}

void doBenchmark(pcstr name)
{
	g_bBenchmark = true;
	string_path in_file;
	FS.update_path(in_file, "$app_data_root$", name);
	CIniFile ini(in_file);
	s32 test_count = ini.line_count("benchmark");
	pcstr test_name;
	pcstr t;
	shared_str test_command;
	for (s32 i = 0; i < test_count; ++i)
	{
		ini.r_line("benchmark", i, &test_name, &t);
		strcpy_s(g_sBenchmarkName, test_name);

		test_command = ini.r_string_wb("benchmark", test_name);
		strcpy_s(Core.Params, *test_command);
		_strlwr_s(Core.Params);

		InitInput( );
		if (i)
		{
			ZeroMemory(&HW, sizeof(CHW));
			InitEngine( );
		}

		Engine.External.Initialize( );

		strcpy_s(Console->ConfigFile, "user.ltx");
		if (strstr(Core.Params, "-ltx "))
		{
			string64				c_name;
			sscanf(strstr(Core.Params, "-ltx ") + 5, "%[^ ] ", c_name);
			strcpy_s(Console->ConfigFile, c_name);
		}

		Startup( );
	}
}

#pragma optimize("g", off)
void CApplication::load_draw_internal( )
{
	if (!sh_progress)
	{
		CHK_DX(HW.pDevice->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1, 0));
		return;
	}

	// Draw logo
	u32	Offset;
	u32	C = 0xffffffff;
	u32	_w = Device.dwWidth;
	u32	_h = Device.dwHeight;
	FVF::TL* pv = NULL;

	// progress
	f32 bw = 1024.0f;
	f32 bh = 768.0f;
	fVector2					k;
	k.set(f32(_w) / bw, f32(_h) / bh);

	RCache.set_Shader(sh_progress);
	CTexture* T = RCache.get_ActiveTexture(0);
	fVector2					tsz;
	tsz.set((f32)T->get_Width( ), (f32)T->get_Height( ));
	fRect						back_text_coords;
	fRect						back_coords;
	fVector2					back_size;

	// progress background
	static f32 offs = -0.5f;

	back_size.set(1024.0f, 768.0f);
	back_text_coords.lt.set(0.0f, 0.0f);
	back_text_coords.rb.add(back_text_coords.lt, back_size);
	back_coords.lt.set(offs, offs);
	back_coords.rb.add(back_coords.lt, back_size);

	back_coords.lt.mul(k);
	back_coords.rb.mul(k);

	back_text_coords.lt.x /= tsz.x;
	back_text_coords.lt.y /= tsz.y;
	back_text_coords.rb.x /= tsz.x;
	back_text_coords.rb.y /= tsz.y;

	pv = (FVF::TL*)RCache.Vertex.Lock(4, ll_hGeom.stride( ), Offset);
	pv->set(back_coords.lt.x, back_coords.rb.y, C, back_text_coords.lt.x, back_text_coords.rb.y);
	pv++;
	pv->set(back_coords.lt.x, back_coords.lt.y, C, back_text_coords.lt.x, back_text_coords.lt.y);
	pv++;
	pv->set(back_coords.rb.x, back_coords.rb.y, C, back_text_coords.rb.x, back_text_coords.rb.y);
	pv++;
	pv->set(back_coords.rb.x, back_coords.lt.y, C, back_text_coords.rb.x, back_text_coords.lt.y);
	pv++;
	RCache.Vertex.Unlock(4, ll_hGeom.stride( ));

	RCache.set_Geometry(ll_hGeom);
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

	// progress bar
	back_size.set(268.0f, 37.0f);
	back_text_coords.lt.set(0.0f, 768.0f);
	back_text_coords.rb.add(back_text_coords.lt, back_size);
	back_coords.lt.set(379.0f, 726.0f);
	back_coords.rb.add(back_coords.lt, back_size);

	back_coords.lt.mul(k);
	back_coords.rb.mul(k);

	back_text_coords.lt.x /= tsz.x;
	back_text_coords.lt.y /= tsz.y;
	back_text_coords.rb.x /= tsz.x;
	back_text_coords.rb.y /= tsz.y;

	u32 v_cnt = 40;
	pv = (FVF::TL*)RCache.Vertex.Lock(2 * (v_cnt + 1), ll_hGeom2.stride( ), Offset);
	FVF::TL* _pv = pv;
	f32 pos_delta = back_coords.width( ) / v_cnt;
	f32 tc_delta = back_text_coords.width( ) / v_cnt;
	u32 clr = C;

	for (u32 idx = 0; idx < v_cnt + 1; ++idx)
	{
		clr = calc_progress_color(idx, v_cnt, load_stage, max_load_stage);
		pv->set(back_coords.lt.x + pos_delta * idx + offs, back_coords.rb.y + offs, 0 + EPSILON_7, 1, clr, back_text_coords.lt.x + tc_delta * idx, back_text_coords.rb.y);
		pv++;
		pv->set(back_coords.lt.x + pos_delta * idx + offs, back_coords.lt.y + offs, 0 + EPSILON_7, 1, clr, back_text_coords.lt.x + tc_delta * idx, back_text_coords.lt.y);
		pv++;
	}

	VERIFY(u32(pv - _pv) == 2 * (v_cnt + 1));
	RCache.Vertex.Unlock(2 * (v_cnt + 1), ll_hGeom2.stride( ));

	RCache.set_Geometry(ll_hGeom2);
	RCache.Render(D3DPT_TRIANGLESTRIP, Offset, 2 * v_cnt);

	// Draw title
	VERIFY(pFontSystem);
	pFontSystem->Clear( );
	pFontSystem->SetColor(color_rgba(157, 140, 120, 255));
	pFontSystem->SetAligment(CGameFont::alCenter);
	pFontSystem->OutI(0.0f, 0.815f, app_title);
	pFontSystem->OnRender( );

	// draw level-specific screenshot
	if (hLevelLogo)
	{
		fRect r;
		r.lt.set(257.0f, 369.0f);
		r.lt.x += offs;
		r.lt.y += offs;
		r.rb.add(r.lt, fVector2( ).set(512.0f, 256.0f));
		r.lt.mul(k);
		r.rb.mul(k);
		pv = (FVF::TL*)RCache.Vertex.Lock(4, ll_hGeom.stride( ), Offset);
		pv->set(r.lt.x, r.rb.y, C, 0.0f, 1.0f);
		pv++;
		pv->set(r.lt.x, r.lt.y, C, 0.0f, 0.0f);
		pv++;
		pv->set(r.rb.x, r.rb.y, C, 1.0f, 1.0f);
		pv++;
		pv->set(r.rb.x, r.lt.y, C, 1.0f, 0.0f);
		pv++;
		RCache.Vertex.Unlock(4, ll_hGeom.stride( ));

		RCache.set_Shader(hLevelLogo);
		RCache.set_Geometry(ll_hGeom);
		RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
	}
}

u32 calc_progress_color(u32 idx, u32 total, s32 stage, s32 max_stage)
{
	if (idx > (total / 2))
	{
		idx = total - idx;
	}

	f32 kk = (f32(stage + 1) / f32(max_stage)) * (total / 2.0f);
	f32 f = 1 / (exp((f32(idx) - kk) * 0.5f) + 1.0f);

	return color_argb_f(f, 1.0f, 1.0f, 1.0f);
}
