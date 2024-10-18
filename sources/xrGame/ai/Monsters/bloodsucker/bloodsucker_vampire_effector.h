#pragma once

#include "../../../..\XR_3DA\effectorPP.h"
#include "../../../CameraEffector.h"
#include "../../../..\XR_3DA\cameramanager.h"

//////////////////////////////////////////////////////////////////////////
// Vampire Postprocess Effector
//////////////////////////////////////////////////////////////////////////
class CVampirePPEffector : public CEffectorPP {
	typedef CEffectorPP inherited;	

	SPPInfo state;			//current state
	f32	m_total;		// total PP time

public:
					CVampirePPEffector		(const SPPInfo &ppi, f32 life_time);
	virtual	BOOL	Process					(SPPInfo& pp);
};

//////////////////////////////////////////////////////////////////////////
// Vampire Camera Effector
//////////////////////////////////////////////////////////////////////////
class CVampireCameraEffector : public CEffectorCam {
	typedef CEffectorCam inherited;	

	f32	m_time_total;
	Fvector	dangle_target;
	Fvector dangle_current;

	f32	m_dist;
	Fvector m_direction;

public:
					CVampireCameraEffector	(f32 time, const Fvector &src, const Fvector &tgt);
	virtual	BOOL	Process					(Fvector &p, Fvector &d, Fvector &n, f32& fFov, f32& fFar, f32& fAspect);
};
