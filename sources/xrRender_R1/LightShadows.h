// LightShadows.h: interface for the CLightShadows class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTSHADOWS_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_)
#define AFX_LIGHTSHADOWS_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_
#pragma once

#include "..\xrRender\light.h"
#include "..\xrRender\r__dsgraph_types.h"

class	CLightShadows
{
private:
	//
	typedef	R_dsgraph::_MatrixItem	NODE;
	struct	caster
	{
		IRenderable* O;
		fVector3				C;
		f32				D;
		xr_vector<NODE>		nodes;
	};
	struct	shadow
	{

#ifdef	DEBUG
		f32				dbg_HAT;
#endif

		IRenderable* O;
		int					slot;
		fVector3				C;
		fMatrix4x4				M;
		light* L;
		f32				E;
	};
	struct	tess_tri
	{
		fVector3				v[3];
		fVector3				N;
	};

public:
	struct	cache_item
	{
		IRenderable* O;
		fVector3				Op;
		light* L;
		fVector3				Lp;
		u32					time;
		tess_tri* tris;
		u32					tcnt;

		cache_item( )
		{
			O = 0;
			L = 0;
			tris = 0;
		}
	};

private:
	IRenderable* current;
	xr_vector<caster*>		casters_pool;
	xr_vector<caster*>		casters;
	xr_vector<shadow>		shadows;
	xr_vector<tess_tri>		tess;
	xr_vector<cache_item>	cache;
	xrXRC					xrc;

	ref_rt					RT;
	ref_rt					RT_temp;
	ref_shader				sh_BlurTR;
	ref_shader				sh_BlurRT;
	ref_geom				geom_Blur;
	ref_shader				sh_World;
	ref_geom				geom_World;
	ref_shader				sh_Screen;
	ref_geom				geom_Screen;

public:
	void					set_object(IRenderable* O);
	void					add_element(NODE& N);
	void					calculate( );
	void					render( );

	CLightShadows( );
	~CLightShadows( );
};

#endif // !defined(AFX_LIGHTSHADOWS_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_)
