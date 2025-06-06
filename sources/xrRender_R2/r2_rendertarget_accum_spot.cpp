#include "stdafx.h"

extern fVector3 du_cone_vertices[DU_CONE_NUMVERTEX];

void CRenderTarget::accum_spot	(light* L)
{
	phase_accumulator				();
	RImplementation.stats.l_visible	++;

	// *** assume accumulator setted up ***
	// *****************************	Mask by stencil		*************************************
	ref_shader			shader;
	if (IRender_Light::OMNIPART == L->flags.type)	{
			shader		= L->s_point;
		if (!shader)	shader		= s_accum_point;
	} else {
			shader		= L->s_spot;
		if (!shader)	shader		= s_accum_spot;
	}

	BOOL	bIntersect			= FALSE; //enable_scissor(L);
	{
		// setup xform
		L->xform_calc					();
		RCache.set_xform_world			(L->m_xform			);
		RCache.set_xform_view			(Device.mView		);
		RCache.set_xform_project		(Device.mProject	);
		bIntersect						= enable_scissor	(L);
		enable_dbt_bounds				(L);

		// *** similar to "Carmack's reverse", but assumes convex, non intersecting objects,
		// *** thus can cope without stencil clear with 127 lights
		// *** in practice, 'cause we "clear" it back to 0x1 it usually allows us to > 200 lights :)
		RCache.set_ColorWriteEnable		(FALSE);
		RCache.set_Element				(s_accum_mask->E[SE_MASK_SPOT]);		// masker

		// backfaces: if (stencil>=1 && zfail)			stencil = light_id
		RCache.set_CullMode				(CULL_CW);
		RCache.set_Stencil				(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0x01,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE);
		draw_volume						(L);

		// frontfaces: if (stencil>=light_id && zfail)	stencil = 0x1
		RCache.set_CullMode				(CULL_CCW);
		RCache.set_Stencil				(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE);
		draw_volume						(L);
	}

	// nv-stencil recompression
	if (RImplementation.o.nvstencil)	u_stencil_optimize();

	// *****************************	Minimize overdraw	*************************************
	// Select shader (front or back-faces), *** back, if intersect near plane
	RCache.set_ColorWriteEnable		();
	RCache.set_CullMode				(CULL_CW);		// back

	// 2D texgens 
	fMatrix4x4			m_Texgen;
	u_compute_texgen_screen	(m_Texgen	);
	fMatrix4x4			m_Texgen_J;
	u_compute_texgen_jitter	(m_Texgen_J	);

	// Shadow xform (+texture adjustment matrix)
	fMatrix4x4			m_Shadow;
	fMatrix4x4			m_Lmap;
	{
		f32			smapsize			= f32(RImplementation.o.smapsize);
		f32			fTexelOffs			= (.5f / smapsize);
		f32			view_dim			= f32(L->X.S.size-2)/smapsize;
		f32			view_sx				= f32(L->X.S.posX+1)/smapsize;
		f32			view_sy				= f32(L->X.S.posY+1)/smapsize;
		f32			fRange				= f32(1.f)*ps_r2_ls_depth_scale;
		f32			fBias				= ps_r2_ls_depth_bias;
		fMatrix4x4			m_TexelAdjust		= {
			view_dim/2.f,							0.0f,									0.0f,		0.0f,
			0.0f,									-view_dim/2.f,							0.0f,		0.0f,
			0.0f,									0.0f,									fRange,		0.0f,
			view_dim/2.f + view_sx + fTexelOffs,	view_dim/2.f + view_sy + fTexelOffs,	fBias,		1.0f
		};

		// compute xforms
		fMatrix4x4			xf_world;
		xf_world.invert	(Device.mView);
		fMatrix4x4			xf_view			= L->X.S.view;
		fMatrix4x4			xf_project;
		xf_project.mul	(m_TexelAdjust,L->X.S.project);
		m_Shadow.mul					(xf_view, xf_world);
		m_Shadow.mulA_44				(xf_project	);

		// lmap
						view_dim			= 1.f;
						view_sx				= 0.f;
						view_sy				= 0.f;
						fMatrix4x4			m_TexelAdjust2		= {
			view_dim/2.f,							0.0f,									0.0f,		0.0f,
			0.0f,									-view_dim/2.f,							0.0f,		0.0f,
			0.0f,									0.0f,									fRange,		0.0f,
			view_dim/2.f + view_sx + fTexelOffs,	view_dim/2.f + view_sy + fTexelOffs,	fBias,		1.0f
		};

		// compute xforms
		xf_project.mul		(m_TexelAdjust2,L->X.S.project);
		m_Lmap.mul			(xf_view, xf_world);
		m_Lmap.mulA_44		(xf_project	);
	}

	// Common constants
	fVector3		L_dir;
	fVector3		L_clr;
	fVector3		L_pos;
	f32 L_spec;
	L_clr.set					(L->color.r,L->color.g,L->color.b);
	L_clr.mul					(L->get_LOD());
	L_spec						= u_diffuse2s	(L_clr);
	Device.mView.transform_tiny	(L_pos,L->position);
	Device.mView.transform_dir	(L_dir,L->direction);
	L_dir.normalize				();

	// Draw volume with projective texgen
	{
		// Select shader
		u32		_id					= 0;
		if (L->flags.bShadow)		{
			bool	bFullSize			= (L->X.S.size == RImplementation.o.smapsize);
			if (L->X.S.transluent)	_id	= SE_L_TRANSLUENT;
			else if		(bFullSize)	_id	= SE_L_FULLSIZE;
			else					_id	= SE_L_NORMAL;
		} else {
			_id						= SE_L_UNSHADOWED;
			m_Shadow				= m_Lmap;
		}
		RCache.set_Element			(shader->E[ _id ]	);

		// Constants
		f32	att_R				= L->range*.95f;
		f32	att_factor			= 1.f/(att_R*att_R);
		RCache.set_c				("Ldynamic_pos",	L_pos.x,L_pos.y,L_pos.z,att_factor);
		RCache.set_c				("Ldynamic_color",	L_clr.x,L_clr.y,L_clr.z,L_spec);
		RCache.set_c				("m_texgen",		m_Texgen	);
		RCache.set_c				("m_texgen_J",		m_Texgen_J	);
		RCache.set_c				("m_shadow",		m_Shadow	);
		RCache.set_ca				("m_lmap",		0,	m_Lmap._11, m_Lmap._21, m_Lmap._31, m_Lmap._41	);
		RCache.set_ca				("m_lmap",		1,	m_Lmap._12, m_Lmap._22, m_Lmap._32, m_Lmap._42	);

		// Fetch4 : enable
		if (RImplementation.o.HW_smap_FETCH4)	{
			//. we hacked the shader to force smap on S0
#			define FOURCC_GET4  MAKEFOURCC('G','E','T','4') 
			HW.pDevice->SetSamplerState	( 0, D3DSAMP_MIPMAPLODBIAS, FOURCC_GET4 );
		}

		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x00);
		draw_volume					(L);

		// Fetch4 : disable
		if (RImplementation.o.HW_smap_FETCH4)	{
			//. we hacked the shader to force smap on S0
#			define FOURCC_GET1  MAKEFOURCC('G','E','T','1') 
			HW.pDevice->SetSamplerState	( 0, D3DSAMP_MIPMAPLODBIAS, FOURCC_GET1 );
		}
	}

	// blend-copy
	if (!RImplementation.o.fp16_blend)	{
		u_setrt						(rt_Accumulator,NULL,NULL,HW.pBaseZB);
		RCache.set_Element			(s_accum_mask->E[SE_MASK_ACCUM_VOL]	);
		RCache.set_c				("m_texgen",		m_Texgen);
		RCache.set_c				("m_texgen_J",		m_Texgen_J	);
		draw_volume					(L);
	}
	dwLightMarkerID					+=	2;	// keep lowest bit always setted up
	CHK_DX		(HW.pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE));

	u_DBT_disable				();
}
