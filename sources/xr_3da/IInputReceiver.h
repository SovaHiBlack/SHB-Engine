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
	BOOL			IR_GetKeyState					(s32 dik);
	BOOL			IR_GetBtnState					(s32 btn);
	void			IR_Capture						( );
	void			IR_Release						( );

	virtual void	IR_OnDeactivate					( );
	virtual void	IR_OnActivate					( );

	virtual void	IR_OnMousePress					(s32 btn)		{}
	virtual void	IR_OnMouseRelease				(s32 btn)		{}
	virtual void	IR_OnMouseHold					(s32 btn)		{}
	virtual void	IR_OnMouseWheel					(s32 direction)	{}
	virtual void	IR_OnMouseMove					(s32 x, s32 y)	{}
	virtual void	IR_OnMouseStop					(s32 x, s32 y)	{}

	virtual void	IR_OnKeyboardPress				(s32 dik)		{}
	virtual void	IR_OnKeyboardRelease			(s32 dik)		{}
	virtual void	IR_OnKeyboardHold				(s32 dik)		{}
};

ENGINE_API extern f32			psMouseSens;
ENGINE_API extern f32			psMouseSensScale;
ENGINE_API extern flags32		psMouseInvert;
