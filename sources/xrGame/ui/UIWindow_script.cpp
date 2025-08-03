#include "stdafx.h"

#include "UIWindow.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIDialogWnd.h"
#include "../HUDManager.h"
#include "../GamePersistent.h"
#include "UILabel.h"
#include "UIMMShniaga.h"//
#include "UITextureMaster.h"
#include "UIScrollView.h"

CFontManager& mngr( )
{
	return *(UI( )->Font( ));
}

// hud font
s32 GetARGB(u16 a, u16 r, u16 g, u16 b)
{
	return color_argb(a, r, g, b);
}

CGameFont* GetFontSmall( )
{
	return mngr( ).pFontSmall;
}
CGameFont* GetFontMedium( )
{
	return mngr( ).pFontMedium;
}
CGameFont* GetFontDI( )
{
	return mngr( ).pFontDI;
}
CGameFont* GetFontGraffiti19Russian( )
{
	return mngr( ).pFontGraffiti19Russian;
}
CGameFont* GetFontGraffiti22Russian( )
{
	return mngr( ).pFontGraffiti22Russian;
}
CGameFont* GetFontGraffiti32Russian( )
{
	return mngr( ).pFontGraffiti32Russian;
}
CGameFont* GetFontGraffiti40Russian( )
{
	return mngr( ).pFontGraffiti40Russian;
}
CGameFont* GetFontGraffiti50Russian( )
{
	return mngr( ).pFontGraffiti50Russian;
}
CGameFont* GetFontLetterica16Russian( )
{
	return mngr( ).pFontLetterica16Russian;
}
CGameFont* GetFontLetterica18Russian( )
{
	return mngr( ).pFontLetterica18Russian;
}
CGameFont* GetFontLetterica25Russian( )
{
	return mngr( ).pFontLetterica25Russian;
}


fRect get_texture_rect(pcstr icon_name)
{
	return CUITextureMaster::GetTextureRect(icon_name);
}

pcstr get_texture_name(pcstr icon_name)
{
	return CUITextureMaster::GetTextureFileName(icon_name);
}

STextureInfo get_texture_info(pcstr name, pcstr def_name)
{
	return CUITextureMaster::FindItem(name, def_name);
}

using namespace luabind;

