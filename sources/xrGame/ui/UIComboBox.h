#pragma once

#include "UIEditBox.h"
#include "UIListBox.h"
#include "UIInteractiveBackground.h"
#include "UIOptionsItem.h"//

class CUIListBoxItem;

class CUIComboBox : public CUIWindow, public CUIOptionsItem
{
	friend class CUIXmlInit;
	typedef enum
	{
		LIST_EXPANDED,
		LIST_FONDED
	} E_COMBO_STATE;

public:
	CUIComboBox( );
	virtual				~CUIComboBox( );
	// CUIOptionsItem
	virtual void		SetCurrentValue( );
	virtual void		SaveValue( );
	virtual bool		IsChanged( );
	virtual void 		SeveBackUpValue( );
	virtual void 		Undo( );

	pcstr		GetText( );

	// methods
	CUIListBox* GetListWnd( );
	void		SetListLength(s32 length);
	void		SetVertScroll(bool bVScroll = true)
	{
		m_list.SetFixedScrollBar(bVScroll);
	}

	CUIListBoxItem* AddItem_(pcstr str, s32 _data);
	virtual void		Init(f32 x, f32 y, f32 width);
	void		SetItem(s32 i);

	virtual void		Init(f32 x, f32 y, f32 width, f32 height);
	virtual void		SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData = 0);
	virtual void		OnFocusLost( );
	virtual void		OnFocusReceive( );
	s32			CurrentID( )
	{
		return m_itoken_id;
	}

protected:
	virtual void		SetState(UIState state);
	virtual bool		OnMouse(f32 x, f32 y, EUIMessages mouse_action);
	virtual void		OnBtnClicked( );
	void		ShowList(bool bShow);
	void		OnListItemSelect( );
	virtual void		Update( );

protected:
	bool				m_bInited;
	s32					m_iListHeight;
	s32					m_itoken_id;
	E_COMBO_STATE		m_eState;
	s32					m_backup_itoken_id;

	CUI_IB_FrameLineWnd	m_frameLine;
	CUILabel			m_text;
	CUIFrameWindow		m_frameWnd;

	u32					m_textColor[2];

public:
	CUIListBox			m_list;
	void				SetTextColor(u32 color)
	{
		m_textColor[0] = color;
	}
	void				SetTextColorD(u32 color)
	{
		m_textColor[1] = color;
	}

protected:
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIComboBox)
#undef script_type_list
#define script_type_list save_type_list(CUIComboBox)