#pragma once

#include "UIStatic.h"
#include "UIDialogWnd.h"

class CUIDragItem;
class CUIDragDropListEx;
class CUICellItem;

class ICustomDrawCell
{
public:
	virtual				~ICustomDrawCell( )
	{ }
	virtual void		OnDraw(CUICellItem* cell) = 0;
};

class CUICellItem :public CUIStatic
{
private:
	typedef		CUIStatic	inherited;

protected:
	xr_vector<CUICellItem*> m_childs;

	CUIDragDropListEx* m_pParentList;
	iVector2				m_grid_size;
	ICustomDrawCell* m_custom_draw;
	s32						m_accelerator;
	virtual void			UpdateItemText( );

public:
	CUICellItem( );
	virtual					~CUICellItem( );

	virtual		bool		OnKeyboard(s32 dik, EUIMessages keyboard_action);
	virtual		bool		OnMouse(f32 x, f32 y, EUIMessages mouse_action);
	virtual		void		Draw( );
	virtual		void		Update( )
	{
		inherited::Update( );
		m_b_already_drawn = false;
	}

	virtual		void		OnAfterChild( )
	{ }

	u32			ChildsCount( );
	void		 PushChild(CUICellItem*);
	CUICellItem* PopChild( );
	CUICellItem* Child(u32 idx)
	{
		return m_childs[idx];
	}
	bool		HasChild(CUICellItem* item);
	virtual		bool		EqualTo(CUICellItem* itm);
	IC const	iVector2& GetGridSize( )
	{	 //size in grid
		return m_grid_size;
	}
	IC			void		SetAccelerator(s32 dik)
	{
		m_accelerator = dik;
	}
	IC			s32			GetAccelerator( ) const
	{
		return m_accelerator;
	}

	virtual		CUIDragItem* CreateDragItem( );

	CUIDragDropListEx* OwnerList( )
	{
		return m_pParentList;
	}
	void		SetOwnerList(CUIDragDropListEx* p);
	void		SetCustomDraw(ICustomDrawCell* c);
	void* m_pData;
	s32			m_index;
	bool		m_b_already_drawn;
	bool		m_b_destroy_childs;
};

class CUIDragItem : public CUIWindow, public pureRender, public pureFrame
{
private:
	typedef		CUIWindow	inherited;
	CUIStatic				m_static;
	CUICellItem* m_pParent;
	fVector2				m_pos_offset;
	CUIDragDropListEx* m_back_list;

public:
	CUIDragItem(CUICellItem* parent);
	virtual		void		Init(const ref_shader& sh, const fRect& rect, const fRect& text_rect);
	virtual					~CUIDragItem( );
	CUIStatic* wnd( )
	{
		return &m_static;
	}
	virtual		bool		OnMouse(f32 x, f32 y, EUIMessages mouse_action);
	virtual		void		Draw( );
	virtual		void		OnRender( );
	virtual		void		OnFrame( );
	CUICellItem* ParentItem( )
	{
		return m_pParent;
	}
	void		SetBackList(CUIDragDropListEx* l);
	CUIDragDropListEx* BackList( )
	{
		return m_back_list;
	}
	fVector2	GetPosition( );
};
