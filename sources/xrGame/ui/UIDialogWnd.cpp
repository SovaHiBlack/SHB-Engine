// UIDialogWnd.cpp: ����� �������� �������, ����� ��� ������������ �������
// ������ ������� ����� ������ ����������� Show() � Hide()
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "uidialogwnd.h"
#include "../hudmanager.h"
#include "../xr_level_controller.h"
#include "../..\XR_3DA\Console.h"
#include "../Level.h"
#include "../GameObject.h"

CUIDialogWnd::CUIDialogWnd( )
{
	m_pHolder = NULL;
	m_bWorkInPause = false;
	Hide( );
}

CUIDialogWnd::~CUIDialogWnd( )
{ }

void CUIDialogWnd::Show( )
{
	inherited::Enable(true);
	inherited::Show(true);

	ResetAll( );
}

void CUIDialogWnd::Hide( )
{
	inherited::Enable(false);
	inherited::Show(false);
}

bool CUIDialogWnd::IR_OnKeyboardHold(s32 dik)
{
	if (!IR_process( ))
	{
		return false;
	}

	if (OnKeyboardHold(dik))
	{
		return true;
	}

	if (!StopAnyMove( ) && g_pGameLevel)
	{
		CObject* O = Level( ).CurrentEntity( );
		if (O)
		{
			IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(O));
			if (!IR)
			{
				return false;
			}

			IR->IR_OnKeyboardHold(get_binded_action(dik));
		}
	}

	return false;
}

bool CUIDialogWnd::IR_OnKeyboardPress(s32 dik)
{
	if (!IR_process( ))
	{
		return false;
	}

	//mouse click
	if (dik == MOUSE_1 || dik == MOUSE_2 || dik == MOUSE_3)
	{
		fVector2 cp = GetUICursor( )->GetCursorPosition( );
		EUIMessages action = (dik == MOUSE_1) ? WINDOW_LBUTTON_DOWN : (dik == MOUSE_2) ? WINDOW_RBUTTON_DOWN : WINDOW_CBUTTON_DOWN;
		if (OnMouse(cp.x, cp.y, action))
		{
			return true;
		}
	}

	if (OnKeyboard(dik, WINDOW_KEY_PRESSED))
	{
		return true;
	}

	if (!StopAnyMove( ) && g_pGameLevel)
	{
		CObject* O = Level( ).CurrentEntity( );
		if (O)
		{
			IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(O));
			if (!IR)
			{
				return false;
			}

			IR->IR_OnKeyboardPress(get_binded_action(dik));
		}
	}

	return false;
}

bool CUIDialogWnd::IR_OnKeyboardRelease(s32 dik)
{
	if (!IR_process( ))
	{
		return false;
	}

	//mouse click
	if (dik == MOUSE_1 || dik == MOUSE_2 || dik == MOUSE_3)
	{
		fVector2 cp = GetUICursor( )->GetCursorPosition( );
		EUIMessages action = (dik == MOUSE_1) ? WINDOW_LBUTTON_UP : (dik == MOUSE_2) ? WINDOW_RBUTTON_UP : WINDOW_CBUTTON_UP;
		if (OnMouse(cp.x, cp.y, action))
		{
			return true;
		}
	}

	if (OnKeyboard(dik, WINDOW_KEY_RELEASED))
	{
		return true;
	}

	if (!StopAnyMove( ) && g_pGameLevel)
	{
		CObject* O = Level( ).CurrentEntity( );
		if (O)
		{
			IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(O));
			if (!IR)
			{
				return false;
			}

			IR->IR_OnKeyboardRelease(get_binded_action(dik));
		}
	}

	return false;
}

bool CUIDialogWnd::IR_OnMouseWheel(s32 direction)
{
	if (!IR_process( ))
	{
		return false;
	}

	fVector2 pos = GetUICursor( )->GetCursorPosition( );

	if (direction > 0)
	{
		OnMouse(pos.x, pos.y, WINDOW_MOUSE_WHEEL_UP);
	}
	else
	{
		OnMouse(pos.x, pos.y, WINDOW_MOUSE_WHEEL_DOWN);
	}

	return true;
}

bool CUIDialogWnd::IR_OnMouseMove(s32 dx, s32 dy)
{
	if (!IR_process( ))
	{
		return false;
	}

	if (GetUICursor( )->IsVisible( ))
	{
		GetUICursor( )->UpdateCursorPosition( );
		fVector2 cPos = GetUICursor( )->GetCursorPosition( );

		OnMouse(cPos.x, cPos.y, WINDOW_MOUSE_MOVE);
	}
	else if (!StopAnyMove( ) && g_pGameLevel)
	{
		CObject* O = Level( ).CurrentEntity( );
		if (O)
		{
			IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(O));
			if (!IR)
			{
				return false;
			}

			IR->IR_OnMouseMove(dx, dy);
		}
	}

	return true;
}

bool CUIDialogWnd::OnKeyboardHold(s32 dik)
{
	if (!IR_process( ))
	{
		return false;
	}

	return inherited::OnKeyboardHold(dik);
}

bool CUIDialogWnd::OnKeyboard(s32 dik, EUIMessages keyboard_action)
{
	if (!IR_process( ))
	{
		return false;
	}

	if (inherited::OnKeyboard(dik, keyboard_action))
	{
		return true;
	}

	return false;
}

bool CUIDialogWnd::IR_process( )
{
	if (!IsEnabled( ))
	{
		return false;
	}

	if (Device.Paused( ) && !WorkInPause( ))
	{
		return false;
	}

	return true;
}

void CUIDialogWnd::Update( )
{
	CUIWindow::Update( );
}
