#include "stdafx.h"
#include "..\XR_3DA\igame_persistent.h"
#include "..\XR_3DA\environment.h"

void CRenderTarget::accum_direct		(u32 sub_phase)
{
	// Choose normal code-path or filtered
	phase_accumulator					();
	if (RImplementation.o.sunfilter)	{
		accum_direct_f	(sub_phase);
		return			;
	}

	// *** assume accumulator setted up ***
	light*			fuckingsun			= (light*)RImplementation.Lights.sun_adapted._get()	;

	// Common calc for quad-rendering
	u32		Offset;
	u32		C					= color_rgba	(255,255,255,255);
	f32	_w						= f32(Device.dwWidth);
	f32	_h						= f32(Device.dwHeight);
	fVector2					p0;
	fVector2					p1;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );
	f32	d_Z = EPSILON_7;
	f32 d_W = 1.f;

	// Common constants (light-related)
	fVector3		L_dir;
	fVector3		L_clr;
	f32 L_spec;
	L_clr.set					(fuckingsun->color.r,fuckingsun->color.g,fuckingsun->color.b);
	L_spec						= u_diffuse2s	(L_clr);
	Device.mView.transform_dir	(L_dir,fuckingsun->direction);
	L_dir.normalize				();

	// Perform masking (only once - on the first/near phase)
	RCache.set_CullMode			(CULL_NONE	);
	if (SE_SUN_NEAR==sub_phase)	//.
	{
		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*)	RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(EPSILON_5, f32(_h+ EPSILON_5),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
		pv->set						(EPSILON_5, EPSILON_5,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
		pv->set						(f32(_w+ EPSILON_5), f32(_h+ EPSILON_5),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
		pv->set						(f32(_w+ EPSILON_5), EPSILON_5,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);
		RCache.set_Geometry			(g_combine);

		// setup
		f32	intensity			= 0.3f*fuckingsun->color.r + 0.48f*fuckingsun->color.g + 0.22f*fuckingsun->color.b;
		fVector3	dir					= L_dir;
				dir.normalize().mul	(- _sqrt(intensity+ EPSILON_5));
		RCache.set_Element			(s_accum_mask->E[SE_MASK_DIRECT]);		// masker
		RCache.set_c				("Ldynamic_dir",		dir.x,dir.y,dir.z,0		);

		// if (stencil>=1 && aref_pass)	stencil = light_id
		RCache.set_ColorWriteEnable	(FALSE		);
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0x01,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// recalculate d_Z, to perform depth-clipping
	fVector3	center_pt;
	center_pt.mad	(Device.vCameraPosition,Device.vCameraDirection,ps_r2_sun_near);
	Device.mFullTransform.transform(center_pt)	;
	d_Z							= center_pt.z	;

	// nv-stencil recompression
	if (RImplementation.o.nvstencil  && (SE_SUN_NEAR==sub_phase))	u_stencil_optimize();	//. driver bug?

	// Perform lighting
	{
		phase_accumulator					()	;
		RCache.set_CullMode					(CULL_NONE);
		RCache.set_ColorWriteEnable			()	;

		// texture adjustment matrix
		f32			fTexelOffs			= (.5f / f32(RImplementation.o.smapsize));
		f32			fRange				= (SE_SUN_NEAR==sub_phase)?ps_r2_sun_depth_near_scale:ps_r2_sun_depth_far_scale;
		f32			fBias				= (SE_SUN_NEAR==sub_phase)?ps_r2_sun_depth_near_bias:ps_r2_sun_depth_far_bias;
		fMatrix4x4			m_TexelAdjust		=
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				fRange,			0.0f,
			0.5f + fTexelOffs,	0.5f + fTexelOffs,	fBias,			1.0f
		};

		// compute xforms
		FPU::m64r			();
		fMatrix4x4				xf_invview;
		xf_invview.invert	(Device.mView)	;

		// shadow xform
		fMatrix4x4				m_shadow;
		{
			fMatrix4x4			xf_project;
			xf_project.mul		(m_TexelAdjust,fuckingsun->X.D.combine);
			m_shadow.mul	(xf_project,	xf_invview);

			// tsm-bias
			if ( (SE_SUN_FAR == sub_phase) && (RImplementation.o.HW_smap) )
			{
				fVector3		bias;
				bias.mul		(L_dir,ps_r2_sun_tsm_bias);
				fMatrix4x4		bias_t;
				bias_t.translate(bias);
				m_shadow.mulB_44	(bias_t);
			}
			FPU::m24r		();
		}

		// clouds xform
		fMatrix4x4				m_clouds_shadow;
		{
			static	f32	w_shift		= 0;
			fMatrix4x4			m_xform;
			fVector3			direction	= fuckingsun->direction	;
			f32	w_dir				= g_pGamePersistent->Environment().CurrentEnv.wind_direction	;
			//f32	w_speed				= g_pGamePersistent->Environment().CurrentEnv.wind_velocity	;
			fVector3			normal	;
			normal.setHP(w_dir,0.0f);
							w_shift		+=	0.003f*Device.fTimeDelta;
							fVector3			position;
							position.set(0.0f,0.0f,0.0f);
			m_xform.build_camera_dir	(position,direction,normal)	;
			fVector3			localnormal;
			m_xform.transform_dir(localnormal,normal);
			localnormal.normalize();
			m_clouds_shadow.mul			(m_xform,xf_invview)		;
			m_xform.scale				(0.002f,0.002f,1.f)			;
			m_clouds_shadow.mulA_44		(m_xform)					;
			m_xform.translate			(localnormal.mul(w_shift))	;
			m_clouds_shadow.mulA_44		(m_xform)					;
		}

		// Make jitter texture
		fVector2					j0;
		fVector2					j1;
		f32	scale_X				= f32(Device.dwWidth)	/ f32(TEX_jitter);
		//f32	scale_Y				= f32(Device.dwHeight)/ f32TEX_jitter);
		f32	offset				= (.5f / f32(TEX_jitter));
		j0.set						(offset,offset);
		j1.set						(scale_X,scale_X).add(offset);

		// Fill vertex buffer
		FVF::TL2uv* pv				= (FVF::TL2uv*) RCache.Vertex.Lock	(4,g_combine_2UV->vb_stride,Offset);
		pv->set						(EPSILON_5, f32(_h+ EPSILON_5),	d_Z,	d_W, C, p0.x, p1.y, j0.x, j1.y);	pv++;
		pv->set						(EPSILON_5, EPSILON_5,			d_Z,	d_W, C, p0.x, p0.y, j0.x, j0.y);	pv++;
		pv->set						(f32(_w+ EPSILON_5), f32(_h+ EPSILON_5),	d_Z,	d_W, C, p1.x, p1.y, j1.x, j1.y);	pv++;
		pv->set						(f32(_w+ EPSILON_5), EPSILON_5,			d_Z,	d_W, C, p1.x, p0.y, j1.x, j0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine_2UV->vb_stride);
		RCache.set_Geometry			(g_combine_2UV);

		// setup
		RCache.set_Element			(s_accum_direct->E[sub_phase]);
		RCache.set_c				("Ldynamic_dir",		L_dir.x,L_dir.y,L_dir.z,0		);
		RCache.set_c				("Ldynamic_color",		L_clr.x,L_clr.y,L_clr.z,L_spec	);
		RCache.set_c				("m_shadow",			m_shadow						);
		RCache.set_c				("m_sunmask",			m_clouds_shadow					);
		
		// nv-DBT
		f32 zMin;
		f32 zMax;
		if (SE_SUN_NEAR==sub_phase)	{
			zMin = 0;
			zMax = ps_r2_sun_near;
		} else {
			extern f32	OLES_SUN_LIMIT_27_01_07;
			zMin = ps_r2_sun_near;
			zMax = OLES_SUN_LIMIT_27_01_07;
		}
		center_pt.mad(Device.vCameraPosition,Device.vCameraDirection,zMin);	Device.mFullTransform.transform	(center_pt);
		zMin = center_pt.z	;

		center_pt.mad(Device.vCameraPosition,Device.vCameraDirection,zMax);	Device.mFullTransform.transform	(center_pt);
		zMax = center_pt.z	;

		if (u_DBT_enable(zMin,zMax))	{
			// z-test always
			HW.pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
			HW.pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		}

		// Fetch4 : enable
		if (RImplementation.o.HW_smap_FETCH4)	{
			//. we hacked the shader to force smap on S0
#			define FOURCC_GET4  MAKEFOURCC('G','E','T','4') 
			HW.pDevice->SetSamplerState	( 0, D3DSAMP_MIPMAPLODBIAS, FOURCC_GET4 );
		}

		// setup stencil
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x00);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);

		// Fetch4 : disable
		if (RImplementation.o.HW_smap_FETCH4)	{
			//. we hacked the shader to force smap on S0
#			define FOURCC_GET1  MAKEFOURCC('G','E','T','1') 
			HW.pDevice->SetSamplerState	( 0, D3DSAMP_MIPMAPLODBIAS, FOURCC_GET1 );
		}

		// disable depth bounds
		u_DBT_disable	();
	}
}

