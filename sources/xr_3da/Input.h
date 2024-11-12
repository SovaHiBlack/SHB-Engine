#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class	ENGINE_API				IInputReceiver;

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//описание класса
const s32 mouse_device_key = 1;
const s32 keyboard_device_key = 2;
const s32 all_device_key = mouse_device_key | keyboard_device_key;
const s32 default_key = mouse_device_key | keyboard_device_key;

class ENGINE_API CInput : public pureFrame, public pureAppActivate, public pureAppDeactivate
{
public:
	enum
	{
		COUNT_MOUSE_BUTTONS = 3,
		COUNT_MOUSE_AXIS = 3,
		COUNT_KB_BUTTONS = 256
	};
	struct sxr_mouse
	{
		DIDEVCAPS					capabilities;
		DIDEVICEINSTANCE			deviceInfo;
		DIDEVICEOBJECTINSTANCE		objectInfo;
		u32							mouse_dt;
	};
	struct sxr_key
	{
		DIDEVCAPS					capabilities;
		DIDEVICEINSTANCE			deviceInfo;
		DIDEVICEOBJECTINSTANCE		objectInfo;
	};

private:
	LPDIRECTINPUT8				pDI;			// The DInput object
	LPDIRECTINPUTDEVICE8		pMouse;			// The DIDevice7 interface
	LPDIRECTINPUTDEVICE8		pKeyboard;		// The DIDevice7 interface
	//----------------------
	u32							timeStamp[COUNT_MOUSE_AXIS];
	u32							timeSave[COUNT_MOUSE_AXIS];
	s32 						offs[COUNT_MOUSE_AXIS];
	BOOL						mouseState[COUNT_MOUSE_BUTTONS];

	//----------------------
	BOOL						KBState[COUNT_KB_BUTTONS];

	HRESULT						CreateInputDevice(LPDIRECTINPUTDEVICE8* device, GUID guidDevice,
												  const DIDATAFORMAT* pdidDataFormat, u32 dwFlags,
												  u32 buf_size);

	xr_vector<IInputReceiver*>	cbStack;

	void						MouseUpdate( );
	void						KeyUpdate( );

public:
	sxr_mouse					mouse_property;
	sxr_key						key_property;
	u32							dwCurTime;

	void						SetAllAcquire(BOOL bAcquire = TRUE);
	void						SetMouseAcquire(BOOL bAcquire);
	void						SetKBDAcquire(BOOL bAcquire);

	void						iCapture(IInputReceiver* pc);
	void						iRelease(IInputReceiver* pc);
	BOOL						iGetAsyncKeyState(s32 dik);
	BOOL						iGetAsyncBtnState(s32 btn);
	void						iGetLastMouseDelta(iVector2& p)
	{
		p.set(offs[0], offs[1]);
	}

	CInput(BOOL bExclusive = true, s32 deviceForInit = default_key);
	~CInput( );

	virtual void				OnFrame( );
	virtual void				OnAppActivate( );
	virtual void				OnAppDeactivate( );

	IInputReceiver* CurrentIR( );

public:
	void				exclusive_mode(const bool& exclusive);
	bool				get_dik_name(s32 dik, pstr dest, s32 dest_sz);
};

extern ENGINE_API CInput* pInput;
