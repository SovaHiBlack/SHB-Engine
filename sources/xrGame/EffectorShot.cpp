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
	fAngleHorz = 0.0f;
	fAngleVert = -EPSILON_7;
	bActive = FALSE;
	bSingleShoot = FALSE;
	bSSActive = FALSE;
	m_LastSeed = 0;
	fRelaxSpeed = EPSILON_3;
	fAngleVertMax = 0.0f;
	fAngleVertFrac = 1.0f;
	fAngleHorzMax = 0.0f;
	fAngleHorzStep = 0.0f;

	fLastDeltaVert = 0.0f;
	fLastDeltaHorz = 0.0f;
}

void CWeaponShotEffector::Initialize(f32 max_angle, f32 relax_speed, f32 max_angle_horz, f32 step_angle_horz, f32 angle_frac)
{
	fRelaxSpeed = _abs(relax_speed);
	VERIFY(!fis_zero(fRelaxSpeed));
	fAngleVertMax = _abs(max_angle);
	VERIFY(!fis_zero(fAngleVertMax));
	fAngleVertFrac = _abs(angle_frac);
	fAngleHorzMax = max_angle_horz;
	fAngleHorzStep = step_angle_horz;
}

void CWeaponShotEffector::Shot(f32 angle)
{
	f32 OldAngleVert = fAngleVert;
	f32 OldAngleHorz = fAngleHorz;

	fAngleVert += (angle * fAngleVertFrac + m_Random.randF(-1.0f, 1.0f) * angle * (1 - fAngleVertFrac));
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
	bSSActive = TRUE;
}

void CWeaponShotEffector::Update( )
{
	if (bActive)
	{
		f32 time_to_relax = _abs(fAngleVert) / fRelaxSpeed;
		//		VERIFY(_valid(time_to_relax));
		f32 relax_speed = (fis_zero(time_to_relax)) ? 0.0f : _abs(fAngleHorz) / time_to_relax;
		//		VERIFY(_valid(relax_speed));
		f32 time_to_relax_l = _abs(fLastDeltaVert) / fRelaxSpeed;
		//		VERIFY(_valid(time_to_relax_l));
		f32 relax_speed_l = (fis_zero(time_to_relax_l)) ? 0.0f : _abs(fLastDeltaHorz) / time_to_relax_l;
		//		VERIFY(_valid(relax_speed_l));
		//-------------------------------------------------------
		if (fAngleHorz >= 0.0f)
		{
			fAngleHorz -= relax_speed * Device.fTimeDelta;
		}
		else
		{
			fAngleHorz += relax_speed * Device.fTimeDelta;
		}

		if (bSSActive)
		{
			if (fLastDeltaHorz >= 0.0f)
			{
				fLastDeltaHorz -= relax_speed_l * Device.fTimeDelta;
			}
			else
			{
				fLastDeltaHorz += relax_speed_l * Device.fTimeDelta;
			}
		}
		//		VERIFY(_valid(fLastDeltaHorz));
		//-------------------------------------------------------
		if (fAngleVert >= 0.0f)
		{
			fAngleVert -= fRelaxSpeed * Device.fTimeDelta;
			if (fAngleVert < 0.0f)
			{
				bActive = FALSE;
			}
		}
		else
		{
			fAngleVert += fRelaxSpeed * Device.fTimeDelta;
			if (fAngleVert > 0.0f)
			{
				bActive = FALSE;
			}
		}

		if (bSSActive)
		{
			if (fLastDeltaVert >= 0.0f)
			{
				fLastDeltaVert -= fRelaxSpeed * Device.fTimeDelta;
				if (fLastDeltaVert < 0.0f)
				{
					bSSActive = FALSE;
				}
			}
			else
			{
				fLastDeltaVert += fRelaxSpeed * Device.fTimeDelta;
				if (fLastDeltaVert > 0.0f)
				{
					bSSActive = FALSE;
				}
			}
		}

		//-------------------------------------------------------
		if (!bActive)
		{
			fAngleVert = 0.0f;
			fAngleHorz = 0.0f;
			m_LastSeed = 0;
			bSSActive = FALSE;
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

void CWeaponShotEffector::GetDeltaAngle(fVector3& delta_angle)
{
	delta_angle.x = -fAngleVert;
	delta_angle.y = -fAngleHorz;
	delta_angle.z = 0.0f;
}

void CWeaponShotEffector::GetLastDelta(fVector3& delta_angle)
{
	delta_angle.x = -fLastDeltaVert;
	delta_angle.y = -fLastDeltaHorz;
	delta_angle.z = 0.0f;
}

void CWeaponShotEffector::SetRndSeed(s32 Seed)
{
	if (m_LastSeed == 0)
	{
		m_LastSeed = Seed;
		m_Random.seed(Seed);
	}
}

void CWeaponShotEffector::ApplyLastAngles(f32* pitch, f32* yaw)
{
	*pitch -= fLastDeltaVert;
	*yaw -= fLastDeltaHorz;
}
void CWeaponShotEffector::ApplyDeltaAngles(f32* pitch, f32* yaw)
{
	*pitch -= fAngleVert;
	*yaw -= fAngleHorz;
}
//-----------------------------------------------------------------------------
// Camera shot effector
//-----------------------------------------------------------------------------
CCameraShotEffector::CCameraShotEffector(f32 max_angle, f32 relax_speed, f32 max_angle_horz, f32 step_angle_horz, f32 angle_frac) : CCameraEffector(eCEShot, 100000.0f)
{
	CWeaponShotEffector::Initialize(max_angle, relax_speed, max_angle_horz, step_angle_horz, angle_frac);
	m_pActor = NULL;
}

CCameraShotEffector::~CCameraShotEffector( )
{ }

BOOL CCameraShotEffector::Process(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect)
{
	if (bActive)
	{
		f32 h;
		f32 p;
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
