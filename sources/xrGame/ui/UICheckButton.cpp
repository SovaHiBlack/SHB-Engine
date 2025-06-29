// UICheckButton.cpp: ����� ������, ������� 2 ���������:
// � �������� � ���
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UICheckButton.h"
#include "UILines.h"

CUICheckButton::CUICheckButton( )
{
	SetTextAlignment(CGameFont::alLeft);
	m_bCheckMode = true;
	m_pDependControl = NULL;
}

CUICheckButton::~CUICheckButton( )
{ }

void CUICheckButton::SetDependControl(CUIWindow* pWnd)
{
	m_pDependControl = pWnd;
}

void CUICheckButton::Update( )
{
	CUI3tButton::Update( );

	if (m_pDependControl)
	{
		m_pDependControl->Enable(GetCheck( ));
	}
}

void CUICheckButton::SetCurrentValue( )
{
	SetCheck(GetOptBoolValue( ));
}

void CUICheckButton::SaveValue( )
{
	CUIOptionsItem::SaveValue( );
	SaveOptBoolValue(GetCheck( ));
}

bool CUICheckButton::IsChanged( )
{
	return (b_backup_val != GetCheck( ));
}

void CUICheckButton::Init(f32 x, f32 y, f32 width, f32 height)
{
	CUIWindow::Init(x, y, width, height);
	InitTexture( );
	m_pLines->Init(x, y, width, m_background.GetE( )->GetStaticItem( )->GetRect( ).height( ));
}

void CUICheckButton::InitTexture( )
{
	CUI3tButton::InitTexture("ui_checker");
	fRect r = m_background.GetE( )->GetStaticItem( )->GetOriginalRect( );
	CUI3tButton::SetTextX(r.width( ));
}

void CUICheckButton::SeveBackUpValue( )
{
	b_backup_val = GetCheck( );
}

void CUICheckButton::Undo( )
{
	SetCheck(b_backup_val);
	SaveValue( );
}
