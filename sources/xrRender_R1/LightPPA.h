// LightPPA.h: interface for the CLightPPA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_)
#define AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_
#pragma once

#include "..\xrRender\light.h"

struct	CLightR_Vertex
{
	fVector3			P;
	fVector3			N;
	f32				u0;
	f32				v0;
	f32				u1;
	f32				v1;
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

#endif // !defined(AFX_LIGHTPPA_H__E5B97AC9_84A6_4773_9FEF_3BC5D1CEF8B6__INCLUDED_)
