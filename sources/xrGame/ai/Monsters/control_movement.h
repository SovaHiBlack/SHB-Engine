#pragma once

#include "control_combase.h"

struct SControlMovementData : public ControlCom::IComData {
	f32	velocity_target;
	f32	acc;
};

class CControlMovement : public CControl_ComPure<SControlMovementData> {
	typedef CControl_ComPure<SControlMovementData> inherited;

	f32		m_velocity_current;
	
public:
	virtual void	reinit			();
	virtual void	update_frame	();

	f32	velocity_current	() {return m_velocity_current;}
	f32	velocity_target		() {return m_data.velocity_target;}

			// return object's real velocity
	f32	real_velocity	();
};
