#include "stdafx.h"

//UI-controls
#include "UIListWnd.h"//
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

#pragma optimize("s",on)
export_class& script_register_ui_window2(export_class& instance)
{
	instance
		.def("GetStatic", (CUIStatic * (BaseType::*)(const char*)) & BaseType::GetControl<CUIStatic>)
		.def("GetEditBox", (CUIEditBox * (BaseType::*)(const char*)) & BaseType::GetControl<CUIEditBox>)
		.def("GetDialogWnd", (CUIDialogWnd * (BaseType::*)(const char*)) & BaseType::GetControl<CUIDialogWnd>)
		.def("GetFrameWindow", (CUIFrameWindow * (BaseType::*)(const char*)) & BaseType::GetControl<CUIFrameWindow>)
		.def("GetFrameLineWnd", (CUIFrameLineWnd * (BaseType::*)(const char*)) & BaseType::GetControl<CUIFrameLineWnd>)
		.def("GetProgressBar", (CUIProgressBar * (BaseType::*)(const char*)) & BaseType::GetControl<CUIProgressBar>)
		.def("GetTabControl", (CUITabControl * (BaseType::*)(const char*)) & BaseType::GetControl<CUITabControl>)
		.def("GetListWnd", (CUIListWnd * (BaseType::*)(const char*)) & BaseType::GetControl<CUIListWnd>)

		.def("OnKeyboard", &BaseType::OnKeyboard, &WrapType::OnKeyboard_static)
		.def("Update", &BaseType::Update, &WrapType::Update_static)
		.def("Dispatch", &BaseType::Dispatch, &WrapType::Dispatch_static)

		; return	(instance);
}
