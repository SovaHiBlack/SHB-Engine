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
	fVector3	dangle_target;
	fVector3 dangle_current;

	f32	m_dist;
	fVector3 m_direction;

public:
					CVampireCameraEffector	(f32 time, const fVector3& src, const fVector3& tgt);
	virtual	BOOL	Process					(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect);
};
