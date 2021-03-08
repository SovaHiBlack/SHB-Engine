#pragma once

#include "../../../..\ENGINE\effectorPP.h"
#include "../../../CameraEffector.h"
#include "../../../..\ENGINE\cameramanager.h"

//////////////////////////////////////////////////////////////////////////
// Vampire Postprocess Effector
//////////////////////////////////////////////////////////////////////////
class CVampirePPEffector : public CEffectorPP
{
	using  inherited = CEffectorPP;

	SPPInfo state;			//current state
	float	m_total;		// total PP time

public:
					CVampirePPEffector		(const SPPInfo &ppi, float life_time);
	virtual	BOOL	Process					(SPPInfo& pp);
};

//////////////////////////////////////////////////////////////////////////
// Vampire Camera Effector
//////////////////////////////////////////////////////////////////////////
class CVampireCameraEffector : public CEffectorCam 
{
	using inherited = CEffectorCam;

	float	m_time_total;
	Fvector3	dangle_target;
	Fvector3 dangle_current;

	float	m_dist;
	Fvector3 m_direction;

public:
					CVampireCameraEffector	(float time, const Fvector3& src, const Fvector3& tgt);
	virtual	BOOL	Process					(Fvector3& p, Fvector3& d, Fvector3& n, float& fFov, float& fFar, float& fAspect);
};
