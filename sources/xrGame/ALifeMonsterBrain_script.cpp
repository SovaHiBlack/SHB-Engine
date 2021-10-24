//	Module 		: ALifeMonsterBrain_script.cpp
//	Description : ALife monster brain class script export

#include "stdafx.h"

#include "ALifeMonsterBrain.h"
#include "ALifeMonsterMovementManager.h"

using namespace luabind;

CALifeMonsterMovementManager* get_movement(const CALifeMonsterBrain* brain)
{
	return &brain->movement( );
}

#pragma optimize("s",on)
void CALifeMonsterBrain::script_register(lua_State* L)
{
	module(L)
		[
			class_<CALifeMonsterBrain>("CALifeMonsterBrain")
			.def("movement", &get_movement)
			.def("update", &CALifeMonsterBrain::update)
			.def("can_choose_alife_tasks", (void (CALifeMonsterBrain::*)(bool)) & CALifeMonsterBrain::can_choose_alife_tasks)
		];
}
