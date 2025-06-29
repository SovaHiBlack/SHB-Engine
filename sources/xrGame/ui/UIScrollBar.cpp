#include "stdafx.h"
#include "uiscrollbar.h"
#include "UI3tButton.h"
#include "UIScrollBox.h"
#include "UIXmlInit.h"
#include "UITextureMaster.h"
#include "UICursor.h"

CUIScrollBar::CUIScrollBar( )
{
	m_iMinPos = 1;
	m_iMaxPos = 1;
	m_iPageSize = 0;
	m_iStepSize = 1;
	m_iScrollPos = 0;
	m_b_enabled = true;
	m_DecButton = xr_new<CUI3tButton>( );
	m_DecButton->SetAutoDelete(true);
	AttachChild(m_DecButton);
	m_IncButton = xr_new<CUI3tButton>( );
	m_IncButton->SetAutoDelete(true);
	AttachChild(m_IncButton);
	m_ScrollBox = xr_new<CUIScrollBox>( );
	m_ScrollBox->SetAutoDelete(true);
	AttachChild(m_ScrollBox);
	m_StaticBackground = xr_new<CUIStaticItem>( );
}

CUIScrollBar::~CUIScrollBar( )
{
	xr_delete(m_StaticBackground);
}

void CUIScrollBar::Init(f32 x, f32 y, f32 length, bool bIsHorizontal, pcstr profile)
{
	string256 _path;
	CUIXml uiXml;
	R_ASSERT(uiXml.Init(CONFIG_PATH, UI_PATH, "scroll_bar.xml"));

	f32 height = uiXml.ReadAttribFlt(profile, 0, "height", 16);

	m_bIsHorizontal = bIsHorizontal;
	if (m_bIsHorizontal)
	{
		CUIWindow::Init(x, y, length, height);

		strconcat(sizeof(_path), _path, profile, ":left_arrow");
		CUIXmlInit::Init3tButton(uiXml, _path, 0, m_DecButton);
		m_DecButton->SetWndPos(0.0f, 0.0f);

		strconcat(sizeof(_path), _path, profile, ":right_arrow");
		CUIXmlInit::Init3tButton(uiXml, _path, 0, m_IncButton);
		m_IncButton->SetWndPos(length - m_IncButton->GetWidth( ), 0.0f);

		m_ScrollBox->SetHorizontal( );

		strconcat(sizeof(_path), _path, profile, ":box");
		CUIXmlInit::InitStatic(uiXml, _path, 0, m_ScrollBox);
		m_IncButton->SetWndPos(0.0f, length / 2);

		strconcat(sizeof(_path), _path, profile, ":back:texture");
		pcstr texture = uiXml.Read(_path, 0, "");
		R_ASSERT(texture);
		CUITextureMaster::InitTexture(texture, m_StaticBackground);
		m_ScrollWorkArea = _max(0, iFloor(GetWidth( ) - 2 * height));
	}
	else
	{
		CUIWindow::Init(x, y, height, length);

		strconcat(sizeof(_path), _path, profile, ":up_arrow");
		CUIXmlInit::Init3tButton(uiXml, _path, 0, m_DecButton);
		m_DecButton->SetWndPos(0.0f, 0.0f);

		strconcat(sizeof(_path), _path, profile, ":down_arrow");
		CUIXmlInit::Init3tButton(uiXml, _path, 0, m_IncButton);
		m_IncButton->SetWndPos(0.0f, length - height);

		m_ScrollBox->SetVertical( );

		strconcat(sizeof(_path), _path, profile, ":box_v");
		CUIXmlInit::InitStatic(uiXml, _path, 0, m_ScrollBox);
		strconcat(sizeof(_path), _path, profile, ":back_v:texture");
		pcstr texture = uiXml.Read(_path, 0, "");
		R_ASSERT(texture);

		CUITextureMaster::InitTexture(texture, m_StaticBackground);
		m_ScrollWorkArea = _max(0, iFloor(GetHeight( ) - 2 * height));
	}

	UpdateScrollBar( );
}

//������������� �������� ���������
void CUIScrollBar::SetWidth(f32 width)
{
	if (width <= 0.0f)
	{
		width = 1.0f;
	}

	inherited::SetWidth(width);
}

void CUIScrollBar::SetHeight(f32 height)
{
	if (height <= 0.0f)
	{
		height = 1.0f;
	}

	inherited::SetHeight(height);
	UpdateScrollBar( );
}

void CUIScrollBar::SetStepSize(s32 step)
{
	m_iStepSize = step;
	UpdateScrollBar( );
}

void CUIScrollBar::SetRange(s32 iMin, s32 iMax)
{
	m_iMinPos = iMin;
	m_iMaxPos = iMax;
	VERIFY(iMax >= iMin);
	if (iMax < iMin)
	{
		iMax = iMin;
	}

	UpdateScrollBar( );
}

void CUIScrollBar::Show(bool b)
{
	if (!m_b_enabled)
	{
		return;
	}

	inherited::Show(b);
}

