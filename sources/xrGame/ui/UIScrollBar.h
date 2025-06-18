#pragma once
#include "UIWindow.h"

class CUI3tButton;
class CUIScrollBox;
class CUIStaticItem;

class CUIScrollBar : public CUIWindow
{
private:
	typedef CUIWindow inherited;

protected:
	CUI3tButton* m_DecButton;
	CUI3tButton* m_IncButton;

	CUIScrollBox* m_ScrollBox;

	CUIStaticItem* m_StaticBackground;

	s32				m_iScrollPos;

	s32				m_iStepSize;

	s32				m_iMinPos;
	s32				m_iMaxPos;

	s32				m_iPageSize;

	s32				m_ScrollWorkArea;
	bool			m_b_enabled;
	bool			m_bIsHorizontal;

	bool			ScrollInc( );
	bool			ScrollDec( );
	void			UpdateScrollBar( );

	u32				ScrollSize( )
	{
		return _max(1, m_iMaxPos - m_iMinPos - m_iPageSize + 1);
	}
	void			ClampByViewRect( );
	void			SetPosScrollFromView(f32 view_pos, f32 view_width, f32 view_offs);
	s32				PosViewFromScroll(s32 view_size, s32 view_offs);
	void			SetScrollPosClamped(s32 iPos)
	{
		m_iScrollPos = iPos;
		clamp(m_iScrollPos, m_iMinPos, m_iMaxPos - m_iPageSize + 1);
	}

public:
	CUIScrollBar( );
	virtual			~CUIScrollBar( );

	void	SetEnabled(bool b)
	{
		m_b_enabled = b;
		if (!m_b_enabled)
		{
			Show(m_b_enabled);
		}
	}
	bool	GetEnabled( )
	{
		return m_b_enabled;
	}
	virtual void	Show(bool b);
	virtual void	Enable(bool b);
	virtual void	Init(f32 x, f32 y, f32 length, bool bIsHorizontal, pcstr profile = "default");

	virtual void	SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData);
	virtual bool	OnMouse(f32 x, f32 y, EUIMessages mouse_action);
	virtual bool	OnKeyboardHold(s32 dik);

	virtual void	Draw( );

	virtual void	SetWidth(f32 width);
	virtual void	SetHeight(f32 height);

	virtual void	Reset( );
	void			Refresh( );
	void			SetStepSize(s32 step);
	void			SetRange(s32 iMin, s32 iMax);
	void			GetRange(s32& iMin, s32& iMax)
	{
		iMin = m_iMinPos;
		iMax = m_iMaxPos;
	}
	s32			GetMaxRange( )
	{
		return m_iMaxPos;
	}
	s32			GetMinRange( )
	{
		return m_iMinPos;
	}

	void			SetPageSize(s32 iPage)
	{
		m_iPageSize = _max(0, iPage);
		UpdateScrollBar( );
	}
	s32				GetPageSize( )
	{
		return m_iPageSize;
	}

	void			SetScrollPos(s32 iPos)
	{
		SetScrollPosClamped(iPos);
		UpdateScrollBar( );
	}
	s32				GetScrollPos( )
	{
		return _max(m_iMinPos, m_iScrollPos);
	}

	void			TryScrollInc( );
	void			TryScrollDec( );
};