#pragma optimize("s",on)
void CUIWindow::script_register(lua_State* L)
{
	module(L)
		[
			def("GetARGB", &GetARGB),
				def("GetFontSmall", &GetFontSmall),
				def("GetFontMedium", &GetFontMedium),
				def("GetFontDI", &GetFontDI),
				def("GetFontGraffiti19Russian", &GetFontGraffiti19Russian),
				def("GetFontGraffiti22Russian", &GetFontGraffiti22Russian),
				def("GetFontGraffiti32Russian", &GetFontGraffiti32Russian),
				def("GetFontGraffiti40Russian", &GetFontGraffiti40Russian),
				def("GetFontGraffiti50Russian", &GetFontGraffiti50Russian),
				def("GetFontLetterica16Russian", &GetFontLetterica16Russian),
				def("GetFontLetterica18Russian", &GetFontLetterica18Russian),
				def("GetFontLetterica25Russian", &GetFontLetterica25Russian),

				class_<STextureInfo>("STextureInfo")
				.def("get_file_name", &STextureInfo::get_file_name)
				.def("get_rect", &STextureInfo::get_rect),

				def("GetTextureName", &get_texture_name),
				def("GetTextureRect", &get_texture_rect),
				def("GetTextureInfo", &get_texture_info),

				class_<CUIWindow>("CUIWindow")
				.def(constructor<>( ))
				.def("AttachChild", &CUIWindow::AttachChild, adopt(_2))
				.def("DetachChild", &CUIWindow::DetachChild)
				.def("SetAutoDelete", &CUIWindow::SetAutoDelete)
				.def("IsAutoDelete", &CUIWindow::IsAutoDelete)

				.def("SetWndRect", (void (CUIWindow::*)(fRect)) & CUIWindow::SetWndRect_script)
				.def("SetWndRect", (void (CUIWindow::*)(f32, f32, f32, f32)) & CUIWindow::SetWndRect_script)
				.def("Init", (void (CUIWindow::*)(f32, f32, f32, f32)) & CUIWindow::Init)
				.def("Init", (void (CUIWindow::*)(fRect*)) & CUIWindow::Init)
				.def("SetWndPos", (void (CUIWindow::*)(f32, f32)) & CUIWindow::SetWndPos)
				.def("SetWndSize", (void (CUIWindow::*)(f32, f32)) & CUIWindow::SetWndSize)
				.def("GetWidth", &CUIWindow::GetWidth)
				.def("SetWidth", &CUIWindow::SetWidth)
				.def("GetHeight", &CUIWindow::GetHeight)
				.def("SetHeight", &CUIWindow::SetHeight)

				.def("Enable", &CUIWindow::Enable)
				.def("IsEnabled", &CUIWindow::IsEnabled)
				.def("Show", &CUIWindow::Show)
				.def("IsShown", &CUIWindow::IsShown)
				.def("SetFont", &CUIWindow::SetFont)
				.def("GetFont", &CUIWindow::GetFont)

				.def("WindowName", &CUIWindow::WindowName_script)
				.def("SetWindowName", &CUIWindow::SetWindowName)
				.def("SetPPMode", &CUIWindow::SetPPMode)
				.def("ResetPPMode", &CUIWindow::ResetPPMode),

				class_<CUIDialogHolder>("CUIDialogHolder")
				.def("MainInputReceiver", &CUIDialogHolder::MainInputReceiver)
				.def("start_stop_menu", &CUIDialogHolder::StartStopMenu)
				.def("AddDialogToRender", &CUIDialogHolder::AddDialogToRender)
				.def("RemoveDialogToRender", &CUIDialogHolder::RemoveDialogToRender),

				class_<CUIDialogWnd, CUIWindow>("CUIDialogWnd")
				.def("GetHolder", &CUIDialogWnd::GetHolder)
				.def("SetHolder", &CUIDialogWnd::SetHolder),

				class_<CUIFrameWindow, CUIWindow>("CUIFrameWindow")
				.def(constructor<>( ))
				.def("SetWidth", &CUIFrameWindow::SetWidth)
				.def("SetHeight", &CUIFrameWindow::SetHeight)
				.def("SetColor", &CUIFrameWindow::SetColor)
				.def("GetTitleStatic", &CUIFrameWindow::GetTitleStatic)
				.def("Init", (void(CUIFrameWindow::*)(pcstr, f32, f32, f32, f32)) & CUIFrameWindow::Init),

				class_<CUIFrameLineWnd, CUIWindow>("CUIFrameLineWnd")
				.def(constructor<>( ))
				.def("SetWidth", &CUIFrameLineWnd::SetWidth)
				.def("SetHeight", &CUIFrameLineWnd::SetHeight)
				.def("SetOrientation", &CUIFrameLineWnd::SetOrientation)
				.def("SetColor", &CUIFrameLineWnd::SetColor)
				.def("GetTitleStatic", &CUIFrameLineWnd::GetTitleStatic)
				.def("Init", (void(CUIFrameLineWnd::*)(pcstr, f32, f32, f32, f32, bool)) & CUIFrameLineWnd::Init),

				class_<CUILabel, CUIFrameLineWnd>("CUILabel")
				.def(constructor<>( ))
				.def("SetText", &CUILabel::SetText)
				.def("GetText", &CUILabel::GetText),

				class_<CUIMMShniaga, CUIWindow>("CUIMMShniaga")
				.def("SetVisibleMagnifier", &CUIMMShniaga::SetVisibleMagnifier),

				class_<CUIScrollView, CUIWindow>("CUIScrollView")
				.def(constructor<>( ))
				.def("AddWindow", &CUIScrollView::AddWindow)
				.def("RemoveWindow", &CUIScrollView::RemoveWindow)
				.def("Clear", &CUIScrollView::Clear)
				.def("ScrollToBegin", &CUIScrollView::ScrollToBegin)
				.def("ScrollToEnd", &CUIScrollView::ScrollToEnd)
				.def("GetMinScrollPos", &CUIScrollView::GetMinScrollPos)
				.def("GetMaxScrollPos", &CUIScrollView::GetMaxScrollPos)
				.def("GetCurrentScrollPos", &CUIScrollView::GetCurrentScrollPos)
				.def("SetScrollPos", &CUIScrollView::SetScrollPos),

				class_<enum_exporter<EUIMessages> >("ui_events")
				.enum_("events")
				[
					// CUIWindow
					value("WINDOW_LBUTTON_DOWN", s32(WINDOW_LBUTTON_DOWN)),
						value("WINDOW_RBUTTON_DOWN", s32(WINDOW_RBUTTON_DOWN)),
						value("WINDOW_LBUTTON_UP", s32(WINDOW_LBUTTON_UP)),
						value("WINDOW_RBUTTON_UP", s32(WINDOW_RBUTTON_UP)),
						value("WINDOW_MOUSE_MOVE", s32(WINDOW_MOUSE_MOVE)),
						value("WINDOW_LBUTTON_DB_CLICK", s32(WINDOW_LBUTTON_DB_CLICK)),
						value("WINDOW_KEY_PRESSED", s32(WINDOW_KEY_PRESSED)),
						value("WINDOW_KEY_RELEASED", s32(WINDOW_KEY_RELEASED)),
						value("WINDOW_MOUSE_CAPTURE_LOST ", s32(WINDOW_MOUSE_CAPTURE_LOST)),
						value("WINDOW_KEYBOARD_CAPTURE_LOST", s32(WINDOW_KEYBOARD_CAPTURE_LOST)),

						// CUIStatic
						value("STATIC_FOCUS_RECEIVED", s32(STATIC_FOCUS_RECEIVED)),
						value("STATIC_FOCUS_LOST", s32(STATIC_FOCUS_LOST)),

						// CUIButton
						value("BUTTON_CLICKED", s32(BUTTON_CLICKED)),
						value("BUTTON_DOWN", s32(BUTTON_DOWN)),

						// CUITabControl
						value("TAB_CHANGED", s32(TAB_CHANGED)),
						value("EDIT_TEXT_COMMIT", s32(EDIT_TEXT_COMMIT)),

						// CUICheckButton
//						value("CHECK_BUTTON_SET", s32(CHECK_BUTTON_SET)),
//						value("CHECK_BUTTON_RESET", s32(CHECK_BUTTON_RESET)),

						// CUIRadioButton
//						value("RADIOBUTTON_SET", s32(RADIOBUTTON_SET)),

						// CUIdragDropItem
						value("DRAG_DROP_ITEM_DRAG", s32(DRAG_DROP_ITEM_DRAG)),
						value("DRAG_DROP_ITEM_DROP ", s32(DRAG_DROP_ITEM_DROP)),
						value("DRAG_DROP_ITEM_DB_CLICK", s32(DRAG_DROP_ITEM_DB_CLICK)),
						value("DRAG_DROP_ITEM_RBUTTON_CLICK", s32(DRAG_DROP_ITEM_RBUTTON_CLICK)),

						// CUIScrollBox
						value("SCROLLBOX_MOVE", s32(SCROLLBOX_MOVE)),

						// CUIScrollBar
						value("SCROLLBAR_VSCROLL", s32(SCROLLBAR_VSCROLL)),
						value("SCROLLBAR_HSCROLL", s32(SCROLLBAR_HSCROLL)),

						// CUIListWnd
						value("LIST_ITEM_CLICKED", s32(LIST_ITEM_CLICKED)),
						value("LIST_ITEM_SELECT", s32(LIST_ITEM_SELECT)),

						// CUIInteractiveItem
//						value("INTERACTIVE_ITEM_CLICK", s32(INTERACTIVE_ITEM_CLICK)),

						// UIPropertiesBox
						value("PROPERTY_CLICKED", s32(PROPERTY_CLICKED)),

						// CUIMessageBox
						value("MESSAGE_BOX_OK_CLICKED", s32(MESSAGE_BOX_OK_CLICKED)),
						value("MESSAGE_BOX_YES_CLICKED", s32(MESSAGE_BOX_YES_CLICKED)),
						value("MESSAGE_BOX_NO_CLICKED", s32(MESSAGE_BOX_NO_CLICKED)),
						value("MESSAGE_BOX_CANCEL_CLICKED", s32(MESSAGE_BOX_CANCEL_CLICKED)),
						value("MESSAGE_BOX_QUIT_GAME_CLICKED", s32(MESSAGE_BOX_QUIT_GAME_CLICKED)),
						value("MESSAGE_BOX_QUIT_WIN_CLICKED", s32(MESSAGE_BOX_QUIT_WIN_CLICKED)),

						value("EDIT_TEXT_CHANGED", s32(EDIT_TEXT_CHANGED)),
						value("EDIT_TEXT_COMMIT", s32(EDIT_TEXT_COMMIT)),

						// CUITalkDialogWnd
						value("TALK_DIALOG_TRADE_BUTTON_CLICKED", s32(TALK_DIALOG_TRADE_BUTTON_CLICKED)),
						value("TALK_DIALOG_QUESTION_CLICKED", s32(TALK_DIALOG_QUESTION_CLICKED)),

						// CUIPdaDialogWnd
//						value("PDA_DIALOG_WND_BACK_BUTTON_CLICKED", s32(PDA_DIALOG_WND_BACK_BUTTON_CLICKED)),
//						value("PDA_DIALOG_WND_MESSAGE_BUTTON_CLICKED", s32(PDA_DIALOG_WND_MESSAGE_BUTTON_CLICKED)),

						// CUIPdaContactsWnd
//						value("PDA_CONTACTS_WND_CONTACT_SELECTED", s32(PDA_CONTACTS_WND_CONTACT_SELECTED)),

						// CUITradeWnd
						value("TRADE_WND_CLOSED", s32(TRADE_WND_CLOSED)),

						// CUIOutfitSlot
//						value("UNDRESS_OUTFIT", s32(UNDRESS_OUTFIT)),
//						value("OUTFIT_RETURNED_BACK", s32(OUTFIT_RETURNED_BACK)),

						// CUIInventroyWnd
						value("INVENTORY_DROP_ACTION", s32(INVENTORY_DROP_ACTION)),
						value("INVENTORY_EAT_ACTION", s32(INVENTORY_EAT_ACTION)),
						value("INVENTORY_TO_BELT_ACTION ", s32(INVENTORY_TO_BELT_ACTION)),
						value("INVENTORY_TO_SLOT_ACTION", s32(INVENTORY_TO_SLOT_ACTION)),
						value("INVENTORY_TO_BAG_ACTION", s32(INVENTORY_TO_BAG_ACTION)),
						value("INVENTORY_ATTACH_ADDON ", s32(INVENTORY_ATTACH_ADDON)),
						value("INVENTORY_DETACH_SCOPE_ADDON", s32(INVENTORY_DETACH_SCOPE_ADDON)),
						value("INVENTORY_DETACH_SILENCER_ADDON", s32(INVENTORY_DETACH_SILENCER_ADDON)),
						value("INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON", s32(INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON))
				]
		];
}