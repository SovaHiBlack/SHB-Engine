#include "stdafx.h"

// startup
void	CRenderTarget::phase_scene_prepare	()
{
	// Clear depth & stencil
	u_setrt	( Device.dwWidth,Device.dwHeight,HW.pBaseRT,NULL,NULL,HW.pBaseZB );
	CHK_DX	( HW.pDevice->Clear	( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x0, 1.0f, 0L) );
}

// begin
void	CRenderTarget::phase_scene_begin	()
{
	// Enable ANISO
	for (u32 i=0; i<HW.Caps.raster.dwStages; i++)
		CHK_DX(HW.pDevice->SetSamplerState( i, D3DSAMP_MAXANISOTROPY, ps_r__tf_Anisotropic	));

	// Targets, use accumulator for temporary storage
	if (RImplementation.o.albedo_wo)	u_setrt		(rt_Position,	rt_Normal,	rt_Accumulator,	HW.pBaseZB);
	else								u_setrt		(rt_Position,	rt_Normal,	rt_Color,		HW.pBaseZB);

	// Stencil - write 0x1 at pixel pos
	RCache.set_Stencil					( TRUE,D3DCMP_ALWAYS,0x01,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);

	// Misc		- draw only front-faces
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_TWOSIDEDSTENCILMODE,FALSE				));
	RCache.set_CullMode					( CULL_CCW );
	RCache.set_ColorWriteEnable			( );
}

void	CRenderTarget::disable_aniso		()
{
	// Disable ANISO
	for (u32 i=0; i<HW.Caps.raster.dwStages; i++)
		CHK_DX(HW.pDevice->SetSamplerState( i, D3DSAMP_MAXANISOTROPY, 1	));
}

// end
void	CRenderTarget::phase_scene_end		()
{
	disable_aniso	();

	if (!RImplementation.o.albedo_wo)		return;

	// transfer from "rt_Accumulator" into "rt_Color"
	u_setrt								( rt_Color,	0,	0,	HW.pBaseZB	);
	RCache.set_CullMode					( CULL_NONE );
	RCache.set_Stencil					(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);	// stencil should be >= 1
	if (RImplementation.o.nvstencil)	u_stencil_optimize	(FALSE);
	RCache.set_Stencil					(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);	// stencil should be >= 1
	RCache.set_ColorWriteEnable			();

	// common calc for quad-rendering
	u32		Offset;
	u32		C					= color_rgba	(255,255,255,255);
	f32	_w					= f32(Device.dwWidth);
	f32	_h					= f32(Device.dwHeight);
	fVector2					p0;
	fVector2					p1;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );
	f32	d_Z = EPSILON_7;
	f32	d_W = 1.f;

	// Fill vertex buffer
	FVF::TL* pv					= (FVF::TL*)	RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
	pv->set						(EPSILON_5, f32(_h+ EPSILON_5),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
	pv->set						(EPSILON_5, EPSILON_5,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
	pv->set						(f32(_w+ EPSILON_5), f32(_h+ EPSILON_5),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
	pv->set						(f32(_w+ EPSILON_5), EPSILON_5,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
	RCache.Vertex.Unlock		(4,g_combine->vb_stride);

	// if (stencil>=1 && aref_pass)	stencil = light_id
	RCache.set_Element			(s_accum_mask->E[SE_MASK_ALBEDO]);		// masker
	RCache.set_Geometry			(g_combine);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}
