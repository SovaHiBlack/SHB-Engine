#include "stdafx.h"

#include "Bloodsucker.h"

using namespace luabind;

#pragma optimize("s",on)
void CBloodsucker::script_register(lua_State *L)
{
	module(L)
	[
		class_<CBloodsucker,CGameObject>("CBloodsucker")
			.def(constructor<>())
	];
}