void CRenderTarget::accum_direct_blend	()
{
	// blend-copy
	if (!RImplementation.o.fp16_blend)	{
		u_setrt						(rt_Accumulator,NULL,NULL,HW.pBaseZB);

		// Common calc for quad-rendering
		u32		Offset;
		u32		C					= color_rgba	(255,255,255,255);
		f32	_w					= f32(Device.dwWidth);
		f32	_h					= f32(Device.dwHeight);
		fVector2					p0;
		fVector2					p1;
		p0.set						(.5f/_w, .5f/_h);
		p1.set						((_w+.5f)/_w, (_h+.5f)/_h );
		f32	d_Z = EPSILON_7;
		f32 d_W = 1.f;

		// Fill vertex buffer
		FVF::TL2uv* pv				= (FVF::TL2uv*) RCache.Vertex.Lock	(4,g_combine_2UV->vb_stride,Offset);
		pv->set						(EPSILON_5, f32(_h+ EPSILON_5),	d_Z,	d_W, C, p0.x, p1.y, p0.x, p1.y);	pv++;
		pv->set						(EPSILON_5, EPSILON_5,			d_Z,	d_W, C, p0.x, p0.y, p0.x, p0.y);	pv++;
		pv->set						(f32(_w+ EPSILON_5), f32(_h+ EPSILON_5),	d_Z,	d_W, C, p1.x, p1.y, p1.x, p1.y);	pv++;
		pv->set						(f32(_w+ EPSILON_5), EPSILON_5,			d_Z,	d_W, C, p1.x, p0.y, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine_2UV->vb_stride);
		RCache.set_Geometry			(g_combine_2UV);
		RCache.set_Element			(s_accum_mask->E[SE_MASK_ACCUM_2D]	);
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x00);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2	);
	}
	dwLightMarkerID				+= 2;
}

