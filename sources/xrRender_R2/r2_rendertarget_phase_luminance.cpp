#include "stdafx.h"

#pragma pack(push,4)
struct v_build	{
	Fvector4	p;
	fVector2	uv0;
	fVector2	uv1;
	fVector2	uv2;
	fVector2	uv3;
};

struct v_filter {
	Fvector4	p;
	Fvector4	uv[8];
};
#pragma pack(pop)

void	CRenderTarget::phase_luminance()
{
	u32		Offset	= 0;
	f32	eps		= EPSILON_7;

	// Targets
	RCache.set_Stencil						(FALSE);
	RCache.set_CullMode						(CULL_NONE);
	RCache.set_ColorWriteEnable				();
	CHK_DX									(HW.pDevice->SetRenderState	(D3DRS_ZENABLE,FALSE));

	// 000: Perform LUM-SAT, pass 0, 256x256 => 64x64
	u_setrt									(rt_LUM_64,NULL,NULL,NULL);
	{
		f32		ts				= 64;
		f32		_w				= f32(BLOOM_size_X);
		f32		_h				= f32(BLOOM_size_Y);
		fVector2	one				= { 2.f/_w, 2.f/_h };	// two, infact
		fVector2	half			= { 1.f/_w, 1.f/_h };	// one, infact
		fVector2	a_0				= { half.x + 0,		half.y + 0		};
		fVector2	a_1				= { half.x + one.x, half.y + 0		};
		fVector2	a_2				= { half.x + 0,		half.y + one.y	};
		fVector2	a_3				= { half.x + one.x,	half.y + one.y	};
		fVector2	b_0				= { 1 + a_0.x,		1 + a_0.y		};
		fVector2	b_1				= { 1 + a_1.x,		1 + a_1.y		};
		fVector2	b_2				= { 1 + a_2.x,		1 + a_2.y		};
		fVector2	b_3				= { 1 + a_3.x,		1 + a_3.y		};

		// Fill vertex buffer
		v_build* pv					= (v_build*) RCache.Vertex.Lock	(4,g_bloom_build->vb_stride,Offset);
		pv->p.set	(eps, f32(ts+eps),	eps,1.f);	pv->uv0.set	(a_0.x,b_0.y);	pv->uv1.set	(a_1.x,b_1.y);	pv->uv2.set	(a_2.x,b_2.y);	pv->uv3.set	(a_3.x,b_3.y);	pv++;
		pv->p.set	(eps,			eps,			eps,1.f);	pv->uv0.set	(a_0.x,a_0.y);	pv->uv1.set	(a_1.x,a_1.y);	pv->uv2.set	(a_2.x,a_2.y);	pv->uv3.set	(a_3.x,a_3.y);	pv++;
		pv->p.set	(f32(ts+eps), f32(ts+eps),	eps,1.f);	pv->uv0.set	(b_0.x,b_0.y);	pv->uv1.set	(b_1.x,b_1.y);	pv->uv2.set	(b_2.x,b_2.y);	pv->uv3.set	(b_3.x,b_3.y);	pv++;
		pv->p.set	(f32(ts+eps), eps,			eps,1.f);	pv->uv0.set	(b_0.x,a_0.y);	pv->uv1.set	(b_1.x,a_1.y);	pv->uv2.set	(b_2.x,a_2.y);	pv->uv3.set	(b_3.x,a_3.y);	pv++;
		RCache.Vertex.Unlock		(4,g_bloom_build->vb_stride);
		RCache.set_Element			(s_luminance->E[0]);
		RCache.set_Geometry			(g_bloom_build		);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// 111: Perform LUM-SAT, pass 1, 64x64 => 8x8
	u_setrt									(rt_LUM_8,NULL,NULL,NULL);
	{
		// Build filter-kernel
		f32		_ts				= 8;
		f32		_src			= f32(64);
		fVector2	a[16];
		fVector2	b[16];
		for (int k=0; k<16; k++)	{
			int	_x	= (k*2+1)%8;	// 1,3,5,7
			int	_y	= ((k/4)*2+1);	// 1,1,1,1 ~ 3,3,3,3 ~...etc...
			a[k].set(_x,_y).div		(_src);
			b[k].set(a[k]).add		(1);
		}

		// Fill vertex buffer
		v_filter* pv				= (v_filter*) RCache.Vertex.Lock	(4,g_bloom_filter->vb_stride,Offset);
		pv->p.set	(eps, f32(_ts+eps),	eps,1.f);	for (int t=0; t<8; t++)	pv->uv[t].set(a[t].x,b[t].y, b[t+8].y,a[t+8].x);	// xy/yx	- left+down
		pv++;
		pv->p.set	(eps,			eps,			eps,1.f);	for (int t=0; t<8; t++)	pv->uv[t].set(a[t].x,a[t].y, a[t+8].y,a[t+8].x);	// xy/yx	- left+up
		pv++;
		pv->p.set	(f32(_ts+eps), f32(_ts+eps),	eps,1.f);	for (int t=0; t<8; t++)	pv->uv[t].set(b[t].x,b[t].y, b[t+8].y,b[t+8].x);	// xy/yx	- right+down
		pv++;
		pv->p.set	(f32(_ts+eps),eps,			eps,1.f);	for (int t=0; t<8; t++)	pv->uv[t].set(b[t].x,a[t].y, a[t+8].y,b[t+8].x);	// xy/yx	- right+up
		pv++;
		RCache.Vertex.Unlock		(4,g_bloom_filter->vb_stride);
		RCache.set_Element			(s_luminance->E[1]	);
		RCache.set_Geometry			(g_bloom_filter		);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// 222: Perform LUM-SAT, pass 2, 8x8 => 1x1
	u32			gpu_id	= Device.dwFrame%2;
	u_setrt		(rt_LUM_pool[gpu_id*2+1],NULL,NULL,NULL);
	{
		// Build filter-kernel
		f32		_ts				= 1;
		f32		_src			= f32(8);
		fVector2	a[16];
		fVector2	b[16];
		for (int k=0; k<16; k++)	{
			int	_x	= (k*2+1)%8;	// 1,3,5,7
			int	_y	= ((k/4)*2+1);	// 1,1,1,1 ~ 3,3,3,3 ~...etc...
			a[k].set(_x,_y).div		(_src);
			b[k].set(a[k]).add		(1);
		}

		// Fill vertex buffer
		v_filter* pv				= (v_filter*) RCache.Vertex.Lock	(4,g_bloom_filter->vb_stride,Offset);
		pv->p.set	(eps, f32(_ts+eps),	eps,1.f);	for (int t=0; t<8; t++)	pv->uv[t].set(a[t].x,b[t].y, b[t+8].y,a[t+8].x);	// xy/yx	- left+down
		pv++;
		pv->p.set	(eps,			eps,			eps,1.f);	for (int t=0; t<8; t++)	pv->uv[t].set(a[t].x,a[t].y, a[t+8].y,a[t+8].x);	// xy/yx	- left+up
		pv++;
		pv->p.set	(f32(_ts+eps), f32(_ts+eps),	eps,1.f);	for (int t=0; t<8; t++)	pv->uv[t].set(b[t].x,b[t].y, b[t+8].y,b[t+8].x);	// xy/yx	- right+down
		pv++;
		pv->p.set	(f32(_ts+eps),eps,			eps,1.f);	for (int t=0; t<8; t++)	pv->uv[t].set(b[t].x,a[t].y, a[t+8].y,b[t+8].x);	// xy/yx	- right+up
		pv++;
		RCache.Vertex.Unlock		(4,g_bloom_filter->vb_stride);

		f_luminance_adapt			= .9f*f_luminance_adapt + .1f*Device.fTimeDelta*ps_r2_tonemap_adaptation;
		f32		amount			= ps_r2_ls_flags.test(R2FLAG_TONEMAP)?ps_r2_tonemap_amount:0;
		Fvector3	_none, _full, _result;
				_none.set			(1,							0,		1						);
				_full.set			(ps_r2_tonemap_middlegray,	1.f,	ps_r2_tonemap_low_lum	);
				_result.lerp		(_none, _full, amount	);

		RCache.set_Element			(s_luminance->E[2]		);
		RCache.set_Geometry			(g_bloom_filter			);
		RCache.set_c("MiddleGray",	_result.x,_result.y,_result.z,f_luminance_adapt	);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// Cleanup states
	CHK_DX		(HW.pDevice->SetRenderState(D3DRS_ZENABLE,TRUE));
}
