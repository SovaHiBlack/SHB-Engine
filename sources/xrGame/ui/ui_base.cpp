#include "stdafx.h"
#include "ui_base.h"
#include "..\GamePersistent.h"
#include "UICursor.h"
#include "..\HUDManager.h"

CUICursor* GetUICursor( )
{
	return UI( )->GetUICursor( );
}

ui_core* UI( )
{
	return GamePersistent( ).m_pUI_core;
}

extern ENGINE_API fVector2		g_current_font_scale;

void S2DVert::rotate_pt(const fVector2& pivot, f32 cosA, f32 sinA, f32 kx)
{
	fVector2 t = pt;
	t.sub(pivot);
	pt.x = t.x * cosA + t.y * sinA;
	pt.y = t.y * cosA - t.x * sinA;
	pt.x *= kx;
	pt.add(pivot);
}

void C2DFrustum::CreateFromRect(const fRect& rect)
{
	m_rect.set(f32(rect.x1), f32(rect.y1), f32(rect.x2), f32(rect.y2));
	planes.resize(4);
	planes[0].build(rect.lt, fVector2( ).set(-1.0f, 0.0f));
	planes[1].build(rect.lt, fVector2( ).set(0.0f, -1.0f));
	planes[2].build(rect.rb, fVector2( ).set(+1.0f, 0.0f));
	planes[3].build(rect.rb, fVector2( ).set(0.0f, +1.0f));
}

sPoly2D* C2DFrustum::ClipPoly(sPoly2D& S, sPoly2D& D) const
{
	bool bFullTest = false;
	for (u32 j = 0; j < S.size( ); j++)
	{
		if (!m_rect.in(S[j].pt))
		{
			bFullTest = true;
			break;
		}
	}

	sPoly2D* src = &D;
	sPoly2D* dest = &S;
	if (!bFullTest)
	{
		return dest;
	}

	for (u32 i = 0; i < planes.size( ); i++)
	{
		// cache plane and swap lists
		const fPlane2& P = planes[i];
		std::swap(src, dest);
		dest->clear( );

		// classify all points relative to plane #i
		f32 cls[UI_FRUSTUM_SAFE];
		for (u32 j = 0; j < src->size( ); j++) cls[j] = P.classify((*src)[j].pt);

		// clip everything to this plane
		cls[src->size( )] = cls[0];
		src->push_back((*src)[0]);
		fVector2 dir_pt;
		fVector2 dir_uv;
		f32 denum, t;
		for (j = 0; j < src->size( ) - 1; j++)
		{
			if ((*src)[j].pt.similar((*src)[j + 1].pt, EPSILON_7))
			{
				continue;
			}

			if (negative(cls[j]))
			{
				dest->push_back((*src)[j]);
				if (positive(cls[j + 1]))
				{
					// segment intersects plane
					dir_pt.sub((*src)[j + 1].pt, (*src)[j].pt);
					dir_uv.sub((*src)[j + 1].uv, (*src)[j].uv);
					denum = P.n.dotproduct(dir_pt);
					if (denum != 0)
					{
						t = -cls[j] / denum; //VERIFY(t<=1.f && t>=0);
						dest->last( ).pt.mad((*src)[j].pt, dir_pt, t);
						dest->last( ).uv.mad((*src)[j].uv, dir_uv, t);
						dest->inc( );
					}
				}
			}
			else
			{
				// J - outside
				if (negative(cls[j + 1]))
				{
					// J+1  - inside
					// segment intersects plane
					dir_pt.sub((*src)[j + 1].pt, (*src)[j].pt);
					dir_uv.sub((*src)[j + 1].uv, (*src)[j].uv);
					denum = P.n.dotproduct(dir_pt);
					if (denum != 0)
					{
						t = -cls[j] / denum; //VERIFY(t<=1.f && t>=0);
						dest->last( ).pt.mad((*src)[j].pt, dir_pt, t);
						dest->last( ).uv.mad((*src)[j].uv, dir_uv, t);
						dest->inc( );
					}
				}
			}
		}

		// here we end up with complete polygon in 'dest' which is inside plane #i
		if (dest->size( ) < 3)
		{
			return 0;
		}
	}

	return dest;
}

void ui_core::OnDeviceReset( )
{
	m_scale_.set(f32(Device.dwWidth) / UI_BASE_WIDTH, f32(Device.dwHeight) / UI_BASE_HEIGHT);

	m_2DFrustum.CreateFromRect(fRect( ).set(0.0f, 0.0f, f32(Device.dwWidth), f32(Device.dwHeight)));
}

void ui_core::ClientToScreenScaled(fVector2& dest, f32 left, f32 top)
{
	dest.set(ClientToScreenScaledX(left), ClientToScreenScaledY(top));
}

void ui_core::ClientToScreenScaled(fVector2& src_and_dest)
{
	src_and_dest.set(ClientToScreenScaledX(src_and_dest.x), ClientToScreenScaledY(src_and_dest.y));
}

void ui_core::ClientToScreenScaledWidth(f32& src_and_dest)
{
	src_and_dest /= m_current_scale->x;
}

