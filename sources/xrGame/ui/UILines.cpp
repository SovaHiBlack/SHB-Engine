// File:		UILines.cpp
// Description:	Multi-line Text Control
// Created:		12.03.2005
// Author:		Serge Vynnycheko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#include "stdafx.h"

#include "UILines.h"
#include "../HUDmanager.h"
#include "UIXmlInit.h"
#include "uilinestd.h"

CUILines::CUILines( )
{
	m_pFont = NULL;
	m_interval = 0.0f;
	m_eTextAlign = CGameFont::alLeft;
	m_eVTextAlign = valTop;
	m_dwTextColor = 0xffffffff;
	m_dwCursorColor = 0xAAFFFF00;

	m_bShowMe = true;
	uFlags.zero( );
	uFlags.set(flNeedReparse, FALSE);
	uFlags.set(flComplexMode, FALSE);
	uFlags.set(flPasswordMode, FALSE);
	uFlags.set(flColoringMode, TRUE);
	uFlags.set(flCutWordsMode, FALSE);
	uFlags.set(flRecognizeNewLine, TRUE);
	m_pFont = UI( )->Font( )->pFontLetterica16Russian;
	m_cursor_pos.set(0, 0);
	m_iCursorPos = 0;
}

CUILines::~CUILines( )
{ }

void CUILines::SetTextComplexMode(bool mode)
{
	uFlags.set(flComplexMode, mode);
	if (mode)
	{
		uFlags.set(flPasswordMode, FALSE);
	}
}

bool CUILines::GetTextComplexMode( ) const
{
	return (uFlags.test(flComplexMode) ? true : false);
}

void CUILines::SetPasswordMode(bool mode)
{
	uFlags.set(flPasswordMode, mode);
	if (mode)
	{
		uFlags.set(flComplexMode, false);
	}
}

void CUILines::SetColoringMode(bool mode)
{
	uFlags.set(flColoringMode, mode);
}

void CUILines::SetCutWordsMode(bool mode)
{
	uFlags.set(flCutWordsMode, mode);
}

void CUILines::SetUseNewLineMode(bool mode)
{
	uFlags.set(flRecognizeNewLine, mode);
}

void CUILines::Init(f32 x, f32 y, f32 width, f32 heigt)
{
	CUISimpleWindow::Init(x, y, width, heigt);
}

void CUILines::SetText(pcstr text)
{
	if (!m_pFont)
	{
		m_pFont = UI( )->Font( )->pFontLetterica16Russian;
	}

	if (text && text[0] != 0)
	{
		if (m_text == text)
		{
			return;
		}

		m_text = text;

		uFlags.set(flNeedReparse, TRUE);
	}
	else
	{
		m_text = "";
		Reset( );
	}

	MoveCursorToEnd( );
}

void CUILines::AddCharAtCursor(const char ch)
{
	uFlags.set(flNeedReparse, TRUE);
	m_text.insert(m_text.begin( ) + m_iCursorPos, ch);
	IncCursorPos( );
}

void CUILines::MoveCursorToEnd( )
{
	m_iCursorPos = (s32)m_text.size( );
}

void CUILines::DelChar( )
{
	const s32 sz = (s32)m_text.size( );
	if (m_iCursorPos < sz)
	{
		m_text.erase(m_text.begin( ) + m_iCursorPos);
		uFlags.set(flNeedReparse, TRUE);
	}
}

void CUILines::DelLeftChar( )
{
	if (m_iCursorPos > 0)
	{
		DecCursorPos( );
		DelChar( );
	}
}

pcstr CUILines::GetText( )
{
	return m_text.c_str( );
}

void CUILines::Reset( )
{
	m_lines.clear( );
}

f32 get_str_width(CGameFont* pFont, char ch)
{
	f32 ll = pFont->SizeOf_(ch);
	UI( )->ClientToScreenScaledWidth(ll);
	return ll;
}

