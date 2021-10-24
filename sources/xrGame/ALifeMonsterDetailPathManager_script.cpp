//	Module 		: ALifeMonsterDetailPathManager_script.cpp
//	Description : ALife monster detail path manager class script export

#include "stdafx.h"

#include "ALifeMonsterDetailPathManager.h"
#include "alife_smart_terrain_task.h"

using namespace luabind;

#pragma optimize("s", on)
void CALifeMonsterDetailPathManager::script_register(lua_State* L)
{
	module(L)
		[
			class_<CALifeMonsterDetailPathManager>("CALifeMonsterDetailPathManager")
			.def("target", (void (CALifeMonsterDetailPathManager::*)(const GameGraph::_GRAPH_ID&, const u32&, const Fvector3&))(&CALifeMonsterDetailPathManager::target))
			.def("target", (void (CALifeMonsterDetailPathManager::*)(const GameGraph::_GRAPH_ID&))(&CALifeMonsterDetailPathManager::target))
			.def("target", (void (CALifeMonsterDetailPathManager::*)(const CALifeSmartTerrainTask*))(&CALifeMonsterDetailPathManager::target))
			.def("speed	", (void (CALifeMonsterDetailPathManager::*)(const float&))(&CALifeMonsterDetailPathManager::speed))
			.def("speed	", (const float& (CALifeMonsterDetailPathManager::*)() const)(&CALifeMonsterDetailPathManager::speed))
			.def("completed", &CALifeMonsterDetailPathManager::completed)
			.def("actual", &CALifeMonsterDetailPathManager::actual)
			.def("failed", &CALifeMonsterDetailPathManager::failed)
		];
}
