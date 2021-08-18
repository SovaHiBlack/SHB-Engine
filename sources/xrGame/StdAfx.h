#pragma once

#pragma warning(disable:4995)
#include "..\ENGINE\stdafx.h"
#pragma warning(default:4995)
#pragma warning( 4 : 4018 )
#pragma warning( 4 : 4244 )
#pragma warning(disable:4505)

// this include MUST be here, since smart_cast is used >1800 times in the project
#include "smart_cast.h"

#define READ_IF_EXISTS(ltx,method,section,name,default_value)\
	((ltx->line_exist(section,name)) ? (ltx->method(section,name)) : (default_value))

#if XRAY_EXCEPTIONS
inline xr_string	string2xr_string(const char* s) {return *CSharedString(s ? s : "");}
inline void		throw_and_log(const xr_string &s) {Msg("! %s",s.c_str()); throw * CSharedString(s.c_str());}
#	define THROW(xpr)				if (!(xpr)) {throw_and_log (__FILE__LINE__" Expression \""#xpr"\"");}
#	define THROW2(xpr, msg0)		if (!(xpr)) {throw *CSharedString(xr_string(__FILE__LINE__).append(" \"").append(#xpr).append(string2xr_string(msg0)).c_str());}
#	define THROW3(xpr, msg0, msg1)	if (!(xpr)) {throw *CSharedString(xr_string(__FILE__LINE__).append(" \"").append(#xpr).append(string2xr_string(msg0)).append(", ").append(string2xr_string(msg1)).c_str());}
#else
#	define THROW					VERIFY
#	define THROW2					VERIFY2
#	define THROW3					VERIFY3
#endif

#include "..\ENGINE\GameFont.h"
#include "..\ENGINE\Object.h"
#include "..\ENGINE\IGameLevel.h"

#ifdef PUBLIC_BUILD
#ifndef DEBUG
#	define MASTER_GOLD
#endif // DEBUG
#endif

extern "C" {
#include "..\LUA\src\lua.h"
#include "..\LUA\src\lualib.h"
#include "..\LUA\src\lauxlib.h"
#include "..\LUA\src\luajit.h"
};

#pragma warning(push)
#pragma warning(disable:4995)
#include "..\LUA\luabind/luabind.hpp"
#pragma warning(pop)

#include "..\LUA\luabind\object.hpp"
#include "..\LUA\luabind/functor.hpp"
#include "..\LUA\luabind/operator.hpp"
#include "..\LUA\luabind/adopt_policy.hpp"
#include "..\LUA\luabind/return_reference_to_policy.hpp"
#include "..\LUA\luabind/out_value_policy.hpp"
#include "..\LUA\luabind/iterator_policy.hpp"

#include <depr/dplay8.h>// раньше небыло
