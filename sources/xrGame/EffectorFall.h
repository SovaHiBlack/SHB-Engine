#pragma once

#include "..\ENGINE\Effector.h"

// приседание после падения
class CEffectorFall : public CEffectorCam
{
	float	fPower;
	float	fPhase;

public:
	virtual	BOOL	Process(Fvector3& p, Fvector3& d, Fvector3& n, float& fFov, float& fFar, float& fAspect);

	CEffectorFall(float power, float life_time=1);
	virtual ~CEffectorFall();
};
