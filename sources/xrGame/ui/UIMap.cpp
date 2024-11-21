#include "stdafx.h"
#include "../level.h"
#include "../map_location.h"
#include "../map_manager.h"
#include "../map_spot.h"//
#include "UIMap.h"//
#include "UIMapWnd.h"//
#include "../..\XR_3DA\Input.h"		//remove me !!!

const u32			activeLocalMapColor = 0xffffffff;//0xffc80000;
const u32			inactiveLocalMapColor = 0xffffffff;//0xff438cd1;
const u32			ourLevelMapColor = 0xffffffff;

CUICustomMap::CUICustomMap( )
{
	m_BoundRect.set(0, 0, 0, 0);
	SetWindowName("map");
	m_flags.zero( );
	SetPointerDistance(0.0f);
}

CUICustomMap::~CUICustomMap( )
{ }

void CUICustomMap::Update( )
{
	SetPointerDistance(0.0f);
	if (!Locked( ))
	{
		UpdateSpots( );
	}

	CUIStatic::Update( );
}

void CUICustomMap::Init(shared_str name, CIniFile& gameLtx, pcstr sh_name)
{
	m_name = name;
	pcstr tex;
	fVector4 tmp;
	if (gameLtx.line_exist(m_name, "texture"))
	{
		tex = gameLtx.r_string(m_name, "texture");
		tmp = gameLtx.r_fvector4(m_name, "bound_rect");
	}
	else
	{
		tex = "ui\\ui_nomap2";
		tmp.set(-10000.0f, -10000.0f, 10000.0f, 10000.0f);
	}

	m_BoundRect.set(tmp.x, tmp.y, tmp.z, tmp.w);
	CUIStatic::InitEx(tex, sh_name, 0, 0, m_BoundRect.width( ), m_BoundRect.height( ));

	SetStretchTexture(true);
	ClipperOn( );
}

void rotation_(f32 x, f32 y, const f32 angle, f32& x_, f32& y_)
{
	f32 _sc = _cos(angle);
	f32 _sn = _sin(angle);
	x_ = x * _sc + y * _sn;
	y_ = y * _sc - x * _sn;
}

fVector2 CUICustomMap::ConvertLocalToReal(const fVector2& src)
{
	fVector2 res;
	res.x = m_BoundRect.lt.x + src.x / GetCurrentZoom( );
	res.y = m_BoundRect.height( ) + m_BoundRect.lt.y - src.y / GetCurrentZoom( );

	return res;
}

fVector2 CUICustomMap::ConvertRealToLocal(const fVector2& src)// meters->pixels (relatively own left-top pos)
{
	fVector2 res;
	if (!Heading( ))
	{
		return ConvertRealToLocalNoTransform(src);
	}
	else
	{
		fVector2 heading_pivot = GetStaticItem( )->GetHeadingPivot( );

		res = ConvertRealToLocalNoTransform(src);
		res.sub(heading_pivot);
		rotation_(res.x, res.y, GetHeading( ), res.x, res.y);
		res.add(heading_pivot);
		return res;
	}
}

fVector2 CUICustomMap::ConvertRealToLocalNoTransform(const fVector2& src)// meters->pixels (relatively own left-top pos)
{
	fVector2 res;
	res.x = (src.x - m_BoundRect.lt.x) * GetCurrentZoom( );
	res.y = (m_BoundRect.height( ) - (src.y - m_BoundRect.lt.y)) * GetCurrentZoom( );

	return res;
}

//position and heading for drawing pointer to src pos
bool CUICustomMap::GetPointerTo(const fVector2& src, f32 item_radius, fVector2& pos, f32& heading)
{
	fRect		clip_rect_abs = GetClipperRect( ); //absolute rect coords
	fRect		map_rect_abs;
	GetAbsoluteRect(map_rect_abs);

	fRect		rect;
	BOOL res = rect.intersection(clip_rect_abs, map_rect_abs);
	if (!res)
	{
		return false;
	}

	rect = clip_rect_abs;
	rect.sub(map_rect_abs.lt.x, map_rect_abs.lt.y);

	fBox2 f_clip_rect_local;
	f_clip_rect_local.set(rect.x1, rect.y1, rect.x2, rect.y2);

	fVector2 f_center;
	f_clip_rect_local.getcenter(f_center);

	fVector2 f_dir;
	fVector2 f_src;

	f_src.set(src.x, src.y);
	f_dir.sub(f_center, f_src);
	f_dir.normalize_safe( );
	fVector2 f_intersect_point;
	res = f_clip_rect_local.Pick2(f_src, f_dir, f_intersect_point);
	VERIFY(res);

	heading = -f_dir.getH( );

	f_intersect_point.mad(f_intersect_point, f_dir, item_radius);

	pos.set(iFloor(f_intersect_point.x), iFloor(f_intersect_point.y));
	return true;
}

