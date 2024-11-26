//=============================================================================
//  Filename:   UITreeViewItem.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  TreeView Item class
//=============================================================================
#pragma once

#include "UIListItem.h"

class CUIListWnd;

class CUITreeViewItem : public CUIListItem
{
	typedef CUIListItem inherited;
	// �������� �� �� ������� �����������
	bool			isRoot;
	// ���� �� ���, �� ���� ���� ���������� ������� ���� ����������� ��� ���
	bool			isOpened;
	// �������� � ��������
	s32				iTextShift;
	// ���� �� �����������
	CUITreeViewItem* pOwner;

public:
	void			SetRoot(bool set);
	bool			IsRoot( ) const
	{
		return isRoot;
	}

	// ������������� �������� ������
	void			SetTextShift(s32 delta)
	{
		iTextShift += delta;
	}

	// ��������
	CUITreeViewItem* GetOwner( ) const
	{
		return pOwner;
	}
	void			SetOwner(CUITreeViewItem* owner)
	{
		pOwner = owner;
	}

protected:
	// ������� ���������� ��� ��������� �������� ����
	// ��� ��������� ����������� ������������� ����
	virtual void	OnRootChanged( );

public:
	// ��������/�������� ��������� ��������
	void			Open( );
	void			Close( );
	bool			IsOpened( ) const
	{
		return isOpened;
	}

protected:
	// ������� ���������� ��� ��������� c�������� �������/������� 
	// ��� ��������� ����������� ������������� ����
	virtual void	OnOpenClose( );

public:
	// ������ ���������, ������� ������� ���� ���
	typedef			xr_vector<CUITreeViewItem*>		SubItems;
	typedef			SubItems::iterator				SubItems_it;
	SubItems		vSubItems;

	CUIStatic		UIBkg;

	// �������� �������
	void AddItem(CUITreeViewItem* pItem);
	// ������� ���
	void DeleteAllSubItems( );
	// ����� ������� � �������� ������
	// Return:	��������� �� �������, ���� �����, ���� NULL � ��������� ������
	CUITreeViewItem* Find(pcstr text) const;
	// ����� ������� � �������� ���������
	// Return:	��������� �� �������, ���� �����, ���� NULL � ��������� ������
	CUITreeViewItem* Find(s32 value) const;
	// ����� �������� �������
	// Return:	��������� �� �������, ���� �����, ���� NULL � ��������� ������
	CUITreeViewItem* Find(CUITreeViewItem* pItem) const;
	// ������� �������� �� ����� �� �������� �������� � ���� ������-����
	// ������� �������� ������������� �������� "/"
	xr_string GetHierarchyAsText( );

	// Redefine some stuff
	// ATTENTION! ��� ����������� ���������������� ������� [+-] ����� SetText
	// ������ �������������� SetRoot
	virtual void SetText(pcstr str);
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData);

	// Ctor and Dtor
	CUITreeViewItem( );
	virtual		~CUITreeViewItem( );

	// ������������� ���� ������ � ����������� �� ����, �������� �� ������
	void	MarkArticleAsRead(bool value);
	bool	IsArticleReaded( )
	{
		return m_bArticleRead;
	}
	// ���� ������ ����� ������ �� �������� � �� ��������
	void	SetReadedColor(u32 cl)
	{
		m_uReadedColor = cl;
	}
	void	SetUnreadedColor(u32 cl)
	{
		m_uUnreadedColor = cl;
	}
	void	SetManualSetColor(bool val)
	{
		m_bManualSetColor = val;
	}
	// ������������� ���� � ����������� �� ��������� ��������
	void	SetItemColor( )
	{
		m_bArticleRead ? SetTextColor(m_uReadedColor) : SetTextColor(m_uUnreadedColor);
	}

private:
	friend class CUITreeViewItem;

	// ��������� ��������� ����� �� ��������
	void	CheckParentMark(CUITreeViewItem* pOwner);
	// ���� ������ ����� ������ �� ��������
	u32		m_uUnreadedColor;
	// ���� ������ ����� ������ �� ��������
	u32		m_uReadedColor;
	// ������ ��������� ��������
	bool	m_bArticleRead;
	// ���� true, �� MarkArticleAsRead �� ����� ��������
	// SetItemColor()
	bool	m_bManualSetColor;
};

//////////////////////////////////////////////////////////////////////////
//  Function for automatic tree hierarchy creation
//////////////////////////////////////////////////////////////////////////

DEF_VECTOR(GroupTree, shared_str);

//////////////////////////////////////////////////////////////////////////

void CreateTreeBranch(shared_str nestingTree, shared_str leafName, CUIListWnd* pListToAdd, s32 leafProperty,
					  CGameFont* pRootFont, u32 rootColor, CGameFont* pLeafFont, u32 leafColor, bool markRead);
