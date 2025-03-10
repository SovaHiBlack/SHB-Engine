#pragma once

#include "uiframewindow.h"
#include "uilistbox.h"

#include "../script_export_space.h"

class CUIPropertiesBox : public CUIFrameWindow
{
private:
	typedef CUIFrameWindow inherited;

public:
	CUIPropertiesBox( );
	virtual				~CUIPropertiesBox( );

	virtual void		Init(f32 x, f32 y, f32 width, f32 height);

	virtual void		SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData);
	virtual bool		OnMouse(f32 x, f32 y, EUIMessages mouse_action);
	virtual bool		OnKeyboard(s32 dik, EUIMessages keyboard_action);

	bool				AddItem(pcstr str, pvoid pData = NULL, u32 tag_value = 0);
	bool				AddItem_script(pcstr str)
	{
		return AddItem(str);
	}
	u32					GetItemsCount( )
	{
		return m_UIListWnd.GetSize( );
	}
	void				RemoveItemByTAG(u32 tag_value);
	void				RemoveAll( );

	virtual void		Show(const fRect& parent_rect, const fVector2& point);
	virtual void		Hide( );

	virtual void		Update( );
	virtual void		Draw( );

	CUIListBoxItem* GetClickedItem( );

	void				AutoUpdateSize( );

protected:
	CUIListBox			m_UIListWnd;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIPropertiesBox)
#undef script_type_list
#define script_type_list save_type_list(CUIPropertiesBox)