void CUILines::ParseText( )
{
	if (!fsimilar(m_oldWidth, m_wndSize.x))
	{
		uFlags.set(flNeedReparse, TRUE);
		m_oldWidth = m_wndSize.x;
	}

	if (!uFlags.test(flComplexMode) || !uFlags.test(flNeedReparse))
	{
		return;
	}

	if (NULL == m_pFont)
	{
		return;
	}

	Reset( );
	if (!m_text.empty( ) && NULL == m_pFont)
	{
		R_ASSERT2(false, "can't parse text without font");
	}

	CUILine* line = NULL;
	if (uFlags.test(flColoringMode))
	{
		line = ParseTextToColoredLine(m_text.c_str( ));
	}
	else
	{
		line = xr_new<CUILine>( );
		CUISubLine subline;
		subline.m_text = m_text;
		subline.m_color = GetTextColor( );
		line->AddSubLine(&subline);
	}

	BOOL bNewLines = FALSE;

	if (uFlags.test(flRecognizeNewLine))
	{
		if (m_pFont->IsMultibyte( ))
		{
			CUILine* ptmp_line = xr_new<CUILine>( );
			s32 vsz = line->m_subLines.size( );
			VERIFY(vsz);
			for (s32 i = 0; i < vsz; i++)
			{
				pstr pszTemp = NULL;
				const u32 tcolor = line->m_subLines[i].m_color;
				char szTempLine[MAX_MB_CHARS];
				pstr pszSearch = NULL;
				size_t llen = xr_strlen(line->m_subLines[i].m_text.c_str( ));
				VERIFY(llen < MAX_MB_CHARS);
				strcpy(szTempLine, line->m_subLines[i].m_text.c_str( ));
				pszSearch = szTempLine;
				while ((pszTemp = strstr(pszSearch, "\\n")) != NULL)
				{
					bNewLines = TRUE;
					*pszTemp = '\0';
					ptmp_line->AddSubLine(pszSearch, tcolor);
					pszSearch = pszTemp + 2;
				}

				ptmp_line->AddSubLine(pszSearch, tcolor);
			}

			line->Clear( );
			xr_free(line);
			line = ptmp_line;
		}
		else
		{	// process "\n"
			line->ProcessNewLines( );
		}
	}

	if (m_pFont->IsMultibyte( ))
	{
#define UBUFFER_SIZE 100
		u16	aMarkers[UBUFFER_SIZE];
		CUILine tmp_line;
		char szTempLine[MAX_MB_CHARS];
		f32 fTargetWidth = 1.0f;
		UI( )->ClientToScreenScaledWidth(fTargetWidth);
		VERIFY((m_wndSize.x > 0) && (fTargetWidth > 0));
		fTargetWidth = m_wndSize.x / fTargetWidth;
		s32 vsz = line->m_subLines.size( );
		VERIFY(vsz);
		if ((vsz > 1) && (!bNewLines))
		{	// only colored line, pizdets
			for (s32 i = 0; i < vsz; i++)
			{
				pcstr pszText = line->m_subLines[i].m_text.c_str( );
				const u32 tcolor = line->m_subLines[i].m_color;
				VERIFY(pszText);
				tmp_line.AddSubLine(pszText, tcolor);
			}

			m_lines.push_back(tmp_line);
			tmp_line.Clear( );
		}
		else
		{
			for (s32 i = 0; i < vsz; i++)
			{
				pcstr pszText = line->m_subLines[i].m_text.c_str( );
				const u32 tcolor = line->m_subLines[i].m_color;
				u16 uFrom = 0;
				u16 uPartLen = 0;
				VERIFY(pszText);
				u16 nMarkers = m_pFont->SplitByWidth(aMarkers, UBUFFER_SIZE, fTargetWidth, pszText);
				for (u16 j = 0; j < nMarkers; j++)
				{
					uPartLen = aMarkers[j] - uFrom;
					VERIFY((uPartLen > 0) && (uPartLen < MAX_MB_CHARS));
					strncpy(szTempLine, pszText + uFrom, uPartLen);
					szTempLine[uPartLen] = '\0';
					tmp_line.AddSubLine(szTempLine, tcolor);
					m_lines.push_back(tmp_line);
					tmp_line.Clear( );
					// Compiler bug :)
#pragma warning( disable : 4244 )
					uFrom += uPartLen;
#pragma warning( default : 4244 )
				}

				strncpy(szTempLine, pszText + uFrom, MAX_MB_CHARS);
				tmp_line.AddSubLine(szTempLine, tcolor);
				m_lines.push_back(tmp_line);
				tmp_line.Clear( );
			}
		}
	}
	else
	{
		f32 max_width = m_wndSize.x;
		u32 sbl_cnt = line->m_subLines.size( );
		CUILine tmp_line;
		string4096 buff;
		f32 curr_width = 0.0f;
		bool bnew_line = false;
		f32 __eps = get_str_width(m_pFont, 'o');//hack -(
		for (u32 sbl_idx = 0; sbl_idx < sbl_cnt; ++sbl_idx)
		{
			bool b_last_subl = (sbl_idx == sbl_cnt - 1);
			CUISubLine& sbl = line->m_subLines[sbl_idx];
			//.			Msg("%s",sbl.m_text.c_str());
			u32 sub_len = (u32)sbl.m_text.length( );
			u32 curr_w_pos = 0;

			u32 last_space_idx = 0;
			for (u32 idx = 0; idx < sub_len; ++idx)
			{
				bool b_last_ch = (idx == sub_len - 1);

				if (isspace(sbl.m_text[idx]))
				{
					last_space_idx = idx;
				}

				f32 w1 = get_str_width(m_pFont, sbl.m_text[idx]);
				bool bOver = (curr_width + w1 + __eps > max_width);
				if (bOver || b_last_ch)
				{
					if (last_space_idx && !b_last_ch)
					{
						idx = last_space_idx;
						last_space_idx = 0;
					}

					strncpy_s(buff, sizeof(buff), sbl.m_text.c_str( ) + curr_w_pos, idx - curr_w_pos + 1);
					//.					Msg					("-%s",buff);
					tmp_line.AddSubLine(buff, sbl.m_color);
					curr_w_pos = idx + 1;
				}
				else
				{
					curr_width += w1;
				}

				if (bOver || (b_last_ch && sbl.m_last_in_line))
				{
					m_lines.push_back(tmp_line);
					tmp_line.Clear( );
					curr_width = 0.0f;
					bnew_line = false;
				}
			}

			if (b_last_subl && !tmp_line.IsEmpty( ))
			{
				m_lines.push_back(tmp_line);
				tmp_line.Clear( );
				curr_width = 0.0f;
				bnew_line = false;
			}
		}
	}

	xr_delete(line);
	uFlags.set(flNeedReparse, FALSE);
}

