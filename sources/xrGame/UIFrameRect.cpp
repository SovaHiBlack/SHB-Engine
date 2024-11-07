#include "stdafx.h"
#include "uiFrameRect.h"
//#include "hudmanager.h"
#include "ui\uitexturemaster.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIFrameRect::CUIFrameRect( )
{
	uFlags.zero( );
	m_itm_mask.one( );
}
//--------------------------------------------------------------------

void CUIFrameRect::Init(pcstr texture, f32 x, f32 y, f32 w, f32 h)
{
	CUISimpleWindow::Init(x, y, w, h);
	InitTexture(texture);
}

void CUIFrameRect::InitTexture(pcstr texture)
{
	string_path fn;
	string_path buf;
	strcpy(buf, texture);
	if (strext(buf))
	{
		*strext(buf) = 0;
	}

	if (FS.exist(fn, "$game_textures$", buf, ".ini"))
	{
		fVector4 v;
		CInifile* ini = CInifile::Create(fn, TRUE);
		pcstr sh = ini->r_string("frame", "shader");
		frame[fmBK].CreateShader(texture, sh);
		frame[fmL].CreateShader(texture, sh);
		frame[fmR].CreateShader(texture, sh);
		frame[fmT].CreateShader(texture, sh);
		frame[fmB].CreateShader(texture, sh);
		frame[fmLT].CreateShader(texture, sh);
		frame[fmRT].CreateShader(texture, sh);
		frame[fmRB].CreateShader(texture, sh);
		frame[fmLB].CreateShader(texture, sh);
		v = ini->r_fvector4("frame", "back");
		frame[fmBK].SetOriginalRect(v.x, v.y, v.z, v.w);
		frame[fmBK].SetRect(0.0f, 0.0f, v.z, v.w);
		v = ini->r_fvector4("frame", "l");
		frame[fmL].SetOriginalRect(v.x, v.y, v.z, v.w);
		frame[fmL].SetRect(0.0f, 0.0f, v.z, v.w);
		v = ini->r_fvector4("frame", "r");
		frame[fmR].SetOriginalRect(v.x, v.y, v.z, v.w);
		frame[fmR].SetRect(0.0f, 0.0f, v.z, v.w);
		v = ini->r_fvector4("frame", "t");
		frame[fmT].SetOriginalRect(v.x, v.y, v.z, v.w);
		frame[fmT].SetRect(0.0f, 0.0f, v.z, v.w);
		v = ini->r_fvector4("frame", "b");
		frame[fmB].SetOriginalRect(v.x, v.y, v.z, v.w);
		frame[fmB].SetRect(0.0f, 0.0f, v.z, v.w);
		v = ini->r_fvector4("frame", "lt");
		frame[fmLT].SetOriginalRect(v.x, v.y, v.z, v.w);
		frame[fmLT].SetRect(0.0f, 0.0f, v.z, v.w);
		v = ini->r_fvector4("frame", "rt");
		frame[fmRT].SetOriginalRect(v.x, v.y, v.z, v.w);
		frame[fmRT].SetRect(0.0f, 0.0f, v.z, v.w);
		v = ini->r_fvector4("frame", "rb");
		frame[fmRB].SetOriginalRect(v.x, v.y, v.z, v.w);
		frame[fmRB].SetRect(0.0f, 0.0f, v.z, v.w);
		v = ini->r_fvector4("frame", "lb");
		frame[fmLB].SetOriginalRect(v.x, v.y, v.z, v.w);
		frame[fmLB].SetRect(0.0f, 0.0f, v.z, v.w);
		CInifile::Destroy(ini);
	}
	else
	{
		CUITextureMaster::InitTexture(strconcat(sizeof(buf), buf, texture, "_back"), &frame[CUIFrameRect::fmBK]);
		CUITextureMaster::InitTexture(strconcat(sizeof(buf), buf, texture, "_l"), &frame[CUIFrameRect::fmL]);
		CUITextureMaster::InitTexture(strconcat(sizeof(buf), buf, texture, "_r"), &frame[CUIFrameRect::fmR]);
		CUITextureMaster::InitTexture(strconcat(sizeof(buf), buf, texture, "_t"), &frame[CUIFrameRect::fmT]);
		CUITextureMaster::InitTexture(strconcat(sizeof(buf), buf, texture, "_b"), &frame[CUIFrameRect::fmB]);
		CUITextureMaster::InitTexture(strconcat(sizeof(buf), buf, texture, "_lt"), &frame[CUIFrameRect::fmLT]);
		CUITextureMaster::InitTexture(strconcat(sizeof(buf), buf, texture, "_rt"), &frame[CUIFrameRect::fmRT]);
		CUITextureMaster::InitTexture(strconcat(sizeof(buf), buf, texture, "_rb"), &frame[CUIFrameRect::fmRB]);
		CUITextureMaster::InitTexture(strconcat(sizeof(buf), buf, texture, "_lb"), &frame[CUIFrameRect::fmLB]);
	}
}

