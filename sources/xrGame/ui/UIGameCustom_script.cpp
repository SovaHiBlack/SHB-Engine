#include "stdafx.h"

#include "UIGameCustom.h"//
#include "..\Level.h"
#include "..\HUDManager.h"//
#include "UIStatic.h"//

using namespace luabind;

CUIGameCustom* get_hud(){
	return HUD().GetUI()->UIGame();
}

#pragma optimize("s",on)
void CUIGameCustom::script_register(lua_State *L)
{
	module(L)
		[
			class_< SDrawStaticStruct >("SDrawStaticStruct")
			.def_readwrite("m_endTime",		&SDrawStaticStruct::m_endTime)
			.def("wnd",					&SDrawStaticStruct::wnd),

			class_< CUIGameCustom >("CUIGameCustom")
			.def("AddDialogToRender",		&CUIGameCustom::AddDialogToRender)
			.def("RemoveDialogToRender",	&CUIGameCustom::RemoveDialogToRender)
			.def("AddCustomMessage",		(void(CUIGameCustom::*)(const char*, float, float, float, CGameFont*, u16, u32/*, const char**/))&CUIGameCustom::AddCustomMessage)
			.def("AddCustomMessage",		(void(CUIGameCustom::*)(const char*, float, float, float, CGameFont*, u16, u32/*, const char**/, float))&CUIGameCustom::AddCustomMessage)
			.def("CustomMessageOut",		&CUIGameCustom::CustomMessageOut)
			.def("RemoveCustomMessage",		&CUIGameCustom::RemoveCustomMessage)
			.def("AddCustomStatic",			&CUIGameCustom::AddCustomStatic)
			.def("RemoveCustomStatic",		&CUIGameCustom::RemoveCustomStatic)
			.def("GetCustomStatic",			&CUIGameCustom::GetCustomStatic),
			def("get_hud",					&get_hud)
		];
}