f32 CUILines::GetVisibleHeight( )
{
	f32 _curr_h = m_pFont->CurrentHeight_( );
	UI( )->ClientToScreenScaledHeight(_curr_h);

	if (uFlags.test(flComplexMode))
	{
		if (uFlags.test(flNeedReparse))
		{
			ParseText( );
		}

		return (_curr_h + m_interval) * m_lines.size( ) - m_interval;
	}
	else
	{
		return _curr_h;
	}
}

void CUILines::SetTextColor(u32 color)
{
	if (color == m_dwTextColor)
	{
		return;
	}

	uFlags.set(flNeedReparse, true);
	m_dwTextColor = color;
}

void CUILines::SetFont(CGameFont* pFont)
{
	if (pFont == m_pFont)
	{
		return;
	}

	uFlags.set(flNeedReparse, true);
	m_pFont = pFont;
}

void CUILines::Draw(f32 x, f32 y)
{
	static string256 passText;

	if (m_text.empty( ))
	{
		return;
	}

	R_ASSERT(m_pFont);
	m_pFont->SetColor(m_dwTextColor);

	if (!uFlags.is(flComplexMode))
	{
		fVector2 text_pos;
		text_pos.set(0.0f, 0.0f);

		text_pos.x = x + GetIndentByAlign( );
		text_pos.y = y + GetVIndentByAlign( );
		UI( )->ClientToScreenScaled(text_pos);

		if (uFlags.test(flPasswordMode))
		{
			s32 sz = (s32)m_text.size( );
			for (s32 i = 0; i < sz; i++)
			{
				passText[i] = '*';
			}

			passText[sz] = 0;
			m_pFont->SetAligment((CGameFont::EAligment)m_eTextAlign);
			m_pFont->Out(text_pos.x, text_pos.y, "%s", passText);
		}
		else
		{
			m_pFont->SetAligment((CGameFont::EAligment)m_eTextAlign);
			m_pFont->Out(text_pos.x, text_pos.y, "%s", m_text.c_str( ));
		}
	}
	else
	{
		ParseText( );

		fVector2 pos;
		// get vertical indent
		pos.y = y + GetVIndentByAlign( );
		f32 height = m_pFont->CurrentHeight_( );
		UI( )->ClientToScreenScaledHeight(height);

		u32 size = m_lines.size( );

		m_pFont->SetAligment((CGameFont::EAligment)m_eTextAlign);
		for (s32 i = 0; i < (s32)size; i++)
		{
			pos.x = x + GetIndentByAlign( );
			m_lines[i].Draw(m_pFont, pos.x, pos.y);
			pos.y += height + m_interval;
		}
	}

	m_pFont->OnRender( );
}

