#pragma once

#include "CycleConstStorage.h"
#include "ode_include.h"

class CPHInterpolation {

public:
CPHInterpolation();
void SetBody(dBodyID body);
static const u16 PH_INTERPOLATION_POINTS=2;
void	InterpolatePosition	(fVector3& pos);
void	InterpolateRotation	(fMatrix4x4& rot);
void	UpdatePositions		();
void	UpdateRotations		();
void	ResetPositions		();
void	ResetRotations		();
void	GetRotation			(Fquaternion& q, u16 num);
void	GetPosition			(fVector3& p, u16 num);
void	SetRotation			(const Fquaternion& q, u16 num);
void	SetPosition			(const fVector3& p, u16 num);
private:
	dBodyID m_body;
	CCycleConstStorage<fVector3,PH_INTERPOLATION_POINTS>				qPositions;
	CCycleConstStorage<Fquaternion,PH_INTERPOLATION_POINTS>			qRotations;
};
