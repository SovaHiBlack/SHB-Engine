// File:        UIComboBox.cpp
// Description: guess :)

#include "stdafx.h"
#include "UIComboBox.h"
#include "UITextureMaster.h"
#include "UIScrollBar.h"
#include "uilistboxitem.h"
#include "../StringTable.h"

#define CB_HEIGHT 23.0f
#define BTN_SIZE  23.0f

CUIComboBox::CUIComboBox( )
{
	AttachChild(&m_frameLine);
	AttachChild(&m_text);

	AttachChild(&m_frameWnd);
	AttachChild(&m_list);

	m_iListHeight = 0;
	m_bInited = false;
	m_eState = LIST_FONDED;

	m_textColor[0] = 0xff00ff00;
}

CUIComboBox::~CUIComboBox( )
{ }

void CUIComboBox::SetListLength(s32 length)
{
	R_ASSERT(0 == m_iListHeight);
	m_iListHeight = length;
}

void CUIComboBox::Init(f32 x, f32 y, f32 width)
{
	m_bInited = true;
	if (0 == m_iListHeight)
	{
		m_iListHeight = 4;
	}

	CUIWindow::Init(x, y, width, CB_HEIGHT);
	// Frame Line
	m_frameLine.Init(0.0f, 0.0f, width, CB_HEIGHT);
	m_frameLine.InitEnabledState("ui_cb_linetext_e"); // horizontal by default
	m_frameLine.InitHighlightedState("ui_cb_linetext_h");

	// Edit Box on left side of frame line
	m_text.Init(0.0f, 0.0f, width, CB_HEIGHT);
	m_text.SetTextColor(m_textColor[0]);
	m_text.Enable(false);

	// height of list equal to height of ONE element
	f32 item_height = CUITextureMaster::GetTextureHeight("ui_cb_listline_b");
	m_list.Init(0.0f, CB_HEIGHT, width, item_height * m_iListHeight);
	m_list.Init( );
	m_list.SetTextColor(m_textColor[0]);
	m_list.SetSelectionTexture("ui_cb_listline");
	m_list.SetItemHeight(CUITextureMaster::GetTextureHeight("ui_cb_listline_b"));
	// frame(texture) for list
	m_frameWnd.Init(0.0f, CB_HEIGHT, width, m_list.GetItemHeight( ) * m_iListHeight);
	m_frameWnd.InitTexture("ui_cb_listbox");

	m_list.Show(false);
	m_frameWnd.Show(false);
}

void CUIComboBox::Init(f32 x, f32 y, f32 width, f32 height)
{
	this->Init(x, y, width);
}

CUIListBoxItem* CUIComboBox::AddItem_(pcstr str, s32 _data)
{
	R_ASSERT2(m_bInited, "Can't add item to ComboBox before Initialization");
	CUIListBoxItem* itm = m_list.AddItem(str);
	itm->SetData((pvoid)(__int64)_data);
	return itm;
}

void CUIComboBox::OnListItemSelect( )
{
	m_text.SetText(m_list.GetSelectedText( ));
	CUIListBoxItem* itm = m_list.GetSelectedItem( );

	s32 bk_itoken_id = m_itoken_id;

	m_itoken_id = (s32)(__int64)itm->GetData( );
	ShowList(false);

	if (bk_itoken_id != m_itoken_id)
	{
		SaveValue( );
		GetMessageTarget( )->SendMessage(this, LIST_ITEM_SELECT, NULL);
	}
}

void CUIComboBox::SetCurrentValue( )
{
	m_list.Clear( );
	xr_token* tok = GetOptToken( );

	while (tok->name)
	{
		AddItem_(tok->name, tok->id);
		tok++;
	}

	pcstr cur_val = *CStringTable( ).translate(GetOptTokenValue( ));
	m_text.SetText(cur_val);
	m_list.SetSelectedText(cur_val);

	CUIListBoxItem* itm = m_list.GetSelectedItem( );
	if (itm)
	{
		m_itoken_id = (s32)(__int64)itm->GetData( );
	}
	else
	{	 //first
		m_itoken_id = 1;
	}
}

void CUIComboBox::SaveValue( )
{
	CUIOptionsItem::SaveValue( );
	xr_token* tok = GetOptToken( );
	pcstr cur_val = get_token_name(tok, m_itoken_id);
	SaveOptTokenValue(cur_val);
}

