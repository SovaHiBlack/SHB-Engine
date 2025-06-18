// UIDialogWnd.h: класс простого диалога, нужен для стандартного запуска
// разным менюшек путем вызова виртуальных Show() И Hide()
//////////////////////////////////////////////////////////////////////
#pragma once

#include "UIWindow.h"

class CUIDialogHolder;

class CUIDialogWnd : public CUIWindow
{
private:
	typedef CUIWindow inherited;
	CUIDialogHolder* m_pHolder;

protected:
	bool			IR_process( );

public:
	bool										m_bWorkInPause;
	CUIDialogWnd( );
	virtual ~CUIDialogWnd( );

	virtual void Show( );
	virtual void Hide( );
	virtual void Update( );

	virtual bool IR_OnKeyboardPress(s32 dik);
	virtual bool IR_OnKeyboardRelease(s32 dik);
	virtual bool IR_OnMouseMove(s32 dx, s32 dy);
	virtual bool IR_OnMouseWheel(s32 direction);
	virtual bool IR_OnKeyboardHold(s32 dik);
	virtual bool OnKeyboard(s32 dik, EUIMessages keyboard_action);
	virtual bool OnKeyboardHold(s32 dik);

	CUIDialogHolder* GetHolder( )
	{
		return m_pHolder;
	}
	void SetHolder(CUIDialogHolder* h)
	{
		m_pHolder = h;
	}
	virtual bool StopAnyMove( )
	{
		return true;
	}
	virtual bool NeedCursor( ) const
	{
		return true;
	}
	virtual bool WorkInPause( ) const
	{
		return m_bWorkInPause;
	}
	virtual bool Dispatch(s32 cmd, s32 param)
	{
		return true;
	}
};
