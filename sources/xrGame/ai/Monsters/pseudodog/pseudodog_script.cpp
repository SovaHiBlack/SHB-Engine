#include "stdafx.h"

#include "PseudoDog.h"
#include "psy_dog.h"

using namespace luabind;

#pragma optimize("s",on)
void CPseudoDog::script_register(lua_State *L)
{
	module(L)
	[
		class_<CPseudoDog,CGameObject>("CPseudoDog")
			.def(constructor<>())
	];
}

void CPsyDog::script_register(lua_State *L)
{
	module(L)
		[
			class_<CPsyDog,CGameObject>("CPsyDog")
			.def(constructor<>())
		];
}

void CPsyDogPhantom::script_register(lua_State *L)
{
	module(L)
		[
			class_<CPsyDogPhantom,CGameObject>("CPsyDogPhantom")
			.def(constructor<>())
		];
}
