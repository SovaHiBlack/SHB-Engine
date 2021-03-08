//	Module 		: stalker_animation_script_inline.h
//	Description : Stalker animation script inline functions

#pragma once

inline	CStalkerAnimationScript::CStalkerAnimationScript				(
		const MotionID &animation,
		bool hand_usage,
		bool use_movement_controller
	) :
	m_animation					(animation),
	m_hand_usage				(hand_usage),
	m_use_movement_controller	(use_movement_controller)
{
}

inline	const MotionID &CStalkerAnimationScript::animation				() const
{
	return						(m_animation);
}

inline	const bool &CStalkerAnimationScript::hand_usage					() const
{
	return						(m_hand_usage);
}

inline	const bool &CStalkerAnimationScript::use_movement_controller	() const
{
	return						(m_use_movement_controller);
}