void CUILines::Draw( )
{
	fVector2 p = GetWndPos( );
	Draw(p.x, p.y);
}

void CUILines::Update( )
{ }

void CUILines::OnDeviceReset( )
{
	uFlags.set(flNeedReparse, TRUE);
}

f32 CUILines::GetIndentByAlign( )const
{
	switch (m_eTextAlign)
	{
		case CGameFont::alCenter:
		{
			return (m_wndSize.x /*- length*/) / 2;
		}
		break;
		case CGameFont::alLeft:
		{
			return 0;
		}
		break;
		case CGameFont::alRight:
		{
			return (m_wndSize.x /*- length*/);
		}
		break;
		default:
		{
			NODEFAULT;
		}
	}

#ifdef DEBUG
	return 0;
#endif

}

f32 CUILines::GetVIndentByAlign( )
{
	switch (m_eVTextAlign)
	{
		case valTop:
		{
			return 0;
		}
		break;
		case valCenter:
		{
			return (m_wndSize.y - GetVisibleHeight( )) / 2;
		}
		break;
		case valBotton:
		{
			return m_wndSize.y - GetVisibleHeight( );
		}
		break;
		default:
		{
			NODEFAULT;
		}
	}

#ifdef DEBUG
	return 0;
#endif

}

u32 CUILines::GetColorFromText(const xr_string& str)const
{
	StrSize begin;
	StrSize end;
	StrSize comma1_pos;
	StrSize comma2_pos;
	StrSize comma3_pos;

	begin = str.find(BEGIN);
	end = str.find(END, begin);
	R_ASSERT2(npos != begin, "CUISubLine::GetColorFromText -- can't find beginning tag %c[");
	R_ASSERT2(npos != end, "CUISubLine::GetColorFromText -- can't find ending tag ]");

	// try default color
	if (npos != str.find("%c[default]", begin, end - begin))
	{
		return m_dwTextColor;
	}

	// Try predefined in XML colors
	for (CUIXmlInit::ColorDefs::const_iterator it = CUIXmlInit::GetColorDefs( )->begin( ); it != CUIXmlInit::GetColorDefs( )->end( ); ++it)
	{
		s32 cmp = str.compare(begin + 3, end - begin - 3, *it->first);
		if (cmp == 0)
		{
			return it->second;
		}
	}

	// try parse values separated by commas
	comma1_pos = str.find(",", begin);
	comma2_pos = str.find(",", comma1_pos + 1);
	comma3_pos = str.find(",", comma2_pos + 1);

	R_ASSERT2(npos != comma1_pos, "CUISubLine::GetColorFromText -- can't find first comma");
	R_ASSERT2(npos != comma2_pos, "CUISubLine::GetColorFromText -- can't find second comma");
	R_ASSERT2(npos != comma3_pos, "CUISubLine::GetColorFromText -- can't find third comma");

	u32 a;
	u32 r;
	u32 g;
	u32 b;
	xr_string single_color;

	begin += 3;

	single_color = str.substr(begin, comma1_pos - 1);
	a = atoi(single_color.c_str( ));
	single_color = str.substr(comma1_pos + 1, comma2_pos - 1);
	r = atoi(single_color.c_str( ));
	single_color = str.substr(comma2_pos + 1, comma3_pos - 1);
	g = atoi(single_color.c_str( ));
	single_color = str.substr(comma3_pos + 1, end - 1);
	b = atoi(single_color.c_str( ));

	return color_argb(a, r, g, b);
}

