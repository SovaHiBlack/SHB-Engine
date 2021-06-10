#pragma once

#include "..\ENGINE\CameraBase.h"

class CCameraFirstEye : public CCameraBase
{
	using inherited						= CCameraBase;

public:
					CCameraFirstEye		(CObject* p, u32 flags = 0);
	virtual			~CCameraFirstEye	( );

	virtual void	Load				(const char* section);
	virtual void	Move				(int cmd, float val = 0.0f, float factor = 1.0f);

	virtual void	OnActivate			(CCameraBase* old_cam);
	virtual void	Update				(Fvector3& point, Fvector3& noise_angle);

	virtual float	getWorldYaw			( )
	{
		return -yaw;
	}
	virtual float	getWorldPitch		( )
	{
		return pitch;
	}
};
