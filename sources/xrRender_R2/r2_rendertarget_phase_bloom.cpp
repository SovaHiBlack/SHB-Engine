#include "stdafx.h"

#include "..\XR_3DA\IGamePersistent.h"
#include "..\XR_3DA\Environment.h"

#pragma pack(push,4)
struct v_build	{
	fVector4	p;
	fVector2	uv0;
	fVector2	uv1;
	fVector2	uv2;
	fVector2	uv3;
};

struct v_filter {
	fVector4	p;
	fVector4	uv0;
	fVector4	uv1;
	fVector4	uv2;
	fVector4	uv3;
	fVector4	uv4;
	fVector4	uv5;
	fVector4	uv6;
	fVector4	uv7;
};
#pragma pack(pop)

// Gauss filtering coeffs
// Samples:			0-central, -1, -2,..., -7, 1, 2,... 7
// 
void	CalcGauss_k7(
	fVector4&	w0,					// weight
	fVector4&	w1,					// weight
	f32		r		=3.3f,		// gaussian radius
	f32		s_out	=1.0f		// resulting magnitude
					 )
{
	f32				W[8];

	// calculate
	f32 mag					=	0;
	for (int i=-7; i<=0; i++)	W[-i]	=	expf	(-f32(i*i)/(2*r*r));	// weight
	for (i=0; i<8; i++)	mag		+= i?2*W[i]:W[i];							// symmetrical weight
	for (i=0; i<8; i++)	W[i]	= s_out*W[i]/mag;

	// W[0]=0, W[7]=-7
	w0.set	(W[1],W[2],W[3],W[4]);		// -1, -2, -3, -4
	w1.set	(W[5],W[6],W[7],W[0]);		// -5, -6, -7, 0
}
void	CalcGauss_wave(
	fVector4&	w0,						// weight
	fVector4&	w1,						// weight
	f32		r_base		=3.3f,		// gaussian radius
	f32		r_detail	=1.0f,		// gaussian radius
	f32		s_out		=1.0f		// resulting magnitude
					   )
{
	fVector4	t0;
	fVector4	t1;
	CalcGauss_k7(w0,w1,r_base,  s_out);
	CalcGauss_k7(t0,t1,r_detail,s_out);
	w0.add		(t0);
	w1.add		(t1);
}

