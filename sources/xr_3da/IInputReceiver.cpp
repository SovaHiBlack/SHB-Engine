#include "stdafx.h"

#include "Input.h"
#include "IInputReceiver.h"

void	IInputReceiver::IR_Capture( )
{
	VERIFY(pInput);
	pInput->iCapture(this);
}

void	IInputReceiver::IR_Release( )
{
	VERIFY(pInput);
	pInput->iRelease(this);
}

void	IInputReceiver::IR_GetLastMouseDelta(iVector2& p)
{
	VERIFY(pInput);
	pInput->iGetLastMouseDelta(p);
}

void IInputReceiver::IR_OnDeactivate( )
{
	s32 i;
	for (i = 0; i < CInput::COUNT_KB_BUTTONS; i++)
	{
		if (IR_GetKeyState(i))
		{
			IR_OnKeyboardRelease(i);
		}
	}

	for (i = 0; i < CInput::COUNT_MOUSE_BUTTONS; i++)
	{
		if (IR_GetBtnState(i))
		{
			IR_OnMouseRelease(i);
		}
	}

	IR_OnMouseStop(DIMOFS_X, 0);
	IR_OnMouseStop(DIMOFS_Y, 0);
}

void IInputReceiver::IR_OnActivate( )
{ }

BOOL IInputReceiver::IR_GetKeyState(s32 dik)
{
	VERIFY(pInput);
	return pInput->iGetAsyncKeyState(dik);
}

BOOL IInputReceiver::IR_GetBtnState(s32 btn)
{
	VERIFY(pInput);
	return pInput->iGetAsyncBtnState(btn);
}

void	IInputReceiver::IR_GetMousePosScreen(iVector2& p)
{
	GetCursorPos((LPPOINT) &p);
}

void	IInputReceiver::IR_GetMousePosReal(HWND hwnd, iVector2& p)
{
	IR_GetMousePosScreen(p);
	if (hwnd)
	{
		ScreenToClient(hwnd, (LPPOINT) &p);
	}
}

void	IInputReceiver::IR_GetMousePosReal(iVector2& p)
{
	IR_GetMousePosReal(Device.m_hWnd, p);
}

void	IInputReceiver::IR_GetMousePosIndependent(fVector2& f)
{
	iVector2 p;
	IR_GetMousePosReal(p);
	f.set(2.0f * f32(p.x) / f32(Device.dwWidth) - 1.0f, 2.0f * f32(p.y) / f32(Device.dwHeight) - 1.0f);
}

void	IInputReceiver::IR_GetMousePosIndependentCrop(fVector2& f)
{
	IR_GetMousePosIndependent(f);
	if (f.x < -1.0f)
	{
		f.x = -1.0f;
	}

	if (f.x > 1.0f)
	{
		f.x = 1.0f;
	}

	if (f.y < -1.0f)
	{
		f.y = -1.0f;
	}

	if (f.y > 1.0f)
	{
		f.y = 1.0f;
	}
}
