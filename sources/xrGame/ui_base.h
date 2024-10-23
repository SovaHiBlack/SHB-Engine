#pragma once

#define UI_BASE_WIDTH	1024.0f
#define UI_BASE_HEIGHT	768.0f

struct CFontManager;
class CUICursor;


class CDeviceResetNotifier :public pureDeviceReset
{
public:
						CDeviceResetNotifier					()	{Device.seqDeviceReset.Add(this,REG_PRIORITY_NORMAL);};
	virtual				~CDeviceResetNotifier					()	{Device.seqDeviceReset.Remove(this);};
	virtual void		OnDeviceReset							()	{};

};



//---------------------------------------------------------------------------------------
// 2D Frustum & 2D Vertex
//---------------------------------------------------------------------------------------
struct S2DVert{
	fVector2	pt;
	fVector2	uv;
				S2DVert		(){}
				S2DVert		(f32 pX, f32 pY, f32 tU, f32 tV){pt.set(pX,pY);uv.set(tU,tV);}
	void		set			(f32 pt_x, f32 pt_y, f32 uv_x, f32 uv_y){pt.set(pt_x,pt_y);uv.set(uv_x,uv_y);}
	void		set			(const fVector2& _pt, const fVector2& _uv){pt.set(_pt);uv.set(_uv);}
	void		rotate_pt	(const fVector2& pivot, f32 cosA, f32 sinA, f32 kx);
};
#define UI_FRUSTUM_MAXPLANES	12
#define UI_FRUSTUM_SAFE			(UI_FRUSTUM_MAXPLANES*4)
typedef svector<S2DVert,UI_FRUSTUM_SAFE>		sPoly2D;

class C2DFrustum{//only rect form
	svector<fPlane2, FRUSTUM_MAXPLANES> planes;
	fRect						m_rect;
public:
	void		CreateFromRect	(const fRect& rect);
	sPoly2D*	ClipPoly		(sPoly2D& S, sPoly2D& D) const;
};

class ui_core: public CDeviceResetNotifier
{
	C2DFrustum		m_2DFrustum;
	C2DFrustum		m_2DFrustumPP;
	bool			m_bPostprocess;

	CFontManager*	m_pFontManager;
	CUICursor*		m_pUICursor;

	fVector2		m_pp_scale_;
	fVector2		m_scale_;
	fVector2*		m_current_scale;

	IC f32		ClientToScreenScaledX			(f32 left)				{return left * m_current_scale->x;};
	IC f32		ClientToScreenScaledY			(f32 top)					{return top * m_current_scale->y;};

public:
	xr_stack<fRect> m_Scissors;
	
					ui_core							();
					~ui_core						();
	CFontManager*	Font							()							{return m_pFontManager;}
	CUICursor*		GetUICursor						()							{return m_pUICursor;}

	void			ClientToScreenScaled			(fVector2& dest, f32 left, f32 top);
	void			ClientToScreenScaled			(fVector2& src_and_dest);
	void			ClientToScreenScaledWidth		(f32& src_and_dest);
	void			ClientToScreenScaledHeight		(f32& src_and_dest);

	fRect			ScreenRect						();
	const C2DFrustum& ScreenFrustum					(){return (m_bPostprocess)?m_2DFrustumPP:m_2DFrustum;}
	void			PushScissor						(const fRect& r, bool overlapped=false);
	void			PopScissor						();

	void			pp_start						();
	void			pp_stop							();
	void			RenderFont						();

	virtual void	OnDeviceReset					();
	static	bool	is_16_9_mode					();
	shared_str		get_xml_name					(pcstr fn);
};

extern CUICursor*	GetUICursor						();
extern ui_core*		UI								();
