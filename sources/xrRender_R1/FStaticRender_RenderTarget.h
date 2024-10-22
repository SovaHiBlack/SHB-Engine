#pragma once


class CRenderTarget		: public IRender_Target
{
private:
	BOOL				bAvailable;
	u32					rtWidth;
	u32					rtHeight;

	u32					curWidth;
	u32					curHeight;

	ref_rt				RT;
	ref_rt				RT_distort;
	IDirect3DSurface9*	ZB;

	ref_shader			s_postprocess;
	ref_shader			s_postprocess_D;
	ref_geom			g_postprocess;
	
	f32				im_noise_time;
	u32					im_noise_shift_w;
	u32					im_noise_shift_h;
	
	f32				param_blur;
	f32				param_gray;
	f32				param_duality_h;
	f32				param_duality_v;
	f32				param_noise;
	f32				param_noise_scale;
	f32				param_noise_fps;

	u32					param_color_base;
	u32					param_color_gray;
	u32					param_color_add;

	u32					frame_distort;

public:
	IDirect3DSurface9*	pTempZB;

private:
	BOOL				Create				()	;
	BOOL				NeedPostProcess		()	;
	BOOL				Available			()	{ return bAvailable; }
	BOOL				Perform				()	;

	void				calc_tc_noise		(fVector2& p0, fVector2& p1);
	void				calc_tc_duality_ss	(fVector2& r0, fVector2& r1, fVector2& l0, fVector2& l1);
	void				phase_distortion	();

public:
	CRenderTarget		();
	~CRenderTarget		();

	void				Begin				();
	void				End					();

	virtual void		set_blur			(f32	f)		{ param_blur=f;												}
	virtual void		set_gray			(f32	f)		{ param_gray=f;												}
	virtual void		set_duality_h		(f32	f)		{ param_duality_h=_abs(f);									}
	virtual void		set_duality_v		(f32	f)		{ param_duality_v=_abs(f);									}
	virtual void		set_noise			(f32	f)		{ param_noise=f;											}
	virtual void		set_noise_scale		(f32	f)		{ param_noise_scale=f;										}
	virtual void		set_noise_fps		(f32	f)		{ param_noise_fps=_abs(f)+ EPSILON_7;							}

	virtual void		set_color_base		(u32	f)		{ param_color_base=f;										}
	virtual void		set_color_gray		(u32	f)		{ param_color_gray=f;										}
	virtual void		set_color_add		(u32	f)		{ param_color_add=f;										}

	virtual u32			get_width			()				{ return curWidth;											}
	virtual u32			get_height			()				{ return curHeight;											}
};
