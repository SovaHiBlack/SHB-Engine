////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_names.h
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation names
////////////////////////////////////////////////////////////////////////////

#pragma once

extern pcstr state_names			[];
extern pcstr weapon_names			[];
extern pcstr weapon_action_names	[];
extern pcstr food_names			[];
extern pcstr food_action_names		[];
extern pcstr movement_names		[];
extern pcstr movement_action_names	[];
extern pcstr in_place_names		[];
extern pcstr global_names			[];
extern pcstr head_names			[];

enum ECriticalWoundType {
	critical_wound_type_head		= u32(4),
	critical_wound_type_torso,
	critical_wound_type_hand_left,
	critical_wound_type_hand_right,
	critical_wound_type_leg_left,
	critical_wound_type_leg_right,
	critical_wound_type_dummy		= u32(-1),
};
