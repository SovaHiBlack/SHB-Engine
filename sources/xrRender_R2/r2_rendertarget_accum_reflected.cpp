#include "stdafx.h"

void CRenderTarget::accum_reflected		(light* L)
{
	phase_accumulator				();
	RImplementation.stats.l_visible	++;

	// *** assume accumulator setted up ***
	// *****************************	Mask by stencil		*************************************
	ref_shader		shader		= s_accum_reflected;

	BOOL	bIntersect			= FALSE; //enable_scissor(L);
	L->xform_calc				();
	RCache.set_xform_world		(L->m_xform			);
	RCache.set_xform_view		(Device.mView		);
	RCache.set_xform_project	(Device.mProject	);
	bIntersect					= enable_scissor	(L);
	enable_dbt_bounds			(L);

	// *****************************	Minimize overdraw	*************************************
	// Select shader (front or back-faces), *** back, if intersect near plane
	RCache.set_ColorWriteEnable				();
	if (bIntersect)	RCache.set_CullMode		(CULL_CW);		// back
	else			RCache.set_CullMode		(CULL_CCW);		// front

	// 2D texgen (texture adjustment matrix)
	fMatrix4x4			m_Texgen;
	{
		f32	_w						= f32(Device.dwWidth);
		f32	_h						= f32(Device.dwHeight);
		f32	o_w						= (.5f / _w);
		f32	o_h						= (.5f / _h);
		fMatrix4x4			m_TexelAdjust		=
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				1.0f,			0.0f,
			0.5f + o_w,			0.5f + o_h,			0.0f,			1.0f
		};
		m_Texgen.mul	(m_TexelAdjust,RCache.xforms.m_wvp);
	}

	// Common constants
	fVector3		L_dir;
	fVector3	L_clr;
	fVector3	L_pos;
	f32 L_spec;
	L_clr.set					(L->color.r,L->color.g,L->color.b);
	L_spec						= u_diffuse2s	(L_clr);
	Device.mView.transform_tiny	(L_pos,L->position);
	Device.mView.transform_dir	(L_dir,L->direction);
	L_dir.normalize				();

	{
		// Lighting
		RCache.set_Shader			(shader);

		// Constants
		RCache.set_c				("Ldynamic_pos",	L_pos.x,L_pos.y,L_pos.z,1/(L->range*L->range));
		RCache.set_c				("Ldynamic_color",	L_clr.x,L_clr.y,L_clr.z,L_spec);
		RCache.set_c				("direction",		L_dir.x,L_dir.y,L_dir.z,0);
		RCache.set_c				("m_texgen",		m_Texgen);

		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP);
		draw_volume					(L);
	}

	// blend-copy
	if (!RImplementation.o.fp16_blend)	{
		u_setrt						(rt_Accumulator,NULL,NULL,HW.pBaseZB);
		RCache.set_Element			(s_accum_mask->E[SE_MASK_ACCUM_VOL]	);
		RCache.set_c				("m_texgen",		m_Texgen);
		draw_volume					(L);
	}

	// 
	u_DBT_disable	();
}
