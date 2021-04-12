// EffectorZoomInertion.h: инерция(покачивания) оружия в режиме приближения

#pragma once

#include "CameraEffector.h"
#include "..\ENGINE\CameraManager.h"
#include "WeaponMagazined.h"

class CEffectorZoomInertion : public CEffectorCam
{
	//коэффициент скорости "покачивания" прицела
	float														m_fFloatSpeed;
	float														m_fDispRadius;

	float														m_fEpsilon;
	Fvector3													m_vCurrentPoint;
	Fvector3													m_vLastPoint;
	Fvector3													m_vTargetPoint;
	Fvector3													m_vTargetVel;

	Fvector3													m_vOldCameraDir;

	u32															m_dwTimePassed;

	//параметры настройки эффектора
	float														m_fCameraMoveEpsilon;
	float														m_fDispMin;
	float														m_fSpeedMin;
	float														m_fZoomAimingDispK;
	float														m_fZoomAimingSpeedK;
	//время через которое эффектор меняет направление движения
	u32															m_dwDeltaTime;

	CRandom														m_Random;

	void							CalcNextPoint				( );
	void							LoadParams					(const char* Section, const char* Prefix);

public:
									CEffectorZoomInertion		( );
	virtual							~CEffectorZoomInertion		( );

	void							Load						( );
	void							SetParams					(float disp);

	virtual BOOL					Process						(Fvector3& delta_p, Fvector3& delta_d, Fvector3& delta_n, float& fFov, float& fFar, float& fAspect);
	virtual void					SetRndSeed					(int Seed)
	{
		m_Random.seed(Seed);
	}
	virtual void					Init						(CWeaponMagazined* pWeapon);

	virtual CEffectorZoomInertion*	cast_effector_zoom_inertion	( )
	{
		return this;
	}
};
