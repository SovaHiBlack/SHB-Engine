#pragma once
#include "../../../CameraEffector.h"

class CPseudogigantStepEffector : public CEffectorCam {
	typedef CEffectorCam inherited;	

	F32 total;
	F32 max_amp;
	F32 period_number;
	F32 power;

public:
					CPseudogigantStepEffector	(F32 time, F32 amp, F32 periods, F32 power);
	virtual	BOOL	Process						(Fvector &p, Fvector &d, Fvector &n, F32& fFov, F32& fFar, F32& fAspect);
};
