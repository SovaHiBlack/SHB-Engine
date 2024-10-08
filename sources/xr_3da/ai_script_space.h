////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_space.h
//	Created 	: 22.09.2003
//  Modified 	: 22.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script space
////////////////////////////////////////////////////////////////////////////

#pragma once

// Lua
#pragma warning(disable:4244)
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#pragma warning(disable:4267)

//#define LUABIND_NO_EXCEPTIONS

extern "C" {
	//#define BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

	#include "..\xrLua\src\lua.h"
	#include "..\xrLua\src\lualib.h"
	#include "..\xrLua\src\lauxlib.h"
	#include "..\xrLua\src\luajit.h"
}

// Lua-bind
//#include <luabind/luabind.hpp>
//#include <luabind/object.hpp>

#include <luabind/luabind.hpp>
#include <luabind/class.hpp>
#include <luabind/object.hpp>
#include <luabind/operator.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/return_reference_to_policy.hpp>
#include <luabind/out_value_policy.hpp>
#include <luabind/iterator_policy.hpp>


#pragma warning(default:4244)
#pragma warning(default:4995)
#pragma warning(default:4267)

using CLuaVirtualMachine = lua_State;
//typedef lua_State CLuaVirtualMachine;

struct SMemberCallback {
	luabind::functor<void>	*m_lua_function;
	luabind::object			*m_lua_object;
	shared_str				m_method_name;
};

#include "ai_script_lua_space.h"
