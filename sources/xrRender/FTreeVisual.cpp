#include "stdafx.h"

#include "..\XR_3DA\igame_persistent.h"
#include "..\XR_3DA\igame_level.h"
#include "..\XR_3DA\environment.h"
#include "..\XR_3DA\mesh.h"

#include "ftreevisual.h"

shared_str					m_xform		;
shared_str					m_xform_v	;
shared_str					c_consts	;
shared_str					c_wave		;
shared_str					c_wind		;
shared_str					c_c_bias	;
shared_str					c_c_scale	;
shared_str					c_c_sun		;

FTreeVisual::FTreeVisual	( )
{
}

FTreeVisual::~FTreeVisual	( )
{
}

void FTreeVisual::Release	()
{
	IRenderVisual::Release	();
}

void FTreeVisual::Load		(pcstr N, IReader *data, u32 dwFlags)
{
	IRenderVisual::Load		(N,data,dwFlags);

	D3DVERTEXELEMENT9*	vFormat	= NULL;

	// read vertices
	R_ASSERT			(data->find_chunk(OGF_GCONTAINER));
	{
		// verts
		u32 ID				= data->r_u32				();
		vBase				= data->r_u32				();
		vCount				= data->r_u32				();
		vFormat				= RImplementation.getVB_Format	(ID);
		
		VERIFY				(NULL==p_rm_Vertices);

		p_rm_Vertices		= RImplementation.getVB			(ID);
		p_rm_Vertices->AddRef();

		// indices
		dwPrimitives		= 0;
		ID					= data->r_u32				();
		iBase				= data->r_u32				();
		iCount				= data->r_u32				();
		dwPrimitives		= iCount/3;
		
		VERIFY				(NULL==p_rm_Indices);
		p_rm_Indices			= RImplementation.getIB		(ID);
		p_rm_Indices->AddRef	();
	}

	// load tree-def
	R_ASSERT			(data->find_chunk(OGF_TREEDEF2));
	{
		data->r			(&xform,	sizeof(xform));
		data->r			(&c_scale,	sizeof(c_scale));	c_scale.rgb.mul	(.5f);	c_scale.hemi*=.5f;	c_scale.sun	*=.5f;
		data->r			(&c_bias,	sizeof(c_bias));	c_bias.rgb.mul	(.5f);	c_bias.hemi	*=.5f;	c_bias.sun	*=.5f;
		//Msg				("hemi[%f / %f], sun[%f / %f]",c_scale.hemi,c_bias.hemi,c_scale.sun,c_bias.sun);
	}

	// Geom
	rm_geom.create			(vFormat,p_rm_Vertices,p_rm_Indices);

	// Get constants
	m_xform				= "m_xform";
	m_xform_v			= "m_xform_v";
	c_consts			= "consts";
	c_wave				= "wave";
	c_wind				= "wind";
	c_c_bias			= "c_bias";
	c_c_scale			= "c_scale";
	c_c_sun				= "c_sun";
}

struct	FTreeVisual_setup
{
	u32			dwFrame;
	f32		scale;
	fVector4	wave;
	fVector4	wind;

	FTreeVisual_setup() 
	{
		dwFrame	= 0;
	}

	void		calculate	()
	{
		dwFrame					= Device.dwFrame;

		// Calc wind-vector3, scale
		f32	tm_rot			= PI_MUL_2*Device.fTimeGlobal/ps_r__Tree_w_rot;
		wind.set				(_sin(tm_rot),0,_cos(tm_rot),0);	wind.normalize	();	wind.mul(ps_r__Tree_w_amp);	// dir1*amplitude
		scale					= 1.0f/ f32(FTreeVisual_quant);

		// setup constants
		wave.set				(ps_r__Tree_Wave.x,	ps_r__Tree_Wave.y,	ps_r__Tree_Wave.z,	Device.fTimeGlobal*ps_r__Tree_w_speed);			// wave
		wave.div				(PI_MUL_2);
	}
};

