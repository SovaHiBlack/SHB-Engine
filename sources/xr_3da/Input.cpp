#include "stdafx.h"

#include "Input.h"
#include "IInputReceiver.h"

CInput* pInput = NULL;
IInputReceiver		dummyController;

ENGINE_API f32	psMouseSens = 1.0f;
ENGINE_API f32	psMouseSensScale = 1.0f;
ENGINE_API flags32	psMouseInvert = { FALSE };

#define MOUSEBUFFERSIZE			64
#define KEYBOARDBUFFERSIZE		64
#define _KEYDOWN(name,key)		( name[key] & 0x80 )

static bool g_exclusive = true;
void on_error_dialog(bool before)
{
	if (!pInput || !g_exclusive)
	{
		return;
	}

	pInput->exclusive_mode(!before);
}

CInput::CInput(BOOL bExclusive, s32 deviceForInit)
{
	g_exclusive = !!bExclusive;

	Log("Starting INPUT device...");

	pDI = NULL;
	pMouse = NULL;
	pKeyboard = NULL;

	//=====================Mouse
	mouse_property.mouse_dt = 25;

	ZeroMemory(mouseState, sizeof(mouseState));
	ZeroMemory(KBState, sizeof(KBState));
	ZeroMemory(timeStamp, sizeof(timeStamp));
	ZeroMemory(timeSave, sizeof(timeStamp));
	ZeroMemory(offs, sizeof(offs));

	//===================== Dummy pack
	iCapture(&dummyController);

	if (!pDI)
	{
		CHK_DX(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (pvoid*) &pDI, NULL));
	}

	// KEYBOARD
	if (deviceForInit & keyboard_device_key)
	{
		CHK_DX(CreateInputDevice(&pKeyboard, GUID_SysKeyboard, &c_dfDIKeyboard, ((bExclusive) ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND, KEYBOARDBUFFERSIZE));
	}

	// MOUSE
	if (deviceForInit & mouse_device_key)
	{
		CHK_DX(CreateInputDevice(&pMouse, GUID_SysMouse, &c_dfDIMouse2, ((bExclusive) ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND | DISCL_NOWINKEY, MOUSEBUFFERSIZE));
	}

	Debug.set_on_dialog(&on_error_dialog);

#ifdef ENGINE_BUILD
	Device.seqAppActivate.Add(this);
	Device.seqAppDeactivate.Add(this);
	Device.seqFrame.Add(this, REG_PRIORITY_HIGH);
#endif

}

CInput::~CInput( )
{

#ifdef ENGINE_BUILD
	Device.seqFrame.Remove(this);
	Device.seqAppDeactivate.Remove(this);
	Device.seqAppActivate.Remove(this);
#endif

	// Unacquire and release the device's interfaces
	if (pMouse)
	{
		pMouse->Unacquire( );
		_RELEASE(pMouse);
	}

	if (pKeyboard)
	{
		pKeyboard->Unacquire( );
		_RELEASE(pKeyboard);
	}

	_SHOW_REF("Input: ", pDI);
	_RELEASE(pDI);
}

//-----------------------------------------------------------------------------
// Name: CreateInputDevice()
// Desc: Create a DirectInput device.
//-----------------------------------------------------------------------------
HRESULT CInput::CreateInputDevice(LPDIRECTINPUTDEVICE8* device, GUID guidDevice, const DIDATAFORMAT* pdidDataFormat, u32 dwFlags, u32 buf_size)
{
	// Obtain an interface to the input device
	CHK_DX(pDI->CreateDevice(guidDevice, device, NULL));

	// Set the device data format. Note: a data format specifies which
	// controls on a device we are interested in, and how they should be
	// reported.
	CHK_DX((*device)->SetDataFormat(pdidDataFormat));

	// Set the cooperativity level to let DirectInput know how this device
	// should interact with the system and with other DirectInput applications.
	HRESULT _hr = (*device)->SetCooperativeLevel(Device.m_hWnd, dwFlags);
	if (FAILED(_hr) && (_hr == E_NOTIMPL))
	{
		Msg("! INPUT: Can't set coop level. Emulation???");
	}
	else
	{
		R_CHK(_hr);
	}

	// setup the buffer size for the keyboard data
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = buf_size;

	CHK_DX((*device)->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph));

	return S_OK;
}
//-----------------------------------------------------------------------

