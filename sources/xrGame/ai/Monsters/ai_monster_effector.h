#pragma once

#include "..\..\..\XR_3DA\effectorPP.h"
#include "../../CameraEffector.h"
#include "..\..\..\XR_3DA\cameramanager.h"

//////////////////////////////////////////////////////////////////////////
// CMonsterEffector
//////////////////////////////////////////////////////////////////////////
class CMonsterEffector : public CEffectorPP {
	typedef CEffectorPP inherited;	
	
	SPPInfo state;			// current state
	F32	m_total;		// total PP time
	F32	m_attack;		// attack time in percents	[0..1]
	F32	m_release;		// release time in percents	[0..1]
	
	F32	m_spec_factor;

public:
					CMonsterEffector		(const SPPInfo &ppi, F32 life_time, F32 attack_time = 0.0f, F32 release_time = 0.0f, F32 spec_factor = 1.f);
	virtual	BOOL	Process					(SPPInfo& pp);
};

//////////////////////////////////////////////////////////////////////////
// CMonsterEffectorHit
//////////////////////////////////////////////////////////////////////////
class CMonsterEffectorHit : public CEffectorCam {

	F32 total;
	F32 max_amp;
	F32 period_number;
	F32 power;

	Fvector offset;
public:
					CMonsterEffectorHit		(F32 time, F32 amp, F32 periods, F32 power);
	virtual	BOOL	Process					(Fvector &p, Fvector &d, Fvector &n, F32& fFov, F32& fFar, F32& fAspect);
};


