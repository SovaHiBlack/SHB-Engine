#pragma once

#include "..\..\..\XR_3DA\effectorPP.h"
#include "../../CameraEffector.h"
#include "..\..\..\XR_3DA\cameramanager.h"

//////////////////////////////////////////////////////////////////////////
// CMonsterEffector
//////////////////////////////////////////////////////////////////////////
class CMonsterEffector : public CEffectorPP
{
	typedef CEffectorPP inherited;

	SPPInfo state;			// current state
	f32 m_total;		// total PP time
	f32 m_attack;		// attack time in percents	[0..1]
	f32 m_release;		// release time in percents	[0..1]

	f32 m_spec_factor;

public:
	CMonsterEffector(const SPPInfo& ppi, f32 life_time, f32 attack_time = 0.0f, f32 release_time = 0.0f, f32 spec_factor = 1.0f);
	virtual BOOL	Process(SPPInfo& pp);
};

//////////////////////////////////////////////////////////////////////////
// CMonsterEffectorHit
//////////////////////////////////////////////////////////////////////////
class CMonsterEffectorHit : public CEffectorCam
{
	f32 total;
	f32 max_amp;
	f32 period_number;
	f32 power;

	fVector3 offset;

public:
	CMonsterEffectorHit(f32 time, f32 amp, f32 periods, f32 power);
	virtual BOOL	Process(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect);
};
