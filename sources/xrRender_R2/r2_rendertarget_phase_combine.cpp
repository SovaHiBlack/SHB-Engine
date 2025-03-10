#include "stdafx.h"
#include "..\XR_3DA\igame_persistent.h"
#include "..\XR_3DA\environment.h"

#define STENCIL_CULL 0

f32	hclip(f32 v, f32 dim)		{ return 2.f*v/dim - 1.f; }
void	CRenderTarget::phase_combine	()
{
	bool	_menu_pp	= g_pGamePersistent?g_pGamePersistent->OnRenderPPUI_query():false;

	u32			Offset					= 0;
	fVector2	p0;
	fVector2	p1;

	//*** exposure-pipeline
	u32			gpu_id	= Device.dwFrame%2;
	{
		t_LUM_src->surface_set		(rt_LUM_pool[gpu_id*2+0]->pSurface);
		t_LUM_dest->surface_set		(rt_LUM_pool[gpu_id*2+1]->pSurface);
	}

	// low/hi RTs
	u_setrt				( rt_Generic_0,rt_Generic_1,0,HW.pBaseZB );
	RCache.set_CullMode	( CULL_NONE );
	RCache.set_Stencil	( FALSE		);

	BOOL	split_the_scene_to_minimize_wait			= FALSE;
	if (ps_r2_ls_flags.test(R2FLAG_EXP_SPLIT_SCENE))	split_the_scene_to_minimize_wait=TRUE;

	// draw skybox
	if (1)
	{
		RCache.set_ColorWriteEnable					();
		CHK_DX(HW.pDevice->SetRenderState			( D3DRS_ZENABLE,	FALSE				));
		g_pGamePersistent->Environment().RenderSky	();
		CHK_DX(HW.pDevice->SetRenderState			( D3DRS_ZENABLE,	TRUE				));
	}

	// 
	//if (RImplementation.o.bug)	{
		RCache.set_Stencil					(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);	// stencil should be >= 1
		if (RImplementation.o.nvstencil)	{
			u_stencil_optimize				(FALSE);
			RCache.set_ColorWriteEnable		();
		}
	//}

	// calc m-blur matrices
		fMatrix4x4		m_previous;
		fMatrix4x4		m_current;
	fVector2	m_blur_scale;
	{
		static fMatrix4x4		m_saved_viewproj;
		
		// (new-camera) -> (world) -> (old_viewproj)
		fMatrix4x4	m_invview;
		m_invview.invert	(Device.mView);
		m_previous.mul		(m_saved_viewproj,m_invview);
		m_current.set		(Device.mProject)		;
		m_saved_viewproj.set(Device.mFullTransform)	;
		f32	scale		= ps_r2_mblur/2.f;
		m_blur_scale.set	(scale,-scale).div(12.f);
	}

	// Draw full-screen quad textured with our scene image
	if (!_menu_pp)
	{
		// Compute params
		fMatrix4x4		m_v2w;
		m_v2w.invert				(Device.mView		);
		CEnvDescriptorMixer& envdesc= g_pGamePersistent->Environment().CurrentEnv		;
		const f32 minamb			= 0.001f;
		fVector4	ambclr			= { _max(envdesc.ambient.x*2,minamb),	_max(envdesc.ambient.y*2,minamb),			_max(envdesc.ambient.z*2,minamb),	0	};
					ambclr.mul		(ps_r2_sun_lumscale_amb);
					fVector4	envclr			= { envdesc.sky_color.x*2+ EPSILON_5,	envdesc.sky_color.y*2+ EPSILON_5,	envdesc.sky_color.z*2+ EPSILON_5,	envdesc.weight					};
					fVector4	fogclr			= { envdesc.fog_color.x,	envdesc.fog_color.y,	envdesc.fog_color.z,		0	};
					envclr.x		*= 2*ps_r2_sun_lumscale_hemi; 
					envclr.y		*= 2*ps_r2_sun_lumscale_hemi; 
					envclr.z		*= 2*ps_r2_sun_lumscale_hemi;
					fVector4	sunclr;
					fVector4	sundir;

		// sun-params
		{
			light*		fuckingsun		= (light*)RImplementation.Lights.sun_adapted._get()	;
			fVector3		L_dir;
			fVector3		L_clr;
			f32 L_spec;
			L_clr.set					(fuckingsun->color.r,fuckingsun->color.g,fuckingsun->color.b);
			L_spec						= u_diffuse2s	(L_clr);
			Device.mView.transform_dir	(L_dir,fuckingsun->direction);
			L_dir.normalize				();

			sunclr.set				(L_clr.x,L_clr.y,L_clr.z,L_spec);
			sundir.set				(L_dir.x,L_dir.y,L_dir.z,0);
		}

		// Fill VB
		f32	_w					= f32(Device.dwWidth);
		f32	_h					= f32(Device.dwHeight);
		p0.set						(.5f/_w, .5f/_h);
		p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

		// Fill vertex buffer
		fVector4* pv				= (fVector4*)	RCache.Vertex.Lock	(4,g_combine_VP->vb_stride,Offset);
		pv->set						(hclip(EPSILON_5,		_w),	hclip(_h+ EPSILON_5,	_h),	p0.x, p1.y);	pv++;
		pv->set						(hclip(EPSILON_5,		_w),	hclip(EPSILON_5,		_h),	p0.x, p0.y);	pv++;
		pv->set						(hclip(_w+ EPSILON_5,	_w),	hclip(_h+ EPSILON_5,	_h),	p1.x, p1.y);	pv++;
		pv->set						(hclip(_w+ EPSILON_5,	_w),	hclip(EPSILON_5,		_h),	p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine_VP->vb_stride);

		// Setup textures
		IDirect3DBaseTexture9*	e0	= _menu_pp?0:envdesc.sky_r_textures_env[0].second->surface_get();
		IDirect3DBaseTexture9*	e1	= _menu_pp?0:envdesc.sky_r_textures_env[1].second->surface_get();
		t_envmap_0->surface_set		(e0);	_RELEASE(e0);
		t_envmap_1->surface_set		(e1);	_RELEASE(e1);
	
		// Draw
		RCache.set_Element			(s_combine->E[0]	);
		RCache.set_Geometry			(g_combine_VP		);

		RCache.set_c				("m_v2w",			m_v2w	);
		RCache.set_c				("L_ambient",		ambclr	);

		RCache.set_c				("Ldynamic_color",	sunclr	);
		RCache.set_c				("Ldynamic_dir",	sundir	);

		RCache.set_c				("env_color",		envclr	);
		RCache.set_c				("fog_color",		fogclr	);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// Forward rendering
	{
		u_setrt							(rt_Generic_0,0,0,HW.pBaseZB);		// LDR RT
		RCache.set_CullMode				(CULL_CCW);
		RCache.set_Stencil				(FALSE);
		RCache.set_ColorWriteEnable		();
		g_pGamePersistent->Environment().RenderClouds	();
		RImplementation.render_forward	();
		if (g_pGamePersistent)	g_pGamePersistent->OnRenderPPUI_main()	;	// PP-UI
	}

	// Perform blooming filter and distortion if needed
	RCache.set_Stencil	(FALSE);
	phase_bloom			( );												// HDR RT invalidated here

	// Distortion filter
	BOOL	bDistort	= RImplementation.o.distortion_enabled;				// This can be modified
	{
		if		((0==RImplementation.mapDistort.size()) && !_menu_pp)		bDistort= FALSE;
		if (bDistort)		{
			u_setrt						(rt_Generic_1,0,0,HW.pBaseZB);		// Now RT is a distortion mask
			RCache.set_CullMode			(CULL_CCW);
			RCache.set_Stencil			(FALSE);
			RCache.set_ColorWriteEnable	();
			CHK_DX(HW.pDevice->Clear	( 0L, NULL, D3DCLEAR_TARGET, color_rgba(127,127,0,127), 1.0f, 0L));
			RImplementation.r_dsgraph_render_distort	();
			if (g_pGamePersistent)	g_pGamePersistent->OnRenderPPUI_PP()	;	// PP-UI
		}
	}

	// PP enabled ?
	BOOL	PP_Complex		= u_need_PP	();
	if (_menu_pp)			PP_Complex	= FALSE;

	// Combine everything + perform AA
	if		(PP_Complex)	u_setrt		( rt_Color,0,0,HW.pBaseZB );			// LDR RT
	else					u_setrt		( Device.dwWidth,Device.dwHeight,HW.pBaseRT,NULL,NULL,HW.pBaseZB);
	//. u_setrt				( Device.dwWidth,Device.dwHeight,HW.pBaseRT,NULL,NULL,HW.pBaseZB);
	RCache.set_CullMode		( CULL_NONE )	;
	RCache.set_Stencil		( FALSE		)	;
	if (1)	
	{
		// 
		struct v_aa	{
			fVector4	p;
			fVector2	uv0;
			fVector2	uv1;
			fVector2	uv2;
			fVector2	uv3;
			fVector2	uv4;
			fVector4	uv5;
			fVector4	uv6;
		};

		f32	_w					= f32(Device.dwWidth);
		f32	_h					= f32(Device.dwHeight);
		f32	ddw					= 1.f/_w;
		f32	ddh					= 1.f/_h;
		p0.set						(.5f/_w, .5f/_h);
		p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

		// Fill vertex buffer
		v_aa* pv					= (v_aa*) RCache.Vertex.Lock	(4,g_aa_AA->vb_stride,Offset);
		pv->p.set(EPSILON_5, f32(_h+ EPSILON_5), EPSILON_5,1.f); pv->uv0.set(p0.x, p1.y);pv->uv1.set(p0.x-ddw,p1.y-ddh);pv->uv2.set(p0.x+ddw,p1.y+ddh);pv->uv3.set(p0.x+ddw,p1.y-ddh);pv->uv4.set(p0.x-ddw,p1.y+ddh);pv->uv5.set(p0.x-ddw,p1.y,p1.y,p0.x+ddw);pv->uv6.set(p0.x,p1.y-ddh,p1.y+ddh,p0.x);pv++;
		pv->p.set(EPSILON_5, EPSILON_5, EPSILON_5,1.f); pv->uv0.set(p0.x, p0.y);pv->uv1.set(p0.x-ddw,p0.y-ddh);pv->uv2.set(p0.x+ddw,p0.y+ddh);pv->uv3.set(p0.x+ddw,p0.y-ddh);pv->uv4.set(p0.x-ddw,p0.y+ddh);pv->uv5.set(p0.x-ddw,p0.y,p0.y,p0.x+ddw);pv->uv6.set(p0.x,p0.y-ddh,p0.y+ddh,p0.x);pv++;
		pv->p.set(f32(_w+ EPSILON_5), f32(_h+ EPSILON_5), EPSILON_5,1.f); pv->uv0.set(p1.x, p1.y);pv->uv1.set(p1.x-ddw,p1.y-ddh);pv->uv2.set(p1.x+ddw,p1.y+ddh);pv->uv3.set(p1.x+ddw,p1.y-ddh);pv->uv4.set(p1.x-ddw,p1.y+ddh);pv->uv5.set(p1.x-ddw,p1.y,p1.y,p1.x+ddw);pv->uv6.set(p1.x,p1.y-ddh,p1.y+ddh,p1.x);pv++;
		pv->p.set(f32(_w+ EPSILON_5), EPSILON_5, EPSILON_5,1.f); pv->uv0.set(p1.x, p0.y);pv->uv1.set(p1.x-ddw,p0.y-ddh);pv->uv2.set(p1.x+ddw,p0.y+ddh);pv->uv3.set(p1.x+ddw,p0.y-ddh);pv->uv4.set(p1.x-ddw,p0.y+ddh);pv->uv5.set(p1.x-ddw,p0.y,p0.y,p1.x+ddw);pv->uv6.set(p1.x,p0.y-ddh,p0.y+ddh,p1.x);pv++;
		RCache.Vertex.Unlock		(4,g_aa_AA->vb_stride);

		// Draw COLOR
		if (ps_r2_ls_flags.test(R2FLAG_AA))			RCache.set_Element	(s_combine->E[bDistort?3:1]);	// look at blender_combine.cpp
		else										RCache.set_Element	(s_combine->E[bDistort?4:2]);	// look at blender_combine.cpp
		RCache.set_c				("e_barrier",	ps_r2_aa_barier.x,	ps_r2_aa_barier.y,	ps_r2_aa_barier.z,	0);
		RCache.set_c				("e_weights",	ps_r2_aa_weight.x,	ps_r2_aa_weight.y,	ps_r2_aa_weight.z,	0);
		RCache.set_c				("e_kernel",	ps_r2_aa_kernel,	ps_r2_aa_kernel,	ps_r2_aa_kernel,	0);
		RCache.set_c				("m_current",	m_current);
		RCache.set_c				("m_previous",	m_previous);
		RCache.set_c				("m_blur",		m_blur_scale.x,m_blur_scale.y, 0,0);
		RCache.set_Geometry			(g_aa_AA);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}
	RCache.set_Stencil		(FALSE);

	//	if FP16-BLEND !not! supported - draw flares here, overwise they are already in the bloom target
	/* if (!RImplementation.o.fp16_blend)*/	g_pGamePersistent->Environment().RenderFlares	();	// lens-flares

	//	PP-if required
	if (PP_Complex)		{
		phase_pp		();
	}

	//	Re-adapt luminance
	RCache.set_Stencil		(FALSE);

	//*** exposure-pipeline-clear
	{
		std::swap					(rt_LUM_pool[gpu_id*2+0],rt_LUM_pool[gpu_id*2+1]);
		t_LUM_src->surface_set		(NULL);
		t_LUM_dest->surface_set		(NULL);
	}

#ifdef DEBUG
	RCache.set_CullMode	( CULL_CCW );
	static	xr_vector<fPlane3>		saved_dbg_planes;
	if (bDebug)		saved_dbg_planes= dbg_planes;
	else			dbg_planes		= saved_dbg_planes;
	if (1) for (u32 it=0; it<dbg_planes.size(); it++)
	{
		fPlane3&		P	=	dbg_planes[it];
		fVector3		zero	;
		zero.mul	(P.n,P.d);
		
		fVector3             L_dir;
		fVector3			L_up = P.n;
		fVector3			L_right;
		L_dir.set           (0,0,1);                if (_abs(L_up.dotproduct(L_dir))>.99f)  L_dir.set(1,0,0);
		L_right.crossproduct(L_up,L_dir);           L_right.normalize       ();
		L_dir.crossproduct  (L_right,L_up);         L_dir.normalize         ();

		fVector3				p0;
		fVector3			p1;
		fVector3			p2;
		fVector3			p3;
		f32				sz	= 100.0f;
		p0.mad				(zero,L_right,sz).mad	(L_dir,sz);
		p1.mad				(zero,L_right,sz).mad	(L_dir,-sz);
		p2.mad				(zero,L_right,-sz).mad	(L_dir,-sz);
		p3.mad				(zero,L_right,-sz).mad	(L_dir,+sz);
		RCache.dbg_DrawTRI	(Fidentity,p0,p1,p2,0xffffffff);
		RCache.dbg_DrawTRI	(Fidentity,p2,p3,p0,0xffffffff);
	}

	static	xr_vector<dbg_line_t>	saved_dbg_lines;
	if (bDebug)		saved_dbg_lines	= dbg_lines;
	else			dbg_lines		= saved_dbg_lines;
	if (1) for (u32 it=0; it<dbg_lines.size(); it++)
	{
		RCache.dbg_DrawLINE		(Fidentity,dbg_lines[it].P0,dbg_lines[it].P1,dbg_lines[it].color);
	}
#endif

	// ********************* Debug
	/*
	if (0)		{
		u32		C					= color_rgba	(255,255,255,255);
		f32	_w					= f32(Device.dwWidth)/3;
		f32	_h					= f32(Device.dwHeight)/3;

		// draw light-spheres
#ifdef DEBUG
		if (0) for (u32 it=0; it<dbg_spheres.size(); it++)
		{
			fSphere				S	= dbg_spheres[it].first;
			fMatrix4x4				M;	
			u32				ccc		= dbg_spheres[it].second.get();
			M.scale					(S.R,S.R,S.R);
			M.translate_over		(S.P);
			RCache.dbg_DrawEllipse	(M,ccc);
			RCache.dbg_DrawAABB		(S.P,.05f,.05f,.05f,ccc);
		}
#endif
		// Draw quater-screen quad textured with our direct-shadow-map-image
		if (1) 
		{
			u32							IX=0,IY=1;
			p0.set						(.5f/_w, .5f/_h);
			p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

			// Fill vertex buffer
			FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
			pv->set						((IX+0)*_w+EPSILON_5,	(IY+1)*_h+EPSILON_5,	EPSILON_5,	1.f, C, p0.x, p1.y);	pv++;
			pv->set						((IX+0)*_w+EPSILON_5,	(IY+0)*_h+EPSILON_5,	EPSILON_5,	1.f, C, p0.x, p0.y);	pv++;
			pv->set						((IX+1)*_w+EPSILON_5,	(IY+1)*_h+EPSILON_5,	EPSILON_5,	1.f, C, p1.x, p1.y);	pv++;
			pv->set						((IX+1)*_w+EPSILON_5,	(IY+0)*_h+EPSILON_5,	EPSILON_5,	1.f, C, p1.x, p0.y);	pv++;
			RCache.Vertex.Unlock		(4,g_combine->vb_stride);

			// Draw COLOR
			RCache.set_Shader			(s_combine_dbg_0);
			RCache.set_Geometry			(g_combine);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}

		// Draw quater-screen quad textured with our accumulator
		if (0)
		{
			u32							IX=1,IY=1;
			p0.set						(.5f/_w, .5f/_h);
			p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

			// Fill vertex buffer
			FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
			pv->set						((IX+0)*_w+EPSILON_5,	(IY+1)*_h+EPSILON_5,	EPSILON_5,	1.f, C, p0.x, p1.y);	pv++;
			pv->set						((IX+0)*_w+EPSILON_5,	(IY+0)*_h+EPSILON_5,	EPSILON_5,	1.f, C, p0.x, p0.y);	pv++;
			pv->set						((IX+1)*_w+EPSILON_5,	(IY+1)*_h+EPSILON_5,	EPSILON_5,	1.f, C, p1.x, p1.y);	pv++;
			pv->set						((IX+1)*_w+EPSILON_5,	(IY+0)*_h+EPSILON_5,	EPSILON_5,	1.f, C, p1.x, p0.y);	pv++;
			RCache.Vertex.Unlock		(4,g_combine->vb_stride);

			// Draw COLOR
			RCache.set_Shader			(s_combine_dbg_1);
			RCache.set_Geometry			(g_combine);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}
	}
	*/
#ifdef DEBUG
	dbg_spheres.clear	();
	dbg_lines.clear		();
	dbg_planes.clear	();
#endif
}

void CRenderTarget::phase_wallmarks		()
{
	// Targets
	RCache.set_RT(NULL,2);
	RCache.set_RT(NULL,1);
	u_setrt								(rt_Color,NULL,NULL,HW.pBaseZB);
	// Stencil	- draw only where stencil >= 0x1
	RCache.set_Stencil					(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);
	RCache.set_CullMode					(CULL_CCW);
	RCache.set_ColorWriteEnable			(D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE);
}
