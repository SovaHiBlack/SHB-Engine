// EffectorShot.h: interface for the CCameraShotEffector class.

#pragma once

#include "CameraEffector.h"
#include "..\ENGINE\cameramanager.h"
#include "Actor.h"

class CWeaponShotEffector
{
protected:
	float													fAngleVert;
	float													fAngleVertMax;
	float													fAngleVertFrac;
	float													fAngleHorz;
	float													fAngleHorzMax;
	float													fAngleHorzStep;
	float													fRelaxSpeed;

	float													fLastDeltaVert;
	float													fLastDeltaHorz;

	BOOL													bActive;
	BOOL													bSingleShoot;
	bool													bSSActive;

private:
	CRandom													m_Random;
	int														m_LastSeed;

public:
									CWeaponShotEffector		( );
	virtual							~CWeaponShotEffector	( )
	{ }

	void							Initialize				(float max_angle, float relax_speed, float max_angle_horz, float step_angle_horz, float angle_frac);
	inline BOOL						IsActive				( )
	{
		return bActive;
	}
	virtual void					SetActive				(BOOL Active)
	{
		bActive = Active;
	}
	inline BOOL						IsSingleShot			( )
	{
		return bSingleShoot;
	}
	virtual void					SetSingleShoot			(BOOL Single)
	{
		bSingleShoot = Single;
	}
	void							Update					( );

	void							SetRndSeed				(int Seed);

	virtual void					Shot					(float angle);
	virtual void					GetDeltaAngle			(Fvector3& delta_angle);
	virtual void					GetLastDelta			(Fvector3& delta_angle);
	virtual void					Clear					( );

	virtual void					ApplyLastAngles			(float* pitch, float* yaw);
	virtual void					ApplyDeltaAngles		(float* pitch, float* yaw);
};

class CCameraShotEffector : public CWeaponShotEffector, public CEffectorCam
{
protected:
	CActor*													m_pActor;

public:
									CCameraShotEffector		(float max_angle, float relax_speed, float max_angle_horz, float step_angle_horz, float angle_frac);
	virtual							~CCameraShotEffector	( );

	virtual BOOL					Process					(Fvector3& delta_p, Fvector3& delta_d, Fvector3& delta_n, float& fFov, float& fFar, float& fAspect);

	virtual void					SetActor				(CActor* pActor)
	{
		m_pActor = pActor;
	}

	virtual CCameraShotEffector*	cast_effector_shot		( )
	{
		return this;
	}
};