void CUICustomMap::FitToWidth(f32 width)
{
	f32 k = m_BoundRect.width( ) / m_BoundRect.height( );
	f32 w = width;
	f32 h = width / k;
	SetWndRect(0.0f, 0.0f, w, h);
}

void CUICustomMap::FitToHeight(f32 height)
{
	f32 k = m_BoundRect.width( ) / m_BoundRect.height( );
	f32 h = height;
	f32 w = k * height;
	SetWndRect(0.0f, 0.0f, w, h);
}

void CUICustomMap::OptimalFit(const fRect& r)
{
	if ((m_BoundRect.height( ) / r.height( )) < (m_BoundRect.width( ) / r.width( )))
	{
		FitToHeight(r.height( ));
	}
	else
	{
		FitToWidth(r.width( ));
	}
}

// try to positioning clipRect center to vNewPoint
void CUICustomMap::SetActivePoint(const fVector3& vNewPoint)
{
	fVector2 pos;
	pos.set(vNewPoint.x, vNewPoint.z);
	fRect bound = BoundRect( );
	if (FALSE == bound.in(pos))
	{
		return;
	}

	fVector2	pos_on_map = ConvertRealToLocalNoTransform(pos);
	fRect		map_abs_rect;
	GetAbsoluteRect(map_abs_rect);
	fVector2	pos_abs;

	pos_abs.set(map_abs_rect.lt);
	pos_abs.add(pos_on_map);

	fRect		clip_abs_rect = GetClipperRect( );
	fVector2	clip_center;
	clip_abs_rect.getcenter(clip_center);
	clip_center.sub(pos_abs);
	MoveWndDelta(clip_center);
	SetHeadingPivot(pos_on_map);
}

bool CUICustomMap::IsRectVisible(fRect r)
{
	fRect map_visible_rect = GetClipperRect( );
	fVector2 pos;
	GetAbsolutePos(pos);
	r.add(pos.x, pos.y);

	return !!map_visible_rect.intersected(r);
}

bool CUICustomMap::NeedShowPointer(fRect r)
{
	fRect map_visible_rect = GetClipperRect( );
	map_visible_rect.shrink(5, 5);
	fVector2 pos;
	GetAbsolutePos(pos);
	r.add(pos.x, pos.y);

	return !map_visible_rect.intersected(r);
}

void	CUICustomMap::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

bool CUIGlobalMap::OnMouse(f32 x, f32 y, EUIMessages mouse_action)
{
	if (inherited::OnMouse(x, y, mouse_action))
	{
		return true;
	}

	if (mouse_action == WINDOW_MOUSE_MOVE && (FALSE == pInput->iGetAsyncBtnState(0)))
	{
		if (MapWnd( ))
		{
			MapWnd( )->Hint(MapName( ));
			return			true;
		}
	}

	return false;
}

CUIGlobalMap::CUIGlobalMap(CUIMapWnd* pMapWnd)
{
	m_mapWnd = pMapWnd;
	m_minZoom = 1.f;
	Show(false);
}

CUIGlobalMap::~CUIGlobalMap( )
{ }

void CUIGlobalMap::Init(shared_str name, CIniFile& gameLtx, pcstr sh_name)
{
	inherited::Init(name, gameLtx, sh_name);
	SetMaxZoom(gameLtx.r_float(m_name, "max_zoom"));
}

void CUIGlobalMap::Update( )
{
	for (WINDOW_LIST_it it = m_ChildWndList.begin( ); m_ChildWndList.end( ) != it; ++it)
	{
		CUICustomMap* m = smart_cast<CUICustomMap*>(*it);
		if (!m)					continue;
		m->DetachAll( );
	}

	inherited::Update( );
}

