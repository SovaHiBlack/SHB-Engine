#include "stdafx.h"
#include "UIProgressShape.h"

#include "UIStatic.h"

CUIProgressShape::CUIProgressShape( )
{
	m_pBackground = NULL;
	m_bText = false;
	m_pTexture = xr_new<CUIStatic>( );
	AttachChild(m_pTexture);
	m_pBackground = xr_new<CUIStatic>( );
	AttachChild(m_pBackground);
}

CUIProgressShape::~CUIProgressShape( )
{
	xr_delete(m_pTexture);
	xr_delete(m_pBackground);
}

void CUIProgressShape::SetPos(f32 pos)
{
	m_stage = pos;
}

void CUIProgressShape::SetPos(s32 pos, s32 max)
{
	m_stage = f32(pos) / f32(max);
	if (m_bText)
	{
		string256 _buff;
		m_pTexture->SetText(itoa(pos, _buff, 10));
	}
}

void CUIProgressShape::SetTextVisible(bool b)
{
	m_bText = b;
}

void _make_rot(fVector2& pt, const fVector2& src, f32 sin_a, f32 cos_a, f32 angle)
{
	pt.x = src.x * cos_a + src.y * sin_a;
	pt.y = src.y * cos_a - src.x * sin_a;
}

f32 calc_color(u32 idx, u32 total, f32 stage, f32 max_stage)
{
	f32 kk = (stage / max_stage) * (f32(total + 1));
	f32 f = 1 / (exp((f32(idx) - kk) * 0.9f) + 1.0f);

	return f;
}

void CUIProgressShape::Draw( )
{
	if (m_pBackground)
	{
		m_pBackground->Draw( );
	}

	R_ASSERT(m_pTexture);

	if (m_bText)
	{
		m_pTexture->DrawText( );
	}

	ref_shader sh = m_pTexture->GetShader( );
	ref_geom gm = GetUIGeom( );
	RCache.set_Shader(sh);
	CTexture* T = RCache.get_ActiveTexture(0);
	fVector2 tsize;
	tsize.set(f32(T->get_Width( )), f32(T->get_Height( )));

	u32	offset;
	FVF::TL* pv = (FVF::TL*) RCache.Vertex.Lock(m_sectorCount * 3, gm.stride( ), offset);

	fRect pos_rect;
	m_pTexture->GetAbsoluteRect(pos_rect);
	UI( )->ClientToScreenScaled(pos_rect.lt, pos_rect.x1, pos_rect.y1);
	UI( )->ClientToScreenScaled(pos_rect.rb, pos_rect.x2, pos_rect.y2);

	fVector2 center_pos;
	pos_rect.getcenter(center_pos);

	fRect tex_rect = m_pTexture->GetUIStaticItem( ).GetOriginalRect( );

	tex_rect.lt.x /= tsize.x;
	tex_rect.lt.y /= tsize.y;
	tex_rect.rb.x /= tsize.x;
	tex_rect.rb.y /= tsize.y;

	fVector2 center_tex;
	tex_rect.getcenter(center_tex);

	f32 radius_pos = pos_rect.width( ) / 2.0f;
	f32 radius_tex = tex_rect.width( ) / 2.0f;

	f32 curr_angle = 0.0f;
	f32 sin_a = _sin(curr_angle);
	f32 cos_a = _cos(curr_angle);
	fVector2	start_pos_pt;
	fVector2	prev_pos_pt;
	fVector2	start_tex_pt;
	fVector2	prev_tex_pt;

	start_pos_pt.set(0.0f, -radius_pos);
	prev_pos_pt = start_pos_pt;

	start_tex_pt.set(0.0f, -radius_tex);
	prev_tex_pt = start_tex_pt;

	for (u32 i = 0; i < m_sectorCount; ++i)
	{
		f32 ffff = calc_color(i + 1, m_sectorCount, m_stage, 1.0f);
		u32 color = color_argb_f(ffff, 1.0f, 1.0f, 1.0f);

		pv->set(center_pos.x, center_pos.y, color, center_tex.x, center_tex.y); ++pv;

		fVector2	tp;
		tp.set(prev_pos_pt);
		tp.add(center_pos);

		fVector2	tx;
		tx.set(prev_tex_pt);
		tx.add(center_tex);

		pv->set(tp.x, tp.y, color, tx.x, tx.y); ++pv;

		if (m_bClockwise)
		{
			curr_angle -= PI_MUL_2 / f32(m_sectorCount);
		}
		else
		{
			curr_angle += PI_MUL_2 / f32(m_sectorCount);
		}

		sin_a = _sin(curr_angle);
		cos_a = _cos(curr_angle);

		_make_rot(prev_pos_pt, start_pos_pt, sin_a, cos_a, curr_angle);
		_make_rot(prev_tex_pt, start_tex_pt, sin_a, cos_a, curr_angle);

		tp.set(prev_pos_pt);
		tp.add(center_pos);

		tx.set(prev_tex_pt);
		tx.add(center_tex);

		pv->set(tp.x, tp.y, color, tx.x, tx.y); ++pv;

		if (!m_bClockwise)
		{
			std::swap(*(pv - 1), *(pv - 2));
		}
	}

	RCache.Vertex.Unlock(m_sectorCount * 3, gm.stride( ));
	RCache.set_Geometry(gm);

	RCache.Render(D3DPT_TRIANGLELIST, offset, m_sectorCount);
}
