#pragma once

#include "..\..\..\ENGINE\effectorPP.h"
#include "..\..\CameraEffector.h"
#include "..\..\..\ENGINE\CameraManager.h"

class CMonsterEffector : public CEffectorPP
{
	using inherited						= CEffectorPP;

	SPPInfo								state;			// current state
	float								m_total;		// total PP time
	float								m_attack;		// attack time in percents	[0..1]
	float								m_release;		// release time in percents	[0..1]

	float								m_spec_factor;

public:
					CMonsterEffector	(const SPPInfo& ppi, float life_time, float attack_time = 0.0f, float release_time = 0.0f, float spec_factor = 1.0f);
	virtual BOOL	Process				(SPPInfo& pp);
};

class CMonsterEffectorHit : public CEffectorCam
{
	float								total;
	float								max_amp;
	float								period_number;
	float								power;

	Fvector3							offset;

public:
					CMonsterEffectorHit	(float time, float amp, float periods, float power);
	virtual BOOL	Process				(Fvector3& p, Fvector3& d, Fvector3& n, float& fFov, float& fFar, float& fAspect);
};
