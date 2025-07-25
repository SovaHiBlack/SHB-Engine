////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_alife_task_actions.cpp
//	Created 	: 25.10.2004
//  Modified 	: 25.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker alife task action classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_alife_task_actions.h"
#include "ai/stalker/Stalker.h"
#include "ai/trader/Trader.h"
//#include "InventoryItem.h"
#include "Weapon.h"
#include "script_game_object.h"
#include "Inventory.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "stalker_decision_space.h"
#include "cover_manager.h"
#include "cover_evaluators.h"
#include "cover_point.h"
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"
#include "game_location_selector.h"
#include "sight_manager.h"
#include "ai_object_location.h"
#include "stalker_movement_manager.h"
#include "ai/stalker/ai_stalker_space.h"
#include "AISpace.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "alife_human_brain.h"
#include "alife_smart_terrain_task.h"

using namespace StalkerSpace;
using namespace StalkerDecisionSpace;

//#define GRENADE_TEST

#ifdef GRENADE_TEST
#	include "Actor.h"
#endif

//////////////////////////////////////////////////////////////////////////
// CStalkerActionSolveZonePuzzle
//////////////////////////////////////////////////////////////////////////

CStalkerActionSolveZonePuzzle::CStalkerActionSolveZonePuzzle(CStalker* object, pcstr action_name) :
	inherited				(object,action_name)
{ }

void CStalkerActionSolveZonePuzzle::initialize	()
{
	inherited::initialize						();

#ifndef GRENADE_TEST
	m_stop_weapon_handling_time					= Device.dwTimeGlobal;
	if (object().inventory().ActiveItem() && object().best_weapon() && (object().inventory().ActiveItem()->object().ID() == object().best_weapon()->object().ID()))
		m_stop_weapon_handling_time				+= ::Random32.random(30000) + 30000;

//	object().movement().set_desired_position	(0);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeGamePath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeWalk);
	object().movement().set_mental_state		(eMentalStateFree);
	object().sight().setup						(CSightAction(SightManager::eSightTypeCover,false,true));
#else
#	if 1
//		object().movement().set_desired_position	(0);
		object().movement().set_desired_direction	(0);
		object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
		object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
		object().movement().set_body_state			(eBodyStateStand);
		object().movement().set_movement_type		(eMovementTypeStand);
		object().movement().set_mental_state		(eMentalStateFree);
//		object().sight().setup						(CSightAction(g_actor,true));
		object().sight().setup						(CSightAction(SightManager::eSightTypeCurrentDirection));
#	else
		object().movement().set_mental_state		(eMentalStateDanger);
		object().movement().set_movement_type		(eMovementTypeStand);
		object().movement().set_body_state			(eBodyStateStand);
		object().movement().set_desired_direction	(0);
		object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
		object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
		object().movement().set_nearest_accessible_position();
		object().sight().setup						(CSightAction(SightManager::eSightTypeCurrentDirection));
		object().CObjectHandler::set_goal			(eObjectActionFire1,object().inventory().m_slots[1].m_pIItem,0,1,2500,3000);
#	endif
#endif
}

