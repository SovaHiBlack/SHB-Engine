#include "stdafx.h"
#include "fstaticrender_rendertarget.h"
#include "..\XR_3DA\IGame_Persistent.h"


static pcstr		RTname			= "$user$rendertarget";
static pcstr		RTname_distort	= "$user$distort";

CRenderTarget::CRenderTarget()
{
	bAvailable			= FALSE;
	RT					= 0;
	pTempZB				= 0;
	ZB					= 0;

	param_blur			= 0.f;
	param_gray			= 0.f;
	param_noise			= 0.f;
	param_duality_h		= 0.f;
	param_duality_v		= 0.f;
	param_noise_fps		= 25.f;
	param_noise_scale	= 1.f;

	im_noise_time		= 0.01f			;1 / 100;
	im_noise_shift_w	= 0;
	im_noise_shift_h	= 0;

	param_color_base	= color_rgba(127,127,127,	0);
	param_color_gray	= color_rgba(85,85,85,		0);
	param_color_add		= color_rgba(0,0,0,			0);

	bAvailable			= Create	();
	Msg					("* SSample: %s",bAvailable?"enabled":"disabled");
}

BOOL CRenderTarget::Create	()
{
	curWidth			= Device.dwWidth;
	curHeight			= Device.dwHeight;

	// Select mode to operate in
	f32	amount		= ps_r__Supersample? f32(ps_r__Supersample):1	;
	f32	scale		= _sqrt	(amount);
	rtWidth				= clampr(iFloor(scale*Device.dwWidth  + .5f), 128, 2048);
	rtHeight			= clampr(iFloor(scale*Device.dwHeight + .5f), 128, 2048);
	while (rtWidth%2)	rtWidth	--;
	while (rtHeight%2)	rtHeight--;
	Msg					("* SSample: %dx%d",rtWidth,rtHeight);

	// Bufferts
	RT.create			(RTname,			rtWidth,rtHeight,HW.Caps.fTarget);
	RT_distort.create	(RTname_distort,	rtWidth,rtHeight,HW.Caps.fTarget);
	if ((rtHeight!=Device.dwHeight) || (rtWidth!=Device.dwWidth))	{
		R_CHK		(HW.pDevice->CreateDepthStencilSurface	(rtWidth,rtHeight,HW.Caps.fDepth,D3DMULTISAMPLE_NONE,0,TRUE,&ZB,NULL));
	} else {
		ZB			= HW.pBaseZB;
		ZB->AddRef	();
	}

	// Temp ZB, used by some of the shadowing code
	R_CHK	(HW.pDevice->CreateDepthStencilSurface	(512,512,HW.Caps.fDepth,D3DMULTISAMPLE_NONE,0,TRUE,&pTempZB,NULL));

	// Shaders and stream
	s_postprocess.create				("postprocess");
	if (RImplementation.o.distortion)	s_postprocess_D.create("postprocess_d");
	g_postprocess.create				(D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX3,RCache.Vertex.Buffer(),RCache.QuadIB);
	return	RT->valid() && RT_distort->valid();
}

CRenderTarget::~CRenderTarget	()
{
	_RELEASE					(pTempZB);
	_RELEASE					(ZB);
	s_postprocess_D.destroy		();
	s_postprocess.destroy		();
	g_postprocess.destroy		();
	RT_distort.destroy			();
	RT.destroy					();
}

void	CRenderTarget::calc_tc_noise		(Fvector2& p0, Fvector2& p1)
{
//.	CTexture*	T					= RCache.get_ActiveTexture	(2);
//.	VERIFY2		(T, "Texture #3 in noise shader should be setted up");
//.	u32			tw					= iCeil(f32(T->get_Width	())*param_noise_scale+EPSILON_7);
//.	u32			th					= iCeil(f32(T->get_Height ())*param_noise_scale+EPSILON_7);
	u32			tw					= iCeil(256*param_noise_scale+ EPSILON_7);
	u32			th					= iCeil(256*param_noise_scale+ EPSILON_7);
	VERIFY2		(tw && th, "Noise scale can't be zero in any way");
//.	if (bDebug)	Msg			("%d,%d,%f",tw,th,param_noise_scale);

	// calculate shift from FPSes
	im_noise_time					-= Device.fTimeDelta;
	if (im_noise_time<0)			{
		im_noise_shift_w			= ::Random.randI(tw?tw:1);
		im_noise_shift_h			= ::Random.randI(th?th:1);
		f32	fps_time			= 1/param_noise_fps;
		while (im_noise_time<0)		im_noise_time += fps_time;
	}

	u32			shift_w				= im_noise_shift_w;
	u32			shift_h				= im_noise_shift_h;
	f32		start_u				= (f32(shift_w)+.5f)/(tw);
	f32		start_v				= (f32(shift_h)+.5f)/(th);
	u32			_w					= Device.dwWidth;
	u32			_h					= Device.dwHeight;
	u32			cnt_w				= _w / tw;
	u32			cnt_h				= _h / th;
	f32		end_u				= start_u + f32(cnt_w) + 1;
	f32		end_v				= start_v + f32(cnt_h) + 1;
 
	p0.set		(start_u,	start_v	);
	p1.set		(end_u,		end_v	);
}

