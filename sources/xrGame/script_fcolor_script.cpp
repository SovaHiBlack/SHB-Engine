////////////////////////////////////////////////////////////////////////////
//	Module 		: script_Fcolor_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script float vector script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_fcolor.h"

using namespace luabind;

#pragma optimize("s",on)
void CScriptFcolor::script_register(lua_State *L)
{
	module(L)
	[
		class_<fColor>("fColor")
			.def_readwrite("r",					&fColor::r)
			.def_readwrite("g",					&fColor::g)
			.def_readwrite("b",					&fColor::b)
			.def_readwrite("a",					&fColor::a)
			.def(								constructor<>())
			.def("set",							(fColor & (fColor::*)(f32, f32, f32, f32))(&fColor::set),														return_reference_to(_1))
			.def("set",							(fColor & (fColor::*)(const fColor&))(&fColor::set),																return_reference_to(_1))
			.def("set",							(fColor & (fColor::*)(u32))(&fColor::set),																			return_reference_to(_1))
	];
}
