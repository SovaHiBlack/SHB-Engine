// IInputReceiver.h: interface for the IInputReceiver class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class ENGINE_API	IInputReceiver
{
public:
	void			IR_GetLastMouseDelta			(iVector2& p);
	void			IR_GetMousePosScreen			(iVector2& p);
	void			IR_GetMousePosReal				(HWND hwnd, iVector2& p);
	void			IR_GetMousePosReal				(iVector2& p);
	void			IR_GetMousePosIndependent		(fVector2& f);
	void			IR_GetMousePosIndependentCrop	(fVector2& f);
	BOOL			IR_GetKeyState					(int dik);
	BOOL			IR_GetBtnState					(int btn);
	void			IR_Capture						(void);
	void			IR_Release						(void);

	virtual void	IR_OnDeactivate					(void);
	virtual void	IR_OnActivate					(void);

	virtual void	IR_OnMousePress					(int btn)		{};
	virtual void	IR_OnMouseRelease				(int btn)		{};
	virtual void	IR_OnMouseHold					(int btn)		{};
	virtual void	IR_OnMouseWheel					(int direction)	{};
	virtual void	IR_OnMouseMove					(int x, int y)	{};
	virtual void	IR_OnMouseStop					(int x, int y)	{};

	virtual void	IR_OnKeyboardPress				(int dik)		{};
	virtual void	IR_OnKeyboardRelease			(int dik)		{};
	virtual void	IR_OnKeyboardHold				(int dik)		{};
};

ENGINE_API extern f32			psMouseSens;
ENGINE_API extern f32			psMouseSensScale;
ENGINE_API extern Flags32		psMouseInvert;
