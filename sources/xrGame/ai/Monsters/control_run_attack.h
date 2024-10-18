#pragma once
#include "control_combase.h"

class CControlRunAttack : public CControl_ComCustom<> {
	f32			m_min_dist;
	f32			m_max_dist;

	u32				m_min_delay;
	u32				m_max_delay;

	u32				m_time_next_attack;

public:
	virtual void	load					(pcstr section);
	virtual void	reinit					();

	virtual void	on_event				(ControlCom::EEventType, ControlCom::IEventData*);
	virtual void	activate				();
	virtual void	on_release				();
	virtual bool	check_start_conditions	();
};

