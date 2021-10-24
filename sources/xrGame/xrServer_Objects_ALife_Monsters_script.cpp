//	Module 		: xrServer_Objects_ALife_Monsters_script.cpp
//	Description : Server monsters for ALife simulator, script export

#include "stdafx.h"

#include "xrServer_Objects_ALife_Monsters.h"
#include "xrServer_script_macroses.h"
#include "ALifeHumanBrain.h"

using namespace luabind;

const char* profile_name_script(CSE_ALifeTraderAbstract* ta)
{
	return *ta->character_profile( );
}

CALifeMonsterBrain* monster_brain(CSE_ALifeMonsterAbstract* monster)
{
	THROW(monster);
	return	(&monster->brain( ));
}

CALifeHumanBrain* human_brain(CSE_ALifeHumanAbstract* human)
{
	THROW(human);
	return &human->brain( );
}

void clear_smart_terrain(CSE_ALifeMonsterAbstract* monster)
{
	THROW(monster);
	monster->m_smart_terrain_id = 0xffff;
}

void smart_terrain_task_activate(CSE_ALifeMonsterAbstract* monster)
{
	THROW(monster);
	monster->m_task_reached = true;
}

void smart_terrain_task_deactivate(CSE_ALifeMonsterAbstract* monster)
{
	THROW(monster);
	monster->m_task_reached = false;
}

ALife::_OBJECT_ID smart_terrain_id(CSE_ALifeMonsterAbstract* monster)
{
	THROW(monster);
	return monster->m_smart_terrain_id;
}

#pragma optimize("s",on)
void CSE_ALifeTraderAbstract::script_register(lua_State* L)
{
	module(L)[
		class_<CSE_ALifeTraderAbstract>
			("cse_alife_trader_abstract")
//			.def(		constructor<const char*>())
.def("community", &CommunityName)
.def("profile_name", &profile_name_script)
.def("rank", &Rank)
.def("reputation", &Reputation)
	];
}

void CSE_ALifeTrader::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife2(
			CSE_ALifeTrader,
			"cse_alife_trader",
			CSE_ALifeDynamicObjectVisual,
			CSE_ALifeTraderAbstract
		)
	];
}

void CSE_ALifeCustomZone::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife2(
			CSE_ALifeCustomZone,
			"cse_custom_zone",
			CSE_ALifeDynamicObject,
			CSE_Shape
		)
	];
}

void CSE_ALifeAnomalousZone::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife1(
			CSE_ALifeAnomalousZone,
			"cse_anomalous_zone",
			CSE_ALifeCustomZone
		)
			.def("spawn_artefacts", &CSE_ALifeAnomalousZone::spawn_artefacts)
	];
}

void CSE_ALifeCreatureCrow::script_register(lua_State* L)
{
	module(L)[
		luabind_class_creature1(
			CSE_ALifeCreatureCrow,
			"cse_alife_creature_crow",
			CSE_ALifeCreatureAbstract
		)
	];
}

void CSE_ALifeMonsterZombie::script_register(lua_State* L)
{
	module(L)[
		luabind_class_monster1(
			CSE_ALifeMonsterZombie,
			"cse_alife_monster_zombie",
			CSE_ALifeMonsterAbstract
		)
	];
}

void CSE_ALifeMonsterBase::script_register(lua_State* L)
{
	module(L)[
		luabind_class_monster2(
			CSE_ALifeMonsterBase,
			"cse_alife_monster_base",
			CSE_ALifeMonsterAbstract,
			CSE_PHSkeleton
		)
	];
}

void CSE_ALifeHumanStalker::script_register(lua_State* L)
{
	module(L)[
		luabind_class_monster2(
			CSE_ALifeHumanStalker,
			"cse_alife_human_stalker",
			CSE_ALifeHumanAbstract,
			CSE_PHSkeleton
		)
	];
}

void CSE_ALifeCreatureActor::script_register(lua_State* L)
{
	module(L)[
		luabind_class_creature3(
			CSE_ALifeCreatureActor,
			"cse_alife_creature_actor",
			CSE_ALifeCreatureAbstract,
			CSE_ALifeTraderAbstract,
			CSE_PHSkeleton
		)
	];
}

void CSE_ALifeTorridZone::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife2(
			CSE_ALifeTorridZone,
			"cse_torrid_zone",
			CSE_ALifeCustomZone,
			CSE_Motion
		)
	];
}

void CSE_ALifeZoneVisual::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife2(
			CSE_ALifeZoneVisual,
			"cse_zone_visual",
			CSE_ALifeAnomalousZone,
			CSE_Visual
		)
	];
}

void CSE_ALifeCreaturePhantom::script_register(lua_State* L)
{
	module(L)[
		luabind_class_creature1(
			CSE_ALifeCreaturePhantom,
			"cse_alife_creature_phantom",
			CSE_ALifeCreatureAbstract
		)
	];
}

void CSE_ALifeCreatureAbstract::script_register(lua_State* L)
{
	module(L)[
		luabind_class_creature1(
			CSE_ALifeCreatureAbstract,
			"cse_alife_creature_abstract",
			CSE_ALifeDynamicObjectVisual
		)
			.def("health", &CSE_ALifeCreatureAbstract::g_Health)
			.def("alive", &CSE_ALifeCreatureAbstract::g_Alive)
			.def_readwrite("team", &CSE_ALifeCreatureAbstract::s_team)
			.def_readwrite("squad", &CSE_ALifeCreatureAbstract::s_squad)
			.def_readwrite("group", &CSE_ALifeCreatureAbstract::s_group)
	];
}

void CSE_ALifeOnlineOfflineGroup::script_register(lua_State* L)
{
	module(L)[
		luabind_class_dynamic_alife2(
			CSE_ALifeOnlineOfflineGroup,
			"cse_alife_online_offline_group",
			CSE_ALifeDynamicObject,
			CSE_ALifeSchedulable
		)
	];
}

void CSE_ALifeMonsterAbstract::script_register(lua_State* L)
{
	module(L)[
		luabind_class_monster2(
			CSE_ALifeMonsterAbstract,
			"cse_alife_monster_abstract",
			CSE_ALifeCreatureAbstract,
			CSE_ALifeSchedulable
		)
			.def("smart_terrain_id", &smart_terrain_id)
			.def_readwrite("m_smart_terrain_id", &CSE_ALifeMonsterAbstract::m_smart_terrain_id)
			.def("clear_smart_terrain", &clear_smart_terrain)
			.def("brain", &monster_brain)
			.def("rank", &CSE_ALifeMonsterAbstract::Rank)
			.def("smart_terrain_task_activate", &smart_terrain_task_activate)
			.def("smart_terrain_task_deactivate", &smart_terrain_task_deactivate)
	];
}

void CSE_ALifeHumanAbstract::script_register(lua_State* L)
{
	module(L)[
		luabind_class_monster2(
			CSE_ALifeHumanAbstract,
			"cse_alife_human_abstract",
			CSE_ALifeTraderAbstract,
			CSE_ALifeMonsterAbstract
		)
			.def("brain", &human_brain)
			.def("rank", &CSE_ALifeTraderAbstract::Rank)
			.def("set_rank", &CSE_ALifeTraderAbstract::SetRank)
	];
}

void CSE_ALifePsyDogPhantom::script_register(lua_State* L)
{
	module(L)[
		luabind_class_monster1(
			CSE_ALifePsyDogPhantom,
			"cse_alife_psydog_phantom",
			CSE_ALifeMonsterBase
		)
	];
}
