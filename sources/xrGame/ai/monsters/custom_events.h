#pragma once

#include "monster_event_manager_defs.h"

struct CEventTAPrepareAnimation : public IEventData {
	u32		m_current_state;
	
	inline		CEventTAPrepareAnimation(u32 state) : m_current_state(state) {}
};

struct CEventVelocityBounce : public IEventData {
	float	m_ratio;

	inline		CEventVelocityBounce(float ratio) : m_ratio(ratio) {}
};
