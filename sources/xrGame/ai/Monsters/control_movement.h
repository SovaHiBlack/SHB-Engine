#pragma once

#include "control_combase.h"

struct SControlMovementData : public ControlCom::IComData {
	F32	velocity_target;
	F32	acc;
};

class CControlMovement : public CControl_ComPure<SControlMovementData> {
	typedef CControl_ComPure<SControlMovementData> inherited;

	F32		m_velocity_current;
	
public:
	virtual void	reinit			();
	virtual void	update_frame	();

	F32	velocity_current	() {return m_velocity_current;}
	F32	velocity_target		() {return m_data.velocity_target;}

			// return object's real velocity
	F32	real_velocity	();
};