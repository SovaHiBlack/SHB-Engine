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
	float							yaw;
	float							pitch;
	float							roll;

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
	float							f_fov;
	float							f_aspect;

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
	virtual void	Move			(int cmd, float val = 0.0f, float factor = 1.0f)
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
	virtual void	Set				(float Y, float P, float R)
	{
		yaw = Y;
		pitch = P;
		roll = R;
	}

	virtual float	getWorldYaw		( )
	{
		return 0.0f;
	}
	virtual float	getWorldPitch	( )
	{
		return 0.0f;
	}
};
