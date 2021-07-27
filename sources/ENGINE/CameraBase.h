// CameraBase.h: interface for the CCameraBase class.

#pragma once

#include "Camera_defs.h"

class CObject;

class ENGINE_API CCameraBase
{
protected:
	CObject*						parent;

public:
	BOOL							bClampYaw;
	BOOL							bClampPitch;
	BOOL							bClampRoll;
	F32							yaw;
	F32							pitch;
	F32							roll;

	enum
	{
		flRelativeLink				= (1 << 0),
		flPositionRigid				= (1 << 1),
		flDirectionRigid			= (1 << 2)
	};
	Flags32							m_Flags;

	ECameraStyle					style;
	Fvector2						lim_yaw;
	Fvector2						lim_pitch;
	Fvector2						lim_roll;
	Fvector3						rot_speed;

	Fvector3						vPosition;
	Fvector3						vDirection;
	Fvector3						vNormal;
	F32							f_fov;
	F32							f_aspect;

	int								tag;

					CCameraBase		(CObject* p, u32 flags);
	virtual			~CCameraBase	( );
	virtual void	Load			(const char* section);
	void			SetParent		(CObject* p)
	{
		parent = p;
		VERIFY(p);
	}
	virtual void	OnActivate		(CCameraBase* old_cam)
	{ }
	virtual void	OnDeactivate	( )
	{ }
	virtual void	Move			(int cmd, F32 val = 0.0f, F32 factor = 1.0f)
	{ }
	virtual void	Update			(Fvector3& point, Fvector3& noise_angle)
	{ }
	virtual void	Get				(Fvector3& P, Fvector3& D, Fvector3& N)
	{
		P.set(vPosition);
		D.set(vDirection);
		N.set(vNormal);
	}
	virtual void	Set				(const Fvector3& P, const Fvector3& D, const Fvector3& N)
	{
		vPosition.set(P);
		vDirection.set(D);
		vNormal.set(N);
	}
	virtual void	Set				(F32 Y, F32 P, F32 R)
	{
		yaw = Y;
		pitch = P;
		roll = R;
	}

	virtual F32	getWorldYaw		( )
	{
		return 0.0f;
	}
	virtual F32	getWorldPitch	( )
	{
		return 0.0f;
	}
};