void CStalkerActionSolveZonePuzzle::finalize	()
{
	inherited::finalize				();

//	object().movement().set_desired_position	(0);

	if (!object().g_Alive())
		return;

	object().sound().remove_active_sounds		(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionSolveZonePuzzle::execute		()
{
	inherited::execute				();

#ifndef GRENADE_TEST
	if (Device.dwTimeGlobal >= m_stop_weapon_handling_time)
		if (!object().best_weapon())
			object().CObjectHandler::set_goal	(eObjectActionIdle);
		else
			object().CObjectHandler::set_goal	(eObjectActionStrapped,object().best_weapon());
	else
		object().CObjectHandler::set_goal		(eObjectActionIdle,object().best_weapon());
#else
#	if 0
		object().throw_target					(g_actor->Position());
		object().CObjectHandler::set_goal		(eObjectActionFire1,object().inventory().m_slots[3].m_pIItem);
#	else
#		if 0
			const CWeapon							*weapon = smart_cast<const CWeapon*>(object().best_weapon());
			VERIFY									(weapon);
			if (!weapon->strapped_mode())
				object().CObjectHandler::set_goal	(eObjectActionStrapped,object().best_weapon());
			else
				object().CObjectHandler::set_goal	(eObjectActionIdle,object().best_weapon());
#		else
			const CWeapon							*weapon = smart_cast<const CWeapon*>(object().best_weapon());
			VERIFY									(weapon);
//			Msg										("weapon %s is strapped : %c",*weapon->cName(),weapon->strapped_mode() ? '+' : '-');

			static u32 m_time_to_strap = 0;
			static u32 m_time_to_idle = 0;
			if (!object().inventory().ActiveItem() || (object().inventory().ActiveItem() == object().inventory().m_slots[1].m_pIItem)) {
				if (!m_time_to_strap)
					m_time_to_strap					= Device.dwTimeGlobal + 10000;
				if (Device.dwTimeGlobal >= m_time_to_strap) {
					m_time_to_idle					= 0;
					object().CObjectHandler::set_goal	(eObjectActionStrapped,object().best_weapon());
				}
			}
			else {
				const CWeapon						*weapon = smart_cast<const CWeapon*>(object().best_weapon());
				VERIFY								(weapon);
				if (weapon->strapped_mode()) {
					if (!m_time_to_idle)
						m_time_to_idle					= Device.dwTimeGlobal + 10000;
					if (Device.dwTimeGlobal >= m_time_to_idle) {
						m_time_to_strap					= 0;
						object().CObjectHandler::set_goal	(eObjectActionIdle,object().inventory().m_slots[1].m_pIItem);
					}
				}
			}

#		endif
#	endif
#endif
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionSmartTerrain
//////////////////////////////////////////////////////////////////////////

CStalkerActionSmartTerrain::CStalkerActionSmartTerrain(CStalker* object, pcstr action_name) :
	inherited				(object,action_name)
{
	set_inertia_time		(30000);
}

void CStalkerActionSmartTerrain::initialize				()
{
	inherited::initialize							();
//	object().movement().set_desired_position		(0);
	object().movement().set_desired_direction		(0);
	object().movement().game_selector().set_selection_type		(eSelectionTypeMask);
	object().movement().set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state				(eBodyStateStand);
	object().movement().set_movement_type			(eMovementTypeWalk);
	object().movement().set_mental_state			(eMentalStateFree);
	object().sight().setup							(CSightAction(SightManager::eSightTypePathDirection));

	if (!object().best_weapon()) {
		object().CObjectHandler::set_goal		(eObjectActionIdle);
		return;
	}

	object().CObjectHandler::set_goal			(eObjectActionIdle);

	CWeapon										*best_weapon = smart_cast<CWeapon*>(object().best_weapon());
	if (object().CObjectHandler::weapon_strapped(best_weapon))
		return;

	object().CObjectHandler::set_goal			(eObjectActionIdle,object().best_weapon());
}

void CStalkerActionSmartTerrain::finalize				()
{
	inherited::finalize					();
//	object().movement().set_desired_position	(0);
	object().movement().game_selector().set_selection_type		(eSelectionTypeRandomBranching);
}

void CStalkerActionSmartTerrain::execute				()
{
	inherited::execute					();

	if (completed())
		object().CObjectHandler::set_goal		(eObjectActionStrapped,object().best_weapon());

	object().sound().play						(eStalkerSoundHumming,60000,10000);

	CSE_ALifeHumanAbstract						*stalker = smart_cast<CSE_ALifeHumanAbstract*>(ai().alife().objects().object(m_object->ID()));
	VERIFY										(stalker);
	VERIFY										(stalker->m_smart_terrain_id != 0xffff);

	CALifeSmartTerrainTask						*task = stalker->brain().smart_terrain().task(stalker);
	THROW2										(task,"Smart terrain is assigned but returns no task");
	if (object().ai_location().game_vertex_id() != task->game_vertex_id()) {
		object().movement().set_path_type		(MovementManager::ePathTypeGamePath);
		object().movement().set_game_dest_vertex(task->game_vertex_id());
		return;
	}

	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	if (object().movement().accessible(task->level_vertex_id())) {
		object().movement().set_level_dest_vertex	(task->level_vertex_id());
		fVector3										temp = task->position();
		object().movement().set_desired_position	(&temp);
		return;
	}

	object().movement().set_nearest_accessible_position	(task->position(),task->level_vertex_id());
}