void CRenderTarget::accum_direct_f		(u32 sub_phase)
{
	// Select target
	if (SE_SUN_LUMINANCE==sub_phase)	{
		accum_direct_lum	();
		return				;
	}

	phase_accumulator					();
	u_setrt								(rt_Generic_0,NULL,NULL,HW.pBaseZB);

	// *** assume accumulator setted up ***
	light*			fuckingsun			= (light*)RImplementation.Lights.sun_adapted._get()	;

	// Common calc for quad-rendering
	u32		Offset;
	u32		C					= color_rgba	(255,255,255,255);
	f32	_w					= f32(Device.dwWidth);
	f32	_h					= f32(Device.dwHeight);
	fVector2					p0;
	fVector2					p1;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );
	f32	d_Z = EPSILON_7;
	f32 d_W = 1.f;

	// Common constants (light-related)
	fVector3		L_dir;
	fVector3		L_clr;
	f32 L_spec;
	L_clr.set					(fuckingsun->color.r,fuckingsun->color.g,fuckingsun->color.b);
	L_spec						= u_diffuse2s	(L_clr);
	Device.mView.transform_dir	(L_dir,fuckingsun->direction);
	L_dir.normalize				();

	// Perform masking (only once - on the first/near phase)
	RCache.set_CullMode			(CULL_NONE	);
	if (SE_SUN_NEAR==sub_phase)	//.
	{
		// For sun-filter - clear to zero
		CHK_DX	(HW.pDevice->Clear	( 0L, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0L));

		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*)	RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(EPSILON_5, f32(_h+ EPSILON_5),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
		pv->set						(EPSILON_5, EPSILON_5,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
		pv->set						(f32(_w+ EPSILON_5), f32(_h+ EPSILON_5),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
		pv->set						(f32(_w+ EPSILON_5), EPSILON_5,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);
		RCache.set_Geometry			(g_combine);

		// setup
		f32	intensity			= 0.3f*fuckingsun->color.r + 0.48f*fuckingsun->color.g + 0.22f*fuckingsun->color.b;
		fVector3	dir					= L_dir;
		dir.normalize().mul	(- _sqrt(intensity+ EPSILON_5));
		RCache.set_Element			(s_accum_mask->E[SE_MASK_DIRECT]);		// masker
		RCache.set_c				("Ldynamic_dir",		dir.x,dir.y,dir.z,0		);

		// if (stencil>=1 && aref_pass)	stencil = light_id
		RCache.set_ColorWriteEnable	(FALSE		);
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0x01,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// recalculate d_Z, to perform depth-clipping
	fVector3	center_pt;
	center_pt.mad	(Device.vCameraPosition,Device.vCameraDirection,ps_r2_sun_near);
	Device.mFullTransform.transform(center_pt)	;
	d_Z							= center_pt.z	;

	// nv-stencil recompression
	if (RImplementation.o.nvstencil  && (SE_SUN_NEAR==sub_phase))	u_stencil_optimize();	//. driver bug?

	// Perform lighting
	{
		u_setrt								(rt_Generic_0,NULL,NULL,HW.pBaseZB);  // enshure RT setup
		RCache.set_CullMode					(CULL_NONE	);
		RCache.set_ColorWriteEnable			();

		// texture adjustment matrix
		f32			fTexelOffs			= (.5f / f32(RImplementation.o.smapsize));
		f32			fRange				= (SE_SUN_NEAR==sub_phase)?ps_r2_sun_depth_near_scale:ps_r2_sun_depth_far_scale;
		f32			fBias				= (SE_SUN_NEAR==sub_phase)?ps_r2_sun_depth_near_bias:ps_r2_sun_depth_far_bias;
		fMatrix4x4			m_TexelAdjust		=
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				fRange,			0.0f,
			0.5f + fTexelOffs,	0.5f + fTexelOffs,	fBias,			1.0f
		};

		// compute xforms
		fMatrix4x4				m_shadow;
		{
			FPU::m64r		();
			fMatrix4x4			xf_invview;
			xf_invview.invert	(Device.mView)	;
			fMatrix4x4			xf_project;
			xf_project.mul		(m_TexelAdjust,fuckingsun->X.D.combine);
			m_shadow.mul	(xf_project,	xf_invview);

			// tsm-bias
			if (SE_SUN_FAR == sub_phase)
			{
				fVector3		bias;
				bias.mul		(L_dir,ps_r2_sun_tsm_bias);
				fMatrix4x4		bias_t;
				bias_t.translate(bias);
				m_shadow.mulB_44	(bias_t);
			}
			FPU::m24r		();
		}

		// Make jitter texture
		fVector2					j0;
		fVector2					j1;
		f32	scale_X				= f32(Device.dwWidth)	/ f32(TEX_jitter);
		//f32	scale_Y				= f32(Device.dwHeight)/ f32(TEX_jitter);
		f32	offset				= (.5f / f32(TEX_jitter));
		j0.set						(offset,offset);
		j1.set						(scale_X,scale_X).add(offset);

		// Fill vertex buffer
		FVF::TL2uv* pv				= (FVF::TL2uv*) RCache.Vertex.Lock	(4,g_combine_2UV->vb_stride,Offset);
		pv->set						(EPSILON_5, f32(_h+ EPSILON_5),	d_Z,	d_W, C, p0.x, p1.y, j0.x, j1.y);	pv++;
		pv->set						(EPSILON_5, EPSILON_5,			d_Z,	d_W, C, p0.x, p0.y, j0.x, j0.y);	pv++;
		pv->set						(f32(_w+ EPSILON_5), f32(_h+ EPSILON_5),	d_Z,	d_W, C, p1.x, p1.y, j1.x, j1.y);	pv++;
		pv->set						(f32(_w+ EPSILON_5), EPSILON_5,			d_Z,	d_W, C, p1.x, p0.y, j1.x, j0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine_2UV->vb_stride);
		RCache.set_Geometry			(g_combine_2UV);

		// setup
		RCache.set_Element			(s_accum_direct->E[sub_phase]);
		RCache.set_c				("Ldynamic_dir",		L_dir.x,L_dir.y,L_dir.z,0		);
		RCache.set_c				("Ldynamic_color",		L_clr.x,L_clr.y,L_clr.z,L_spec	);
		RCache.set_c				("m_shadow",			m_shadow						);

		// setup stencil
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x00);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}
}

