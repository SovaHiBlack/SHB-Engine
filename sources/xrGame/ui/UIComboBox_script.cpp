// File:        UIComboBox_script.cpp
// Description: exports CUIComobBox to LUA environment
// Created:     11.12.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
//
// Copyright 2004 GSC Game World
//
#include "stdafx.h"

#include "UIComboBox.h"

using namespace luabind;

#pragma optimize("s",on)
void CUIComboBox::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUIComboBox, CUIWindow>("CUIComboBox")
		.def(						constructor<>())
		.def("Init",				(void (CUIComboBox::*)(f32, f32, f32))   &CUIComboBox::Init)
		.def("Init",				(void (CUIComboBox::*)(f32, f32, f32, f32))   &CUIComboBox::Init)
		.def("SetVertScroll",		&CUIComboBox::SetVertScroll)
		.def("SetListLength",		&CUIComboBox::SetListLength)
		.def("CurrentID",			&CUIComboBox::CurrentID)
		.def("SetCurrentID",		&CUIComboBox::SetItem)
		
//		.def("AddItem",				(void (CUIComboBox::*)(pcstr, bool)) CUIComboBox::AddItem)
//		.def("AddItem",				(void (CUIComboBox::*)(pcstr)) CUIComboBox::AddItem)
	];
}