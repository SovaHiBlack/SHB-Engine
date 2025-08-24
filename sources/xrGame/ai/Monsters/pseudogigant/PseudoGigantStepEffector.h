#pragma once
#include "../../../CameraEffects.h"

class CPseudoGigantStepEffector : public CCameraEffector
{
	typedef CCameraEffector inherited;

	f32 total;
	f32 max_amp;
	f32 period_number;
	f32 power;

public:
	CPseudoGigantStepEffector(f32 time, f32 amp, f32 periods, f32 power);
	virtual	BOOL	Process						(fVector3& p, fVector3& d, fVector3&n, f32& fFov, f32& fFar, f32& fAspect);
};
