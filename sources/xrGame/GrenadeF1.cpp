#include "stdafx.h"

#include "GrenadeF1.h"

CGrenadeF1::CGrenadeF1( )
{ }

CGrenadeF1::~CGrenadeF1( )
{ }

using namespace luabind;

#pragma optimize("s",on)
void CGrenadeF1::script_register(lua_State* L)
{
	module(L)
		[
			class_<CGrenadeF1, CGameObject>("CGrenadeF1")
			.def(constructor<>( ))
		];
}
