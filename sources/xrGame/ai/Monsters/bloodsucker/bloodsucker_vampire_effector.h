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
	F32	m_total;		// total PP time

public:
					CVampirePPEffector		(const SPPInfo &ppi, F32 life_time);
	virtual	BOOL	Process					(SPPInfo& pp);
};

//////////////////////////////////////////////////////////////////////////
// Vampire Camera Effector
//////////////////////////////////////////////////////////////////////////
class CVampireCameraEffector : public CEffectorCam {
	typedef CEffectorCam inherited;	

	F32	m_time_total;
	Fvector	dangle_target;
	Fvector dangle_current;

	F32	m_dist;
	Fvector m_direction;

public:
					CVampireCameraEffector	(F32 time, const Fvector &src, const Fvector &tgt);
	virtual	BOOL	Process					(Fvector &p, Fvector &d, Fvector &n, F32& fFov, F32& fFar, F32& fAspect);
};



