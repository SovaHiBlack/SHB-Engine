//=============================================================================
//  Filename:   UIListWnd_inline.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ���������� ������������ ������� ���������
//=============================================================================
#pragma once

template <class Element>
bool CUIListWnd::AddItem(pcstr  str, const f32 shift, pvoid pData, s32 value, s32 insertBeforeIdx)
{
	//������� ����� ������� � �������� ��� � ������
	Element* pItem = NULL;
	pItem = xr_new<Element>( );

	VERIFY(pItem);

	pItem->Init(str, shift, m_bVertFlip ? GetHeight( ) - GetSize( ) * m_iItemHeight - m_iItemHeight : GetSize( ) * m_iItemHeight, m_iItemWidth, m_iItemHeight);

	pItem->SetData(pData);
	pItem->SetValue(value);
	pItem->SetTextColor(m_dwFontColor);

	return AddItem<Element>(pItem, insertBeforeIdx);
}

template <class Element>
bool CUIListWnd::AddItem(Element* pItem, s32 insertBeforeIdx)
{
	AttachChild(pItem);

	pItem->Init(pItem->GetWndRect( ).left, m_bVertFlip ? GetHeight( ) - GetItemsCount( ) * m_iItemHeight - m_iItemHeight : GetItemsCount( ) * m_iItemHeight, m_iItemWidth, m_iItemHeight);

	//���������� � ����� ��� ������ ������
	if (-1 == insertBeforeIdx)
	{
		m_ItemList.push_back(pItem);
		pItem->SetIndex(m_ItemList.size( ) - 1);
	}
	else
	{
		//�������� �������� �������� ��� ���������� ���������
		if (!m_ItemList.empty( ))
		{
			R_ASSERT(static_cast<u32>(insertBeforeIdx) <= m_ItemList.size( ));
		}

		LIST_ITEM_LIST_it it2 = m_ItemList.begin( );
		std::advance(it2, insertBeforeIdx);
		for (LIST_ITEM_LIST_it it = it2; m_ItemList.end( ) != it; ++it)
		{
			(*it)->SetIndex((*it)->GetIndex( ) + 1);
		}

		m_ItemList.insert(it2, pItem);
		pItem->SetIndex(insertBeforeIdx);
	}

	UpdateList( );

	//�������� ������ ���������
	m_ScrollBar->SetRange(0, s16(m_ItemList.size( ) - 1));
	m_ScrollBar->SetPageSize(s16((u32)m_iRowNum < m_ItemList.size( ) ? m_iRowNum : m_ItemList.size( )));
	m_ScrollBar->SetScrollPos(s16(m_iFirstShownIndex));

	UpdateScrollBar( );

	return true;
}
