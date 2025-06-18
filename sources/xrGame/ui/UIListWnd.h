#pragma once
#include "UIWindow.h"
#include "uilistitem.h"
#include "uiscrollbar.h"

#include "../script_export_space.h"

#define DEFAULT_ITEM_HEIGHT 30

DEF_LIST(LIST_ITEM_LIST, CUIListItem*);
class CUIScrollBar;
class CUIFrameLineWnd;

class CUIListWnd :public CUIWindow
{
private:
	typedef			CUIWindow inherited;
	friend class	CUIGameLog;

	shared_str		m_scrollbar_profile;
	void			DrawActiveBackFrame(const fRect& rect, CUIListItem* itm);

public:
	CUIListWnd( );
	virtual			~CUIListWnd( );

	virtual void	 Init(f32 x, f32 y, f32 width, f32 height);
	virtual void	 Init(f32 x, f32 y, f32 width, f32 height, f32 item_height);

	virtual bool	 OnMouse(f32 x, f32 y, EUIMessages mouse_action);
	virtual bool	 OnKeyboard(s32 dik, EUIMessages keyboard_action);

	virtual void	 SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData);
	virtual void	 Draw( );
	virtual void	 Update( );
	virtual void	 DetachChild(CUIWindow* pChild);
	void	SetScrollBarProfile(pcstr profile)
	{
		m_scrollbar_profile = profile;
	}

	// Добавление элементов в листбокс
	template <class Element>
	bool			 AddItem(pcstr  str, const f32 shift = 0.0f, pvoid pData = NULL, s32 value = 0, s32 insertBeforeIdx = -1);

	virtual bool	AddItem_script(CUIListItem* item);

	template <class Element>
	bool			AddItem(Element* pItem, s32 insertBeforeIdx = -1);

	void 			RemoveItem(s32 index);
	void 			RemoveAll( );
	//находит первый элемент с заданной pData, иначе -1
	s32 			FindItem(pvoid pData);
	s32 			FindItemWithValue(s32 iValue);
	CUIListItem* GetItem(s32 index);
	// Получить индекс элемента по адресу. Либо -1 если нет такого
	s32				GetItemPos(CUIListItem* pItem);

	void			SetItemWidth(f32 iItemWidth);
	f32			GetItemWidth( )
	{
		return m_iItemWidth;
	}

	void			SetItemHeight(f32 iItemHeight);
	f32			GetItemHeight( )
	{
		return m_iItemHeight;
	}
	virtual void	SetHeight(f32 height);

	void			SetAlwaysShowScroll(bool flag = true)
	{
		m_bAlwaysShowScroll = flag;
	}
	void			EnableAlwaysShowScroll(bool flag)
	{
		m_bAlwaysShowScroll_enable = flag;
	}

	s32				GetItemsCount( )
	{
		return m_ItemList.size( );
	}

	//подготовить все элементы заново
	void 			Reset( );

	void 			EnableScrollBar(bool enable);
	bool 			IsScrollBarEnabled( );
	void 			UpdateScrollBar( );

	void 			ScrollToBegin( );
	void 			ScrollToEnd( );
	void 			ScrollToPos(s32 position);

	IC bool			IsActiveBackgroundEnabled( )
	{
		return m_bActiveBackground;
	}
	void			EnableActiveBackground(bool enable);

	virtual void	SetWidth(f32 width);

	void			SetTextColor(u32 color)
	{
		m_dwFontColor = color;
	}
	u32				GetTextColor( )
	{
		return m_dwFontColor;
	}

	//делает активными (как кнопки) элементы списка
	void 			ActivateList(bool activity);
	bool 			IsListActive( )
	{
		return m_bListActivity;
	}

	void 			SetVertFlip(bool vert_flip)
	{
		m_bVertFlip = vert_flip;
	}
	bool 			GetVertFlip( )
	{
		return m_bVertFlip;
	}

	// Принудительная установка фокуса
	void			SetFocusedItem(s32 iNewFocusedItem);
	s32				GetFocusedItem( )
	{
		return m_iFocusedItem;
	}
	s32				GetSelectedItem( )
	{
		return m_iSelectedItem;
	}
	void			SetSelectedItem(s32 sel)
	{
		m_iSelectedItem = sel;
	}
	void			ShowSelectedItem(bool show = true);

	void			ResetFocusCapture( )
	{
		m_bForceFocusedItem = false;
	}

	s32				GetListPosition( ) const
	{
		return m_iFirstShownIndex;
	}

protected:
	void			create_active_back( );
	void			destroy_active_back( );

	CUIScrollBar* m_ScrollBar;

	//обновления елементов списка, вызвается
	//если произошли изменения
	void			UpdateList( );

	//список элементов листа
	LIST_ITEM_LIST	m_ItemList;

	//размеры элемента списка
	f32		m_iItemHeight;
	f32		m_iItemWidth;

	//количество рядов для элементов
	s32		m_iRowNum;

	//индекс первого показанного элемента
	s32		m_iFirstShownIndex;

	//элемент над которым курсор в данный момент или -1, если такого нет
	s32		m_iFocusedItem;
	s32		m_iFocusedItemGroupID;
	s32		m_iSelectedItem;
	s32		m_iSelectedItemGroupID;

	bool	m_bShowSelectedItem;
	bool	m_bAlwaysShowScroll_enable;
	bool	m_bAlwaysShowScroll;
	bool	m_bActiveBackground;
	// Если хотим принудительно выставлять фокус, то поднять этот флаг
	bool	m_bForceFocusedItem;

	//подсветка активного элемента
	CUIFrameLineWnd* m_ActiveBackgroundFrame;

	//текущий цвет текста
	u32		m_dwFontColor;
	bool	m_bListActivity;

	//переворот списка по вертикали
	bool	m_bVertFlip;

	// Признак того, что мышь подвинули
	bool	m_bUpdateMouseMove;

	// Текущий уникальный идентификатор
	s32		m_iLastUniqueID;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

#include "UIListWnd_inline.h"

add_to_type_list(CUIListWnd)
#undef script_type_list
#define script_type_list save_type_list(CUIListWnd)