void CUIScrollBar::Enable(bool b)
{
	if (!m_b_enabled)
	{
		return;
	}

	inherited::Enable(b);
}

void CUIScrollBar::UpdateScrollBar( )
{
	if (IsShown( ))
	{
		//�c�������� ������ � ��������� �������
		if (m_iMaxPos == m_iMinPos)
		{
			m_iMaxPos++;
		}

		f32 box_sz = f32(m_ScrollWorkArea) * f32(m_iPageSize ? m_iPageSize : 1) / f32(m_iMaxPos - m_iMinPos);
		if (m_bIsHorizontal)
		{
			// set width
			clamp(box_sz, _min(GetHeight( ), GetWidth( ) - m_IncButton->GetWidth( ) - m_DecButton->GetWidth( )), GetWidth( ) - m_IncButton->GetWidth( ) - m_DecButton->GetWidth( ));
			m_ScrollBox->SetWidth(box_sz);
			m_ScrollBox->SetHeight(GetHeight( ));
			// set pos
			s32 pos = PosViewFromScroll(iFloor(m_ScrollBox->GetWidth( )), iFloor(GetHeight( )));
			m_ScrollBox->SetWndPos(f32(pos), m_ScrollBox->GetWndRect( ).top);
			m_IncButton->SetWndPos(GetWidth( ) - m_IncButton->GetWidth( ), 0.0f);
		}
		else
		{
			// set height
			clamp(box_sz, _min(GetWidth( ), GetHeight( ) - m_IncButton->GetHeight( ) - m_DecButton->GetHeight( )), GetHeight( ) - m_IncButton->GetHeight( ) - m_DecButton->GetHeight( ));
			m_ScrollBox->SetHeight(box_sz);
			m_ScrollBox->SetWidth(GetWidth( ));
			// set pos
			s32 pos = PosViewFromScroll(iFloor(m_ScrollBox->GetHeight( )), iFloor(GetWidth( )));
			m_ScrollBox->SetWndPos(m_ScrollBox->GetWndRect( ).left, f32(pos));
			m_IncButton->SetWndPos(0.0f, GetHeight( ) - m_IncButton->GetHeight( ));
		}
	}

	ClampByViewRect( );
}

u32 last_hold_time = 0;

bool CUIScrollBar::OnKeyboardHold(int dik)
{
	if (dik == MOUSE_1 && (last_hold_time + 100) < Device.dwTimeContinual)
	{
		fVector2 cursor_pos = GetUICursor( )->GetCursorPosition( );
		fRect dec_rect;
		fRect inc_rect;

		m_DecButton->GetAbsoluteRect(dec_rect);
		m_IncButton->GetAbsoluteRect(inc_rect);
		if (dec_rect.in(cursor_pos))
		{
			TryScrollDec( );
			last_hold_time = Device.dwTimeContinual;
			return true;
		}
		else if (inc_rect.in(cursor_pos))
		{
			TryScrollInc( );
			last_hold_time = Device.dwTimeContinual;
			return true;
		}
	}

	return false;
}

bool CUIScrollBar::OnMouse(f32 x, f32 y, EUIMessages mouse_action)
{
	switch (mouse_action)
	{
		case WINDOW_MOUSE_WHEEL_DOWN:
		{
			TryScrollInc( );
			return true;
		}
		break;
		case WINDOW_MOUSE_WHEEL_UP:
		{
			TryScrollDec( );
			return true;
		}
		break;
	}

	return inherited::OnMouse(x, y, mouse_action);
}

void CUIScrollBar::ClampByViewRect( )
{
	if (m_bIsHorizontal)
	{
		if (m_ScrollBox->GetWndRect( ).left <= m_DecButton->GetWidth( ))
		{
			m_ScrollBox->SetWndPos(m_DecButton->GetWidth( ), m_ScrollBox->GetWndRect( ).top);
		}
		else if (m_ScrollBox->GetWndRect( ).right >= m_IncButton->GetWndPos( ).x)
		{
			m_ScrollBox->SetWndPos(m_IncButton->GetWndRect( ).left - m_ScrollBox->GetWidth( ), m_ScrollBox->GetWndRect( ).top);
		}
	}
	else
	{
		// limit vertical position (TOP) by position of button	
		if (m_ScrollBox->GetWndRect( ).top <= m_DecButton->GetHeight( ))
		{
			m_ScrollBox->SetWndPos(m_ScrollBox->GetWndRect( ).left, m_DecButton->GetHeight( ));
		}
		// limit vertical position (BOTTOM) by position of button
		else if (m_ScrollBox->GetWndRect( ).bottom >= m_IncButton->GetWndPos( ).y)
		{
			m_ScrollBox->SetWndPos(m_ScrollBox->GetWndRect( ).left, m_IncButton->GetWndPos( ).y - m_ScrollBox->GetHeight( ));
		}
	}
}