void CInput::SetAllAcquire(BOOL bAcquire)
{
	if (pMouse)
	{
		bAcquire ? pMouse->Acquire( ) : pMouse->Unacquire( );
	}

	if (pKeyboard)
	{
		bAcquire ? pKeyboard->Acquire( ) : pKeyboard->Unacquire( );
	}
}

void CInput::SetMouseAcquire(BOOL bAcquire)
{
	if (pMouse)
	{
		bAcquire ? pMouse->Acquire( ) : pMouse->Unacquire( );
	}
}
void CInput::SetKBDAcquire(BOOL bAcquire)
{
	if (pKeyboard)
	{
		bAcquire ? pKeyboard->Acquire( ) : pKeyboard->Unacquire( );
	}
}
//-----------------------------------------------------------------------
BOOL b_altF4 = FALSE;
void CInput::KeyUpdate( )
{
	HRESULT hr;
	DWORD dwElements = KEYBOARDBUFFERSIZE;
	DIDEVICEOBJECTDATA od[KEYBOARDBUFFERSIZE];
	DWORD key = 0;

	VERIFY(pKeyboard);

	hr = pKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od[0], &dwElements, 0);
	if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
	{
		hr = pKeyboard->Acquire( );
		if (hr != S_OK)
		{
			return;
		}

		hr = pKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od[0], &dwElements, 0);
		if (hr != S_OK)
		{
			return;
		}
	}

	for (u32 i = 0; i < dwElements; i++)
	{
		key = od[i].dwOfs;
		KBState[key] = od[i].dwData & 0x80;
		if (KBState[key])
		{
			cbStack.back( )->IR_OnKeyboardPress(key);
		}

		if (!KBState[key])
		{
			cbStack.back( )->IR_OnKeyboardRelease(key);
		}
	}

	for (i = 0; i < COUNT_KB_BUTTONS; i++)
	{
		if (KBState[i])
		{
			cbStack.back( )->IR_OnKeyboardHold(i);
		}
	}

	if (!b_altF4 && iGetAsyncKeyState(DIK_F4) && (iGetAsyncKeyState(DIK_RMENU) || iGetAsyncKeyState(DIK_LMENU)))
	{
		b_altF4 = TRUE;
		Engine.Event.Defer("KERNEL:disconnect");
		Engine.Event.Defer("KERNEL:quit");
	}
}

bool CInput::get_dik_name(s32 dik, pstr dest_str, s32 dest_sz)
{
	DIPROPSTRING keyname;
	keyname.diph.dwSize = sizeof(DIPROPSTRING);
	keyname.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	keyname.diph.dwObj = static_cast<DWORD>(dik);
	keyname.diph.dwHow = DIPH_BYOFFSET;
	HRESULT hr = pKeyboard->GetProperty(DIPROP_KEYNAME, &keyname.diph);
	if (FAILED(hr))
	{
		return false;
	}

	const wchar_t* wct = keyname.wsz;
	if (0 == wcslen(wct))
	{
		return false;
	}

	size_t cnt = wcstombs(dest_str, wct, dest_sz);
	Msg("dik_name for[%i], is w[%S] ch[%s]", dik, wct, dest_str);
	if (cnt == -1)
	{
		Msg("! cant convert dik_name for dik[%i], prop=[%S]", dik, wct);
		return false;
	}

	return (cnt != -1);
}

BOOL CInput::iGetAsyncKeyState(s32 dik)
{
	return !!KBState[dik];
}

BOOL CInput::iGetAsyncBtnState(s32 btn)
{
	return !!mouseState[btn];
}

