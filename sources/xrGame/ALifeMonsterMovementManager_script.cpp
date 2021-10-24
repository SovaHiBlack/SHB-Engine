//	Module 		: ALifeMonsterMovementManager_script.cpp
//	Description : ALife monster movement manager class script export

#include "stdafx.h"

#include "ALifeMonsterMovementManager.h"
#include "ALifeMonsterDetailPathManager.h"
#include "ALifeMonsterPatrolPathManager.h"

using namespace luabind;

CALifeMonsterDetailPathManager* get_detail(const CALifeMonsterMovementManager* self)
{
	return &self->detail( );
}

CALifeMonsterPatrolPathManager* get_patrol(const CALifeMonsterMovementManager* self)
{
	return &self->patrol( );
}

#pragma optimize("s", on)
void CALifeMonsterMovementManager::script_register(lua_State* L)
{
	module(L)
		[
			class_<CALifeMonsterMovementManager>("CALifeMonsterMovementManager")
			.def("detail", &get_detail)
			.def("patrol", &get_patrol)
			.def("path_type", (void (CALifeMonsterMovementManager::*)(const EPathType&))(&CALifeMonsterMovementManager::path_type))
			.def("path_type", (const EPathType & (CALifeMonsterMovementManager::*)() const)(&CALifeMonsterMovementManager::path_type))
			.def("actual", &CALifeMonsterMovementManager::actual)
			.def("completed", &CALifeMonsterMovementManager::completed)
		];
}
