#include "stdafx.h"

#include "UILabel.h"
#include "../..\XR_3DA\LightAnimLibrary.h"

CUILabel::CUILabel( )
{
	m_textPos.set(0, 0);
	m_lines.SetVTextAlignment(valCenter);
	m_lainm_start_time = -1.0f;
	m_lanim = NULL;
}

void CUILabel::Init(f32 x, f32 y, f32 width, f32 height)
{
	CUIFrameLineWnd::Init(x, y, width, height);
	m_lines.Init(0, 0, width, height);
}

void CUILabel::SetFont(CGameFont* pFont)
{
	CUILinesOwner::SetFont(pFont);
}

void CUILabel::Draw( )
{
	CUIFrameLineWnd::Draw( );
	fVector2 p;
	GetAbsolutePos(p);
	m_lines.Draw(p.x + m_textPos.x, p.y + m_textPos.y);
}

void CUILabel::SetWidth(f32 width)
{
	m_lines.SetWidth(width);
	CUIFrameLineWnd::SetWidth(width);
}

void CUILabel::SetHeight(f32 height)
{
	m_lines.SetHeight(height);
	CUIFrameLineWnd::SetHeight(height);
}

void CUILabel::SetLightAnim(pcstr lanim)
{
	if (lanim && xr_strlen(lanim))
	{
		m_lanim = LALib.FindItem(lanim);
	}
	else
	{
		m_lanim = NULL;
	}
}

void CUILabel::Update( )
{
	CUIFrameLineWnd::Update( );
	if (m_lanim)
	{
		if (m_lainm_start_time < 0.0f)
		{
			m_lainm_start_time = Device.fTimeGlobal;
		}

		s32 frame;
		u32 clr = m_lanim->CalculateRGB(Device.fTimeGlobal - m_lainm_start_time, frame);
		SetColor(clr);
		SetTextColor(clr);
	}
}