void CInput::MouseUpdate( )
{
	HRESULT hr;
	DWORD dwElements = MOUSEBUFFERSIZE;
	DIDEVICEOBJECTDATA	od[MOUSEBUFFERSIZE];

	VERIFY(pMouse);

	hr = pMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od[0], &dwElements, 0);
	if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
	{
		hr = pMouse->Acquire( );
		if (hr != S_OK)
		{
			return;
		}

		hr = pMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od[0], &dwElements, 0);
		if (hr != S_OK)
		{
			return;
		}
	}

	BOOL mouse_prev[COUNT_MOUSE_BUTTONS];

	mouse_prev[0] = mouseState[0];
	mouse_prev[1] = mouseState[1];
	mouse_prev[2] = mouseState[2];

	offs[0] = offs[1] = offs[2] = 0;
	for (u32 i = 0; i < dwElements; i++)
	{
		switch (od[i].dwOfs)
		{
			case DIMOFS_X:
			{
				offs[0] += od[i].dwData;
				timeStamp[0] = od[i].dwTimeStamp;
			}
			break;
			case DIMOFS_Y:
			{
				offs[1] += od[i].dwData;
				timeStamp[1] = od[i].dwTimeStamp;
			}
			break;
			case DIMOFS_Z:
			{
				offs[2] += od[i].dwData;
				timeStamp[2] = od[i].dwTimeStamp;
			}
			break;
			case DIMOFS_BUTTON0:
			{
				if (od[i].dwData & 0x80)
				{
					mouseState[0] = TRUE;
					cbStack.back( )->IR_OnMousePress(0);
				}

				if (!(od[i].dwData & 0x80))
				{
					mouseState[0] = FALSE;
					cbStack.back( )->IR_OnMouseRelease(0);
				}
			}
			break;
			case DIMOFS_BUTTON1:
			{
				if (od[i].dwData & 0x80)
				{
					mouseState[1] = TRUE;
					cbStack.back( )->IR_OnMousePress(1);
				}

				if (!(od[i].dwData & 0x80))
				{
					mouseState[1] = FALSE;
					cbStack.back( )->IR_OnMouseRelease(1);
				}
			}
			break;
			case DIMOFS_BUTTON2:
			{
				if (od[i].dwData & 0x80)
				{
					mouseState[2] = TRUE;
					cbStack.back( )->IR_OnMousePress(2);
				}

				if (!(od[i].dwData & 0x80))
				{
					mouseState[2] = FALSE;
					cbStack.back( )->IR_OnMouseRelease(2);
				}
			}
			break;
			case DIMOFS_BUTTON3:
			{
				if (od[i].dwData & 0x80)
				{
					mouseState[2] = TRUE;
					cbStack.back( )->IR_OnKeyboardPress(0xED + 103);
				}

				if (!(od[i].dwData & 0x80))
				{
					mouseState[2] = FALSE;
					cbStack.back( )->IR_OnKeyboardRelease(0xED + 103);
				}
			}
			break;
			case DIMOFS_BUTTON4:
			{
				if (od[i].dwData & 0x80)
				{
					mouseState[2] = TRUE;
					cbStack.back( )->IR_OnKeyboardPress(0xED + 104);
				}

				if (!(od[i].dwData & 0x80))
				{
					mouseState[2] = FALSE;
					cbStack.back( )->IR_OnKeyboardRelease(0xED + 104);
				}
			}
			break;
			case DIMOFS_BUTTON5:
			{
				if (od[i].dwData & 0x80)
				{
					mouseState[2] = TRUE;
					cbStack.back( )->IR_OnKeyboardPress(0xED + 105);
				}

				if (!(od[i].dwData & 0x80))
				{
					mouseState[2] = FALSE;
					cbStack.back( )->IR_OnKeyboardRelease(0xED + 105);
				}
			}
			break;
			case DIMOFS_BUTTON6:
			{
				if (od[i].dwData & 0x80)
				{
					mouseState[2] = TRUE;
					cbStack.back( )->IR_OnKeyboardPress(0xED + 106);
				}

				if (!(od[i].dwData & 0x80))
				{
					mouseState[2] = FALSE;
					cbStack.back( )->IR_OnKeyboardRelease(0xED + 106);
				}
			}
			break;
			case DIMOFS_BUTTON7:
			{
				if (od[i].dwData & 0x80)
				{
					mouseState[2] = TRUE;
					cbStack.back( )->IR_OnKeyboardPress(0xED + 107);
				}

				if (!(od[i].dwData & 0x80))
				{
					mouseState[2] = FALSE;
					cbStack.back( )->IR_OnKeyboardRelease(0xED + 107);
				}
			}
			break;
		}
	}

	if (mouseState[0] && mouse_prev[0])
	{
		cbStack.back( )->IR_OnMouseHold(0);
	}

	if (mouseState[1] && mouse_prev[1])
	{
		cbStack.back( )->IR_OnMouseHold(1);
	}

	if (mouseState[2] && mouse_prev[2])
	{
		cbStack.back( )->IR_OnMouseHold(2);
	}

	if (dwElements)
	{
		if (offs[0] || offs[1])
		{
			cbStack.back( )->IR_OnMouseMove(offs[0], offs[1]);
		}

		if (offs[2])
		{
			cbStack.back( )->IR_OnMouseWheel(offs[2]);
		}
	}
	else
	{
		if (timeStamp[1] && ((dwCurTime - timeStamp[1]) >= mouse_property.mouse_dt))
		{
			cbStack.back( )->IR_OnMouseStop(DIMOFS_Y, timeStamp[1] = 0);
		}

		if (timeStamp[0] && ((dwCurTime - timeStamp[0]) >= mouse_property.mouse_dt))
		{
			cbStack.back( )->IR_OnMouseStop(DIMOFS_X, timeStamp[0] = 0);
		}
	}
}

