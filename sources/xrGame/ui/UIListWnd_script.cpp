#include "stdafx.h"

#include "UIListWnd.h"
#include "UIListItemEx.h"
#include "UISpinText.h"
#include "UIComboBox.h"

using namespace luabind;

bool CUIListWnd::AddItem_script(CUIListItem* item)
{
	return AddItem(item, -1);
}

BOOL	xrRender_test_hw( );
bool xrRender_test_hw_script( )
{
	return !!xrRender_test_hw( );
}

struct CUIListItemWrapper : public CUIListItem, public luabind::wrap_base
{ };

struct CUIListItemExWrapper : public CUIListItemEx, public luabind::wrap_base
{ };


#pragma optimize("s",on)
void CUIListWnd::script_register(lua_State* L)
{

	module(L)
		[

			class_<CUIListWnd, CUIWindow>("CUIListWnd")
				.def(constructor<>( ))
				//		.def("AddText",					&CUIListWnd::AddText_script)
				.def("AddItem", &CUIListWnd::AddItem_script, adopt(_2))
				.def("RemoveItem", &CUIListWnd::RemoveItem)
				.def("RemoveAll", &CUIListWnd::RemoveAll)
				.def("EnableScrollBar", &CUIListWnd::EnableScrollBar)
				.def("IsScrollBarEnabled", &CUIListWnd::IsScrollBarEnabled)
				.def("ScrollToBegin", &CUIListWnd::ScrollToBegin)
				.def("ScrollToEnd", &CUIListWnd::ScrollToEnd)
				.def("SetItemHeight", &CUIListWnd::SetItemHeight)
				.def("GetItem", &CUIListWnd::GetItem)
				.def("GetItemPos", &CUIListWnd::GetItemPos)
				.def("GetSize", &CUIListWnd::GetItemsCount)
				.def("ScrollToBegin", &CUIListWnd::ScrollToBegin)
				.def("ScrollToEnd", &CUIListWnd::ScrollToEnd)
				.def("ScrollToPos", &CUIListWnd::ScrollToPos)
				.def("SetWidth", &CUIListWnd::SetWidth)
				.def("SetTextColor", &CUIListWnd::SetTextColor)
				.def("ActivateList", &CUIListWnd::ActivateList)
				.def("IsListActive", &CUIListWnd::IsListActive)
				.def("SetVertFlip", &CUIListWnd::SetVertFlip)
				.def("GetVertFlip", &CUIListWnd::GetVertFlip)
				.def("SetFocusedItem", &CUIListWnd::SetFocusedItem)
				.def("GetFocusedItem", &CUIListWnd::GetFocusedItem)
				.def("ShowSelectedItem", &CUIListWnd::ShowSelectedItem)

				.def("GetSelectedItem", &CUIListWnd::GetSelectedItem)
				.def("ResetFocusCapture", &CUIListWnd::ResetFocusCapture),

				class_<CUIListItem, CUIButton, CUIListItemWrapper>("CUIListItem")
				.def(constructor<>( )),

				class_<CUIListItemEx, CUIListItem/**/, CUIListItemExWrapper/**/>("CUIListItemEx")
				.def(constructor<>( ))
				.def("SetSelectionColor", &CUIListItemEx::SetSelectionColor),

				def("xrRender_test_r2_hw", &xrRender_test_hw_script)
		];
}