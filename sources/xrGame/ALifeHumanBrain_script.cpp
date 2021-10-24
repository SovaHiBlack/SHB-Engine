//	Module 		: ALifeHumanBrain_script.cpp
//	Description : ALife human brain class script export

#include "stdafx.h"

#include "ALifeHumanBrain.h"

using namespace luabind;

#pragma optimize("s",on)
void CALifeHumanBrain::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CALifeHumanBrain,CALifeMonsterBrain>("CALifeHumanBrain")
	];
}
