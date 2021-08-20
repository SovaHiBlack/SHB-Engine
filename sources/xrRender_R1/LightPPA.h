// LightPPA.h: interface for the CLightPPA class.

#pragma once

#include "..\xrRender\light.h"

struct	CLightR_Vertex
{
	Fvector3			P;
	Fvector3			N;
	float			u0,v0;
	float			u1,v1;
};

class	CLightR_Manager
{
	xrXRC							xrc;
	xr_vector<light*>				selected_point;
	xr_vector<light*>				selected_spot;
public:
	CLightR_Manager					();
	virtual ~CLightR_Manager		();

	void			add				(light* L);
	void			render			();
	void			render_point	();
	void			render_spot		();
};

//////////////////////////////////////////////////////////////////////////
// binders for lighting
//////////////////////////////////////////////////////////////////////////
class cl_light_PR		: public R_constant_setup		{	virtual void setup	(R_constant* C);	};
class cl_light_C		: public R_constant_setup		{	virtual void setup	(R_constant* C);	};
class cl_light_XFORM	: public R_constant_setup		{	virtual void setup	(R_constant* C);	};
