#pragma once

#include "..\XR_3DA\CameraBase.h"

class CCameraFirstEye : public CCameraBase
{
	typedef CCameraBase inherited;
public:
					CCameraFirstEye	( CObject* p, u32 flags=0);
	virtual			~CCameraFirstEye( );

	virtual void	Load			(pcstr section);
	virtual void	Move			( int cmd, f32 val=0.0f, f32 factor=1.0f );

	virtual	void	OnActivate		( CCameraBase* old_cam );
	virtual void	Update			( Fvector& point, Fvector& noise_angle );

	virtual f32	GetWorldYaw		( )	{ return -yaw;	};
	virtual f32	GetWorldPitch	( )	{ return pitch; };
};
