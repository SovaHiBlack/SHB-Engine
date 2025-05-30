// File:		UILines.h
// Description:	Multilines Text Control
// Created:		11.03.2005
// Author:		Serge Vynnycheko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#pragma once

#include "UILine.h"
//#include "../UI.h"
#include "uiabstract.h"

class CUILines : public IUITextControl, public CUISimpleWindow, public CDeviceResetNotifier
{
	friend class CUICustomEdit;
	friend class CUICDkey;

public:
	CUILines( );
	virtual ~CUILines( );

	// IUITextControl methods
	virtual void			SetText(pcstr text);
	virtual pcstr		GetText( );
	virtual void			SetTextColor(u32 color);
	virtual u32				GetTextColor( )
	{
		return m_dwTextColor;
	}
	u32& GetTextColorRef( )
	{
		return m_dwTextColor;
	}
	virtual void			SetFont(CGameFont* pFont);
	virtual CGameFont* GetFont( )
	{
		return m_pFont;
	}
	virtual void			SetTextAlignment(ETextAlignment al)
	{
		m_eTextAlign = al;
	}
	virtual ETextAlignment	GetTextAlignment( )
	{
		return m_eTextAlign;
	}
	void			SetVTextAlignment(EVTextAlignment al)
	{
		m_eVTextAlign = al;
	}
	EVTextAlignment GetVTextAlignment( )
	{
		return m_eVTextAlign;
	}

	// additional
	void			SetCursorColor(u32 color)
	{
		m_dwCursorColor = color;
	}
	void			AddCharAtCursor(const char ch);
	void			DelChar( );
	void			DelLeftChar( );
	void			MoveCursorToEnd( );

	void			SetTextComplexMode(bool mode = true);
	bool			GetTextComplexMode( ) const;
	void			SetPasswordMode(bool mode = true);
	void			SetColoringMode(bool mode);
	void			SetCutWordsMode(bool mode);
	void			SetUseNewLineMode(bool mode);

	// IUISimpleWindow methods
	virtual void			Init(f32 x, f32 y, f32 width, f32 height);
	virtual void			Draw( );
	virtual void			Draw(f32 x, f32 y);
	virtual void			Update( );
	IC			void			SetWndSize_inline(const fVector2& wnd_size);

	// CDeviceResetNotifier methods
	virtual void			OnDeviceReset( );

	// own methods
	void			Reset( );
	void			ParseText( );
	f32				GetVisibleHeight( );

	// cursor control
	s32				m_iCursorPos;
	void			IncCursorPos( );
	void			DecCursorPos( );

protected:
	iVector2		m_cursor_pos;
	void			UpdateCursor( );
	// %c[255,255,255,255]
	u32					GetColorFromText(const xr_string& str) const;
	f32				GetIndentByAlign( ) const;
	f32				GetVIndentByAlign( );
	void				CutFirstColoredTextEntry(xr_string& entry, u32& color, xr_string& text) const;
	CUILine* ParseTextToColoredLine(const xr_string& str);

	// IUITextControl data
	typedef xr_string						Text;
	typedef xr_vector<CUILine>				LinesVector;
	typedef LinesVector::iterator			LinesVector_it;
	LinesVector				m_lines;	// parsed text
	f32					m_interval; // interval

	Text					m_text;

	ETextAlignment			m_eTextAlign;
	EVTextAlignment			m_eVTextAlign;
	u32						m_dwTextColor;
	u32						m_dwCursorColor;

	CGameFont* m_pFont;

	enum
	{
		flNeedReparse = (1 << 0),
		flComplexMode = (1 << 1),
		flPasswordMode = (1 << 2),
		flColoringMode = (1 << 3),
		flCutWordsMode = (1 << 4),
		flRecognizeNewLine = (1 << 5)
	};

private:
	flags8					uFlags;
	f32					m_oldWidth;
};

class CUILinesOwner : public IUITextControl
{
public:
	virtual					~CUILinesOwner( )
	{ }

	// IUIFontControl{
	virtual void			SetTextColor(u32 color)
	{
		m_lines.SetTextColor(color);
	}
	virtual u32				GetTextColor( )
	{
		return m_lines.GetTextColor( );
	}
	virtual void			SetFont(CGameFont* pFont)
	{
		m_lines.SetFont(pFont);
	}
	virtual CGameFont* GetFont( )
	{
		return m_lines.GetFont( );
	}
	virtual void			SetTextAlignment(ETextAlignment alignment)
	{
		m_lines.SetTextAlignment(alignment);
	}
	virtual ETextAlignment	GetTextAlignment( )
	{
		return m_lines.GetTextAlignment( );
	}

	// IUITextControl : public IUIFontControl{
	virtual void			SetText(pcstr text)
	{
		m_lines.SetText(text);
	}
	virtual pcstr		GetText( )
	{
		return m_lines.GetText( );
	}

	// own
	virtual void			SetTextPosX(f32 x)
	{
		m_textPos.x = x;
	}
	virtual void			SetTextPosY(f32 y)
	{
		m_textPos.y = y;
	}

protected:
	fVector2				m_textPos;
	CUILines				m_lines;
};
