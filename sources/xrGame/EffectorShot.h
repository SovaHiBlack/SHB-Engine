// EffectorShot.h: interface for the CCameraShotEffector class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CameraEffector.h"
#include "..\XR_3DA\cameramanager.h"
#include "Actor.h"

class CWeaponShotEffector{
protected:
	f32			fAngleVert;
	f32			fAngleVertMax;
	f32			fAngleVertFrac;
	f32			fAngleHorz;
	f32			fAngleHorzMax;
	f32			fAngleHorzStep;
	f32			fRelaxSpeed;

	f32			fLastDeltaVert;
	f32			fLastDeltaHorz;
protected:
	BOOL			bActive;
	BOOL			bSingleShoot;
	BOOL			bSSActive;
private:
	CRandom			m_Random;
	s32				m_LastSeed;
public:
					CWeaponShotEffector	();
	virtual			~CWeaponShotEffector(){};

	void			Initialize			(f32 max_angle, f32 relax_speed, f32 max_angle_horz, f32 step_angle_horz, f32 angle_frac);
	IC BOOL			IsActive			(){return bActive;}
	virtual void	SetActive			(BOOL Active) {bActive = Active;};
	IC BOOL			IsSingleShot		(){return bSingleShoot;}
	virtual	void	SetSingleShoot		(BOOL Single) {bSingleShoot = Single;};
	void			Update				();
	
	void			SetRndSeed			(s32 Seed);

	virtual void	Shot				(f32 angle);
	virtual void	GetDeltaAngle		(fVector3& delta_angle);
	virtual void	GetLastDelta		(fVector3& delta_angle);
	virtual	void	Clear				();

	virtual void	ApplyLastAngles			(f32* pitch, f32* yaw);
	virtual void	ApplyDeltaAngles		(f32* pitch, f32* yaw);
};

class CCameraShotEffector : public CWeaponShotEffector, public CEffectorCam
{
protected:
	CActor*			m_pActor;
public:
					CCameraShotEffector	(f32 max_angle, f32 relax_speed, f32 max_angle_horz, f32 step_angle_horz, f32 angle_frac);
	virtual			~CCameraShotEffector();
	
	virtual	BOOL	Process(fVector3& delta_p, fVector3& delta_d, fVector3& delta_n, f32& fFov, f32& fFar, f32& fAspect);

	virtual void	SetActor			(CActor* pActor) {m_pActor = pActor;};
	
	virtual CCameraShotEffector*		cast_effector_shot				()	{return this;}
};