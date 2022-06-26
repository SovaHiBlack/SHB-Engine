// EffectorZoomInertion.h: инерция(покачивания) оружия в режиме 
//						   приближения	
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CameraEffector.h"
#include "..\XR_3DA\cameramanager.h"
#include "WeaponMagazined.h"

class CEffectorZoomInertion : public CEffectorCam
{
	//коэффициент скорости "покачивания" прицела
	f32	m_fFloatSpeed;
	f32	m_fDispRadius;

	f32	m_fEpsilon;
	Fvector	m_vCurrentPoint;
	Fvector m_vLastPoint;
	Fvector	m_vTargetPoint;
	Fvector	m_vTargetVel;

	Fvector m_vOldCameraDir;

	u32		m_dwTimePassed;

	//параметры настройки эффектора
	f32	m_fCameraMoveEpsilon;
	f32	m_fDispMin;
	f32	m_fSpeedMin;
	f32	m_fZoomAimingDispK;
	f32	m_fZoomAimingSpeedK;
	//время через которое эффектор меняет направление движения
	u32		m_dwDeltaTime;

	CRandom		m_Random;

	void			CalcNextPoint		();
	void			LoadParams			(pcstr Section, pcstr Prefix);
public:
	CEffectorZoomInertion				();
	virtual ~CEffectorZoomInertion		();

			void	Load				();
			void	SetParams			(f32 disp);

	virtual	BOOL	Process				(Fvector &delta_p, Fvector &delta_d,
											Fvector &delta_n, f32& fFov,
										 f32& fFar, f32& fAspect);
	virtual	void	SetRndSeed				(s32 Seed) { m_Random.seed(Seed); };
	virtual	void	Init				(CWeaponMagazined*	pWeapon);

	virtual CEffectorZoomInertion	*cast_effector_zoom_inertion	()	{return this;}
};