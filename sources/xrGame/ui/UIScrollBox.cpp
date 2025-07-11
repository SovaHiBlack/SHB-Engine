#include "stdafx.h"
#include "uiscrollbox.h"
#include "UICursor.h"

CUIScrollBox::CUIScrollBox( )
{
	m_bAvailableTexture = true;
	m_bIsHorizontal = true;
}

void CUIScrollBox::SetHorizontal( )
{
	m_bIsHorizontal = true;
}

void CUIScrollBox::SetVertical( )
{
	m_bIsHorizontal = false;
}

bool CUIScrollBox::OnMouse(f32 x, f32 y, EUIMessages mouse_action)
{
	bool cursor_over;

	if (x >= -10.0f && x < GetWidth( ) + 10.0f && y >= -10.0f && y < GetHeight( ) + 10.0f)
	{
		cursor_over = true;
	}
	else
	{
		cursor_over = false;
	}

	bool im_capturer = (GetParent( )->GetMouseCapturer( ) == this);

	if (mouse_action == WINDOW_LBUTTON_DOWN)
	{
		GetParent( )->SetCapture(this, true);
	}
	else if (mouse_action == WINDOW_LBUTTON_UP)
	{
		GetParent( )->SetCapture(this, false);
	}
	else if (im_capturer && mouse_action == WINDOW_MOUSE_MOVE && cursor_over)
	{
		fVector2 pos = GetWndPos( );
		fVector2 delta = GetUICursor( )->GetCursorPositionDelta( );

		if (m_bIsHorizontal)
		{
			pos.x += delta.x;
		}
		else
		{
			pos.y += delta.y;
		}

		SetWndPos(pos);

		GetMessageTarget( )->SendMessage(this, SCROLLBOX_MOVE);
	}

	return true;
}

void CUIScrollBox::Draw( )
{
	if (m_bIsHorizontal)
	{
		if (m_UIStaticItem.GetOriginalRect( ).width( ))
		{
			s32 tile = iFloor(GetWidth( ) / m_UIStaticItem.GetOriginalRect( ).width( ));
			f32 rem = GetWidth( ) - tile * m_UIStaticItem.GetOriginalRect( ).width( );
			m_UIStaticItem.SetTile(tile, 1, rem, 0);
		}
	}
	else
	{
		if (m_UIStaticItem.GetOriginalRect( ).height( ))
		{
			s32 tile = iFloor(GetHeight( ) / m_UIStaticItem.GetOriginalRect( ).height( ));
			f32 rem = GetHeight( ) - tile * m_UIStaticItem.GetOriginalRect( ).height( );
			m_UIStaticItem.SetTile(1, tile, 0, rem);
		}
	}

	inherited::Draw( );
}
