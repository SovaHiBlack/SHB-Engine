// CUIEditBox.cpp: ввод строки с клавиатуры

#include "stdafx.h"
#include "UIEditBox.h"

CUIEditBox::CUIEditBox( )
{
	AttachChild(&m_frameLine);
	m_lines.SetTextComplexMode(false);
}

CUIEditBox::~CUIEditBox( )
{ }

void CUIEditBox::Init(f32 x, f32 y, f32 width, f32 height)
{
	m_frameLine.Init(0.0f, 0.0f, width, height);
	CUICustomEdit::Init(x, y, width, height);
}

void CUIEditBox::InitTexture(pcstr texture)
{
	m_frameLine.InitTexture(texture);
}

void CUIEditBox::SetCurrentValue( )
{
	SetText(GetOptStringValue( ));
}

void CUIEditBox::SaveValue( )
{
	CUIOptionsItem::SaveValue( );
	SaveOptStringValue(GetText( ));
}

bool CUIEditBox::IsChanged( )
{
	return (0 != xr_strcmp(GetOptStringValue( ), GetText( )));
}
