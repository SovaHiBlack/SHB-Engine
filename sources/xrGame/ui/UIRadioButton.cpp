//////////////////////////////////////////////////////////////////////
// UIRadioButton.cpp: класс кнопки, имеющей 2 состояния
// и работающей в группе с такими же кнопками
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiradiobutton.h"
#include "UILines.h"


void CUIRadioButton::Init(f32 x, f32 y, f32 width, f32 height){
	if (!m_pLines)
		m_pLines = xr_new<CUILines>();
	m_pLines->SetTextAlignment(CGameFont::alLeft);
	CUI3tButton::InitTexture("ui_radio");
	fRect r = m_background.GetE()->GetStaticItem()->GetRect();
	CUI3tButton::SetTextX(r.width());
	CUI3tButton::Init(x,y, width, r.height() - 5);
	m_pLines->Init(x,y,width,m_background.GetE()->GetStaticItem()->GetRect().height());
}

void CUIRadioButton::InitTexture(pcstr tex_name){
	// do nothing
}
