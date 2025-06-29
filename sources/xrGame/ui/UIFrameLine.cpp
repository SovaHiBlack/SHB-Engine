//=============================================================================
//  Filename:   UIFrameLine.cpp

//  ����� ����������� UIFrameRect �� ����������� ����, ��� �� ������������ ���
//	����������� ������������������� ������ ������������� ������������ ������ ���
//	������. � �������� �������� ���������� ���������� 3 ��������: ������(������),
//	�����(�������) � �����������
//=============================================================================

#include "stdafx.h"
#include "UIFrameLine.h"
#include "UITextureMaster.h"

CUIFrameLine::CUIFrameLine( ) : uFlags(0), iSize(0), bHorizontalOrientation(true)
{
	iPos.set(0.0f, 0.0f);
}

void CUIFrameLine::Init(pcstr base_name, f32 x, f32 y, f32 size, bool horizontal, DWORD align)
{
	SetPos(x, y);
	SetSize(size);
	SetAlign(align);
	SetOrientation(horizontal);

	InitTexture(base_name);
}

void CUIFrameLine::InitTexture(pcstr texture)
{
	string256 buf;

	CUITextureMaster::InitTexture(strconcat(sizeof(buf), buf, texture, "_back"), &elements[flBack]);
	CUITextureMaster::InitTexture(strconcat(sizeof(buf), buf, texture, "_b"), &elements[flFirst]);
	CUITextureMaster::InitTexture(strconcat(sizeof(buf), buf, texture, "_e"), &elements[flSecond]);
}

void CUIFrameLine::SetColor(u32 cl)
{
	for (s32 i = 0; i < flMax; ++i)
	{
		elements[i].SetColor(cl);
	}
}

void CUIFrameLine::UpdateSize( )
{
	VERIFY(g_bRendering);

	f32 f_width = elements[flFirst].GetOriginalRect( ).width( );
	f32 f_height = elements[flFirst].GetOriginalRect( ).height( );
	elements[flFirst].SetPos(iPos.x, iPos.y);

	// Right or bottom texture
	f32 s_width = elements[flSecond].GetOriginalRect( ).width( );
	f32 s_height = elements[flSecond].GetOriginalRect( ).height( );

	(bHorizontalOrientation) ? elements[flSecond].SetPos(iPos.x + iSize - s_width, iPos.y) : elements[flSecond].SetPos(iPos.x, iPos.y + iSize - s_height);

	// Dimentions of element textures must be the same
	if (bHorizontalOrientation)
	{
		R_ASSERT(s_height == f_height);
	}
	else
	{
		R_ASSERT(f_width == s_width);
	}

	// Now stretch back texture to remaining space
	f32 back_width;
	f32 back_height;

	if (bHorizontalOrientation)
	{
		back_width = iSize - f_width - s_width;
		back_height = f_height;

		// Size of frameline must be equal or greater than sum of size of two side textures
		R_ASSERT(back_width > 0.0f);
	}
	else
	{
		back_width = f_width;
		back_height = iSize - f_height - s_height;

		// Size of frameline must be equal or greater than sum of size of two side textures
		R_ASSERT(back_height > 0.0f);
	}

	// Now resize back texture
	f32 rem;
	s32 tile;

	f32 b_width = elements[flBack].GetOriginalRect( ).width( );
	f32 b_height = elements[flBack].GetOriginalRect( ).height( );

	if (bHorizontalOrientation)
	{
		rem = fmod(back_width, b_width);
		tile = iFloor(back_width / b_width);
		elements[flBack].SetPos(iPos.x + f_width, iPos.y);
		elements[flBack].SetTile(tile, 1, rem, 0.0f);
	}
	else
	{
		rem = fmod(back_height, b_height);
		tile = iFloor(back_height / b_height);
		elements[flBack].SetPos(iPos.x, iPos.y + f_height);
		elements[flBack].SetTile(1, tile, 0.0f, rem);
	}

	uFlags |= flValidSize;
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLine::Render( )
{
	// If size changed - update size
	if (!(uFlags & flValidSize))
	{
		UpdateSize( );
	}
	// Now render all statics
	for (s32 i = 0; i < flMax; ++i)
	{
		elements[i].Render( );
	}
}