bool CUIComboBox::IsChanged( )
{
	return (m_backup_itoken_id != m_itoken_id);
}

pcstr CUIComboBox::GetText( )
{
	return m_text.GetText( );
}

void CUIComboBox::SetItem(s32 idx)
{
	m_list.SetSelectedIDX(idx);
	CUIListBoxItem* itm = m_list.GetSelectedItem( );
	m_itoken_id = (s32)(__int64)itm->GetData( );

	m_text.SetText(m_list.GetSelectedText( ));
}

void CUIComboBox::OnBtnClicked( )
{
	ShowList(!m_list.IsShown( ));
}

void CUIComboBox::ShowList(bool bShow)
{
	if (bShow)
	{
		SetHeight(m_text.GetHeight( ) + m_list.GetHeight( ));

		m_list.Show(true);
		m_frameWnd.Show(true);

		m_eState = LIST_EXPANDED;

		GetParent( )->SetCapture(this, true);
	}
	else
	{
		m_list.Show(false);
		m_frameWnd.Show(false);
		SetHeight(m_frameLine.GetHeight( ));
		GetParent( )->SetCapture(this, false);

		m_eState = LIST_FONDED;
	}
}

CUIListBox* CUIComboBox::GetListWnd( )
{
	return &m_list;
}
void CUIComboBox::Update( )
{
	CUIWindow::Update( );
	if (!m_bIsEnabled)
	{
		SetState(S_Disabled);
		m_text.SetTextColor(m_textColor[1]);
	}
	else
	{
		m_text.SetTextColor(m_textColor[0]);
	}
}

void CUIComboBox::OnFocusLost( )
{
	CUIWindow::OnFocusLost( );
	if (m_bIsEnabled)
	{
		SetState(S_Enabled);
	}
}

void CUIComboBox::OnFocusReceive( )
{
	CUIWindow::OnFocusReceive( );
	if (m_bIsEnabled)
	{
		SetState(S_Highlighted);
	}
}

bool CUIComboBox::OnMouse(f32 x, f32 y, EUIMessages mouse_action)
{
	if (CUIWindow::OnMouse(x, y, mouse_action))
	{
		return true;
	}

	bool bCursorOverScb = false;
	bCursorOverScb = m_list.ScrollBar( )->CursorOverWindow( );
	switch (m_eState)
	{
		case LIST_EXPANDED:
		{
			if ((!bCursorOverScb) && mouse_action == WINDOW_LBUTTON_DOWN)
			{
				ShowList(false);
				return true;
			}
		}
		break;
		case LIST_FONDED:
		{
			if (mouse_action == WINDOW_LBUTTON_DOWN)
			{
				OnBtnClicked( );
				return true;
			}
		}
		break;
		default:
		{
		}
		break;
	}

	return false;
}

void CUIComboBox::SetState(UIState state)
{
	m_frameLine.SetState(state);
}

void CUIComboBox::SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData)
{
	CUIWindow::SendMessage(pWnd, msg, pData);

	switch (msg)
	{
		case LIST_ITEM_CLICKED:
		{
			if (pWnd == &m_list)
			{
				OnListItemSelect( );
			}
		}
		break;
		default:
		{
		}
		break;
	}
}

void CUIComboBox::SeveBackUpValue( )
{
	m_backup_itoken_id = m_itoken_id;
}

void CUIComboBox::Undo( )
{
	SetItem(m_backup_itoken_id);
	SaveValue( );
	SetCurrentValue( );
}

using namespace luabind;

#pragma optimize("s",on)
void CUIComboBox::script_register(lua_State* L)
{
	module(L)
		[
			class_<CUIComboBox, CUIWindow>("CUIComboBox")
				.def(constructor<>( ))
				.def("Init", (void (CUIComboBox::*)(f32, f32, f32)) & CUIComboBox::Init)
				.def("Init", (void (CUIComboBox::*)(f32, f32, f32, f32)) & CUIComboBox::Init)
				.def("SetVertScroll", &CUIComboBox::SetVertScroll)
				.def("SetListLength", &CUIComboBox::SetListLength)
				.def("CurrentID", &CUIComboBox::CurrentID)
				.def("SetCurrentID", &CUIComboBox::SetItem)

		//		.def("AddItem",				(void (CUIComboBox::*)(pcstr, bool)) CUIComboBox::AddItem)
		//		.def("AddItem",				(void (CUIComboBox::*)(pcstr)) CUIComboBox::AddItem)
		];
}