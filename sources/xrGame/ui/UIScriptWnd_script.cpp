#include "stdafx.h"

//UI-controls
#include "UIScriptWnd.h"
#include "UIButton.h"
#include "UIMessageBox.h"
#include "UIPropertiesBox.h"
#include "UICheckButton.h"
#include "UIRadioButton.h"
#include "UIStatic.h"
#include "UIEditBox.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIProgressBar.h"
#include "UITabControl.h"
#include "UIListWnd.h"

#include "uiscriptwnd_script.h"

using namespace luabind;

extern export_class &script_register_ui_window1(export_class &);
extern export_class &script_register_ui_window2(export_class &);

#pragma optimize("s",on)
void CUIDialogWndEx::script_register(lua_State *L)
{
	export_class				instance("CUIScriptWnd");

	module(L)
	[
		script_register_ui_window2(
			script_register_ui_window1(
				instance
			)
		)
		.def("Load",			&BaseType::Load)
	];
}

export_class &script_register_ui_window1(export_class &instance)
{
	instance
		.def(					constructor<>())

		.def("AddCallback",		(void(BaseType::*)(pcstr, s16, const luabind::functor<void>&))&BaseType::AddCallback)
		.def("AddCallback",		(void(BaseType::*)(pcstr, s16, const luabind::functor<void>&, const luabind::object&))&BaseType::AddCallback)

		.def("Register",		(void (BaseType::*)(CUIWindow*))&BaseType::Register)
		.def("Register",		(void (BaseType::*)(CUIWindow*, pcstr))&BaseType::Register)

		.def("GetButton",		(CUIButton* (BaseType::*)(pcstr)) &BaseType::GetControl<CUIButton>)
		.def("GetMessageBox",	(CUIMessageBox* (BaseType::*)(pcstr)) &BaseType::GetControl<CUIMessageBox>)
		.def("GetPropertiesBox",(CUIPropertiesBox* (BaseType::*)(pcstr)) &BaseType::GetControl<CUIPropertiesBox>)
		.def("GetCheckButton",	(CUICheckButton* (BaseType::*)(pcstr)) &BaseType::GetControl<CUICheckButton>)
		.def("GetRadioButton",	(CUIRadioButton* (BaseType::*)(pcstr)) &BaseType::GetControl<CUIRadioButton>)
//		.def("GetRadioGroup",	(CUIRadioGroup* (BaseType::*)(pcstr)) &BaseType::GetControl<CUIRadioGroup>)

	;return	(instance);
}

export_class &script_register_ui_window2(export_class &instance)
{
	instance
		.def("GetStatic",		(CUIStatic* (BaseType::*)(pcstr)) &BaseType::GetControl<CUIStatic>)
		.def("GetEditBox",		(CUIEditBox* (BaseType::*)(pcstr)) &BaseType::GetControl<CUIEditBox>)
		.def("GetDialogWnd",	(CUIDialogWnd* (BaseType::*)(pcstr)) &BaseType::GetControl<CUIDialogWnd>)
		.def("GetFrameWindow",	(CUIFrameWindow* (BaseType::*)(pcstr)) &BaseType::GetControl<CUIFrameWindow>)
		.def("GetFrameLineWnd",	(CUIFrameLineWnd* (BaseType::*)(pcstr)) &BaseType::GetControl<CUIFrameLineWnd>)
		.def("GetProgressBar",	(CUIProgressBar* (BaseType::*)(pcstr)) &BaseType::GetControl<CUIProgressBar>)
		.def("GetTabControl",	(CUITabControl* (BaseType::*)(pcstr)) &BaseType::GetControl<CUITabControl>)
		.def("GetListWnd",		(CUIListWnd* (BaseType::*)(pcstr)) &BaseType::GetControl<CUIListWnd>)

		.def("OnKeyboard",		&BaseType::OnKeyboard, &WrapType::OnKeyboard_static)
		.def("Update",			&BaseType::Update, &WrapType::Update_static)
		.def("Dispatch",		&BaseType::Dispatch, &WrapType::Dispatch_static)

	;return	(instance);
}
