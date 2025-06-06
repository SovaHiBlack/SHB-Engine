#pragma once

#include "UILines.h"
#include "UIWindow.h"

class CUICustomEdit : public CUIWindow, public CUILinesOwner
{
	u32				m_max_symb_count;

public:
	CUICustomEdit( );
	virtual ~CUICustomEdit( );
	// CUILinesOwner
	virtual void			SetFont(CGameFont* pFont)
	{
		CUILinesOwner::SetFont(pFont);
	}
	virtual CGameFont* GetFont( )
	{
		return CUILinesOwner::GetFont( );
	}
	virtual void			SetTextColor(u32 color);
	virtual void			SetTextColorD(u32 color);

	virtual void	Init(f32 x, f32 y, f32 width, f32 height);
	virtual void	SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData = NULL);

	virtual bool	OnMouse(f32 x, f32 y, EUIMessages mouse_action);
	virtual bool	OnKeyboard(s32 dik, EUIMessages keyboard_action);
	virtual void	OnFocusLost( );

	virtual void	Update( );
	virtual void	Draw( );

	void	CaptureFocus(bool bCapture)
	{
		m_bInputFocus = bCapture;
	}
	virtual void	SetText(pcstr str);
	virtual pcstr GetText( );
	void	SetMaxCharCount(u32 cnt)
	{
		m_max_symb_count = cnt;
	}
	virtual void	Enable(bool status);
	void	SetNumbersOnly(bool status);
	void	SetFloatNumbers(bool status);
	void	SetPasswordMode(bool mode = true);
	void	SetDbClickMode(bool mode = true)
	{
		m_bFocusByDbClick = mode;
	}
	void	SetCursorColor(u32 color)
	{
		m_lines.SetCursorColor(color);
	}

	void	SetLightAnim(pcstr lanim);

protected:
	bool KeyPressed(s32 dik);
	bool KeyReleased(s32 dik);

	void AddLetter(char c);
	virtual void AddChar(char c);

	bool m_bInputFocus;
	bool m_bShift;

	bool m_bNumbersOnly;
	bool m_bFloatNumbers;
	bool m_bFocusByDbClick;

	u32 m_textColor[2];

	//DIK �������, ���. ������ � ������������, 0 ���� ����� ���
	s32 m_iKeyPressAndHold;
	bool m_bHoldWaitMode;

	//	u32	m_cursorColor;

	CLightAnimItem* m_lanim;
};
