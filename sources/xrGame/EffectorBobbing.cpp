#include "stdafx.h"
#include "EffectorBobbing.h"

#include "Actor.h"
//#include "Actor_defs.h"

#define BOBBING_SECT "bobbing_effector"

#define CROUCH_FACTOR	0.75f
#define SPEED_REMINDER	5.0f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEffectorBobbing::CEffectorBobbing( ) : CEffectorCam(eCEBobbing, 10000.0f)
{
	fTime = 0.0f;
	fReminderFactor = 0.0f;
	is_limping = false;

	m_fAmplitudeRun = pSettings->r_float(BOBBING_SECT, "run_amplitude");
	m_fAmplitudeWalk = pSettings->r_float(BOBBING_SECT, "walk_amplitude");
	m_fAmplitudeLimp = pSettings->r_float(BOBBING_SECT, "limp_amplitude");

	m_fSpeedRun = pSettings->r_float(BOBBING_SECT, "run_speed");
	m_fSpeedWalk = pSettings->r_float(BOBBING_SECT, "walk_speed");
	m_fSpeedLimp = pSettings->r_float(BOBBING_SECT, "limp_speed");
}

CEffectorBobbing::~CEffectorBobbing( )
{ }

void CEffectorBobbing::SetState(u32 mstate, bool limping, bool ZoomMode)
{
	dwMState = mstate;
	is_limping = limping;
	m_bZoomMode = ZoomMode;
}

BOOL CEffectorBobbing::Process(fVector3& p, fVector3& d, fVector3& n, f32& /*fFov*/, f32& /*fFar*/, f32& /*fAspect*/)
{
	fTime += Device.fTimeDelta;
	if (dwMState & ACTOR_DEFS::mcAnyMove)
	{
		if (fReminderFactor < 1.0f)
		{
			fReminderFactor += SPEED_REMINDER * Device.fTimeDelta;
		}
		else
		{
			fReminderFactor = 1.0f;
		}
	}
	else
	{
		if (fReminderFactor > 0.0f)
		{
			fReminderFactor -= SPEED_REMINDER * Device.fTimeDelta;
		}
		else
		{
			fReminderFactor = 0.0f;
		}
	}

	if (!fsimilar(fReminderFactor, 0.0f))
	{
		fMatrix4x4 M;
		M.identity( );
		M.j.set(n);
		M.k.set(d);
		M.i.crossproduct(n, d);
		M.c.set(p);

		// apply footstep bobbing effect
		fVector3 dangle;
		f32 k = ((dwMState & ACTOR_DEFS::mcCrouch) ? CROUCH_FACTOR : 1.0f);

		f32 A;
		f32 ST;

		if (isActorAccelerated(dwMState, m_bZoomMode))
		{
			A = m_fAmplitudeRun * k;
			ST = m_fSpeedRun * fTime * k;
		}
		else if (is_limping)
		{
			A = m_fAmplitudeLimp * k;
			ST = m_fSpeedLimp * fTime * k;
		}
		else
		{
			A = m_fAmplitudeWalk * k;
			ST = m_fSpeedWalk * fTime * k;
		}

		f32 _sinA = _abs(_sin(ST) * A) * fReminderFactor;
		f32 _cosA = _cos(ST) * A * fReminderFactor;

		p.y += _sinA;
		dangle.x = _cosA;
		dangle.z = _cosA;
		dangle.y = _sinA;

		fMatrix4x4 R;
		R.setHPB(dangle.x, dangle.y, dangle.z);

		fMatrix4x4 mR;
		mR.mul(M, R);

		d.set(mR.k);
		n.set(mR.j);
	}

	return TRUE;
}