void ui_core::ClientToScreenScaledHeight(f32& src_and_dest)
{
	src_and_dest /= m_current_scale->y;
}

fRect ui_core::ScreenRect( )
{
	static fRect R = {0.0f, 0.0f, UI_BASE_WIDTH, UI_BASE_HEIGHT};
	return R;
}

void ui_core::PushScissor(const fRect& r_tgt, bool overlapped)
{
	fRect r_top = ScreenRect( );
	fRect result = r_tgt;
	if (!m_Scissors.empty( ) && !overlapped)
	{
		r_top = m_Scissors.top( );
	}

	if (!result.intersection(r_top, r_tgt))
	{
		result.set(0.0f, 0.0f, 0.0f, 0.0f);
	}

	if (!(result.x1 >= 0 && result.y1 >= 0 && result.x2 <= UI_BASE_WIDTH && result.y2 <= UI_BASE_HEIGHT))
	{
		Msg("! r_tgt [%.3f][%.3f][%.3f][%.3f]", r_tgt.x1, r_tgt.y1, r_tgt.x2, r_tgt.y2);
		Msg("! result [%.3f][%.3f][%.3f][%.3f]", result.x1, result.y1, result.x2, result.y2);
		VERIFY(result.x1 >= 0 && result.y1 >= 0 && result.x2 <= UI_BASE_WIDTH && result.y2 <= UI_BASE_HEIGHT);
	}

	m_Scissors.push(result);

	result.lt.x = ClientToScreenScaledX(result.lt.x);
	result.lt.y = ClientToScreenScaledY(result.lt.y);
	result.rb.x = ClientToScreenScaledX(result.rb.x);
	result.rb.y = ClientToScreenScaledY(result.rb.y);

	iRect r;
	r.x1 = iFloor(result.x1);
	r.x2 = iFloor(result.x2 + 0.5f);
	r.y1 = iFloor(result.y1);
	r.y2 = iFloor(result.y2 + 0.5f);
	RCache.set_Scissor(&r);
}

void ui_core::PopScissor( )
{
	VERIFY(!m_Scissors.empty( ));
	m_Scissors.pop( );

	if (m_Scissors.empty( ))
	{
		RCache.set_Scissor(NULL);
	}
	else
	{
		const fRect& top = m_Scissors.top( );
		iRect tgt;
		tgt.lt.x = iFloor(ClientToScreenScaledX(top.lt.x));
		tgt.lt.y = iFloor(ClientToScreenScaledY(top.lt.y));
		tgt.rb.x = iFloor(ClientToScreenScaledX(top.rb.x));
		tgt.rb.y = iFloor(ClientToScreenScaledY(top.rb.y));

		RCache.set_Scissor(&tgt);
	}
}

ui_core::ui_core( )
{
	m_pUICursor = xr_new<CUICursor>( );
	m_pFontManager = xr_new<CFontManager>( );

	m_bPostprocess = false;

	OnDeviceReset( );

	m_current_scale = &m_scale_;
	g_current_font_scale.set(1.0f, 1.0f);
}

ui_core::~ui_core( )
{
	xr_delete(m_pFontManager);
	xr_delete(m_pUICursor);
}

void ui_core::pp_start( )
{
	m_bPostprocess = true;

	m_pp_scale_.set(f32(::Render->getTarget( )->get_width( )) / f32(UI_BASE_WIDTH), f32(::Render->getTarget( )->get_height( )) / f32(UI_BASE_HEIGHT));
	m_2DFrustumPP.CreateFromRect(fRect( ).set(0.0f, 0.0f, f32(::Render->getTarget( )->get_width( )), f32(::Render->getTarget( )->get_height( ))));

	m_current_scale = &m_pp_scale_;

	g_current_font_scale.set(f32(::Render->getTarget( )->get_width( )) / f32(Device.dwWidth), f32(::Render->getTarget( )->get_height( )) / f32(Device.dwHeight));
}

void ui_core::pp_stop( )
{
	m_bPostprocess = false;
	m_current_scale = &m_scale_;
	g_current_font_scale.set(1.0f, 1.0f);
}

void ui_core::RenderFont( )
{
	Font( )->Render( );
}

bool ui_core::is_16_9_mode( )
{
	return ((Device.dwWidth) / f32(Device.dwHeight) > (UI_BASE_WIDTH / UI_BASE_HEIGHT + 0.01f));
}

shared_str ui_core::get_xml_name(pcstr fn)
{
	string_path str;
	if (!is_16_9_mode( ))
	{
		sprintf_s(str, "%s", fn);
		if (NULL == strext(fn))
		{
			strcat(str, ".xml");
		}
	}
	else
	{
		string_path str_;
		if (strext(fn))
		{
			strcpy(str, fn);
			*strext(str) = 0;
			strcat(str, "_16.xml");
		}
		else
		{
			sprintf_s(str, "%s_16", fn);
		}

		if (NULL == FS.exist(str_, "$game_config$", "ui\\", str))
		{
			sprintf_s(str, "%s", fn);
			if (NULL == strext(fn))
			{
				strcat(str, ".xml");
			}
		}

		Msg("[16-9] get_xml_name for[%s] returns [%s]", fn, str);
	}

	return str;
}