void CUIScrollBar::SetPosScrollFromView(f32 view_pos, f32 view_size, f32 view_offs)
{
	s32 scroll_size = ScrollSize( );
	f32 pos = view_pos - view_offs;
	f32 work_size = m_ScrollWorkArea - view_size;
	SetScrollPosClamped(work_size ? iFloor(((pos / work_size) * (scroll_size) +m_iMinPos)) : 0);
}

s32 CUIScrollBar::PosViewFromScroll(s32 view_size, s32 view_offs)
{
	s32 work_size = m_ScrollWorkArea - view_size;
	s32 scroll_size = ScrollSize( );
	return (scroll_size ? (m_iScrollPos * work_size + scroll_size * view_offs - m_iMinPos * work_size) / scroll_size : 0);
}

void CUIScrollBar::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (pWnd == m_DecButton)
	{
		if (msg == BUTTON_CLICKED || msg == BUTTON_DOWN)
		{
			TryScrollDec( );
		}
	}
	else if (pWnd == m_IncButton)
	{
		if (msg == BUTTON_CLICKED || msg == BUTTON_DOWN)
		{
			TryScrollInc( );
		}
	}
	else if (pWnd == m_ScrollBox)
	{
		if (msg == SCROLLBOX_MOVE)
		{
			//��������� ����� ��������� ���������
			ClampByViewRect( );
			if (m_bIsHorizontal)
			{
				SetPosScrollFromView(m_ScrollBox->GetWndPos( ).x, m_ScrollBox->GetWidth( ), GetHeight( ));
				if (GetMessageTarget( ))
				{
					GetMessageTarget( )->SendMessage(this, SCROLLBAR_HSCROLL);
				}
			}
			else
			{
				SetPosScrollFromView(m_ScrollBox->GetWndPos( ).y, m_ScrollBox->GetHeight( ), GetWidth( ));
				if (GetMessageTarget( ))
				{
					GetMessageTarget( )->SendMessage(this, SCROLLBAR_VSCROLL);
				}
			}
		}
	}

	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUIScrollBar::TryScrollInc( )
{
	if (ScrollInc( ))
	{
		if (m_bIsHorizontal)
		{
			GetMessageTarget( )->SendMessage(this, SCROLLBAR_HSCROLL);
		}
		else
		{
			GetMessageTarget( )->SendMessage(this, SCROLLBAR_VSCROLL);
		}
	}
}

void CUIScrollBar::TryScrollDec( )
{
	if (ScrollDec( ))
	{
		if (m_bIsHorizontal)
		{
			GetMessageTarget( )->SendMessage(this, SCROLLBAR_HSCROLL);
		}
		else
		{
			GetMessageTarget( )->SendMessage(this, SCROLLBAR_VSCROLL);
		}
	}
}

bool CUIScrollBar::ScrollDec( )
{
	if (m_iScrollPos > m_iMinPos)
	{
		if (m_iScrollPos > m_iStepSize)
		{
			SetScrollPos(m_iScrollPos - m_iStepSize);
		}
		else
		{
			SetScrollPos(0);
		}

		return true;
	}

	return false;
}


bool CUIScrollBar::ScrollInc( )
{
	if (m_iScrollPos <= (m_iMaxPos - m_iPageSize))
	{
		SetScrollPos(m_iScrollPos + m_iStepSize);
		return true;
	}

	return false;
}

void CUIScrollBar::Reset( )
{
	ResetAll( );
	inherited::Reset( );
}

void CUIScrollBar::Draw( )
{
	//���������� ������� ��������
	fRect rect;
	GetAbsoluteRect(rect);
	if (m_bIsHorizontal)
	{
		if (m_StaticBackground->GetOriginalRect( ).width( ))
		{
			f32 size = GetWidth( ) - m_DecButton->GetWidth( ) - m_IncButton->GetWidth( );
			f32 w = m_StaticBackground->GetOriginalRect( ).width( );

			s32 tile = iFloor(size / w);
			f32 rem = size - tile * w;

			m_StaticBackground->SetTile(tile, 1, rem, 0.0f);
			m_StaticBackground->SetPos(rect.left + m_DecButton->GetWidth( ), rect.top);
		}
	}
	else
	{
		if (m_StaticBackground->GetOriginalRect( ).height( ))
		{
			f32 size = GetHeight( ) - m_IncButton->GetHeight( ) - m_DecButton->GetHeight( );
			f32 h = m_StaticBackground->GetOriginalRect( ).height( );

			s32 tile = iFloor(size / h);
			f32 rem = size - tile * h;

			m_StaticBackground->SetTile(1, tile, 0.0f, rem);
			m_StaticBackground->SetPos(rect.left, rect.top + m_DecButton->GetHeight( ));
		}
	}

	m_StaticBackground->Render( );

	inherited::Draw( );
}

void CUIScrollBar::Refresh( )
{
	SendMessage(m_ScrollBox, SCROLLBOX_MOVE, NULL);
}