void CUIGlobalMap::ClipByVisRect( )
{
	fRect r = GetWndRect( );
	fRect clip = GetClipperRect( );
	if (r.x2 < clip.width( ))	r.x1 += clip.width( ) - r.x2;
	if (r.y2 < clip.height( ))	r.y1 += clip.height( ) - r.y2;
	if (r.x1 > 0.0f)			r.x1 = 0.0f;
	if (r.y1 > 0.0f)			r.y1 = 0.0f;
	SetWndPos(r.x1, r.y1);
}

fVector2 CUIGlobalMap::ConvertRealToLocal(const fVector2& src)// pixels->pixels (relatively own left-top pos)
{
	fVector2 res;
	res.x = (src.x - m_BoundRect.lt.x) * GetCurrentZoom( );
	res.y = (src.y - m_BoundRect.lt.y) * GetCurrentZoom( );
	return res;
}

void CUIGlobalMap::MoveWndDelta(const fVector2& d)
{
	inherited::MoveWndDelta(d);
	ClipByVisRect( );
	m_mapWnd->UpdateScroll( );
}

f32 CUIGlobalMap::CalcOpenRect(const fVector2& center_point, fRect& map_desired_rect, f32 tgt_zoom)
{
	fVector2                    new_center_pt;
	// calculate desired rect in new zoom
	map_desired_rect.set(0.0f, 0.0f, BoundRect( ).width( ) * tgt_zoom, BoundRect( ).height( ) * tgt_zoom);
	// calculate center point in new zoom (center_point is in identity global map space)
	new_center_pt.set(center_point.x * tgt_zoom, center_point.y * tgt_zoom);
	// get vis width & height
	fRect vis_abs_rect = m_mapWnd->ActiveMapRect( );
	f32 vis_w = vis_abs_rect.width( );
	f32 vis_h = vis_abs_rect.height( );
	// calculate center delta from vis rect
	fVector2 delta_pos;
	delta_pos.set(new_center_pt.x - vis_w * 0.5f, new_center_pt.y - vis_h * 0.5f);
	// correct desired rect
	map_desired_rect.sub(delta_pos.x, delta_pos.y);
	// clamp pos by vis rect
	const fRect& r = map_desired_rect;
	fVector2 np = r.lt;
	if (r.x2 < vis_w)	np.x += vis_w - r.x2;
	if (r.y2 < vis_h)	np.y += vis_h - r.y2;
	if (r.x1 > 0.0f)	np.x = 0.0f;
	if (r.y1 > 0.0f)	np.y = 0.0f;
	np.sub(r.lt);
	map_desired_rect.add(np.x, np.y);
	// calculate max way dist
	f32 dist = 0.f;

	fRect s_rect, t_rect;
	s_rect.div(GetWndRect( ), GetCurrentZoom( ), GetCurrentZoom( ));
	t_rect.div(map_desired_rect, tgt_zoom, tgt_zoom);

	fVector2 cpS;
	fVector2 cpT;
	s_rect.getcenter(cpS);
	t_rect.getcenter(cpT);

	dist = cpS.distance_to(cpT);

	return dist;
}

CUILevelMap::CUILevelMap(CUIMapWnd* p)
{
	m_mapWnd = p;
	Show(false);
}

CUILevelMap::~CUILevelMap( )
{
}

void CUILevelMap::Draw( )
{
	if (MapWnd( ))
	{
		for (WINDOW_LIST_it it = m_ChildWndList.begin( ); m_ChildWndList.end( ) != it; ++it)
		{
			CMapSpot* sp = smart_cast<CMapSpot*>((*it));
			if (sp && sp->m_bScale)
			{
				fVector2 sz = sp->m_originSize;
				sz.mul(MapWnd( )->GlobalMap( )->GetCurrentZoom( ));
				sp->SetWndSize(sz);
			}
		}
	}

	inherited::Draw( );
}

void CUILevelMap::Init(shared_str name, CIniFile& gameLtx, pcstr sh_name)
{
	inherited::Init(name, gameLtx, sh_name);
	fVector4 tmp = gameLtx.r_fvector4(MapName( ), "global_rect");
	m_GlobalRect.set(tmp.x, tmp.y, tmp.z, tmp.w);

#ifdef DEBUG
	f32 kw = m_GlobalRect.width( ) / BoundRect( ).width( );
	f32 kh = m_GlobalRect.height( ) / BoundRect( ).height( );

	if (FALSE == fsimilar(kw, kh, EPSILON_3))
	{
		Msg(" --incorrect global rect definition for map [%s]  kw=%f kh=%f", *MapName( ), kw, kh);
		Msg(" --try x2=%f or  y2=%f", m_GlobalRect.x1 + kh * BoundRect( ).width( ), m_GlobalRect.y1 + kw * BoundRect( ).height( ));
	}
#endif
	//	Msg("Succesfully loaded map %s. Zoom=%f",*name, kw);
}

