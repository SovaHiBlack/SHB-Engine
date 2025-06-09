#include "stdafx.h"

#include "F1.h"

CF1::CF1( )
{ }

CF1::~CF1( )
{ }

using namespace luabind;

#pragma optimize("s",on)
void CF1::script_register(lua_State* L)
{
	module(L)
		[
			class_<CF1, CGameObject>("CF1")
				.def(constructor<>( ))
		];
}
