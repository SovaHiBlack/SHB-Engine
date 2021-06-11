//  Filename:   UIFrameLine.h
//  Класс аналогичный UIFrameRect за исключением того, что он предназначен для
//	отображения затекстурированного узкого прямоуголника произвольной длинны или
//	ширины. В качестве исходных материалов необходимо 3 текстуры: правая(нижняя),
//	левая(верхняя) и центральная

#pragma once

#include "UIStaticItem.h"//

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
	float		iSize;
	Fvector2	iPos;
	U8			uFlags;
	bool		bHorizontalOrientation;

	void		UpdateSize( );

public:
	CUIFrameLine( );
	void		Init(const char* base_name, float x, float y, float size, bool horizontal, DWORD align);
	void		InitTexture(const char* texture);
	void		SetColor(u32 cl);
	inline void		SetPos(float left, float top)
	{
		iPos.set(left, top);
		uFlags &= ~flValidSize;
	}
	inline void		SetSize(float size)
	{
		iSize = size;
		uFlags &= ~flValidSize;
	}
	inline void		SetOrientation(bool bIsHorizontal)
	{
		bHorizontalOrientation = bIsHorizontal;
		uFlags &= ~flValidSize;
	}
	void		Render( );
};
