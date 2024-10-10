#pragma once

#include "UIScrollView.h"

class CUIListBoxItem;

class CUIListBox : 	public CUIScrollView,
					public IUIFontControl
{
public:
					CUIListBox						();
	CUIListBoxItem* AddItem							(pcstr text);
	CUIListBoxItem* GetItemByTAG					(u32 tag_value);
	CUIListBoxItem* GetItemByIDX					(u32 idx);
	CUIListBoxItem* GetItemByText					(pcstr text);
	CUIListBoxItem* GetSelectedItem					();

	pcstr			GetSelectedText					();
	pcstr			GetText							(u32 idx);
	void			MoveSelectedUp					();
	void			MoveSelectedDown				();
	void			SetSelectionTexture				(pcstr texture);
	void			SetItemHeight					(f32 h);
	f32			GetItemHeight					();
	f32			GetLongestLength				();

	virtual	void	SetSelected						(CUIWindow* w)	{CUIScrollView::SetSelected(w);};
		u32			GetSelectedIDX					();
		void		SetSelectedIDX					(u32 idx);
		void		SetSelectedTAG					(u32 tag_val);
		void		SetSelectedText					(pcstr txt);
		void		SetImmediateSelection			(bool f);

virtual bool		OnMouse							(f32 x, f32 y, EUIMessages mouse_action);
virtual void		SendMessage						(CUIWindow* pWnd, s16 msg, void* pData = 0);

	// IUIFontControl
	virtual void			SetTextColor			(u32 color);
			void			SetTextColorS			(u32 color);
	virtual u32				GetTextColor			();
	virtual void			SetFont					(CGameFont* pFont);
	virtual CGameFont*		GetFont					();
	virtual void			SetTextAlignment		(ETextAlignment alignment);
	virtual ETextAlignment	GetTextAlignment		();

protected:
	f32			m_def_item_height;
	int				m_last_selection;
	u32				m_text_color;
	u32				m_text_color_s;
	ETextAlignment	m_text_al;
	shared_str		m_selection_texture;

	bool			m_bImmediateSelection;
};