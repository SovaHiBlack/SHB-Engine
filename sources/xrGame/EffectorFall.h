#pragma once

#include "..\XR_3DA\Effector.h"

// ���������� ����� �������
class CEffectorFall : public CEffectorCam
{
	f32	fPower;
	f32	fPhase;
public:
	virtual	BOOL	Process(Fvector &p, Fvector &d, Fvector &n, f32& fFov, f32& fFar, f32& fAspect);

	CEffectorFall(f32 power, f32 life_time=1);
	virtual ~CEffectorFall();
};