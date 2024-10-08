#pragma once

#pragma warning(disable:4995)
#include "..\XR_3DA\stdafx.h"
#pragma warning(default:4995)
#pragma warning( 4 : 4018 )
#pragma warning( 4 : 4244 )
#pragma warning(disable:4505)

// this include MUST be here, since smart_cast is used >1800 times in the project
#include "smart_cast.h"

#define READ_IF_EXISTS(ltx,method,section,name,default_value)\
	((ltx->line_exist(section,name)) ? (ltx->method(section,name)) : (default_value))


#if XRAY_EXCEPTIONS
IC	xr_string	string2xr_string(pcstr s)
{
	return *shared_str(s ? s : "");
}
IC	void		throw_and_log(const xr_string& s)
{
	Msg("! %s", s.c_str()); throw* shared_str(s.c_str());
}
#	define		THROW(xpr)				if (!(xpr)) {throw_and_log (__FILE__LINE__" Expression \""#xpr"\"");}
#	define		THROW2(xpr,msg0)		if (!(xpr)) {throw *shared_str(xr_string(__FILE__LINE__).append(" \"").append(#xpr).append(string2xr_string(msg0)).c_str());}
#	define		THROW3(xpr,msg0,msg1)	if (!(xpr)) {throw *shared_str(xr_string(__FILE__LINE__).append(" \"").append(#xpr).append(string2xr_string(msg0)).append(", ").append(string2xr_string(msg1)).c_str());}
#else
#	define		THROW					VERIFY
#	define		THROW2					VERIFY2
#	define		THROW3					VERIFY3
#endif

#include "../XR_3DA\gamefont.h"
#include "../XR_3DA\xr_object.h"
#include "../XR_3DA\igame_level.h"

#ifdef PUBLIC_BUILD
#ifndef DEBUG
#	define MASTER_GOLD
#endif // DEBUG
#endif

extern "C" {
#include "..\xrLua\src\lua.h"
#include "..\xrLua\src\lualib.h"
#include "..\xrLua\src\lauxlib.h"
#include "..\xrLua\src\luajit.h"
};

#pragma warning(push)
#pragma warning(disable:4995)
#include <luabind/luabind.hpp>
#pragma warning(pop)

//#include <luabind/object.hpp>
//#include <luabind/functor.hpp>
//#include <luabind/operator.hpp>
//#include <luabind/adopt_policy.hpp>
//#include <luabind/return_reference_to_policy.hpp>
//#include <luabind/out_value_policy.hpp>
//#include <luabind/iterator_policy.hpp>

#include <luabind/class.hpp>
#include <luabind/object.hpp>
#include <luabind/operator.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/return_reference_to_policy.hpp>
#include <luabind/out_value_policy.hpp>
#include <luabind/iterator_policy.hpp>


#include <depr/dplay8.h>// раньше небыло