void CUIFrameRect::UpdateSize( )
{
	VERIFY(g_bRendering);
	// texture size
	fVector2 ts;
	f32 rem_x, rem_y;
	s32 tile_x;
	s32 tile_y;

	fVector2 _bk;
	fVector2 _lt;
	fVector2 _lb;
	fVector2 _rb;
	fVector2 _rt;
	fVector2 _l;
	fVector2 _r;
	fVector2 _t;
	fVector2 _b;

	_bk.set(frame[fmBK].GetOriginalRect( ).width( ), frame[fmBK].GetOriginalRect( ).height( ));
	_lt.set(frame[fmLT].GetOriginalRect( ).width( ), frame[fmLT].GetOriginalRect( ).height( ));
	_lb.set(frame[fmLB].GetOriginalRect( ).width( ), frame[fmLB].GetOriginalRect( ).height( ));
	_rb.set(frame[fmRB].GetOriginalRect( ).width( ), frame[fmRB].GetOriginalRect( ).height( ));
	_rt.set(frame[fmRT].GetOriginalRect( ).width( ), frame[fmRT].GetOriginalRect( ).height( ));
	_l.set(frame[fmL].GetOriginalRect( ).width( ), frame[fmL].GetOriginalRect( ).height( ));
	_r.set(frame[fmR].GetOriginalRect( ).width( ), frame[fmR].GetOriginalRect( ).height( ));
	_t.set(frame[fmT].GetOriginalRect( ).width( ), frame[fmT].GetOriginalRect( ).height( ));
	_b.set(frame[fmB].GetOriginalRect( ).width( ), frame[fmB].GetOriginalRect( ).height( ));

	fVector2 wnd_pos = GetWndPos( );
	frame[fmLT].SetPos(wnd_pos.x, wnd_pos.y);
	frame[fmRT].SetPos(wnd_pos.x + m_wndSize.x - _rt.x, wnd_pos.y);
	frame[fmLB].SetPos(wnd_pos.x, wnd_pos.y + m_wndSize.y - _lb.y);
	frame[fmRB].SetPos(wnd_pos.x + m_wndSize.x - _rb.x, wnd_pos.y + m_wndSize.y - _rb.y);

	f32 size_top = m_wndSize.x - _lt.x - _rt.x;
	f32 size_bottom = m_wndSize.x - _lb.x - _rb.x;
	f32 size_left = m_wndSize.y - _lt.y - _lb.y;
	f32 size_right = m_wndSize.y - _rt.y - _rb.y;

	//Фон
	ts.set(_bk.x, _bk.y);
	rem_x = fmod(size_top, ts.x);
	rem_y = fmod(size_left, ts.y);
	tile_x = iFloor(size_top / ts.x);
	tile_x = _max(tile_x, 0);
	tile_y = iFloor(size_left / ts.y);
	tile_y = _max(tile_y, 0);

	frame[fmBK].SetPos(wnd_pos.x + _lt.x, wnd_pos.y + _lt.y);
	frame[fmBK].SetTile(tile_x, tile_y, rem_x, rem_y);

	//Обрамление
	ts.set(_t.x, _t.y);
	rem_x = fmod(size_top, ts.x);
	tile_x = iFloor(size_top / ts.x);
	tile_x = _max(tile_x, 0);
	frame[fmT].SetPos(wnd_pos.x + _lt.x, wnd_pos.y);
	frame[fmT].SetTile(tile_x, 1, rem_x, 0.0f);

	ts.set(_b.x, _b.y);
	rem_x = fmod(size_bottom, ts.x);
	tile_x = iFloor(size_bottom / ts.x);
	tile_x = _max(tile_x, 0);
	frame[fmB].SetPos(wnd_pos.x + _lb.x, wnd_pos.y + m_wndSize.y - ts.y);
	frame[fmB].SetTile(tile_x, 1, rem_x, 0.0f);

	ts.set(_l.x, _l.y);
	rem_y = fmod(size_left, ts.y);
	tile_y = iFloor(size_left / ts.y);
	tile_y = _max(tile_y, 0);
	frame[fmL].SetPos(wnd_pos.x, wnd_pos.y + _lt.y);
	frame[fmL].SetTile(1, tile_y, 0.0f, rem_y);

	ts.set(_r.x, _r.y);
	rem_y = fmod(size_right, ts.y);
	tile_y = iFloor(size_right / ts.y);
	tile_y = _max(tile_y, 0);
	frame[fmR].SetPos(wnd_pos.x + m_wndSize.x - ts.x, wnd_pos.y + _rt.y);
	frame[fmR].SetTile(1, tile_y, 0.0f, rem_y);

	uFlags.set(flValidSize, TRUE);
}

