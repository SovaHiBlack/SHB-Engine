//	Module 		: StalkerAnimationState.h
//	Description : Stalker state animations

#pragma once

#include "ai/ai_monsters_anims.h"
#include "StalkerAnimationNames.h"

class CStalkerAnimationState
{
public:
	using MOVEMENT_ACTIONS					= CAniCollection<CAniVector, movement_action_names>;
	using WEAPON_ACTIONS					= CAniCollection<CAniVector, weapon_action_names>;
	using GLOBAL_ANIMATIONS					= CAniCollection<CAniVector, global_names>;
	using WEAPON_ANIMATIONS					= CAniCollection<WEAPON_ACTIONS, weapon_names>;
	using MOVEMENT_ANIMATIONS				= CAniCollection<MOVEMENT_ACTIONS, movement_names>;
	using IN_PLACE_ANIMATIONS				= CAniFVector<in_place_names>;

	GLOBAL_ANIMATIONS						m_global;
	WEAPON_ANIMATIONS						m_torso;
	MOVEMENT_ANIMATIONS						m_movement;
	IN_PLACE_ANIMATIONS*					m_in_place;

				CStalkerAnimationState		( );
				CStalkerAnimationState		(const CStalkerAnimationState& animations);
	virtual		~CStalkerAnimationState		( );
	void		Load						(CKinematicsAnimated* kinematics, const char* base_name);
};
