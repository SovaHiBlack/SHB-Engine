#pragma once

#include "render.h"

class ENGINE_API IRenderable
{
public:
	struct
	{
		Fmatrix												xform;
		IRender_Visual*										visual;
		IRender_ObjectSpecific*								pROS;
		BOOL												pROS_Allowed;
	}														renderable;

								IRenderable					( );
	virtual						~IRenderable				( );
	IRender_ObjectSpecific*		renderable_ROS				( );
	virtual void				renderable_Render			( ) = 0;
	virtual bool				renderable_ShadowGenerate	( )
	{
		return false;
	}
	virtual bool				renderable_ShadowReceive	( )
	{
		return false;
	}
};
