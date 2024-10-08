//=============================================================================
//  Filename:   UIFrameLine.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ����� ����������� UIFrameRect �� ����������� ����, ��� �� ������������ ���
//	����������� ������������������� ������ ������������� ������������ ������ ���
//	������. � �������� �������� ���������� ���������� 3 ��������: ������(������),
//	�����(�������) � �����������
//=============================================================================
#pragma once

#include "../UIStaticItem.h"

class CUIFrameLine: public CUICustomItem
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
	u8			uFlags;
	bool		bHorizontalOrientation;

	void		UpdateSize		();
public:
				CUIFrameLine	();
	void		Init			(pcstr base_name, float x, float y, float size, bool horizontal, DWORD align);
	void		InitTexture		(pcstr texture);
	void		SetColor		(u32 cl);
	IC void		SetPos			(float left, float top)		{ iPos.set(left,top);	uFlags &=~ flValidSize; }
	IC void		SetSize			(float size)				{ iSize = size;			uFlags &=~ flValidSize; }
	IC void		SetOrientation	(bool bIsHorizontal)	{ bHorizontalOrientation = bIsHorizontal; uFlags &=~ flValidSize; }
	void		Render			();
};
