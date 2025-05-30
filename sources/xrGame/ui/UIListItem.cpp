//////////////////////////////////////////////////////////////////////
// UIListItem.cpp: ������� ���� ������ CListWnd
//////////////////////////////////////////////////////////////////////

#include"stdafx.h"

#include "UIlistitem.h"

CUIListItem::CUIListItem( )
{
	m_eButtonState = BUTTON_NORMAL;
	m_ePressMode = NORMAL_PRESS;

	m_bButtonClicked = false;

	m_pData = NULL;

	m_iIndex = -1;
	m_iValue = 0;
	m_bHighlightText = false;
	m_iGroupID = -1;
	SetAutoDelete(true);
	SetTextAlignment(CGameFont::alLeft);
}

CUIListItem::~CUIListItem( )
{ }

void CUIListItem::Init(f32 x, f32 y, f32 width, f32 height)
{
	inherited::Init(x, y, width, height);
	SetPressMode(CUIButton::DOWN_PRESS);
	SetPushOffset(fVector2( ).set(0.0f, 0.0f));
}

void CUIListItem::InitTexture(pcstr tex_name)
{
	CUIButton::InitTexture(tex_name);
	SetTextX(m_UIStaticItem.GetRect( ).width( ));
}

void CUIListItem::Init(pcstr str, f32 x, f32 y, f32 width, f32 height)
{
	Init(x, y, width, height);
	SetTextST(str);
}

bool CUIListItem::IsHighlightText( )
{
	return CUIButton::IsHighlightText( );
}
