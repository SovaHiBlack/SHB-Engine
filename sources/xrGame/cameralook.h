#pragma once

#include "..\ENGINE\CameraBase.h"

class CCameraLook : public CCameraBase
{
	typedef CCameraBase inherited;

	Fvector2		lim_zoom;
	float			dist;
	float			prev_d;

public:
					CCameraLook		(CObject* p, u32 flags = 0);
	virtual			~CCameraLook	( );
	virtual void	Load			(const char* section);
	virtual void	Move			(int cmd, float val = 0, float factor = 1.0f);

	virtual void	OnActivate		(CCameraBase* old_cam);
	virtual void	Update			(Fvector3& point, Fvector3& noise_dangle);

	virtual float	GetWorldYaw		( )
	{
		return -yaw;
	};
	virtual float	GetWorldPitch	( )
	{
		return pitch;
	};
};

class CCameraLook2 : public CCameraLook
{
public:
	static Fvector3	m_cam_offset;

protected:
	CObject*		m_locked_enemy;
	Fvector2		m_autoaim_inertion_yaw;
	Fvector2		m_autoaim_inertion_pitch;
	void			UpdateAutoAim	( );

public:
					CCameraLook2	(CObject* p, u32 flags = 0) : CCameraLook(p, flags)
	{
		m_locked_enemy = NULL;
	};
	virtual			~CCameraLook2	( )
	{ }
	virtual	void	OnActivate		(CCameraBase* old_cam);
	virtual void	Update			(Fvector3& point, Fvector3& noise_dangle);
	virtual void	Load			(const char* section);
};
