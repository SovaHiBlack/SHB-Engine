// File:		UISubLine.cpp
// Description:	Text line. Owns color attribute


#include "stdafx.h"
#include "UISubLine.h"
//#include "uilinestd.h"
#include "ui_base.h"

//#define LOG_ALL_LINES
#ifdef LOG_ALL_LINES
s32 ListSubLinesCount = 0;
struct DBGList
{
	CUISubLine* wnd;
	s32 num;
};
xr_vector<DBGList>	dbg_list_sublines;
void dump_list_sublines( )
{
	Msg("------Total  SubLines %d", dbg_list_sublines.size( ));
	xr_vector<DBGList>::iterator _it = dbg_list_sublines.begin( );
	for (; _it != dbg_list_sublines.end( ); ++_it)
	{
		Msg("--leak detected ---- SubLine = %d", (*_it).num);
	}
}
#else
void dump_list_sublines( )
{ }
#endif

CUISubLine::CUISubLine(const CUISubLine& other)
{
	m_color = other.m_color;
	m_last_in_line = other.m_last_in_line;
	m_text = other.m_text;
	m_pTempLine = NULL;

#ifdef LOG_ALL_LINES
	ListSubLinesCount++;
	dbg_list_sublines.push_back(DBGList( ));
	dbg_list_sublines.back( ).wnd = this;
	dbg_list_sublines.back( ).num = ListSubLinesCount;
#endif

}

CUISubLine& CUISubLine::operator=(const CUISubLine& other)
{
	m_color = other.m_color;
	m_text = other.m_text;
	m_last_in_line = other.m_last_in_line;
	xr_delete(m_pTempLine);
	return (*this);
}

CUISubLine::CUISubLine( )
{
	m_color = 0;
	m_pTempLine = NULL;
	m_last_in_line = false;

#ifdef LOG_ALL_LINES
	ListSubLinesCount++;
	dbg_list_sublines.push_back(DBGList( ));
	dbg_list_sublines.back( ).wnd = this;
	dbg_list_sublines.back( ).num = ListSubLinesCount;
#endif

}

CUISubLine::~CUISubLine( )
{
	xr_delete(m_pTempLine);
	m_pTempLine = NULL;

#ifdef LOG_ALL_LINES
	xr_vector<DBGList>::iterator _it = dbg_list_sublines.begin( );
	bool bOK = false;
	for (; _it != dbg_list_sublines.end( ); ++_it)
	{
		if ((*_it).wnd == this)
		{
			bOK = true;
			dbg_list_sublines.erase(_it);
			break;
		}
	}

	if (!bOK)
	{
		Msg("CUISubLine::~CUISubLine()!!!!!!!!!!!!!!!!!!!!!!! cannot find window in list");
	}
#endif

}

const CUISubLine* CUISubLine::Cut2Pos(s32 i)
{
	R_ASSERT2(i < (s32)m_text.size( ), make_string("CUISubLine::Cut2Pos - invalid parameter [%d][%d]", i, m_text.size( )).c_str( ));

	if (!m_pTempLine)
	{
		m_pTempLine = xr_new<CUISubLine>( );
	}

	m_pTempLine->m_color = m_color;
	m_pTempLine->m_text.assign(m_text, 0, i + 1);
	m_text.replace(0, i + 1, "");

	return m_pTempLine;
}

void CUISubLine::FreeBuffer( )
{ }

void CUISubLine::Draw(CGameFont* pFont, f32 x, f32 y) const
{
	pFont->SetColor(m_color);
	fVector2 pos;
	pos.set(x, y);
	UI( )->ClientToScreenScaled(pos);
	pFont->Out(pos.x, pos.y, "%s", m_text.c_str( ));
}
