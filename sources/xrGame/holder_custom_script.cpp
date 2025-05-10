#include "stdafx.h"

#include "holder_custom.h"

using namespace luabind;

#pragma optimize("s",on)
void CHolderCustom::script_register(lua_State *L)
{
	module(L)
		[
			class_<CHolderCustom>("holder")
			.def("engaged",			&CHolderCustom::Engaged)
			.def("Action",			&CHolderCustom::Action)
//			.def("SetParam",		(void (CHolderCustom::*)(s32,fVector2)) &CHolderCustom::SetParam)
			.def("SetParam",		(void (CHolderCustom::*)(s32, fVector3)) &CHolderCustom::SetParam)
		];
}