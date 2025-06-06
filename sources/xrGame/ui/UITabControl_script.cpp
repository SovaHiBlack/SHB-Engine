#include "stdafx.h"

#include "UITabControl.h"
#include "UITabButton.h"

using namespace luabind;

#pragma optimize("s",on)
void CUITabControl::script_register(lua_State* L)
{
	module(L)
		[
			class_<CUITabControl, CUIWindow>("CUITabControl")
				.def(constructor<>( ))
				.def("AddItem", (bool (CUITabControl::*)(CUITabButton*))(&CUITabControl::AddItem), adopt(_2))
				.def("AddItem", (bool (CUITabControl::*)(pcstr, pcstr, f32, f32, f32, f32)) & CUITabControl::AddItem)
				.def("RemoveItem", &CUITabControl::RemoveItem)
				.def("RemoveAll", &CUITabControl::RemoveAll)
				.def("GetActiveIndex", &CUITabControl::GetActiveIndex)
				.def("GetTabsCount", &CUITabControl::GetTabsCount)
				.def("SetNewActiveTab", &CUITabControl::SetNewActiveTab)
				.def("GetButtonByIndex", &CUITabControl::GetButtonByIndex),

				class_<CUITabButton, CUIButton>("CUITabButton")
				.def(constructor<>( ))
		];
}