void FTreeVisual::Render	(f32 LOD)
{
	static FTreeVisual_setup	tvs;
	if (tvs.dwFrame!=Device.dwFrame)	tvs.calculate();
	// setup constants
#if RENDER==R_R2
	fMatrix4x4					xform_v			;
							xform_v.mul_43	(RCache.get_xform_view(),xform);
							RCache.set_c	(m_xform_v,	xform_v);									// matrix
#endif
							f32	s				= ps_r__Tree_SBC;
	RCache.set_c			(m_xform,	xform);														// matrix
	RCache.set_c			(c_consts,	tvs.scale,tvs.scale,0,0);									// consts/scale
	RCache.set_c			(c_wave,	tvs.wave);													// wave
	RCache.set_c			(c_wind,	tvs.wind);													// wind
#if RENDER==R_R2
	s *= 1.3333f;
	RCache.set_c			(c_c_scale,	s*c_scale.rgb.x,	s*c_scale.rgb.y,	s*c_scale.rgb.z,	s*c_scale.hemi);	// scale
	RCache.set_c			(c_c_bias,	s*c_bias.rgb.x,		s*c_bias.rgb.y,		s*c_bias.rgb.z,		s*c_bias.hemi);		// bias
#else
	CEnvDescriptor&	desc	= g_pGamePersistent->Environment().CurrentEnv;
	RCache.set_c			(c_c_scale,	s*c_scale.rgb.x,					s*c_scale.rgb.y,					s*c_scale.rgb.z,				s*c_scale.hemi);	// scale
	RCache.set_c			(c_c_bias,	s*c_bias.rgb.x + desc.ambient.x,	s*c_bias.rgb.y + desc.ambient.y,	s*c_bias.rgb.z+desc.ambient.z,	s*c_bias.hemi);		// bias
#endif
	RCache.set_c			(c_c_sun,	s*c_scale.sun,  s*c_bias.sun,0,0);							// sun
}

#define PCOPY(a)	a = pFrom->a
void	FTreeVisual::Copy(IRenderVisual* pSrc)
{
	IRenderVisual::Copy(pSrc);

	FTreeVisual	*pFrom		= dynamic_cast<FTreeVisual*> (pSrc);

	PCOPY(rm_geom);

	PCOPY(p_rm_Vertices);	if (p_rm_Vertices) p_rm_Vertices->AddRef();

	PCOPY(vBase);
	PCOPY(vCount);

	PCOPY(p_rm_Indices);	if (p_rm_Indices) p_rm_Indices->AddRef();

	PCOPY(iBase);
	PCOPY(iCount);
	PCOPY(dwPrimitives);

	PCOPY(xform);
	PCOPY(c_scale);
	PCOPY(c_bias);
}

//-----------------------------------------------------------------------------------
// Stripified Tree
//-----------------------------------------------------------------------------------
FTreeVisual_ST::FTreeVisual_ST	(void)
{
}
FTreeVisual_ST::~FTreeVisual_ST	(void)
{
}
void FTreeVisual_ST::Release	()
{
	inherited::Release			();
}
void FTreeVisual_ST::Load		(pcstr N, IReader *data, u32 dwFlags)
{
	inherited::Load				(N,data,dwFlags);
}
void FTreeVisual_ST::Render		(f32 LOD)
{
	inherited::Render			(LOD);
	RCache.set_Geometry			(rm_geom);
	RCache.Render				(D3DPT_TRIANGLELIST,vBase,0,vCount,iBase,dwPrimitives);
	RCache.stat.r.s_flora.add	(vCount);
}
void FTreeVisual_ST::Copy(IRenderVisual* pSrc)
{
	inherited::Copy				(pSrc);
}

//-----------------------------------------------------------------------------------
// Progressive Tree
//-----------------------------------------------------------------------------------
FTreeVisual_PM::FTreeVisual_PM( )
{
	pSWI						= 0;
	last_lod					= 0;
}
FTreeVisual_PM::~FTreeVisual_PM(void)
{
}
void FTreeVisual_PM::Release	()
{
	inherited::Release			();
}
void FTreeVisual_PM::Load		(pcstr N, IReader *data, u32 dwFlags)
{
	inherited::Load				(N,data,dwFlags);
	R_ASSERT					(data->find_chunk(OGF_SWICONTAINER));
	{
		u32 ID					= data->r_u32				();
		pSWI					= RImplementation.getSWI	(ID);
	}
}
void FTreeVisual_PM::Render		(f32 LOD)
{
	inherited::Render			(LOD);
	int lod_id					= last_lod;
	if (LOD>=0.f) {
		lod_id					= iFloor((1.f-LOD)* f32(pSWI->count-1)+0.5f);
		last_lod				= lod_id;
	}
	VERIFY						(lod_id>=0 && lod_id<int(pSWI->count));
	FSlideWindow& SW			= pSWI->sw[lod_id];
	RCache.set_Geometry			(rm_geom);
	RCache.Render				(D3DPT_TRIANGLELIST,vBase,0,SW.num_verts,iBase+SW.offset,SW.num_tris);
	RCache.stat.r.s_flora.add	(SW.num_verts);
}
void FTreeVisual_PM::Copy(IRenderVisual* pSrc)
{
	inherited::Copy				(pSrc);
	FTreeVisual_PM	*pFrom		= dynamic_cast<FTreeVisual_PM*> (pSrc);
	PCOPY						(pSWI);
}
