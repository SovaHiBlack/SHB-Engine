#include "stdafx.h"
#include "UIFrameLineWnd.h"

CUIFrameLineWnd::CUIFrameLineWnd( ) : bHorizontal(true), m_bTextureAvailable(false)
{
	AttachChild(&UITitleText);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::Init(f32 x, f32 y, f32 width, f32 height)
{
	inherited::Init(x, y, width, height);
	UITitleText.Init(0, 0, width, 50);
	inherited::Init(x, y, width, height);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::Init(pcstr base_name, f32 x, f32 y, f32 width, f32 height, bool horizontal)
{
	Init(x, y, width, height);
	InitTexture(base_name, horizontal);
	if (horizontal)
	{
		UITitleText.Init(0.0f, 0.0f, width, 50.0f);
	}
	else
	{
		UITitleText.Init(0.0f, 0.0f, 50.0f, height);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::InitTexture(pcstr tex_name, bool horizontal)
{

	fRect rect;
	GetAbsoluteRect(rect);

	bHorizontal = horizontal;

	if (horizontal)
	{
		UIFrameLine.Init(tex_name, rect.left, rect.top, rect.right - rect.left, horizontal, alNone);
		UITitleText.Init(0.0f, 0.0f, rect.right - rect.left, 50.0f);
	}
	else
	{
		UIFrameLine.Init(tex_name, rect.left, rect.top, rect.bottom - rect.top, horizontal, alNone);
		UITitleText.Init(0.0f, 0.0f, 50.0f, rect.bottom - rect.top);
	}

	m_bTextureAvailable = true;
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::Draw( )
{
	if (m_bTextureAvailable)
	{
		fVector2 p;
		GetAbsolutePos(p);
		UIFrameLine.SetPos(p.x, p.y);
		UIFrameLine.Render( );

		inherited::Draw( );
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::SetWidth(f32 width)
{
	inherited::SetWidth(width);
	if (bHorizontal)
	{
		UIFrameLine.SetSize(width);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::SetHeight(f32 height)
{
	inherited::SetHeight(height);
	if (!bHorizontal)
	{
		UIFrameLine.SetSize(height);
	}
}

f32 CUIFrameLineWnd::GetTextureHeight( )
{
	return UIFrameLine.elements[0].GetRect( ).height( );
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::SetOrientation(bool horizontal)
{
	UIFrameLine.SetOrientation(horizontal);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::SetColor(u32 cl)
{
	UIFrameLine.SetColor(cl);
}
