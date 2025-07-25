////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_combat_action_base.cpp
//	Created 	: 25.03.2004
//  Modified 	: 12.01.2007
//	Author		: Dmitriy Iassenev
//	Description : Stalker base combat action
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "stalker_combat_action_base.h"
#include "ai/stalker/Stalker.h"
#include "script_game_object.h"
#include "sound_player.h"
#include "ai/stalker/ai_stalker_space.h"
#include "memory_manager.h"
#include "enemy_manager.h"
#include "stalker_movement_manager.h"
#include "object_handler.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "agent_enemy_manager.h"
#include "InventoryItem.h"

using namespace StalkerSpace;

const f32 start_fire_angle_difference = PI_DIV_8;

CStalkerActionCombatBase::CStalkerActionCombatBase(CStalker* object, pcstr action_name) :
	inherited(object, action_name)
{ }

void CStalkerActionCombatBase::initialize( )
{
	inherited::initialize( );
	object( ).sound( ).remove_active_sounds(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionCombatBase::finalize( )
{
	inherited::finalize( );

	if (!object( ).g_Alive( ))
		return;

	object( ).sound( ).set_sound_mask(0);
}

bool CStalkerActionCombatBase::fire_make_sense( ) const
{
	return					(object( ).fire_make_sense( ));
}

void CStalkerActionCombatBase::fire( )
{
	fVector3								enemy_position = object( ).memory( ).enemy( ).selected( )->Position( );
	fVector3								object_position = object( ).Position( );
	fVector3								direction = fVector3( ).sub(enemy_position, object_position);
	f32								yaw, pitch;
	direction.getHP(yaw, pitch);
	const MonsterSpace::SBoneRotation& current_angles = object( ).movement( ).head_orientation( );
	if (angle_difference(-yaw, current_angles.current.yaw) > start_fire_angle_difference)
	{
		aim_ready( );
		return;
	}

	u32									min_queue_size, max_queue_size, min_queue_interval, max_queue_interval;
	f32								distance = enemy_position.distance_to(object_position);
	select_queue_params(distance, min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
	object( ).CObjectHandler::set_goal(eObjectActionFire1, object( ).best_weapon( ), min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
}

void CStalkerActionCombatBase::aim_ready( )
{
	u32									min_queue_size, max_queue_size, min_queue_interval, max_queue_interval;
	f32								distance = object( ).memory( ).enemy( ).selected( )->Position( ).distance_to(object( ).Position( ));
	select_queue_params(distance, min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
	object( ).CObjectHandler::set_goal(eObjectActionAimReady1, object( ).best_weapon( ), min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
}

void CStalkerActionCombatBase::aim_ready_force_full( )
{
	u32									min_queue_size, max_queue_size, min_queue_interval, max_queue_interval;
	f32								distance = object( ).memory( ).enemy( ).selected( )->Position( ).distance_to(object( ).Position( ));
	select_queue_params(distance, min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
	object( ).CObjectHandler::set_goal(eObjectActionAimForceFull1, object( ).best_weapon( ), min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
}

void CStalkerActionCombatBase::select_queue_params(const f32& distance, u32& min_queue_size, u32& max_queue_size, u32& min_queue_interval, u32& max_queue_interval) const
{
	/**/
	s32									weapon_type = 6;
	if (object( ).best_weapon( ))
		weapon_type = object( ).best_weapon( )->object( ).ef_weapon_type( );

	switch (weapon_type)
	{
// pistols
		case 5:
		{
			if (distance > 30.f)
			{
				min_queue_size = 1;
				max_queue_size = 1;
				min_queue_interval = 1000;
				max_queue_interval = 1250;
			}
			else
				if (distance > 15.f)
				{
					min_queue_size = 2;
					max_queue_size = 4;
					min_queue_interval = 750;
					max_queue_interval = 1000;
				}
				else
				{
					min_queue_size = 3;
					max_queue_size = 5;
					min_queue_interval = 500;
					max_queue_interval = 750;
				}

			break;
		}
		// shotguns
		case 7:
		{
			if (distance > 30.f)
			{
				min_queue_size = 1;
				max_queue_size = 1;
				min_queue_interval = 1250;
				max_queue_interval = 1500;
			}
			else
				if (distance > 15.f)
				{
					min_queue_size = 1;
					max_queue_size = 1;
					min_queue_interval = 750;
					max_queue_interval = 1250;
				}
				else
				{
					min_queue_size = 1;
					max_queue_size = 1;
					min_queue_interval = 500;
					max_queue_interval = 1000;
				}

			break;
		}
		// sniper rifles
		case 8:
		{
			if (distance > 30.f)
			{
				min_queue_size = 1;
				max_queue_size = 1;
				min_queue_interval = 3000;
				max_queue_interval = 4000;
			}
			else
				if (distance > 15.f)
				{
					min_queue_size = 1;
					max_queue_size = 1;
					min_queue_interval = 3000;
					max_queue_interval = 4000;
				}
				else
				{
					min_queue_size = 1;
					max_queue_size = 1;
					min_queue_interval = 3000;
					max_queue_interval = 4000;
				}

			break;
		}
		default:
		{
			if (distance > 30.f)
			{
				min_queue_size = object( ).min_queue_size_far( );
				max_queue_size = object( ).max_queue_size_far( );
				min_queue_interval = object( ).min_queue_interval_far( );
				max_queue_interval = object( ).max_queue_interval_far( );
			}
			else
				if (distance > 15.f)
				{
					min_queue_size = object( ).min_queue_size_medium( );
					max_queue_size = object( ).max_queue_size_medium( );
					min_queue_interval = object( ).min_queue_interval_medium( );
					max_queue_interval = object( ).max_queue_interval_medium( );
				}
				else
				{
					min_queue_size = object( ).min_queue_size_close( );
					max_queue_size = object( ).max_queue_size_close( );
					min_queue_interval = object( ).min_queue_interval_close( );
					max_queue_interval = object( ).max_queue_interval_close( );
				}
		}
	}
	/**
	if (distance > 30.f) {
		min_queue_size					= object().min_queue_size_far();
		max_queue_size					= object().max_queue_size_far();
		min_queue_interval				= object().min_queue_interval_far();
		max_queue_interval				= object().max_queue_interval_far();
	}
	else
		if (distance > 15.f) {
			min_queue_size				= object().min_queue_size_medium();
			max_queue_size				= object().max_queue_size_medium();
			min_queue_interval			= object().min_queue_interval_medium();
			max_queue_interval			= object().max_queue_interval_medium();
		}
		else {
			min_queue_size				= object().min_queue_size_close();
			max_queue_size				= object().max_queue_size_close();
			min_queue_interval			= object().min_queue_interval_close();
			max_queue_interval			= object().max_queue_interval_close();
		}
	/**/
}

void CStalkerActionCombatBase::play_panic_sound(u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time, u32 id)
{
	object( ).sound( ).play(
		object( ).memory( ).enemy( ).selected( )->human_being( ) ?
		eStalkerSoundPanicHuman :
		eStalkerSoundPanicMonster,
		max_start_time,
		min_start_time,
		max_stop_time,
		min_stop_time,
		id
	);
}

void CStalkerActionCombatBase::play_attack_sound(u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time, u32 id)
{
	if (!object( ).memory( ).enemy( ).selected( )->human_being( ))
		return;

	if (!object( ).agent_manager( ).member( ).can_cry_noninfo_phrase( ))
		return;

	u32						sound_type = eStalkerSoundAttackNoAllies;
#ifdef DEBUG
	if (object( ).agent_manager( ).member( ).combat_members( ).empty( ))
		Msg(
		"! I am in combat, but there is no combat members at all (including me), npc[%s],team[%d],squad[%d],group[%d]",
		*object( ).cName( ),
		object( ).g_Team( ),
		object( ).g_Squad( ),
		object( ).g_Group( )
		);
#endif // DEBUG

	if (object( ).agent_manager( ).member( ).combat_members( ).size( ) > 1)
	{
		if (object( ).agent_manager( ).enemy( ).enemies( ).size( ) > 1)
			sound_type = eStalkerSoundAttackAlliesSeveralEnemies;
		else
			sound_type = eStalkerSoundAttackAlliesSingleEnemy;
	}
	else
		sound_type = eStalkerSoundAttackNoAllies;

	object( ).sound( ).play(
		sound_type,
		max_start_time,
		min_start_time,
		max_stop_time,
		min_stop_time,
		id
	);
}

void CStalkerActionCombatBase::play_start_search_sound(u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time, u32 id)
{
	if (!object( ).agent_manager( ).member( ).can_cry_noninfo_phrase( ))
		return;

#ifdef DEBUG
	if (object( ).agent_manager( ).member( ).combat_members( ).empty( ))
		Msg("! I am in combat, but there is no combat members at all (including me), npc[%s],team[%d],squad[%d],group[%d]",
			*object( ).cName( ),
			object( ).g_Team( ),
			object( ).g_Squad( ),
			object( ).g_Group( )
		);
#endif // DEBUG

	bool					search_with_allies = object( ).agent_manager( ).member( ).combat_members( ).size( ) > 1;

	object( ).sound( ).play(
		search_with_allies ?
		eStalkerSoundSearch1WithAllies :
		eStalkerSoundSearch1NoAllies,
		max_start_time,
		min_start_time,
		max_stop_time,
		min_stop_time,
		id
	);
}
