//	Module 		: xrServer_Objects_ALife_script.cpp
//	Description : Server objects for ALife simulator, script export

#include "stdafx.h"

#include "xrServer_Objects_ALife.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "xrServer_script_macroses.h"
#include "alife_smart_terrain_task.h"

using namespace luabind;

#pragma optimize("s",on)
void CSE_ALifeSchedulable::script_register(lua_State* L)
{
	module(L)[
		class_<IPureSchedulableObject>
			("ipure_schedulable_object"),
//			.def(		constructor<>()),

class_<CSE_ALifeSchedulable, IPureSchedulableObject>
("cse_alife_schedulable")
//			.def(		constructor<const char*>())
	];
}

void CSE_ALifeGraphPoint::script_register(lua_State* L)
{
	module(L)[
		luabind_class_abstract1(
			CSE_ALifeGraphPoint,
			"cse_alife_graph_point",
			CSE_Abstract
		)
	];
}

void CSE_ALifeObject::script_register(lua_State* L)
{
	module(L)[
		luabind_class_alife1(
			CSE_ALifeObject,
			"cse_alife_object",
			CSE_Abstract
		)
			.def_readonly("online", &CSE_ALifeObject::m_bOnline)
			.def("move_offline", (bool (CSE_ALifeObject::*)() const)(&CSE_ALifeObject::move_offline))
			.def("move_offline", (void (CSE_ALifeObject::*)(bool))(&CSE_ALifeObject::move_offline))
			.def("visible_for_map", (bool (CSE_ALifeObject::*)() const)(&CSE_ALifeObject::visible_for_map))
			.def("visible_for_map", (void (CSE_ALifeObject::*)(bool))(&CSE_ALifeObject::visible_for_map))
			.def("can_switch_online", (void (CSE_ALifeObject::*)(bool))(&CSE_ALifeObject::can_switch_online))
			.def("can_switch_offline", (void (CSE_ALifeObject::*)(bool))(&CSE_ALifeObject::can_switch_offline))
			.def_readonly("m_level_vertex_id", &CSE_ALifeObject::m_tNodeID)
			.def_readonly("m_game_vertex_id", &CSE_ALifeObject::m_tGraphID)
			.def_readonly("m_story_id", &CSE_ALifeObject::m_story_id)
	];
}

void CSE_ALifeGroupAbstract::script_register(lua_State* L)
{
	module(L)[
		class_<CSE_ALifeGroupAbstract>
			("cse_alife_group_abstract")
//			.def(		constructor<const char*>())
	];
}

void CSE_ALifeDynamicObject::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife1(
			CSE_ALifeDynamicObject,
			"cse_alife_dynamic_object",
			CSE_ALifeObject
		)
	];
}

void CSE_ALifeDynamicObjectVisual::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife2(
			CSE_ALifeDynamicObjectVisual,
			"cse_alife_dynamic_object_visual",
			CSE_ALifeDynamicObject,
			CSE_Visual
		)
	];
}

void CSE_ALifePHSkeletonObject::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife2(
			CSE_ALifePHSkeletonObject,
			"cse_alife_ph_skeleton_object",
			CSE_ALifeDynamicObjectVisual,
			CSE_PHSkeleton
		)
	];
}

void CSE_ALifeSpaceRestrictor::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife2(
			CSE_ALifeSpaceRestrictor,
			"cse_alife_space_restrictor",
			CSE_ALifeDynamicObject,
			CSE_Shape
		)
	];
}

void CSE_ALifeLevelChanger::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife1(
			CSE_ALifeLevelChanger,
			"cse_alife_level_changer",
			CSE_ALifeSpaceRestrictor
		)
	];
}

void CSE_ALifeObjectProjector::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife1(
			CSE_ALifeObjectProjector,
			"cse_alife_object_projector",
			CSE_ALifeDynamicObjectVisual
		)
	];
}

void CSE_ALifeHelicopter::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife3(
			CSE_ALifeHelicopter,
			"cse_alife_helicopter",
			CSE_ALifeDynamicObjectVisual,
			CSE_Motion,
			CSE_PHSkeleton
		)
	];
}

void CSE_ALifeCar::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife2(
			CSE_ALifeCar,
			"cse_alife_car",
			CSE_ALifeDynamicObjectVisual,
			CSE_PHSkeleton
		)
	];
}

void CSE_ALifeObjectBreakable::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife1(
			CSE_ALifeObjectBreakable,
			"cse_alife_object_breakable",
			CSE_ALifeDynamicObjectVisual
		)
	];
}

void CSE_ALifeObjectClimable::script_register(lua_State* L)
{
	module(L)[
		luabind_class_abstract2(
			CSE_ALifeObjectClimable,
			"cse_alife_object_climable",
			CSE_Shape,
			CSE_Abstract
		)
	];
}

void CSE_ALifeMountedWeapon::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife1(
			CSE_ALifeMountedWeapon,
			"cse_alife_mounted_weapon",
			CSE_ALifeDynamicObjectVisual
		)
	];
}

void CSE_ALifeTeamBaseZone::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife1(
			CSE_ALifeTeamBaseZone,
			"cse_alife_team_base_zone",
			CSE_ALifeSpaceRestrictor
		)
	];
}

void CSE_ALifeObjectHangingLamp::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife2(
			CSE_ALifeObjectHangingLamp,
			"cse_alife_object_hanging_lamp",
			CSE_ALifeDynamicObjectVisual,
			CSE_PHSkeleton
		)
	];
}

void CSE_ALifeObjectPhysic::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife2(
			CSE_ALifeObjectPhysic,
			"cse_alife_object_physic",
			CSE_ALifeDynamicObjectVisual,
			CSE_PHSkeleton
		)
	];
}

void CSE_ALifeSmartZone::script_register(lua_State* L)
{
	module(L)[
		luabind_class_zone2(
			CSE_ALifeSmartZone,
			"cse_alife_smart_zone",
			CSE_ALifeSpaceRestrictor,
			CSE_ALifeSchedulable
		)
	];
}
