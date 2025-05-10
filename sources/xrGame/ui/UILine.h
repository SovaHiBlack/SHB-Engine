// File:		UILine.h
// Description:	Single text line
// Created:		11.03.2005
// Author:		Serge Vynnycheko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#pragma once

#include "UISubLine.h"
#include "../HUDManager.h"
#include "UIColorAnimatorWrapper.h"//

class Word
{
public:
	Word( )
	{
		len = 0;
		len_full = 0;
		pos = 0;
	}

	s32 len;
	s32 len_full;
	s32 pos;
	IC	s32  last_symbol( )
	{
		return pos + len - 1;
	}
	IC	s32  last_space( )
	{
		return pos + len_full - 1;
	}
	IC	bool exist( )
	{
		return len > 0;
	}
};

class Position
{
public:
	Word word_1;
	Word word_2;
	u32  curr_subline;
	u32  slash_n_size( )
	{
		return 2;
	}
	IC	bool is_separated( )
	{
		return ((0 == word_2.pos) || (word_1.pos >= word_2.pos));
	}
};

// Attention! Destructor is not virtual.
// if you want to inherit this class then make _coresponding_ changes
class CUILine
{
	friend class CUILines;

public:
	CUILine( );
	~CUILine( );
	CUILine(const CUILine& other);
	CUILine& operator=				(const CUILine& other);
	void 			AddSubLine(const xr_string& str, u32 color);
	void 			AddSubLine(pcstr str, u32 color);
	void 			AddSubLine(const CUISubLine* subLine);
	void 			Clear( );
	void 			ProcessNewLines( );
	const CUILine* CutByLength(CGameFont* pFont, f32 length, BOOL cut_word);

	void 				Draw(CGameFont* pFont, f32 x, f32 y) const;
	bool				IsEmpty( )
	{
		return m_subLines.empty( );
	}

protected:
	s32				GetSize( );
	bool			GetWord(Word& w, const xr_string& text, s32 begin) const;
	const CUILine* Cut2Pos(Position& pos, bool to_first = true);
	const CUILine* CutWord(CGameFont* pFont, f32 length);
	const CUILine* GetEmptyLine( );
	f32			GetLength_inclusiveWord_1(Position& pos, CGameFont* pFont) const;
	f32			GetLength_inclusiveWord_2(Position& pos, CGameFont* pFont) const;
	bool			InitPos(Position& pos) const;
	bool			IncPos(Position& pos) const;

	xr_vector<CUISubLine> m_subLines;

	static CUIColorAnimatorWrapper m_animation;

	CUILine* m_tmpLine;
};
