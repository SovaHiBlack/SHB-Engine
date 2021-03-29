#include "stdafx.h"

//UI-controls
#include "UIScriptWnd.h"
#include "UIButton.h"//
#include "UIMessageBox.h"//
#include "UIPropertiesBox.h"
#include "UICheckButton.h"//
#include "UIRadioButton.h"//
#include "UIStatic.h"//
#include "UIEditBox.h"//
#include "UIFrameWindow.h"//
#include "UIFrameLineWnd.h"//
#include "UIProgressBar.h"//
#include "UITabControl.h"//

#include "uiscriptwnd_script.h"

using namespace luabind;

extern export_class& script_register_ui_window1(export_class&);
extern export_class& script_register_ui_window2(export_class&);

#pragma optimize("s", on)
void CUIDialogWndEx::script_register(lua_State* L)
{
	export_class instance("CUIScriptWnd");

	module(L)
		[
			script_register_ui_window2(script_register_ui_window1(instance))
			.def("Load", &BaseType::Load)
		];
}

export_class& script_register_ui_window1(export_class& instance)
{
	instance
		.def(constructor<>( ))

		.def("AddCallback", (void(BaseType::*)(const char*, s16, const luabind::functor<void>&)) & BaseType::AddCallback)
		.def("AddCallback", (void(BaseType::*)(const char*, s16, const luabind::functor<void>&, const luabind::object&)) & BaseType::AddCallback)

		.def("Register", (void (BaseType::*)(CUIWindow*)) & BaseType::Register)
		.def("Register", (void (BaseType::*)(CUIWindow*, const char*)) & BaseType::Register)

		.def("GetButton", (CUIButton * (BaseType::*)(const char*)) & BaseType::GetControl<CUIButton>)
		.def("GetMessageBox", (CUIMessageBox * (BaseType::*)(const char*)) & BaseType::GetControl<CUIMessageBox>)
		.def("GetPropertiesBox", (CUIPropertiesBox * (BaseType::*)(const char*)) & BaseType::GetControl<CUIPropertiesBox>)
		.def("GetCheckButton", (CUICheckButton * (BaseType::*)(const char*)) & BaseType::GetControl<CUICheckButton>)
		.def("GetRadioButton", (CUIRadioButton * (BaseType::*)(const char*)) & BaseType::GetControl<CUIRadioButton>)
//		.def("GetRadioGroup",	(CUIRadioGroup* (BaseType::*)(const char*)) &BaseType::GetControl<CUIRadioGroup>)

; return	(instance);
}
