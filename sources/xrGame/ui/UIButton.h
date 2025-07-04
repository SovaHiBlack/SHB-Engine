#pragma once

#include "UIStatic.h"
#include "../script_export_space.h"

class CUIButton : public CUIStatic
{
private:
	typedef			CUIStatic				inherited;

public:
	CUIButton( );
	virtual			~CUIButton( );

	virtual bool	OnMouse(f32 x, f32 y, EUIMessages mouse_action);
	virtual void	OnClick( );

	// ���������� ����
	virtual void	DrawTexture( );
	virtual void	DrawText( );
	virtual void	DrawHighlightedText( );

	virtual void	Update( );
	virtual void	Enable(bool status);
	virtual bool	OnKeyboard(s32 dik, EUIMessages keyboard_action);
	virtual void	OnFocusLost( );

	// ������ � ������� ����� �������� ������
	typedef enum
	{
		NORMAL_PRESS, //������ ���������� ��� ������� � ���������� �� ��� ����
		DOWN_PRESS    //����� ��� �������
	} E_PRESS_MODE;

	// ������ ����������� ���������
	virtual void	Reset( );

	//��������� �� ����� �� ������
	// �������������� ���������
	virtual void	HighlightItem(bool bHighlight)
	{
		m_bCursorOverWindow = bHighlight;
	}

	// ��������� � ������� ��������� ������
	typedef enum
	{
		BUTTON_NORMAL, //������ ����� �� �������������
		BUTTON_PUSHED, //� ������� ��������
		BUTTON_UP      //��� ������������ ������ ���� 
	} E_BUTTON_STATE;

	// ��������� ��������� ������: ��������, �� ��������
	void				SetButtonMode(E_BUTTON_STATE eBtnState)
	{
		m_eButtonState = eBtnState;
	}
	E_BUTTON_STATE		GetButtonsState( )
	{
		return m_eButtonState;
	}

	// ��������� ������ ��� ������������� ����������� ���� ������ � ������ NORMAL_PRESS
	void				SetButtonAsSwitch(bool bAsSwitch)
	{
		m_bIsSwitch = bAsSwitch;
	}

	// ������ � �������������
	// ��� ������������ ������� �� ����� dinput.h, �� DirectX SDK.
	// ��������: ������ A - ��� 0x1E(DIK_A)
	void				SetAccelerator(s32 iAccel, s32 idx)
	{
		VERIFY(idx == 0 || idx == 1);
		m_uAccelerator[idx] = iAccel;
	}
	const s32			GetAccelerator(s32 idx) const
	{
		VERIFY(idx == 0 || idx == 1);
		return m_uAccelerator[idx];
	}
	IC bool				IsAccelerator(s32 iAccel) const
	{
		return ((m_uAccelerator[0] == iAccel) || (m_uAccelerator[1] == iAccel));
	}

	void				SetPressMode(E_PRESS_MODE ePressMode)
	{
		m_ePressMode = ePressMode;
	}
	E_PRESS_MODE		GetPressMode( )
	{
		return m_ePressMode;
	}

	void				SetPushOffset(const fVector2& offset)
	{
		m_PushOffset = offset;
	}
	fVector2			GetPushOffset( )
	{
		return m_PushOffset;
	}
	void				SetShadowOffset(const fVector2& offset)
	{
		m_ShadowOffset = offset;
	}
	shared_str			m_hint_text;

protected:
	E_BUTTON_STATE		m_eButtonState;
	bool				m_bIsSwitch;
	bool				m_bButtonClicked;
	E_PRESS_MODE		m_ePressMode;
	fVector2			m_PushOffset;
	s32					m_uAccelerator[2];
	fVector2			m_ShadowOffset;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIButton)
#undef script_type_list
#define script_type_list save_type_list(CUIButton)
