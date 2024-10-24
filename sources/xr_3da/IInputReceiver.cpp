#include "stdafx.h"

#include "xr_input.h"
#include "iinputreceiver.h"

void	IInputReceiver::IR_Capture						(void)
{
	VERIFY(pInput);
	pInput->iCapture(this);
}

void	IInputReceiver::IR_Release						(void)
{
	VERIFY(pInput);
	pInput->iRelease(this);
}

void	IInputReceiver::IR_GetLastMouseDelta			(iVector2& p)
{
	VERIFY(pInput);
	pInput->iGetLastMouseDelta( p );
}

void IInputReceiver::IR_OnDeactivate					(void)
{
	int i;
	for (i = 0; i < CInput::COUNT_KB_BUTTONS; i++ )
		if (IR_GetKeyState(i))	IR_OnKeyboardRelease	(i);
	for (i = 0; i < CInput::COUNT_MOUSE_BUTTONS; i++ )
		if(IR_GetBtnState(i))   IR_OnMouseRelease( i );
	IR_OnMouseStop	( DIMOFS_X, 0 );
	IR_OnMouseStop	( DIMOFS_Y, 0 );
}

void IInputReceiver::IR_OnActivate(void)
{ }

BOOL IInputReceiver::IR_GetKeyState(int dik)
{
	VERIFY(pInput);
	return pInput->iGetAsyncKeyState(dik);
}

BOOL IInputReceiver::IR_GetBtnState(int btn)
{
	VERIFY(pInput);
	return pInput->iGetAsyncBtnState(btn);
}

void	IInputReceiver::IR_GetMousePosScreen			(iVector2& p)
{
	GetCursorPos((LPPOINT)&p);
}

void	IInputReceiver::IR_GetMousePosReal				(HWND hwnd, iVector2& p)
{
	IR_GetMousePosScreen(p);
	if (hwnd) ScreenToClient(hwnd,(LPPOINT)&p);
}

void	IInputReceiver::IR_GetMousePosReal				(iVector2& p)
{
	IR_GetMousePosReal(Device.m_hWnd,p);
}

void	IInputReceiver::IR_GetMousePosIndependent		(fVector2& f)
{
	iVector2 p;
	IR_GetMousePosReal(p);
	f.set(
		2.f* f32(p.x)/ f32(Device.dwWidth)-1.f,
		2.f* f32(p.y)/ f32(Device.dwHeight)-1.f
		);
}

void	IInputReceiver::IR_GetMousePosIndependentCrop	(fVector2& f)
{
	IR_GetMousePosIndependent(f);
	if (f.x<-1.f) f.x=-1.f;
	if (f.x> 1.f) f.x= 1.f;
	if (f.y<-1.f) f.y=-1.f;
	if (f.y> 1.f) f.y= 1.f;
}
