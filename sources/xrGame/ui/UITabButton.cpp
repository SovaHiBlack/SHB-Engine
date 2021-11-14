#include "stdafx.h"

#include "UITabButton.h"//
#include "../HUDManager.h"//

CUITabButton::CUITabButton( )
{ }

CUITabButton::~CUITabButton( )
{ }

bool CUITabButton::OnMouse(float x, float y, EUIMessages mouse_action)
{
	return CUIWindow::OnMouse(x, y, mouse_action);
}

bool CUITabButton::OnMouseDown(int mouse_btn)
{
	if (mouse_btn == MOUSE_1)
	{
		GetMessageTarget( )->SendMessage(this, TAB_CHANGED);
		return true;
	}
	else
	{
		return false;
	}
}

void CUITabButton::Update( )
{
	CUI3tButton::Update( );
}

void CUITabButton::SendMessage(CUIWindow* pWnd, signed short msg, void* pData)
{
	if (!IsEnabled( ))
	{
		return;
	}

	switch (msg)
	{
		case TAB_CHANGED:
		{
			if (this == pWnd)
			{
				m_eButtonState = BUTTON_PUSHED;
				OnClick( );
			}
			else
			{
				m_eButtonState = BUTTON_NORMAL;
			}
		}
		break;
		default:
		{
		}
	}
}
