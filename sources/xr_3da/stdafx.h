#pragma once

#include "../xrCore/xrCore.h"

#ifdef _DEBUG
	#define D3D_DEBUG_INFO
#endif

#pragma warning(disable:4995)
#include <d3d9.h>
#pragma warning(default:4995)

// you must define ENGINE_BUILD then building the engine itself
// and not define it if you are about to build DLL
#ifdef	ENGINE_BUILD
	#define DLL_API			__declspec(dllimport)
	#define ENGINE_API		__declspec(dllexport)
#else
	#define DLL_API			__declspec(dllexport)
	#define ENGINE_API		__declspec(dllimport)
#endif

// Our headers
#include "engine.h"
#include "defines.h"
#include "device.h"

#include "xrXRC.h"

#include "../xrSound/sound.h"

extern ENGINE_API CInifile *pGameIni;

#pragma comment( lib, "xrCore.lib"	)
#pragma comment( lib, "xrCDB.lib"	)
#pragma comment( lib, "xrSound.lib"	)
#pragma comment( lib, "xrLUA.lib"	)

#pragma comment( lib, "winmm.lib"		)

#pragma comment( lib, "d3d9.lib"		)
#pragma comment( lib, "dinput8.lib"		)
#pragma comment( lib, "dxguid.lib"		)

#ifndef DEBUG
#	define LUABIND_NO_ERROR_CHECKING
#endif

#if	!defined(DEBUG) || defined(FORCE_NO_EXCEPTIONS)
	// release: no error checking, no exceptions
	#define LUABIND_NO_EXCEPTIONS
	#define BOOST_THROW_EXCEPTION_HPP_INCLUDED
	namespace std	{	class exception; }
	namespace boost {	ENGINE_API	void throw_exception(const std::exception &A);	};
#endif
#define LUABIND_DONT_COPY_STRINGS