void CRenderTarget::accum_direct_lum	()
{
	// Select target
	phase_accumulator					();

	// *** assume accumulator setted up ***
	light*			fuckingsun			= (light*)RImplementation.Lights.sun_adapted._get()	;

	// Common calc for quad-rendering
	u32		Offset;
	// u32		C					= color_rgba	(255,255,255,255);
	f32	_w					= f32(Device.dwWidth);
	f32	_h					= f32(Device.dwHeight);
	fVector2					p0;
	fVector2					p1;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );
	f32	d_Z	= EPSILON_7;
	//f32 d_W = 1.f;

	// Common constants (light-related)
	fVector3		L_dir;
	fVector3		L_clr;
	f32 L_spec;
	L_clr.set					(fuckingsun->color.r,fuckingsun->color.g,fuckingsun->color.b);
	L_spec						= u_diffuse2s	(L_clr);
	Device.mView.transform_dir	(L_dir,fuckingsun->direction);
	L_dir.normalize				();

	// recalculate d_Z, to perform depth-clipping
	fVector3	center_pt;
	center_pt.mad	(Device.vCameraPosition,Device.vCameraDirection,ps_r2_sun_near);
	Device.mFullTransform.transform(center_pt)	;
	d_Z							= center_pt.z	;

	// nv-stencil recompression
	/*
	if (RImplementation.o.nvstencil  && (SE_SUN_NEAR==sub_phase))	u_stencil_optimize();	//. driver bug?
	*/

	// Perform lighting
		RCache.set_CullMode					(CULL_NONE	);
		RCache.set_ColorWriteEnable			();

		// Make jitter texture
		fVector2					j0;
		fVector2					j1;
		f32	scale_X				= f32(Device.dwWidth)	/ f32(TEX_jitter);
