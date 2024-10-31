#pragma once

#ifndef DEBUG
#	ifdef _DEBUG
#		define DEBUG
#	endif
#endif // DEBUG

#include "build defines.h"

#pragma warning(disable:4996)

#if (defined(DEBUG) && !defined(FORCE_NO_EXCEPTIONS))
// "debug"
#ifndef _CPPUNWIND
#error Please enable exceptions...
#endif
#define _HAS_EXCEPTIONS		1	// STL
#define XRAY_EXCEPTIONS		1	// XRAY
#define BOOST_NO_EXCEPTIONS
#else
// "release"
#ifdef _CPPUNWIND
#error Please disable exceptions...
#endif
//	#define _HAS_EXCEPTIONS		1	// STL
#define XRAY_EXCEPTIONS		0	// XRAY
#define LUABIND_NO_EXCEPTIONS
#define BOOST_NO_EXCEPTIONS
#pragma warning(disable:4530)
#endif

#ifndef _MT
	// multithreading disabled
#error Please enable multi-threaded library...
#endif

#include "xrCore_platform.h"

// *** try to minimize code bloat of STLport
#ifdef CORE_EXPORTS				// no exceptions, export allocator and common stuff
#	define _STLP_DESIGNATED_DLL	1
#	define _STLP_USE_DECLSPEC		1
#else
#	define _STLP_USE_DECLSPEC		1	// no exceptions, import allocator and common stuff
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>

#include <typeinfo>

// inline control - redefine to use compiler's heuristics ONLY
// it seems "IC" is misused in many places which cause code-bloat
// ...and VC7.1 really don't miss opportunities for inline :)
#define _inline			inline
#define __inline		inline
#define IC				inline
#define ICF				__forceinline			// !!! this should be used only in critical places found by PROFILER
#define ICN			__declspec (noinline)	

#ifndef DEBUG
#pragma inline_depth	( 254 )
#pragma inline_recursion( on )
#pragma intrinsic	(abs, fabs, fmod, sin, cos, tan, asin, acos, atan, sqrt, exp, log, log10, strcpy, strcat)
#endif // DEBUG

#include <time.h>

#define ALIGN(a)		__declspec(align(a))
#include <sys\utime.h>
#define MODULE_NAME 	"xrCore.dll"

// Warnings
#pragma warning (disable : 4251 )		// object needs DLL interface
#pragma warning (disable : 4201 )		// nonstandard extension used : nameless struct/union
#pragma warning (disable : 4100 )		// unreferenced formal parameter
#pragma warning (disable : 4127 )		// conditional expression is constant
//#pragma warning (disable : 4530 )		// C++ exception handler used, but unwind semantics are not enabled
#pragma warning (disable : 4345 )
#pragma warning (disable : 4714 )		// __forceinline not inlined

#ifndef DEBUG
#pragma warning (disable : 4189 )		//  local variable is initialized but not refenced
#endif									//	frequently in release code due to large amount of VERIFY

// stl
#pragma warning (push)
#pragma warning (disable:4702)
#include <algorithm>
#include <limits>
#include <vector>
#include <stack>
#include <list>
#include <set>
#include <map>
#include <string>
#pragma warning (pop)
#pragma warning (disable : 4100 )		// unreferenced formal parameter

// Our headers
#ifdef CORE_EXPORTS
#	define CORE_API __declspec(dllexport)
#else
#	define CORE_API __declspec(dllimport)
#endif

#include "_types.h"
#include "_constants.h"
#include "_limits.h"

#include "xrDebug.h"
#include "vector.h"

#include "clsid.h"
#include "xrSyncronize.h"
#include "xrMemory.h"

#include "_stl_extensions.h"
#include "xrsharedmem.h"
#include "xrstring.h"
#include "xr_resource.h"
#include "rt_compressor.h"
#include "xr_shared.h"

// stl ext
struct CORE_API xr_rtoken
{
	shared_str	name;
	int	   	id;
	xr_rtoken(pcstr _nm, int _id)
	{
		name = _nm; id = _id;
	}
public:
	void	rename(pcstr _nm)
	{
		name = _nm;
	}
	bool	equal(pcstr _nm)
	{
		return (0 == xr_strcmp(*name, _nm));
	}
};

#pragma pack (push,1)
struct CORE_API xr_shortcut
{
	enum
	{
		flShift = 0x20,
		flCtrl = 0x40,
		flAlt = 0x80,
	};
	union
	{
		struct
		{
			u8	 	key;
			flags8	ext;
		};
		u16		hotkey;
	};
	xr_shortcut(u8 k, BOOL a, BOOL c, BOOL s) :key(k)
	{
		ext.assign(u8((a ? flAlt : 0) | (c ? flCtrl : 0) | (s ? flShift : 0)));
	}
	xr_shortcut()
	{
		ext.zero(); key = 0;
	}
	bool		similar(const xr_shortcut& v)const
	{
		return ext.equal(v.ext) && (key == v.key);
	}
};
#pragma pack (pop)

DEFINE_VECTOR(shared_str, RStringVec, RStringVecIt);
DEFINE_SET(shared_str, RStringSet, RStringSetIt);
DEFINE_VECTOR(xr_rtoken, RTokenVec, RTokenVecIt);

#include "FS.h"
#include "log.h"
#include "xr_trims.h"
#include "xr_ini.h"
#include "LocatorAPI.h"
#include "FileSystem.h"
#include "FTimer.h"
#include "FastDelegate.h"
#include "intrusive_ptr.h"

// destructor
template <class T>
class destructor
{
	T* ptr;
public:
	destructor(T* p)
	{
		ptr = p;
	}
	~destructor()
	{
		xr_delete(ptr);
	}
	IC T& operator() ()
	{
		return *ptr;
	}
};

// ********************************************** The Core definition
class CORE_API xrCore
{
public:
	string64	ApplicationName;
	string_path	ApplicationPath;
	string_path	WorkingPath;
	string64	UserName;
	string64	CompName;
	string512	Params;

public:
	void		_initialize(pcstr ApplicationName, LogCallback cb = 0, BOOL init_fs = TRUE, pcstr fs_fname = 0);
	void		_destroy();
};
extern CORE_API xrCore Core;
