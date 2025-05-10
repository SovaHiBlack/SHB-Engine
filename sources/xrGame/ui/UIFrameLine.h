//=============================================================================
//  Filename:   UIFrameLine.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Класс аналогичный UIFrameRect за исключением того, что он предназначен для
//	отображения затекстурированного узкого прямоуголника произвольной длинны или
//	ширины. В качестве исходных материалов необходимо 3 текстуры: правая(нижняя),
//	левая(верхняя) и центральная
//=============================================================================
#pragma once

#include "../UIStaticItem.h"

class CUIFrameLine : public CUICustomItem
{
	friend class CUIFrameLineWnd;
	enum
	{
		flFirst = 0,	// Left or top
		flSecond,		// Right or bottom
		flBack,			// Center texture
		flMax
	};

	// Drawing elements
	CUIStaticItem	elements[flMax];

	enum
	{
		flValidSize = 1
	};

protected:
	f32		iSize;
	fVector2	iPos;
	u8			uFlags;
	bool		bHorizontalOrientation;

	void		UpdateSize( );

public:
	CUIFrameLine( );
	void		Init(pcstr base_name, f32 x, f32 y, f32 size, bool horizontal, DWORD align);
	void		InitTexture(pcstr texture);
	void		SetColor(u32 cl);
	IC void		SetPos(f32 left, f32 top)
	{
		iPos.set(left, top);
		uFlags &= ~flValidSize;
	}
	IC void		SetSize(f32 size)
	{
		iSize = size;
		uFlags &= ~flValidSize;
	}
	IC void		SetOrientation(bool bIsHorizontal)
	{
		bHorizontalOrientation = bIsHorizontal;
		uFlags &= ~flValidSize;
	}
	void		Render( );
};
