#pragma once
#include "../../../CameraEffector.h"

class CPseudogigantStepEffector : public CEffectorCam {
	typedef CEffectorCam inherited;	

	f32 total;
	f32 max_amp;
	f32 period_number;
	f32 power;

public:
					CPseudogigantStepEffector	(f32 time, f32 amp, f32 periods, f32 power);
	virtual	BOOL	Process						(Fvector &p, Fvector &d, Fvector &n, f32& fFov, f32& fFar, f32& fAspect);
};