void CRenderTarget::phase_bloom	()
{
	u32		Offset;

	// Targets
	u_setrt									( rt_Bloom_1,NULL,NULL,NULL );		// No need for ZBuffer at all
	
	// Clear	- don't clear - it's stupid here :)
	// Stencil	- disable
	// Misc		- draw everything (no culling)
	CHK_DX		(HW.pDevice->SetRenderState	( D3DRS_ZENABLE,		FALSE				));

	// Transfer into Bloom1
	{
		f32		_w				= f32(Device.dwWidth);
		f32		_h				= f32(Device.dwHeight);
		f32		_2w				= _w/2;
		f32 tw = BLOOM_size_X;
		f32		_2h				= _h/2;
		f32 th = BLOOM_size_Y;
		f32		_aspect_w		= _2w/tw;
		f32		_aspect_h		= _2h/th;
		fVector2	one				= { 1.f/_w, 1.f/_h };	one.x*=_aspect_w; one.y*=_aspect_h;
		fVector2	half			= { .5f/_w, .5f/_h };
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
		pv->p.set	(EPSILON_5, f32(th+ EPSILON_5), EPSILON_5,1.f);
		pv->uv0.set	(a_0.x,b_0.y);	pv->uv1.set	(a_1.x,b_1.y);	pv->uv2.set	(a_2.x,b_2.y);	pv->uv3.set	(a_3.x,b_3.y);
		pv++;
		pv->p.set	(EPSILON_5, EPSILON_5, EPSILON_5,1.f);
		pv->uv0.set	(a_0.x,a_0.y);	pv->uv1.set	(a_1.x,a_1.y);	pv->uv2.set	(a_2.x,a_2.y);	pv->uv3.set	(a_3.x,a_3.y);
		pv++;
		pv->p.set	(f32(tw+ EPSILON_5), f32(th+ EPSILON_5), EPSILON_5,1.f);
		pv->uv0.set	(b_0.x,b_0.y);	pv->uv1.set	(b_1.x,b_1.y);	pv->uv2.set	(b_2.x,b_2.y);	pv->uv3.set	(b_3.x,b_3.y);
		pv++;
		pv->p.set	(f32(tw+ EPSILON_5), EPSILON_5, EPSILON_5,1.f);
		pv->uv0.set	(b_0.x,a_0.y);	pv->uv1.set	(b_1.x,a_1.y);	pv->uv2.set	(b_2.x,a_2.y);	pv->uv3.set	(b_3.x,a_3.y);
		pv++;
		RCache.Vertex.Unlock		(4,g_bloom_build->vb_stride);

		// Perform combine (all scalers must account for 4 samples + final diffuse multiply);
		f32 s						= ps_r2_ls_bloom_threshold;											// scale
		f_bloom_factor				= .9f*f_bloom_factor + .1f*ps_r2_ls_bloom_speed*Device.fTimeDelta;	// speed
		RCache.set_Element			(s_bloom->E[0]);
		RCache.set_c				("b_params", s,s,s,	f_bloom_factor);
		RCache.set_Geometry			(g_bloom_build		);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// Capture luminance values
	phase_luminance					( );

	if (ps_r2_ls_flags.test(R2FLAG_FASTBLOOM))
	{
		// FAST FILTER
		f32	_w					= BLOOM_size_X;
		f32	_h					= BLOOM_size_Y;
		f32	ddw					= (1.f/_w)*ps_r2_ls_bloom_kernel_b;
		f32	ddh					= (1.f/_h)*ps_r2_ls_bloom_kernel_b;
		fVector2				p0;
		p0.set					(0.5f / _w, 0.5f / _h);
		fVector2				p1;
		p1.set					((_w+0.5f)/_w, (_h+0.5f)/_h );

		v_build* pv					= (v_build*) RCache.Vertex.Lock	(4,g_bloom_build->vb_stride,Offset);
		pv->p.set(EPSILON_5, f32(_h+ EPSILON_5), EPSILON_5,1.f); pv->uv0.set(p0.x-ddw,p1.y-ddh);pv->uv1.set(p0.x+ddw,p1.y+ddh);pv->uv2.set(p0.x+ddw,p1.y-ddh);pv->uv3.set(p0.x-ddw,p1.y+ddh);pv++;
		pv->p.set(EPSILON_5, EPSILON_5, EPSILON_5,1.f); pv->uv0.set(p0.x-ddw,p0.y-ddh);pv->uv1.set(p0.x+ddw,p0.y+ddh);pv->uv2.set(p0.x+ddw,p0.y-ddh);pv->uv3.set(p0.x-ddw,p0.y+ddh);pv++;
		pv->p.set(f32(_w+ EPSILON_5), f32(_h+ EPSILON_5), EPSILON_5,1.f); pv->uv0.set(p1.x-ddw,p1.y-ddh);pv->uv1.set(p1.x+ddw,p1.y+ddh);pv->uv2.set(p1.x+ddw,p1.y-ddh);pv->uv3.set(p1.x-ddw,p1.y+ddh);pv++;
		pv->p.set(f32(_w+ EPSILON_5), EPSILON_5, EPSILON_5,1.f); pv->uv0.set(p1.x-ddw,p0.y-ddh);pv->uv1.set(p1.x+ddw,p0.y+ddh);pv->uv2.set(p1.x+ddw,p0.y-ddh);pv->uv3.set(p1.x-ddw,p0.y+ddh);pv++;
		RCache.Vertex.Unlock		(4,g_bloom_build->vb_stride);
		RCache.set_Geometry			(g_bloom_build);

		// P0
		u_setrt						(rt_Bloom_2,NULL,NULL,NULL);			// No need for ZBuffer at all
		RCache.set_Element			(s_bloom->E[3]);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);

		// P1
		u_setrt						(rt_Bloom_1,NULL,NULL,NULL);			// No need for ZBuffer at all
		RCache.set_Element			(s_bloom->E[4]);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	} else {
		// SLOW FILTER
		// Transfer into Bloom2, horizontal filter
		{
			f32		_w				= BLOOM_size_X;
			f32		_h				= BLOOM_size_Y;
			fVector2	two				= { 2.0f/_w, 2.0f/_h };
			fVector2	one				= { 1.0f/_w, 1.0f/_h };
			fVector2	half			= { 0.5f/_w, 0.5f/_h };
			fVector4	a_0				= { half.x,					half.y,	half.y,		half.x					};	// center
			fVector4	a_1				= { a_0.x - one.x - half.x, half.y,	half.y,		a_0.w + one.x + half.x	};	// -1,+1i
			fVector4	a_2				= { a_1.x - two.x,			half.y,	half.y,		a_1.w + two.x			};	// -2,+2i
			fVector4	a_3				= { a_2.x - two.x,			half.y,	half.y,		a_2.w + two.x			};	// -3,+3i
			fVector4	a_4				= { a_3.x - two.x,			half.y,	half.y,		a_3.w + two.x			};	// -4,+4i
			fVector4	a_5				= { a_4.x - two.x,			half.y,	half.y,		a_4.w + two.x			};	// -5,+5i
			fVector4	a_6				= { a_5.x - two.x,			half.y,	half.y,		a_5.w + two.x			};	// -6,+6i
			fVector4	a_7				= { a_6.x - two.x,			half.y,	half.y,		a_6.w + two.x			};	// -7,+7i

			// Fill vertex buffer
			v_filter* pv				= (v_filter*) RCache.Vertex.Lock	(4,g_bloom_filter->vb_stride,Offset);

			// 0 - LB
			pv->p.set	(EPSILON_5, f32(_h+ EPSILON_5), EPSILON_5,1.f);
			pv->uv0.set	(a_0.x,1+a_0.y,0,0);	
			pv->uv1.set	(a_1.x,1+a_1.y,1+a_1.z,a_1.w);	
			pv->uv2.set	(a_2.x,1+a_2.y,1+a_2.z,a_2.w);	
			pv->uv3.set	(a_3.x,1+a_3.y,1+a_3.z,a_3.w);
			pv->uv4.set	(a_4.x,1+a_4.y,1+a_4.z,a_4.w);
			pv->uv5.set	(a_5.x,1+a_5.y,1+a_5.z,a_5.w);
			pv->uv6.set	(a_6.x,1+a_6.y,1+a_6.z,a_6.w);
			pv->uv7.set	(a_7.x,1+a_7.y,1+a_7.z,a_7.w);
			pv++;

			// 1 - LT
			pv->p.set	(EPSILON_5, EPSILON_5, EPSILON_5,1.f);
			pv->uv0.set	(a_0.x,a_0.y,0,0);	
			pv->uv1.set	(a_1.x,a_1.y,a_1.z,a_1.w);
			pv->uv2.set	(a_2.x,a_2.y,a_2.z,a_2.w);
			pv->uv3.set	(a_3.x,a_3.y,a_3.z,a_3.w);
			pv->uv4.set	(a_4.x,a_4.y,a_4.z,a_4.w);
			pv->uv5.set	(a_5.x,a_5.y,a_5.z,a_5.w);
			pv->uv6.set	(a_6.x,a_6.y,a_6.z,a_6.w);
			pv->uv7.set	(a_7.x,a_7.y,a_7.z,a_7.w);
			pv++;

			// 2 - RB
			pv->p.set	(f32(_w+ EPSILON_5), f32(_h+ EPSILON_5), EPSILON_5,1.f);
			pv->uv0.set	(1+a_0.x,1+a_0.y,0,0);	
			pv->uv1.set	(1+a_1.x,1+a_1.y,1+a_1.z,1+a_1.w);	
			pv->uv2.set	(1+a_2.x,1+a_2.y,1+a_2.z,1+a_2.w);	
			pv->uv3.set	(1+a_3.x,1+a_3.y,1+a_3.z,1+a_3.w);
			pv->uv4.set	(1+a_4.x,1+a_4.y,1+a_4.z,1+a_4.w);
			pv->uv5.set	(1+a_5.x,1+a_5.y,1+a_5.z,1+a_5.w);
			pv->uv6.set	(1+a_6.x,1+a_6.y,1+a_6.z,1+a_6.w);
			pv->uv7.set	(1+a_7.x,1+a_7.y,1+a_7.z,1+a_7.w);
			pv++;								

			// 3 - RT
			pv->p.set	(f32(_w+ EPSILON_5), EPSILON_5, EPSILON_5,1.f);
			pv->uv0.set	(1+a_0.x,a_0.y,0,0);	
			pv->uv1.set	(1+a_1.x,a_1.y,a_1.z,1+a_1.w);	
			pv->uv2.set	(1+a_2.x,a_2.y,a_2.z,1+a_2.w);	
			pv->uv3.set	(1+a_3.x,a_3.y,a_3.z,1+a_3.w);
			pv->uv4.set	(1+a_4.x,a_4.y,a_4.z,1+a_4.w);
			pv->uv5.set	(1+a_5.x,a_5.y,a_5.z,1+a_5.w);
			pv->uv6.set	(1+a_6.x,a_6.y,a_6.z,1+a_6.w);
			pv->uv7.set	(1+a_7.x,a_7.y,a_7.z,1+a_7.w);
			pv++;							 
			RCache.Vertex.Unlock		(4,g_bloom_filter->vb_stride);

			// Perform filtering
			fVector4	w0;
			fVector4	w1;
			f32		kernel			= ps_r2_ls_bloom_kernel_g;
			CalcGauss_wave				(w0,w1,kernel,kernel/3.f,ps_r2_ls_bloom_kernel_scale);
			u_setrt						(rt_Bloom_2,NULL,NULL,NULL);		// No need for ZBuffer at all
			RCache.set_Element			(s_bloom->E[1]);
			RCache.set_ca				("weight", 0,			w0);
			RCache.set_ca				("weight", 1,			w1);
			RCache.set_Geometry			(g_bloom_filter);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}

		// Transfer into Bloom1, vertical filter
		{
			f32		_w				= BLOOM_size_X;
			f32		_h				= BLOOM_size_Y;
			fVector2	two				= { 2.0f/_w, 2.0f/_h };
			fVector2	one				= { 1.0f/_w, 1.0f/_h };
			fVector2	half			= { 0.5f/_w, 0.5f/_h };
			fVector4	a_0				= { half.x,	half.y,					half.y,						half.x	};	// center
			fVector4	a_1				= { half.x, a_0.y - one.y - half.y,	half.y + one.y + a_0.z,		half.x	};	// -1,+1i
			fVector4	a_2				= { half.x,			a_1.y - two.y,	two.y + a_1.z,				half.x	};	// -2,+2i
			fVector4	a_3				= { half.x,			a_2.y - two.y,	two.y + a_2.z,				half.x	};	// -3,+3i
			fVector4	a_4				= { half.x,			a_3.y - two.y,	two.y + a_3.z,				half.x	};	// -4,+4i
			fVector4	a_5				= { half.x,			a_4.y -	two.y,	two.y + a_4.z,				half.x	};	// -5,+5i
			fVector4	a_6				= { half.x,			a_5.y -	two.y,	two.y + a_5.z,				half.x	};	// -6,+6i
			fVector4	a_7				= { half.x,			a_6.y - two.y,	two.y + a_6.z,				half.x	};	// -7,+7i

			// Fill vertex buffer
			v_filter* pv				= (v_filter*) RCache.Vertex.Lock	(4,g_bloom_filter->vb_stride,Offset);

			// 0 - LB
			pv->p.set	(EPSILON_5, f32(_h+ EPSILON_5), EPSILON_5,1.f);
			pv->uv0.set	(a_0.x,1+a_0.y,0,0);	
			pv->uv1.set	(a_1.x,1+a_1.y,1+a_1.z,a_1.w);	
			pv->uv2.set	(a_2.x,1+a_2.y,1+a_2.z,a_2.w);	
			pv->uv3.set	(a_3.x,1+a_3.y,1+a_3.z,a_3.w);
			pv->uv4.set	(a_4.x,1+a_4.y,1+a_4.z,a_4.w);
			pv->uv5.set	(a_5.x,1+a_5.y,1+a_5.z,a_5.w);
			pv->uv6.set	(a_6.x,1+a_6.y,1+a_6.z,a_6.w);
			pv->uv7.set	(a_7.x,1+a_7.y,1+a_7.z,a_7.w);
			pv++;

			// 1 - LT
			pv->p.set	(EPSILON_5, EPSILON_5, EPSILON_5,1.f);
			pv->uv0.set	(a_0.x,a_0.y,0,0);	
			pv->uv1.set	(a_1.x,a_1.y,a_1.z,a_1.w);
			pv->uv2.set	(a_2.x,a_2.y,a_2.z,a_2.w);
			pv->uv3.set	(a_3.x,a_3.y,a_3.z,a_3.w);
			pv->uv4.set	(a_4.x,a_4.y,a_4.z,a_4.w);
			pv->uv5.set	(a_5.x,a_5.y,a_5.z,a_5.w);
			pv->uv6.set	(a_6.x,a_6.y,a_6.z,a_6.w);
			pv->uv7.set	(a_7.x,a_7.y,a_7.z,a_7.w);
			pv++;

			// 2 - RB
			pv->p.set	(f32(_w+ EPSILON_5), f32(_h+ EPSILON_5), EPSILON_5,1.f);
			pv->uv0.set	(1+a_0.x,1+a_0.y,0,0);	
			pv->uv1.set	(1+a_1.x,1+a_1.y,1+a_1.z,1+a_1.w);	
			pv->uv2.set	(1+a_2.x,1+a_2.y,1+a_2.z,1+a_2.w);	
			pv->uv3.set	(1+a_3.x,1+a_3.y,1+a_3.z,1+a_3.w);
			pv->uv4.set	(1+a_4.x,1+a_4.y,1+a_4.z,1+a_4.w);
			pv->uv5.set	(1+a_5.x,1+a_5.y,1+a_5.z,1+a_5.w);
			pv->uv6.set	(1+a_6.x,1+a_6.y,1+a_6.z,1+a_6.w);
			pv->uv7.set	(1+a_7.x,1+a_7.y,1+a_7.z,1+a_7.w);
			pv++;								

			// 3 - RT
			pv->p.set	(f32(_w+ EPSILON_5), EPSILON_5, EPSILON_5,1.f);
			pv->uv0.set	(1+a_0.x,a_0.y,0,0);	
			pv->uv1.set	(1+a_1.x,a_1.y,a_1.z,1+a_1.w);	
			pv->uv2.set	(1+a_2.x,a_2.y,a_2.z,1+a_2.w);	
			pv->uv3.set	(1+a_3.x,a_3.y,a_3.z,1+a_3.w);
			pv->uv4.set	(1+a_4.x,a_4.y,a_4.z,1+a_4.w);
			pv->uv5.set	(1+a_5.x,a_5.y,a_5.z,1+a_5.w);
			pv->uv6.set	(1+a_6.x,a_6.y,a_6.z,1+a_6.w);
			pv->uv7.set	(1+a_7.x,a_7.y,a_7.z,1+a_7.w);
			pv++;							 
			RCache.Vertex.Unlock		(4,g_bloom_filter->vb_stride);

			// Perform filtering
			fVector4	w0;
			fVector4	w1;
			f32		kernel			= ps_r2_ls_bloom_kernel_g	* f32(Device.dwHeight)/ f32(Device.dwWidth);
			CalcGauss_wave				(w0,w1,kernel,kernel/3.f,ps_r2_ls_bloom_kernel_scale);
			u_setrt						(rt_Bloom_1,NULL,NULL,NULL);				// No need for ZBuffer at all
			RCache.set_Element			(s_bloom->E[2]);
			RCache.set_ca				("weight", 0,			w0);
			RCache.set_ca				("weight", 1,			w1);
			RCache.set_Geometry			(g_bloom_filter);
			RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		}
	}

	// we are left here with bloom-target setup as primary one
	// for FP16-BLEND capable HW we can blend flares into smaller target, because they are smooth
	//if (RImplementation.o.fp16_blend)		g_pGamePersistent->Environment().RenderFlares	();	// lens-flares
	bool	_menu_pp		= g_pGamePersistent?g_pGamePersistent->OnRenderPPUI_query():false;
	if (_menu_pp)			{
		CHK_DX				(HW.pDevice->Clear( 0L, NULL, D3DCLEAR_TARGET,	0,	1.0f, 0L));
	};

	// re-enable z-buffer
	CHK_DX		(HW.pDevice->SetRenderState	( D3DRS_ZENABLE,	TRUE				));
}