void CUIFrameRect::Draw( )
{
	if (!uFlags.is(flValidSize))
	{
		UpdateSize( );
	}

	for (s32 k = 0; k < fmMax; ++k)
	{
		if (m_itm_mask.test(u16(1 << k)))
		{
			frame[k].Render( );
		}
	}
}

void CUIFrameRect::Update( )
{ }

void CUIFrameRect::SetWndPos(f32 x, f32 y)
{
	fVector2 _old_pos = GetWndPos( );
	fVector2 _new_pos = fVector2( ).set(x, y);
	if (_old_pos.similar(_new_pos, EPSILON_5, EPSILON_5))
	{
		return;
	}

	CUISimpleWindow::SetWndPos(_new_pos);
	uFlags.set(flValidSize, false);
}

void CUIFrameRect::SetWndSize(const fVector2& size)
{
	CUISimpleWindow::SetWndSize(size);
	uFlags.set(flValidSize, false);
}

void CUIFrameRect::SetWndRect(const fRect& rect)
{
	CUISimpleWindow::SetWndRect(rect);
	uFlags.set(flValidSize, false);
}

void CUIFrameRect::SetWndPos(const fVector2& pos)
{
	CUISimpleWindow::SetWndPos(pos);
	uFlags.set(flValidSize, false);
}

void CUIFrameRect::SetHeight(f32 height)
{
	CUISimpleWindow::SetHeight(height);
	uFlags.set(flValidSize, false);
}

void CUIFrameRect::SetWidth(f32 width)
{
	CUISimpleWindow::SetWidth(width);
	uFlags.set(flValidSize, false);
}

void CUIFrameRect::Draw(f32 x, f32 y)
{
	fVector2 p = GetWndPos( );
	f32 dx = p.x - x;
	f32 dy = p.y - y;
	if (!fis_zero(dx) || !fis_zero(dy))
	{
		SetWndPos(x, y);
	}

	Draw( );
}

void CUIFrameRect::SetTextureColor(u32 cl)
{
	for (s32 i = 0; i < fmMax; ++i)
	{
		frame[i].SetColor(cl);
	}
}
