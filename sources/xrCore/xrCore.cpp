// xrCore.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"

#include <mmsystem.h>
#include <objbase.h>

#pragma comment(lib,"winmm.lib")

#ifdef DEBUG
#	include	<malloc.h>
#endif // DEBUG

CORE_API		xrCore	Core;
CORE_API		u32		build_id;
CORE_API		pcstr	build_date;

namespace CPU
{
	extern	void			Detect();
};

static u32	init_counter = 0;

extern char g_application_path[256];

void xrCore::_initialize(pcstr _ApplicationName, LogCallback cb, BOOL init_fs, pcstr fs_fname)
{
	strcpy_s(ApplicationName, _ApplicationName);
	if (0 == init_counter)
	{
		// Init COM so we can use CoCreateInstance
//		HRESULT co_res = 
		CoInitializeEx(NULL, COINIT_MULTITHREADED);

		strcpy_s(Params, sizeof(Params), GetCommandLine());
		_strlwr_s(Params, sizeof(Params));

		string_path		fn, dr, di;

		// application path
		GetModuleFileName(GetModuleHandle(MODULE_NAME), fn, sizeof(fn));
		_splitpath(fn, dr, di, 0, 0);
		strconcat(sizeof(ApplicationPath), ApplicationPath, dr, di);

		strcpy_s(g_application_path, sizeof(g_application_path), ApplicationPath);

		// working path
		if (strstr(Params, "-wf"))
		{
			string_path				c_name;
			sscanf(strstr(Core.Params, "-wf ") + 4, "%[^ ] ", c_name);
			SetCurrentDirectory(c_name);

		}
		GetCurrentDirectory(sizeof(WorkingPath), WorkingPath);

		// User/Comp Name
		DWORD	sz_user = sizeof(UserName);
		GetUserName(UserName, &sz_user);

		DWORD	sz_comp = sizeof(CompName);
		GetComputerName(CompName, &sz_comp);

		// Mathematics & PSI detection
		CPU::Detect();

		Memory._initialize(strstr(Params, "-mem_debug") ? TRUE : FALSE);

		DUMP_PHASE;

		InitLog();
		_initialize_cpu();

		rtc_initialize();

		xr_FS = xr_new<CLocatorAPI>();

		xr_EFS = xr_new<EFS_Utils>();
		//.		R_ASSERT			(co_res==S_OK);
	}
	if (init_fs)
	{
		u32 flags = 0;
		if (0 != strstr(Params, "-build"))	 flags |= CLocatorAPI::flBuildCopy;
		if (0 != strstr(Params, "-ebuild")) flags |= CLocatorAPI::flBuildCopy | CLocatorAPI::flEBuildCopy;
#ifdef DEBUG
		if (strstr(Params, "-cache"))  flags |= CLocatorAPI::flCacheFiles;
		else flags &= ~CLocatorAPI::flCacheFiles;
#endif // DEBUG

		flags |= CLocatorAPI::flScanAppRoot;

		if (0 != strstr(Params, "-file_activity"))	 flags |= CLocatorAPI::flDumpFileActivity;

		FS._initialize(flags, 0, fs_fname);
		Msg("'%s' build %d, %s\n", "xrCore", build_id, build_date);
		EFS._initialize();
#ifdef DEBUG
		Msg("CRT heap 0x%08x", _get_heap_handle());
		Msg("Process heap 0x%08x", GetProcessHeap());
#endif // DEBUG
	}
	SetLogCB(cb);
	init_counter++;
}

void xrCore::_destroy()
{
	--init_counter;
	if (0 == init_counter)
	{
		FS._destroy();
		EFS._destroy();
		xr_delete(xr_FS);
		xr_delete(xr_EFS);

		Memory._destroy();
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD ul_reason_for_call, LPVOID lpvReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			_clear87();
			_control87(_PC_53, MCW_PC);
			_control87(_RC_CHOP, MCW_RC);
			_control87(_RC_NEAR, MCW_RC);
			_control87(_MCW_EM, MCW_EM);
		}
		//.		LogFile.reserve		(256);
		break;
		case DLL_THREAD_ATTACH:
			CoInitializeEx(NULL, COINIT_MULTITHREADED);
			timeBeginPeriod(1);
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
#ifdef USE_MEMORY_MONITOR
			memory_monitor::flush_each_time(true);
#endif // USE_MEMORY_MONITOR
			break;
	}
	return TRUE;
}
