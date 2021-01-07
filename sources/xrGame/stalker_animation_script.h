//	Module 		: stalker_animation_script.h
//	Description : Stalker animation script

#pragma once

class CStalkerAnimationScript {
private:
	MotionID				m_animation;
	bool					m_hand_usage;
	bool					m_use_movement_controller;

public:
	IC						CStalkerAnimationScript	(const MotionID &animation, bool hand_usage, bool use_movement_controller);
	IC	const MotionID		&animation				() const;
	IC	const bool			&hand_usage				() const;
	IC	const bool			&use_movement_controller() const;
};

#include "stalker_animation_script_inline.h"
