#pragma once

#include "EffectorShot.h"

class CCameraShotEffectorX : public CCameraShotEffector
{	
	typedef			CCameraShotEffector	inherited;
public:
					CCameraShotEffectorX(float max_angle, float relax_time, float max_angle_horz, float step_angle_horz, float angle_frac = 0.7f);
	virtual			~CCameraShotEffectorX();

	virtual	BOOL	Process				(Fvector3& delta_p, Fvector3& delta_d, Fvector3& delta_n, float& fFov, float& fFar, float& fAspect);
	virtual	void	GetDeltaAngle		(Fvector3& delta_angle);
	virtual	void	Shot				(float angle);
	virtual	void	Clear				();
protected:
	virtual void	UpdateActorCamera	(float dPitch, float dYaw);
};