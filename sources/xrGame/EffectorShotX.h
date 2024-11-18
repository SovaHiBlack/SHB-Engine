#pragma once

#include "EffectorShot.h"

class CCameraShotEffectorX : public CCameraShotEffector
{
	typedef			CCameraShotEffector	inherited;

public:
	CCameraShotEffectorX(f32 max_angle, f32 relax_time, f32 max_angle_horz, f32 step_angle_horz, f32 angle_frac = 0.7f);
	virtual			~CCameraShotEffectorX( );

	virtual BOOL	Process(fVector3& delta_p, fVector3& delta_d, fVector3& delta_n, f32& fFov, f32& fFar, f32& fAspect);
	virtual void	GetDeltaAngle(fVector3& delta_angle);
	virtual void	Shot(f32 angle);
	virtual void	Clear( );

protected:
	virtual void	UpdateActorCamera(f32 dPitch, f32 dYaw);
};