//-------------------------------------------------------
void CInput::iCapture(IInputReceiver* p)
{
	VERIFY(p);
	if (pMouse)
	{
		MouseUpdate( );
	}

	if (pKeyboard)
	{
		KeyUpdate( );
	}

	// change focus
	if (!cbStack.empty( ))
	{
		cbStack.back( )->IR_OnDeactivate( );
	}

	cbStack.push_back(p);
	cbStack.back( )->IR_OnActivate( );

	// prepare for _new_ controller
	ZeroMemory(timeStamp, sizeof(timeStamp));
	ZeroMemory(timeSave, sizeof(timeStamp));
	ZeroMemory(offs, sizeof(offs));
}

void CInput::iRelease(IInputReceiver* p)
{
	if (p == cbStack.back( ))
	{
		cbStack.back( )->IR_OnDeactivate( );
		cbStack.pop_back( );
		IInputReceiver* ir = cbStack.back( );
		ir->IR_OnActivate( );
	}
	else
	{	// we are not topmost receiver, so remove the nearest one
		u32 cnt = cbStack.size( );
		for (; cnt > 0; --cnt)
		{
			if (cbStack[cnt - 1] == p)
			{
				xr_vector<IInputReceiver*>::iterator it = cbStack.begin( );
				std::advance(it, cnt - 1);
				cbStack.erase(it);
				break;
			}
		}
	}
}

void CInput::OnAppActivate( )
{
	if (CurrentIR( ))
	{
		CurrentIR( )->IR_OnActivate( );
	}

	SetAllAcquire(true);
	ZeroMemory(mouseState, sizeof(mouseState));
	ZeroMemory(KBState, sizeof(KBState));
	ZeroMemory(timeStamp, sizeof(timeStamp));
	ZeroMemory(timeSave, sizeof(timeStamp));
	ZeroMemory(offs, sizeof(offs));
}

void CInput::OnAppDeactivate( )
{
	if (CurrentIR( ))
	{
		CurrentIR( )->IR_OnDeactivate( );
	}

	SetAllAcquire(false);
	ZeroMemory(mouseState, sizeof(mouseState));
	ZeroMemory(KBState, sizeof(KBState));
	ZeroMemory(timeStamp, sizeof(timeStamp));
	ZeroMemory(timeSave, sizeof(timeStamp));
	ZeroMemory(offs, sizeof(offs));
}

void CInput::OnFrame( )
{
	Device.Statistic->Input.Begin( );
	dwCurTime = Device.TimerAsync_MMT( );
	if (pKeyboard)
	{
		KeyUpdate( );
	}

	if (pMouse)
	{
		MouseUpdate( );
	}

	Device.Statistic->Input.End( );
}

IInputReceiver* CInput::CurrentIR( )
{
	if (cbStack.size( ))
	{
		return cbStack.back( );
	}
	else
	{
		return NULL;
	}
}

void CInput::exclusive_mode(const bool& exclusive)
{
	pKeyboard->SetCooperativeLevel(Device.m_hWnd, (exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND);
	pMouse->SetCooperativeLevel(Device.m_hWnd, (exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND | DISCL_NOWINKEY);
}