CUILine* CUILines::ParseTextToColoredLine(const xr_string& str)
{
	CUILine* line = xr_new<CUILine>( );
	xr_string tmp = str;
	xr_string entry;
	u32 color;

	do
	{
		CutFirstColoredTextEntry(entry, color, tmp);
		line->AddSubLine(entry, subst_alpha(color, color_get_A(GetTextColor( ))));
	}
	while (tmp.size( ) > 0);

	return line;
}

void CUILines::CutFirstColoredTextEntry(xr_string& entry, u32& color, xr_string& text) const
{
	entry.clear( );

	StrSize begin = text.find(BEGIN);
	StrSize end = text.find(END, begin);
	if (xr_string::npos == end)
	{
		begin = end;
	}

	StrSize begin2 = text.find(BEGIN, end);
	StrSize end2 = text.find(END, begin2);
	if (xr_string::npos == end2)
	{
		begin2 = end2;
	}

	// if we do not have any color entry or it is single with 0 position
	if (xr_string::npos == begin)
	{
		entry = text;
		color = m_dwTextColor;
		text.clear( );
	}
	else if (0 == begin && xr_string::npos == begin2)
	{
		entry = text;
		color = GetColorFromText(entry);
		entry.replace(begin, end - begin + 1, "");
		text.clear( );
	}
	// if we have color entry not at begin
	else if (0 != begin)
	{
		entry = text.substr(0, begin);
		color = m_dwTextColor;
		text.replace(0, begin, "");
	}
	// if we have two color entries. and first has 0 position
	else if (0 == begin && xr_string::npos != begin2)
	{
		entry = text.substr(0, begin2);
		color = GetColorFromText(entry);
		entry.replace(begin, end - begin + 1, "");
		text.replace(0, begin2, "");
	}
}

void CUILines::SetWndSize_inline(const fVector2& wnd_size)
{
	m_wndSize = wnd_size;
}

void CUILines::IncCursorPos( )
{
	const s32 txt_len = (s32)m_text.size( );
	if (0 == txt_len)
	{
		return;
	}

	if (m_iCursorPos < txt_len)
	{
		m_iCursorPos++;
	}

	return;
}

void CUILines::DecCursorPos( )
{
	const s32 txt_len = (s32)m_text.size( );
	if (0 == txt_len)
	{
		return;
	}

	if (m_iCursorPos > 0)
	{
		m_iCursorPos--;
	}

	return;
}

void CUILines::UpdateCursor( )
{
	if (uFlags.test(flComplexMode) && !m_text.empty( ))
	{
		ParseText( );
		const s32 sz = (s32)m_lines.size( );
		s32 len = 0;
		for (s32 i = 0; i < sz; i++)
		{
			s32 curlen = m_lines[i].GetSize( );
			if (m_iCursorPos <= len + curlen)
			{
				m_cursor_pos.y = i;
				m_cursor_pos.x = m_iCursorPos - len;
				return;
			}

			len += curlen;
		}

		R_ASSERT(false);
	}
	else
	{
		m_cursor_pos.y = 0;
		m_cursor_pos.x = m_iCursorPos;
	}
}
