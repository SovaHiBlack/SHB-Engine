////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_params_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path parameters class script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "patrol_path_params.h"

using namespace luabind;

fVector3 CPatrolPathParams__point(const CPatrolPathParams *self, u32 index)
{
	THROW	(self);
	return	(self->point(index));
}

#pragma optimize("s",on)
void CPatrolPathParams::script_register(lua_State *L)
{
	module(L)
	[
		class_<CPatrolPathParams>("patrol")
			.enum_("start")
			[
				value("start", s32(PatrolPathManager::ePatrolStartTypeFirst)),
				value("stop", s32(PatrolPathManager::ePatrolStartTypeLast)),
				value("nearest", s32(PatrolPathManager::ePatrolStartTypeNearest)),
				value("custom", s32(PatrolPathManager::ePatrolStartTypePoint)),
				value("next", s32(PatrolPathManager::ePatrolStartTypeNext)),
				value("dummy", s32(PatrolPathManager::ePatrolStartTypeDummy))
			]
			.enum_("stop")
			[
				value("stop", s32(PatrolPathManager::ePatrolRouteTypeStop)),
				value("continue", s32(PatrolPathManager::ePatrolRouteTypeContinue)),
				value("dummy", s32(PatrolPathManager::ePatrolRouteTypeDummy))
			]
			.def(								constructor<pcstr>())
			.def(								constructor<pcstr,const PatrolPathManager::EPatrolStartType>())
			.def(								constructor<pcstr,const PatrolPathManager::EPatrolStartType,const PatrolPathManager::EPatrolRouteType>())
			.def(								constructor<pcstr,const PatrolPathManager::EPatrolStartType,const PatrolPathManager::EPatrolRouteType, bool>())
			.def(								constructor<pcstr,const PatrolPathManager::EPatrolStartType,const PatrolPathManager::EPatrolRouteType, bool, u32>())
			.def("count",						&CPatrolPathParams::count)
			.def("level_vertex_id",				&CPatrolPathParams::level_vertex_id)
			.def("game_vertex_id",				&CPatrolPathParams::game_vertex_id)
			.def("point",						&CPatrolPathParams__point)
			.def("name",						&CPatrolPathParams::name)
			.def("index",						(u32			(CPatrolPathParams::*)(pcstr)			const)	(&CPatrolPathParams::point))
			.def("get_nearest",					(u32			(CPatrolPathParams::*)(const fVector3&) const)	(&CPatrolPathParams::point))
			.def("flag",						&CPatrolPathParams::flag)
			.def("flags",						&CPatrolPathParams::flags)
			.def("terminal",					&CPatrolPathParams::terminal)
	];
}