void CRenderTarget::calc_tc_duality_ss	(Fvector2& r0, Fvector2& r1, Fvector2& l0, Fvector2& l1)
{
	// Calculate ordinaty TCs from blur and SS
	f32	tw			= f32(rtWidth);
	f32	th			= f32(rtHeight);
	if (rtHeight!=Device.dwHeight)	param_blur = 1.f;
	Fvector2			shift,p0,p1;
	shift.set			(.5f/tw, .5f/th);
	shift.mul			(param_blur);
	p0.set				(.5f/tw, .5f/th).add			(shift);
	p1.set				((tw+.5f)/tw, (th+.5f)/th ).add	(shift);

	// Calculate Duality TC
	f32	shift_u		= param_duality_h*.5f;
	f32	shift_v		= param_duality_v*.5f;

	r0.set(p0.x,p0.y);					r1.set(p1.x-shift_u,p1.y-shift_v);
	l0.set(p0.x+shift_u,p0.y+shift_v);	l1.set(p1.x,p1.y);
}

BOOL CRenderTarget::NeedPostProcess()
{
	bool	_blur	= (param_blur>0.001f);
	bool	_gray	= (param_gray>0.001f);
	bool	_noise	= (param_noise>0.001f);
	bool	_dual	= (param_duality_h>0.001f)||(param_duality_v>0.001f);

	bool	_menu_pp= g_pGamePersistent?g_pGamePersistent->OnRenderPPUI_query():false;

	bool	_cbase	= false;
	{
		int		_r	= color_get_R(param_color_base)	; _r=_abs(_r-int(0x7f));
		int		_g	= color_get_G(param_color_base)	; _g=_abs(_g-int(0x7f));
		int		_b	= color_get_B(param_color_base)	; _b=_abs(_b-int(0x7f));
		if (_r>2 || _g>2 || _b>2)	_cbase	= true	;
	}
	bool	_cadd	= false;
	{
		int		_r	= color_get_R(param_color_add)	;
		int		_g	= color_get_G(param_color_add)	;
		int		_b	= color_get_B(param_color_add)	;
		if (_r>2 || _g>2 || _b>2)	_cadd	= true	;
	}
	return _blur || _gray || _noise || _dual || _cbase || _cadd || _menu_pp; 
}

BOOL CRenderTarget::Perform		()
{
	return Available() && ( NeedPostProcess() || (ps_r__Supersample>1) || (frame_distort==(Device.dwFrame-1)));
}

#include <dinput.h>
#define SHOW(a)		Log			(#a,a);
#define SHOWX(a)	Msg			("%s %x",#a,a);
void CRenderTarget::Begin		()
{
	/*
	if (g_pGameLevel->IR_GetKeyState(DIK_LSHIFT))	
	{
		Msg					("[%5d]------------------------",Device.dwFrame);
		SHOW				(param_blur)
		SHOW				(param_gray)
		SHOW				(param_duality_h)
		SHOW				(param_duality_v)
		SHOW				(param_noise)
		SHOW				(param_noise_scale)
		SHOW				(param_noise_fps)

		SHOWX				(param_color_base)
		SHOWX				(param_color_gray)
		SHOWX				(param_color_add)
	}
	*/

	if (!Perform())	
	{
		// Base RT
		RCache.set_RT			(HW.pBaseRT);
		RCache.set_ZB			(HW.pBaseZB);
		curWidth				= Device.dwWidth;
		curHeight				= Device.dwHeight;
	} else {
		// Our 
		RCache.set_RT			(RT->pRT);
		RCache.set_ZB			(ZB);
		curWidth				= rtWidth;
		curHeight				= rtHeight;
	}
	Device.Clear				();
}

struct TL_2c3uv {
	Fvector4	p;
	u32			color0;
	u32			color1;
	Fvector2	uv	[3];
	IC void	set	(f32 x, f32 y, u32 c0, u32 c1, f32 u0, f32 v0, f32 u1, f32 v1, f32 u2, f32 v2)	{
		p.set	(x,y, EPSILON_7,1.f);
		color0 = c0; 
		color1 = c1;
		uv[0].set(u0,v0); 
		uv[1].set(u1,v1); 
		uv[2].set(u2,v2); 
	}
};

