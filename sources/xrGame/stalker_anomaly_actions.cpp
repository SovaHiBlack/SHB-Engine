////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_anomaly_actions.cpp
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker anomaly action classes
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "stalker_anomaly_actions.h"
#include "ai/stalker/Stalker.h"
#include "script_game_object.h"
#include "stalker_decision_space.h"
#include "CustomZone.h"
#include "space_restriction_manager.h"
#include "space_restriction_bridge.h"
#include "space_restriction_base.h"
#include "Inventory.h"
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"
#include "memory_manager.h"
#include "enemy_manager.h"
#include "sight_manager.h"
#include "restricted_object.h"
#include "stalker_movement_manager.h"
#include "sound_player.h"
#include "ai/stalker/ai_stalker_space.h"

using namespace StalkerSpace;
using namespace StalkerDecisionSpace;

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetOutOfAnomaly
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetOutOfAnomaly::CStalkerActionGetOutOfAnomaly(CStalker* object, pcstr action_name) : inherited(object, action_name)
{ }

void CStalkerActionGetOutOfAnomaly::initialize	()
{
	inherited::initialize				();

	object().sound().remove_active_sounds		(u32(eStalkerSoundMaskNoHumming));

	object().movement().set_desired_direction		(0);
	object().movement().set_path_type				(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type			(eMovementTypeWalk);
	object().movement().set_mental_state			(eMentalStateDanger);
	object().sight().setup				(SightManager::eSightTypeCurrentDirection);
	if	(	object().memory().enemy().selected() && 
			object().inventory().ActiveItem() && 
			object().best_weapon() && 
			(object().inventory().ActiveItem()->object().ID() == object().best_weapon()->object().ID())
		)
		object().CObjectHandler::set_goal	(eObjectActionIdle,object().best_weapon());
	else
		object().CObjectHandler::set_goal	(eObjectActionIdle);
	set_property						(eWorldPropertyAnomaly,true);
}

void CStalkerActionGetOutOfAnomaly::finalize	()
{
	inherited::finalize					();

	if (!object().g_Alive())
		return;

	object().sound().set_sound_mask			(0);
}

void CStalkerActionGetOutOfAnomaly::execute	()
{
	inherited::execute					();
//
	object().movement().set_path_type				(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type			(eMovementTypeWalk);
	object().movement().set_mental_state			(eMentalStateDanger);
//

	m_temp0.clear						();
	m_temp1.clear						();

	xr_vector<CObject*>::const_iterator	I = object().feel_touch.begin();
	xr_vector<CObject*>::const_iterator	E = object().feel_touch.end();
	for ( ; I != E; ++I) {
		CCustomZone						*zone = smart_cast<CCustomZone*>(*I);
		if (zone)
			m_temp0.push_back			(zone->ID());
	}
	
	object().movement().restrictions().add_restrictions	(m_temp1,m_temp0);
	object().movement().set_nearest_accessible_position	();
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDetectAnomaly
//////////////////////////////////////////////////////////////////////////

CStalkerActionDetectAnomaly::CStalkerActionDetectAnomaly(CStalker* object, pcstr action_name) : inherited				(object,action_name)
{ }

void CStalkerActionDetectAnomaly::initialize	()
{
	inherited::initialize			();
	object().sound().remove_active_sounds	(u32(eStalkerSoundMaskNoHumming));
	m_inertia_time					= 15000 + ::Random32.random(5000);

	fVector3							result;
	object().eye_matrix.transform_tiny	(result, fVector3().set(0.0f,0.0f,10.0f));
	object().throw_target			(result);
}

void CStalkerActionDetectAnomaly::finalize	()
{
	inherited::finalize				();

	if (!object().g_Alive())
		return;

	object().CObjectHandler::set_goal	(eObjectActionIdle);
	object().sound().set_sound_mask		(0);
}

void CStalkerActionDetectAnomaly::execute	()
{
	inherited::execute				();

	if (completed() || object().memory().enemy().selected()) {
		set_property				(eWorldPropertyAnomaly,false);
		return;
	}
	
	object().CObjectHandler::set_goal	(eObjectActionFire1,object().inventory().m_slots[5].m_pIItem);
}