//		f32	scale_Y				= f32(Device.dwHeight)/ f32(TEX_jitter);
		f32	offset				= (.5f / f32(TEX_jitter));
		j0.set						(offset,offset);
		j1.set						(scale_X,scale_X).add(offset);

		struct v_aa	{
			fVector4	p;
			fVector2	uv0;
			fVector2	uvJ;
			fVector2	uv1;
			fVector2	uv2;
			fVector2	uv3;
			fVector4	uv4;
			fVector4	uv5;
		};
		f32	smooth				= 0.6f;
		f32	ddw					= smooth/_w;
		f32	ddh					= smooth/_h;

		// Fill vertex buffer
		VERIFY	(sizeof(v_aa)==g_aa_AA->vb_stride);
		v_aa* pv					= (v_aa*) RCache.Vertex.Lock	(4,g_aa_AA->vb_stride,Offset);
		pv->p.set(EPSILON_5, f32(_h+ EPSILON_5), EPSILON_5,1.f); pv->uv0.set(p0.x, p1.y);pv->uvJ.set(j0.x, j1.y);pv->uv1.set(p0.x-ddw,p1.y-ddh);pv->uv2.set(p0.x+ddw,p1.y+ddh);pv->uv3.set(p0.x+ddw,p1.y-ddh);pv->uv4.set(p0.x-ddw,p1.y+ddh,0,0);pv->uv5.set(0,0,0,0);pv++;
		pv->p.set(EPSILON_5, EPSILON_5, EPSILON_5,1.f); pv->uv0.set(p0.x, p0.y);pv->uvJ.set(j0.x, j0.y);pv->uv1.set(p0.x-ddw,p0.y-ddh);pv->uv2.set(p0.x+ddw,p0.y+ddh);pv->uv3.set(p0.x+ddw,p0.y-ddh);pv->uv4.set(p0.x-ddw,p0.y+ddh,0,0);pv->uv5.set(0,0,0,0);pv++;
		pv->p.set(f32(_w+ EPSILON_5), f32(_h+ EPSILON_5), EPSILON_5,1.f); pv->uv0.set(p1.x, p1.y);pv->uvJ.set(j1.x, j1.y);pv->uv1.set(p1.x-ddw,p1.y-ddh);pv->uv2.set(p1.x+ddw,p1.y+ddh);pv->uv3.set(p1.x+ddw,p1.y-ddh);pv->uv4.set(p1.x-ddw,p1.y+ddh,0,0);pv->uv5.set(0,0,0,0);pv++;
		pv->p.set(f32(_w+ EPSILON_5), EPSILON_5, EPSILON_5,1.f); pv->uv0.set(p1.x, p0.y);pv->uvJ.set(j1.x, j0.y);pv->uv1.set(p1.x-ddw,p0.y-ddh);pv->uv2.set(p1.x+ddw,p0.y+ddh);pv->uv3.set(p1.x+ddw,p0.y-ddh);pv->uv4.set(p1.x-ddw,p0.y+ddh,0,0);pv->uv5.set(0,0,0,0);pv++;
		RCache.Vertex.Unlock		(4,g_aa_AA->vb_stride);
		RCache.set_Geometry			(g_aa_AA);

		// setup
		RCache.set_Element			(s_accum_direct->E[SE_SUN_LUMINANCE]);
		RCache.set_c				("Ldynamic_dir",		L_dir.x,L_dir.y,L_dir.z,0		);
		RCache.set_c				("Ldynamic_color",		L_clr.x,L_clr.y,L_clr.z,L_spec	);

		// setup stencil
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x00);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}
