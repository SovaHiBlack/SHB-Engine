// Реализация окна с закладками.

#include "stdafx.h"
#include "UITabControl.h"
#include "UITabButton.h"

CUITabControl::CUITabControl( )
	: m_iPushedIndex(0),
	m_iPrevPushedIndex(0),
	m_cGlobalTextColor(0xFFFFFFFF),
	m_cActiveTextColor(0xFFFFFFFF),
	m_cActiveButtonColor(0xFFFFFFFF),
	m_cGlobalButtonColor(0xFFFFFFFF),
	m_bAcceleratorsEnable(true)
{

}

CUITabControl::~CUITabControl( )
{
	RemoveAll( );
}

void CUITabControl::SetCurrentValue( )
{
	s32 ival;
	s32 imin;
	s32 imax;

	GetOptIntegerValue(ival, imin, imax);
	SetNewActiveTab(ival);
}

void CUITabControl::SaveValue( )
{
	CUIOptionsItem::SaveValue( );
	SaveOptIntegerValue(GetActiveIndex( ));
}

bool CUITabControl::IsChanged( )
{
	s32 ival;
	s32 imin;
	s32 imax;
	GetOptIntegerValue(ival, imin, imax);

	return GetActiveIndex( ) != ival;
}

// добавление кнопки-закладки в список закладок контрола
bool CUITabControl::AddItem(pcstr pItemName, pcstr pTexName, f32 x, f32 y, f32 width, f32 height)
{
	CUITabButton* pNewButton = xr_new<CUITabButton>( );
	pNewButton->SetAutoDelete(true);
	pNewButton->Init(pTexName, x, y, width, height);
	pNewButton->SetText(pItemName);
	pNewButton->SetTextColor(m_cGlobalTextColor);
	pNewButton->GetUIStaticItem( ).SetColor(m_cGlobalButtonColor);

	return AddItem(pNewButton);
}

bool CUITabControl::AddItem(CUITabButton* pButton)
{
	pButton->SetAutoDelete(true);
	pButton->Show(true);
	pButton->Enable(true);
	pButton->SetButtonAsSwitch(true);

	// Нажимаем кнопку по умолчанию
	if (m_iPushedIndex == static_cast<int>(m_TabsArr.size( ) - 1))
	{
		m_TabsArr[m_iPushedIndex]->SendMessage(m_TabsArr[m_iPushedIndex], TAB_CHANGED, NULL);
	}

	AttachChild(pButton);
	m_TabsArr.push_back(pButton);
	return true;
}

// Удаление элемента по индексу. Индексы начинаются с Index
void CUITabControl::RemoveItem(u32 Index)
{
	R_ASSERT(m_TabsArr.size( ) > Index);
	DetachChild(m_TabsArr[Index]);

	// Меняем значение заданного элемента, и последнего элемента.
	// Так как у нас хранятся указатели операция будет проходить быстро.
	TABS_VECTOR::value_type tmp = m_TabsArr[Index];
	m_TabsArr[Index] = m_TabsArr.back( );
	m_TabsArr.back( ) = tmp;

	xr_delete(m_TabsArr.back( ));
	m_TabsArr.pop_back( );
}

// Удаление всех элементов
void CUITabControl::RemoveAll( )
{
	TABS_VECTOR_it it = m_TabsArr.begin( );
	for (; it != m_TabsArr.end( ); ++it)
	{
		DetachChild(*it);
	}

	m_TabsArr.clear( );
}

// переключение закладок.
void CUITabControl::SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData)
{
	if (TAB_CHANGED == msg)
	{
		for (u32 i = 0; i < m_TabsArr.size( ); ++i)
		{
			if (m_TabsArr[i] == pWnd)
			{
				m_iPushedIndex = i;
				if (m_iPrevPushedIndex == m_iPushedIndex)
				{	// return if nothing was changed
					return;
				}

				OnTabChange(m_iPushedIndex, m_iPrevPushedIndex);
				m_iPrevPushedIndex = m_iPushedIndex;
				break;
			}
		}
	}

	else if (STATIC_FOCUS_RECEIVED == msg || STATIC_FOCUS_LOST == msg)
	{
		for (u8 i = 0; i < m_TabsArr.size( ); ++i)
		{
			if (pWnd == m_TabsArr[i])
			{
				if (msg == STATIC_FOCUS_RECEIVED)
				{
					OnStaticFocusReceive(pWnd);
				}
				else
				{
					OnStaticFocusLost(pWnd);
				}
			}
		}
	}
	else
	{
		inherited::SendMessage(pWnd, msg, pData);
	}
}

void CUITabControl::OnStaticFocusReceive(CUIWindow* pWnd)
{
	GetMessageTarget( )->SendMessage(this, STATIC_FOCUS_RECEIVED, static_cast<void*>(pWnd));
}

void CUITabControl::OnStaticFocusLost(CUIWindow* pWnd)
{
	GetMessageTarget( )->SendMessage(this, STATIC_FOCUS_LOST, static_cast<void*>(pWnd));
}

void CUITabControl::OnTabChange(s32 iCur, s32 iPrev)
{
	if (iPrev != -1)
	{
		m_TabsArr[iPrev]->SendMessage(m_TabsArr[iCur], TAB_CHANGED, NULL);
	}

	m_TabsArr[iCur]->SendMessage(m_TabsArr[iCur], TAB_CHANGED, NULL);
	GetMessageTarget( )->SendMessage(this, TAB_CHANGED, NULL);
}

void CUITabControl::SetNewActiveTab(const s32 iNewTab)
{
	if (m_iPushedIndex == iNewTab)
	{
		return;
	}

	m_iPushedIndex = iNewTab;
	OnTabChange(m_iPushedIndex, m_iPrevPushedIndex);
	m_iPrevPushedIndex = m_iPushedIndex;
}

bool CUITabControl::OnKeyboard(s32 dik, EUIMessages keyboard_action)
{
	if (GetAcceleratorsMode( ) && WINDOW_KEY_PRESSED == keyboard_action)
	{
		for (u32 i = 0; i < m_TabsArr.size( ); ++i)
		{
			if (m_TabsArr[i]->IsAccelerator(dik))
			{
				SetNewActiveTab(i);
				return true;
			}
		}
	}

	return false;
}

CUIButton* CUITabControl::GetButtonByIndex(s32 i)
{
	R_ASSERT(i >= 0 && i < (s32)m_TabsArr.size( ));
	return m_TabsArr[i];
}

const shared_str CUITabControl::GetCommandName(s32 i)
{
	return (GetButtonByIndex(i))->WindowName( );
}

CUIButton* CUITabControl::GetButtonByCommand(const shared_str& n)
{
	for (u32 i = 0; i < m_TabsArr.size( ); ++i)
	{
		if (m_TabsArr[i]->WindowName( ) == n)
		{
			return m_TabsArr[i];
		}
	}

	return NULL;
}

void CUITabControl::ResetTab( )
{
	for (u32 i = 0; i < m_TabsArr.size( ); ++i)
	{
		m_TabsArr[i]->SetButtonMode(CUIButton::BUTTON_NORMAL);
	}

	m_iPushedIndex = m_iPrevPushedIndex = -1;
}
