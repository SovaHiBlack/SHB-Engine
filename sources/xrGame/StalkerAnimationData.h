//	Module 		: StalkerAnimationData.h
//	Description : Stalker animation data

#pragma once

#include "StalkerAnimationState.h"
//#include "stalker_animation_names.h"

class CStalkerAnimationData
{
public:
	using WEAPON_ACTIONS		= CStalkerAnimationState::WEAPON_ACTIONS;
	using GLOBAL_ANIMATIONS		= CAniCollection<WEAPON_ACTIONS, weapon_names>;
	using PART_ANIMATIONS		= CAniCollection<CStalkerAnimationState, state_names>;
	using HEAD_ANIMATIONS		= CAniFVector<head_names>;

	PART_ANIMATIONS				m_part_animations;
	HEAD_ANIMATIONS				m_head_animations;
	GLOBAL_ANIMATIONS			m_global_animations;

	CStalkerAnimationData		(CKinematicsAnimated* skeleton_animated);
};
