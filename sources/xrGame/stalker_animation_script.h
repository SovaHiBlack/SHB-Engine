//	Module 		: stalker_animation_script.h
//	Description : Stalker animation script

#pragma once

class CStalkerAnimationScript {
private:
	MotionID				m_animation;
	bool					m_hand_usage;
	bool					m_use_movement_controller;

public:
	inline						CStalkerAnimationScript	(const MotionID &animation, bool hand_usage, bool use_movement_controller);
	inline	const MotionID		&animation				() const;
	inline	const bool			&hand_usage				() const;
	inline	const bool			&use_movement_controller() const;
};

#include "stalker_animation_script_inline.h"
