// EffectorShot.cpp: implementation of the CCameraShotEffector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectorShot.h"

//-----------------------------------------------------------------------------
// Weapon shot effector
//-----------------------------------------------------------------------------
CWeaponShotEffector::CWeaponShotEffector( )
{
	fAngleHorz = 0.f;
	fAngleVert = -EPS_S;
	bActive = FALSE;
	bSingleShoot = FALSE;
	bSSActive = false;
	m_LastSeed = 0;
	fRelaxSpeed = EPS_L;
	fAngleVertMax = 0.f;
	fAngleVertFrac = 1.f;
	fAngleHorzMax = 0.f;
	fAngleHorzStep = 0.f;

	fLastDeltaVert = 0.f;
	fLastDeltaHorz = 0.f;
}

void CWeaponShotEffector::Initialize(float max_angle, float relax_speed, float max_angle_horz, float step_angle_horz, float angle_frac)
{
	fRelaxSpeed = _abs(relax_speed);
	VERIFY(!fis_zero(fRelaxSpeed));
	fAngleVertMax = _abs(max_angle);
	VERIFY(!fis_zero(fAngleVertMax));
	fAngleVertFrac = _abs(angle_frac);
	fAngleHorzMax = max_angle_horz;
	fAngleHorzStep = step_angle_horz;
}

void CWeaponShotEffector::Shot(float angle)
{
	float OldAngleVert = fAngleVert;
	float OldAngleHorz = fAngleHorz;

	fAngleVert += (angle * fAngleVertFrac + m_Random.randF(-1.0f, 1.0f) * angle * (1.0f - fAngleVertFrac));
//	VERIFY(!fis_zero(fAngleVertMax));
	clamp(fAngleVert, -fAngleVertMax, fAngleVertMax);
	if (fis_zero(fAngleVert - fAngleVertMax))
	{
		fAngleVert *= m_Random.randF(0.9f, 1.1f);
	}

	fAngleHorz = fAngleHorz + (fAngleVert / fAngleVertMax) * m_Random.randF(-1.0f, 1.0f) * fAngleHorzStep;
//	VERIFY(_valid(fAngleHorz));

	clamp(fAngleHorz, -fAngleHorzMax, fAngleHorzMax);
//	VERIFY(_valid(fAngleHorz));
	bActive = TRUE;

	fLastDeltaVert = fAngleVert - OldAngleVert;
	fLastDeltaHorz = fAngleHorz - OldAngleHorz;
//	VERIFY(_valid(fLastDeltaHorz));
	bSSActive = true;
}

void CWeaponShotEffector::Update( )
{
	if (bActive)
	{
		float time_to_relax = _abs(fAngleVert) / fRelaxSpeed;
//		VERIFY(_valid(time_to_relax));
		float relax_speed = (fis_zero(time_to_relax)) ? 0.0f : _abs(fAngleHorz) / time_to_relax;
//		VERIFY(_valid(relax_speed));

		float time_to_relax_l = _abs(fLastDeltaVert) / fRelaxSpeed;
//		VERIFY(_valid(time_to_relax_l));

		float relax_speed_l = (fis_zero(time_to_relax_l)) ? 0.0f : _abs(fLastDeltaHorz) / time_to_relax_l;
//		VERIFY(_valid(relax_speed_l));
		//-------------------------------------------------------
		if (fAngleHorz >= 0.f)
			fAngleHorz -= relax_speed * Device.fTimeDelta;
		else
			fAngleHorz += relax_speed * Device.fTimeDelta;

		if (bSSActive)
		{
			if (fLastDeltaHorz >= 0.f)
				fLastDeltaHorz -= relax_speed_l * Device.fTimeDelta;
			else
				fLastDeltaHorz += relax_speed_l * Device.fTimeDelta;
		}
//		VERIFY(_valid(fLastDeltaHorz));
		//-------------------------------------------------------
		if (fAngleVert >= 0.f)
		{
			fAngleVert -= fRelaxSpeed * Device.fTimeDelta;
			if (fAngleVert < 0.f) bActive = FALSE;
		}
		else
		{
			fAngleVert += fRelaxSpeed * Device.fTimeDelta;
			if (fAngleVert > 0.f)	bActive = FALSE;
		}

		if (bSSActive)
		{
			if (fLastDeltaVert >= 0.f)
			{
				fLastDeltaVert -= fRelaxSpeed * Device.fTimeDelta;
				if (fLastDeltaVert < 0.f) bSSActive = false;
			}
			else
			{
				fLastDeltaVert += fRelaxSpeed * Device.fTimeDelta;
				if (fLastDeltaVert > 0.f)	bSSActive = false;
			}
		};

		//-------------------------------------------------------
		if (!bActive)
		{
			fAngleVert = 0.0f;
			fAngleHorz = 0.0f;
			m_LastSeed = 0;
			bSSActive = false;
		}
		//-------------------------------------------------------
		if (!bSSActive)
		{
			fLastDeltaVert = 0.0f;
			fLastDeltaHorz = 0.0f;
		}

//		VERIFY(_valid(fAngleVert));
//		VERIFY(_valid(fAngleHorz));
//		VERIFY(_valid(fLastDeltaHorz));
//		VERIFY(_valid(fLastDeltaVert));
	}
}

void CWeaponShotEffector::Clear( )
{
	bActive = false;
	fAngleVert = 0.0f;
	fAngleHorz = 0.0f;
	m_LastSeed = 0;
}

void CWeaponShotEffector::GetDeltaAngle(Fvector3& delta_angle)
{
	delta_angle.x = -fAngleVert;
	delta_angle.y = -fAngleHorz;
	delta_angle.z = 0.0f;
}

void CWeaponShotEffector::GetLastDelta(Fvector3& delta_angle)
{
	delta_angle.x = -fLastDeltaVert;
	delta_angle.y = -fLastDeltaHorz;
	delta_angle.z = 0.0f;
}

void CWeaponShotEffector::SetRndSeed(int Seed)
{
	if (m_LastSeed == 0)
	{
		m_LastSeed = Seed;
		m_Random.seed(Seed);
	}
}

void CWeaponShotEffector::ApplyLastAngles(float* pitch, float* yaw)
{
	*pitch -= fLastDeltaVert;
	*yaw -= fLastDeltaHorz;
}
void CWeaponShotEffector::ApplyDeltaAngles(float* pitch, float* yaw)
{
	*pitch -= fAngleVert;
	*yaw -= fAngleHorz;
}

//-----------------------------------------------------------------------------
// Camera shot effector
//-----------------------------------------------------------------------------
CCameraShotEffector::CCameraShotEffector(float max_angle, float relax_speed, float max_angle_horz, float step_angle_horz, float angle_frac) : CEffectorCam(eCEShot, 100000.f)
{
	CWeaponShotEffector::Initialize(max_angle, relax_speed, max_angle_horz, step_angle_horz, angle_frac);
	m_pActor = nullptr;
}

CCameraShotEffector::~CCameraShotEffector( )
{ }

BOOL CCameraShotEffector::Process(Fvector3& p, Fvector3& d, Fvector3& n, float& fFov, float& fFar, float& fAspect)
{
	if (bActive)
	{
		float h;
		float p;
		d.getHP(h, p);
		if (bSingleShoot)
		{
			if (bSSActive)
			{
				d.setHP(h + fLastDeltaHorz, p + fLastDeltaVert);
			}
		}
		else
		{
			d.setHP(h + fAngleHorz, p + fAngleVert);
		}

		Update( );
	}

	return TRUE;
}