void CRenderTarget::End		()
{
	if (g_pGamePersistent)	g_pGamePersistent->OnRenderPPUI_main	()			;	// PP-UI

	// find if distortion is needed at all
	BOOL	bPerform	= Perform				()	;
	BOOL	bDistort	= RImplementation.o.distortion;
	bool	_menu_pp	= g_pGamePersistent?g_pGamePersistent->OnRenderPPUI_query():false;
	if ((0==RImplementation.mapDistort.size()) && !_menu_pp) 	bDistort	= FALSE;
	if (bDistort)		phase_distortion		();

	// combination/postprocess
	RCache.set_RT		(HW.pBaseRT);
	RCache.set_ZB		(HW.pBaseZB);
	curWidth			= Device.dwWidth;
	curHeight			= Device.dwHeight;
	
	if (!bPerform)		return;
	RCache.set_Shader	(bDistort ? s_postprocess_D : s_postprocess );

	int		gblend		= clampr		(iFloor((1-param_gray)*255.f),0,255);
	int		nblend		= clampr		(iFloor((1-param_noise)*255.f),0,255);
	u32					p_color			= subst_alpha		(param_color_base,nblend);
	u32					p_gray			= subst_alpha		(param_color_gray,gblend);
	u32					p_brightness	= param_color_add	;
	// Msg				("param_gray:%f(%d),param_noise:%f(%d)",param_gray,gblend,param_noise,nblend);
	// Msg				("base: %d,%d,%d",	color_get_R(p_color),		color_get_G(p_color),		color_get_B(p_color));
	// Msg				("gray: %d,%d,%d",	color_get_R(p_gray),		color_get_G(p_gray),		color_get_B(p_gray));
	// Msg				("add:  %d,%d,%d",	color_get_R(p_brightness),	color_get_G(p_brightness),	color_get_B(p_brightness));
	
	// Draw full-screen quad textured with our scene image
	u32		Offset;
	f32	_w			= f32(Device.dwWidth);
	f32	_h			= f32(Device.dwHeight);
	
	Fvector2			n0,n1,r0,r1,l0,l1;
	calc_tc_duality_ss	(r0,r1,l0,l1);
	calc_tc_noise		(n0,n1);

	// Fill vertex buffer
	f32				du = ps_r1_pps_u;
	f32				dv = ps_r1_pps_v;
	TL_2c3uv* pv		= (TL_2c3uv*) RCache.Vertex.Lock	(4,g_postprocess.stride(),Offset);
	pv->set(du+0,			dv+ f32(_h),	p_color, p_gray, r0.x, r1.y, l0.x, l1.y, n0.x, n1.y);	pv++;
	pv->set(du+0,			dv+0,			p_color, p_gray, r0.x, r0.y, l0.x, l0.y, n0.x, n0.y);	pv++;
	pv->set(du+ f32(_w),	dv+ f32(_h),	p_color, p_gray, r1.x, r1.y, l1.x, l1.y, n1.x, n1.y);	pv++;
	pv->set(du+ f32(_w),	dv+0,			p_color, p_gray, r1.x, r0.y, l1.x, l0.y, n1.x, n0.y);	pv++;
	RCache.Vertex.Unlock									(4,g_postprocess.stride());

	// Actual rendering
	static	shared_str	s_brightness	= "c_brightness";
	RCache.set_c		(s_brightness,color_get_R(p_brightness)/255.f,color_get_G(p_brightness)/255.f,color_get_B(p_brightness)/255.f,0);
	RCache.set_Geometry	(g_postprocess);
	RCache.Render		(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}

void	CRenderTarget::phase_distortion	()
{
	frame_distort								= Device.dwFrame;
	RCache.set_RT								(RT_distort->pRT);
	RCache.set_ZB								(ZB);
	RCache.set_CullMode							(CULL_CCW);
	RCache.set_ColorWriteEnable					( );
	CHK_DX(HW.pDevice->Clear					( 0L, NULL, D3DCLEAR_TARGET, color_rgba(127,127,127,127), 1.0f, 0L));
	
	if(g_pGameLevel && g_pGamePersistent && !g_pGamePersistent->OnRenderPPUI_query() )
		RImplementation.r_dsgraph_render_distort	( );
	else
		RImplementation.mapDistort.clear();

	if (g_pGamePersistent)	g_pGamePersistent->OnRenderPPUI_PP()	;	// PP-UI
}
