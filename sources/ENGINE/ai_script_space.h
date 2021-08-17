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

extern "C"
{
	//#define BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

#include "..\Lua\src\lua.h"
#include "..\Lua\src\lualib.h"
#include "..\Lua\src\lauxlib.h"
#include "..\Lua\src\luajit.h"
}

// Lua-bind
#include "..\Lua\luabind/luabind.hpp"
#include "..\Lua\luabind/object.hpp"

#pragma warning(default:4244)
#pragma warning(default:4995)
#pragma warning(default:4267)

using CLuaVirtualMachine = lua_State;

//typedef lua_State CLuaVirtualMachine;

struct SMemberCallback
{
	luabind::functor<void>*		m_lua_function;
	luabind::object*			m_lua_object;
	CSharedString					m_method_name;
};

#include "ai_script_lua_space.h"