void CUILevelMap::UpdateSpots( )
{
	DetachAll( );
	if (fsimilar(MapWnd( )->GlobalMap( )->GetCurrentZoom( ), MapWnd( )->GlobalMap( )->GetMinZoom( ), EPSILON_3))
	{
		return;
	}

	fRect _r;
	GetAbsoluteRect(_r);
	if (FALSE == MapWnd( )->ActiveMapRect( ).intersected(_r))
	{
		return;
	}

	Locations& ls = Level( ).MapManager( ).Locations( );
	for (Locations_it it = ls.begin( ); it != ls.end( ); ++it)
	{
		if ((*it).location->Update( ))
		{
			(*it).location->UpdateLevelMap(this);
		}
	}
}

fRect CUILevelMap::CalcWndRectOnGlobal( )
{
	fRect res;
	CUIGlobalMap* globalMap = MapWnd( )->GlobalMap( );

	res.lt = globalMap->ConvertRealToLocal(GlobalRect( ).lt);
	res.rb = globalMap->ConvertRealToLocal(GlobalRect( ).rb);
	res.add(globalMap->GetWndPos( ).x, globalMap->GetWndPos( ).y);

	return res;
}

void CUILevelMap::Update( )
{
	CUIGlobalMap* w = MapWnd( )->GlobalMap( );
	fRect			rect;
	fVector2		tmp;

	tmp = w->ConvertRealToLocal(GlobalRect( ).lt);
	rect.lt = tmp;
	tmp = w->ConvertRealToLocal(GlobalRect( ).rb);
	rect.rb = tmp;

	SetWndRect(rect);

	inherited::Update( );

	if (m_bCursorOverWindow)
	{
		VERIFY(m_dwFocusReceiveTime >= 0);
		if (Device.dwTimeGlobal > (m_dwFocusReceiveTime + 500))
		{
			if (fsimilar(MapWnd( )->GlobalMap( )->GetCurrentZoom( ), MapWnd( )->GlobalMap( )->GetMinZoom( ), EPSILON_3))
			{
				MapWnd( )->ShowHint(this, *MapName( ));
			}
			else
			{
				MapWnd( )->HideHint(this);
			}
		}
	}
}

bool CUILevelMap::OnMouse(f32 x, f32 y, EUIMessages mouse_action)
{
	if (inherited::OnMouse(x, y, mouse_action))
	{
		return true;
	}

	if (MapWnd( )->GlobalMap( )->Locked( ))
	{
		return true;
	}

	if (mouse_action == WINDOW_MOUSE_MOVE && (FALSE == pInput->iGetAsyncBtnState(0)))
	{
		if (MapWnd( ))
		{
			MapWnd( )->Hint(MapName( ));
			return			true;
		}
	}

	return false;
}

void CUILevelMap::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	inherited::SendMessage(pWnd, msg, pData);

	if (msg == MAP_SHOW_HINT)
	{
		CMapSpot* sp = smart_cast<CMapSpot*>(pWnd); VERIFY(sp);
		MapWnd( )->ShowHint(pWnd, sp->GetHint( ));
	}
	else if (msg == MAP_HIDE_HINT)
	{
		MapWnd( )->HideHint(pWnd);
	}
}

void CUILevelMap::OnFocusLost( )
{
	inherited::OnFocusLost( );
	MapWnd( )->HideHint(this);
}

CUIMiniMap::CUIMiniMap( )
{ }

CUIMiniMap::~CUIMiniMap( )
{ }

void CUIMiniMap::Init(shared_str name, CIniFile& gameLtx, pcstr sh_name)
{
	inherited::Init(name, gameLtx, sh_name);
	CUIStatic::SetColor(0x7fffffff);
}

void CUIMiniMap::UpdateSpots( )
{
	DetachAll( );
	Locations& ls = Level( ).MapManager( ).Locations( );
	for (Locations_it it = ls.begin( ); it != ls.end( ); ++it)
	{
		(*it).location->UpdateMiniMap(this);
	}
}
