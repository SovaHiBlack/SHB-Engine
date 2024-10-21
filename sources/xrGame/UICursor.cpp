#include "stdafx.h"
#include "uicursor.h"

#include "..\XR_3DA\CustomHUD.h"
#include "UI.h"
#include "HUDManager.h"
#include "ui/UIStatic.h"

#define C_DEFAULT	D3DCOLOR_XRGB(0xff,0xff,0xff)

CUICursor::CUICursor( )
{
	bVisible = false;
	vPos.set(0.0f, 0.0f);
	InitInternal( );
	Device.seqRender.Add(this, 2);
}

CUICursor::~CUICursor( )
{
	xr_delete(m_static);
	Device.seqRender.Remove(this);
}

void CUICursor::InitInternal( )
{
	m_static = xr_new<CUIStatic>( );
	m_static->InitTextureEx("ui\\ui_ani_cursor", "hud\\cursor");
	fRect						rect;
	rect.set(0.0f, 0.0f, 40.0f, 40.0f);
	m_static->SetOriginalRect(rect);
	fVector2					sz;
	sz.set(rect.rb);
	if (UI( )->is_16_9_mode( ))
	{
		sz.x /= 1.2f;
	}

	m_static->SetWndSize(sz);
	m_static->SetStretchTexture(true);
}

u32 last_render_frame = 0;
void CUICursor::OnRender( )
{
	if (!IsVisible( ))
	{
		return;
	}

#ifdef DEBUG
	VERIFY(last_render_frame != Device.dwFrame);
	last_render_frame = Device.dwFrame;

	if (bDebug)
	{
		CGameFont* F = UI( )->Font( )->pFontDI;
		F->SetAligment(CGameFont::alCenter);
		F->SetHeightI(0.02f);
		F->OutSetI(0.0f, -0.9f);
		F->SetColor(0xffffffff);
		fVector2			pt = GetCursorPosition( );
		F->OutNext("%f-%f", pt.x, pt.y);
	}
#endif

	m_static->SetWndPos(vPos);
	m_static->Update( );
	m_static->Draw( );
}

fVector2 CUICursor::GetCursorPosition( )
{
	return  vPos;
}

fVector2 CUICursor::GetCursorPositionDelta( )
{
	fVector2 res_delta;
	res_delta.x = vPos.x - vPrevPos.x;
	res_delta.y = vPos.y - vPrevPos.y;
	return res_delta;
}

void CUICursor::UpdateCursorPosition( )
{
	POINT		p;
	BOOL r = GetCursorPos(&p);
	R_ASSERT(r);

	vPrevPos = vPos;

	vPos.x = (f32)p.x * (UI_BASE_WIDTH / (f32)Device.dwWidth);
	vPos.y = (f32)p.y * (UI_BASE_HEIGHT / (f32)Device.dwHeight);
	clamp(vPos.x, 0.0f, UI_BASE_WIDTH);
	clamp(vPos.y, 0.0f, UI_BASE_HEIGHT);
}

void CUICursor::SetUICursorPosition(fVector2 pos)
{
	vPos = pos;
	POINT		p;
	p.x = iFloor(vPos.x / (UI_BASE_WIDTH / (f32)Device.dwWidth));
	p.y = iFloor(vPos.y / (UI_BASE_HEIGHT / (f32)Device.dwHeight));

	SetCursorPos(p.x, p.y);
}
