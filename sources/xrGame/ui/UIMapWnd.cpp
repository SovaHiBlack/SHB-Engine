#include "stdafx.h"
#include "UIMapWnd.h"//
#include "UIMap.h"//
#include "UIXmlInit.h"
#include "../MapLocation.h"
#include "../MapManager.h"
#include "UIInventoryUtilities.h"//

#include "UIScrollBar.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UITabControl.h"
#include "UI3tButton.h"
#include "UIMapWndActions.h"//
#include "UIMapWndActionsSpace.h"//
#include "UIMapHint.h"

#include "../HUDManager.h"

#include "../..\XR_3DA\Input.h"		//remove me !!!

const s32			SCROLLBARS_SHIFT = 5;
const s32			VSCROLLBAR_STEP = 20; // � ��������
const s32			HSCROLLBAR_STEP = 20; // � ��������

static bool			MAP_FLY_MODE = true;

CUIMapWnd::CUIMapWnd( )
{
	m_tgtMap = NULL;
	m_GlobalMap = NULL;
	m_flags.zero( );
	m_currentZoom = 1.0f;
	m_hint = NULL;
	m_text_hint = NULL;
}

CUIMapWnd::~CUIMapWnd( )
{
	delete_data(m_ActionPlanner);
	delete_data(m_GameMaps);
	delete_data(m_hint);
	delete_data(m_text_hint);
}

void CUIMapWnd::Init(pcstr xml_name, pcstr start_from)
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, xml_name);
	R_ASSERT3(xml_result, "xml file not found", xml_name);

	string512	pth;
	// load map background
	CUIXmlInit xml_init;
	strconcat(sizeof(pth), pth, start_from, ":main_wnd");
	xml_init.InitWindow(uiXml, pth, 0, this);

	m_UIMainFrame = xr_new<CUIFrameWindow>( );
	m_UIMainFrame->SetAutoDelete(true);
	AttachChild(m_UIMainFrame);
	strconcat(sizeof(pth), pth, start_from, ":main_wnd:main_map_frame");
	xml_init.InitFrameWindow(uiXml, pth, 0, m_UIMainFrame);

	m_UILevelFrame = xr_new<CUIWindow>( );
	m_UILevelFrame->SetAutoDelete(true);
	strconcat(sizeof(pth), pth, start_from, ":main_wnd:main_map_frame:level_frame");
	xml_init.InitWindow(uiXml, pth, 0, m_UILevelFrame);
	m_UIMainFrame->AttachChild(m_UILevelFrame);

	fRect r = m_UILevelFrame->GetWndRect( );

	m_UIMainScrollH = xr_new<CUIScrollBar>( );
	m_UIMainScrollH->SetAutoDelete(true);
	m_UIMainScrollH->Init(r.left, r.bottom + SCROLLBARS_SHIFT, r.right - r.left, true, "pda");
	m_UIMainScrollH->SetWindowName("scroll_h");
	m_UIMainScrollH->SetStepSize(_max(1, iFloor(m_UILevelFrame->GetWidth( ) / 10)));
	m_UIMainScrollH->SetPageSize(iFloor(m_UILevelFrame->GetWidth( )));
	m_UIMainFrame->AttachChild(m_UIMainScrollH);
	Register(m_UIMainScrollH);
	AddCallback("scroll_h", SCROLLBAR_HSCROLL, CUIWndCallback::void_function(this, &CUIMapWnd::OnScrollH));

	m_UIMainScrollV = xr_new<CUIScrollBar>( );
	m_UIMainScrollV->SetAutoDelete(true);
	m_UIMainScrollV->Init(r.right + SCROLLBARS_SHIFT, r.top, m_UIMainScrollH->GetWndRect( ).bottom - r.top, false, "pda");
	m_UIMainScrollV->SetWindowName("scroll_v");
	m_UIMainScrollV->SetStepSize(_max(1, iFloor(m_UILevelFrame->GetHeight( ) / 10)));
	m_UIMainScrollV->SetPageSize(iFloor(m_UILevelFrame->GetHeight( )));
	m_UIMainFrame->AttachChild(m_UIMainScrollV);
	Register(m_UIMainScrollV);
	AddCallback("scroll_v", SCROLLBAR_VSCROLL, CUIWndCallback::void_function(this, &CUIMapWnd::OnScrollV));

	UIMainMapHeader = xr_new<CUIFrameLineWnd>( );
	UIMainMapHeader->SetAutoDelete(true);
	m_UIMainFrame->AttachChild(UIMainMapHeader);
	strconcat(sizeof(pth), pth, start_from, ":main_wnd:map_header_frame_line");
	xml_init.InitFrameLine(uiXml, pth, 0, UIMainMapHeader);

	ZeroMemory(m_ToolBar, sizeof(m_ToolBar));
	xr_string  sToolbar;
	sToolbar = xr_string(start_from) + ":main_wnd:map_header_frame_line:tool_bar";

	EMapToolBtn		btnIndex;
	btnIndex = eGlobalMap;
	strconcat(sizeof(pth), pth, sToolbar.c_str( ), ":global_map_btn");
	if (uiXml.NavigateToNode(pth, 0))
	{
		m_ToolBar[btnIndex] = xr_new<CUI3tButton>( );
		m_ToolBar[btnIndex]->SetAutoDelete(true);
		xml_init.Init3tButton(uiXml, pth, 0, m_ToolBar[btnIndex]);
		UIMainMapHeader->AttachChild(m_ToolBar[btnIndex]);
		Register(m_ToolBar[btnIndex]);
		AddCallback(*m_ToolBar[btnIndex]->WindowName( ), BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIMapWnd::OnToolGlobalMapClicked));
	}

	btnIndex = eActor;
	strconcat(sizeof(pth), pth, sToolbar.c_str( ), ":actor_btn");
	if (uiXml.NavigateToNode(pth, 0))
	{
		m_ToolBar[btnIndex] = xr_new<CUI3tButton>( );
		m_ToolBar[btnIndex]->SetAutoDelete(true);
		xml_init.Init3tButton(uiXml, pth, 0, m_ToolBar[btnIndex]);
		UIMainMapHeader->AttachChild(m_ToolBar[btnIndex]);
		Register(m_ToolBar[btnIndex]);
		AddCallback(*m_ToolBar[btnIndex]->WindowName( ), BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIMapWnd::OnToolActorClicked));
	}

	btnIndex = eZoomIn;
	strconcat(sizeof(pth), pth, sToolbar.c_str( ), ":zoom_in_btn");
	if (uiXml.NavigateToNode(pth, 0))
	{
		m_ToolBar[btnIndex] = xr_new<CUI3tButton>( );
		m_ToolBar[btnIndex]->SetAutoDelete(true);
		xml_init.Init3tButton(uiXml, pth, 0, m_ToolBar[btnIndex]);
		UIMainMapHeader->AttachChild(m_ToolBar[btnIndex]);
		Register(m_ToolBar[btnIndex]);
		AddCallback(*m_ToolBar[btnIndex]->WindowName( ), BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIMapWnd::OnToolZoomInClicked));
	}

	btnIndex = eZoomOut;
	strconcat(sizeof(pth), pth, sToolbar.c_str( ), ":zoom_out_btn");
	if (uiXml.NavigateToNode(pth, 0))
	{
		m_ToolBar[btnIndex] = xr_new<CUI3tButton>( );
		m_ToolBar[btnIndex]->SetAutoDelete(true);
		xml_init.Init3tButton(uiXml, pth, 0, m_ToolBar[btnIndex]);
		UIMainMapHeader->AttachChild(m_ToolBar[btnIndex]);
		Register(m_ToolBar[btnIndex]);
		AddCallback(*m_ToolBar[btnIndex]->WindowName( ), BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIMapWnd::OnToolZoomOutClicked));
	}

	m_text_hint = xr_new<CUIStatic>( );
	strconcat(sizeof(pth), pth, start_from, ":main_wnd:text_hint");
	xml_init.InitStatic(uiXml, pth, 0, m_text_hint);

	m_hint = xr_new<CUIMapHint>( );
	m_hint->Init( );
	m_hint->SetAutoDelete(false);

	// Load maps
	CIniFile& gameLtx = *pGameIni;

	m_GlobalMap = xr_new<CUIGlobalMap>(this);
	m_GlobalMap->SetAutoDelete(true);
	m_GlobalMap->Init("global_map", gameLtx, "hud\\default");

	m_UILevelFrame->AttachChild(m_GlobalMap);
	m_GlobalMap->OptimalFit(m_UILevelFrame->GetWndRect( ));
	m_GlobalMap->SetMinZoom(m_GlobalMap->GetCurrentZoom( ));
	m_currentZoom = m_GlobalMap->GetCurrentZoom( );

	// initialize local maps
	xr_string sect_name;
	sect_name = "level_maps_single";

	if (gameLtx.section_exist(sect_name.c_str( )))
	{
		CIniFile::Sect& S = gameLtx.r_section(sect_name.c_str( ));
		CIniFile::SectCIt	it = S.Data.begin( ), end = S.Data.end( );
		for (; it != end; it++)
		{
			shared_str map_name = it->first;
			xr_strlwr(map_name);
			R_ASSERT2(m_GameMaps.end( ) == m_GameMaps.find(map_name), "Duplicate level name not allowed");

			CUICustomMap*& l = m_GameMaps[map_name];

			l = xr_new<CUILevelMap>(this);

			l->Init(map_name, gameLtx, "hud\\default");

			l->OptimalFit(m_UILevelFrame->GetWndRect( ));
		}
	}

#ifdef DEBUG
	GameMaps::iterator it = m_GameMaps.begin( );
	GameMaps::iterator it2;
	for (; it != m_GameMaps.end( ); ++it)
	{
		CUILevelMap* l = smart_cast<CUILevelMap*>(it->second);
		VERIFY(l);
		for (it2 = it; it2 != m_GameMaps.end( ); ++it2)
		{
			if (it == it2)
			{
				continue;
			}

			CUILevelMap* l2 = smart_cast<CUILevelMap*>(it2->second);
			VERIFY(l2);
			if (l->GlobalRect( ).intersected(l2->GlobalRect( )))
			{
				Msg(" --error-incorrect map definition global rect of map [%s] intersects with [%s]", *l->MapName( ), *l2->MapName( ));
			}
		}

		if (FALSE == l->GlobalRect( ).intersected(GlobalMap( )->BoundRect( )))
		{
			Msg(" --error-incorrect map definition map [%s] places outside global map", *l->MapName( ));
		}
	}
#endif

	Register(m_GlobalMap);
	m_ActionPlanner = xr_new<CMapActionPlanner>( );
	m_ActionPlanner->setup(this);
	m_flags.set(lmFirst, TRUE);
}

void CUIMapWnd::Show(bool status)
{
	inherited::Show(status);
	if (status)
	{
		m_GlobalMap->Show(true);
		m_GlobalMap->SetClipRect(ActiveMapRect( ));
		GameMaps::iterator	it = m_GameMaps.begin( );
		for (; it != m_GameMaps.end( ); ++it)
		{
			m_GlobalMap->AttachChild(it->second);
			it->second->Show(true);
			it->second->SetClipRect(ActiveMapRect( ));
		}

		if (m_flags.test(lmFirst))
		{
			inherited::Update( );// only maps, not action planner
			OnToolActorClicked(NULL, NULL);
			m_flags.set(lmFirst, FALSE);
		}

		InventoryUtilities::SendInfoToActor("ui_pda_map_local");
	}
	else
	{
		if (GlobalMap( ))
		{
			GlobalMap( )->DetachAll( );
			GlobalMap( )->Show(false);
		}

		GameMaps::iterator	it = m_GameMaps.begin( );
		for (; it != m_GameMaps.end( ); ++it)
		{
			it->second->DetachAll( );
		}
	}

	m_hint->SetOwner(NULL);
}

void CUIMapWnd::AddMapToRender(CUICustomMap* m)
{
	Register(m);
	m_UILevelFrame->AttachChild(m);
	m->Show(true);
	m_UILevelFrame->BringToTop(m);
	m->SetClipRect(ActiveMapRect( ));
}

void CUIMapWnd::RemoveMapToRender(CUICustomMap* m)
{
	if (m != GlobalMap( ))
	{
		m_UILevelFrame->DetachChild(smart_cast<CUIWindow*>(m));
	}
}

void CUIMapWnd::SetTargetMap(const shared_str& name, const fVector2& pos, bool bZoomIn)
{
	u16	idx = GetIdxByName(name);
	if (idx != u16(-1))
	{
		CUICustomMap* lm = GetMapByIdx(idx);
		SetTargetMap(lm, pos, bZoomIn);
	}
}

void CUIMapWnd::SetTargetMap(const shared_str& name, bool bZoomIn)
{
	u16	idx = GetIdxByName(name);
	if (idx != u16(-1))
	{
		CUICustomMap* lm = GetMapByIdx(idx);
		SetTargetMap(lm, bZoomIn);
	}
}

void CUIMapWnd::SetTargetMap(CUICustomMap* m, bool bZoomIn)
{
	m_tgtMap = m;
	fVector2 pos;
	fRect r = m->BoundRect( );
	r.getcenter(pos);
	SetTargetMap(m, pos, bZoomIn);
}

void CUIMapWnd::SetTargetMap(CUICustomMap* m, const fVector2& pos, bool bZoomIn)
{
	m_tgtMap = m;

	if (m == GlobalMap( ))
	{
		CUIGlobalMap* gm = GlobalMap( );
		SetZoom(gm->GetMinZoom( ));
		fRect vis_rect = ActiveMapRect( );
		vis_rect.getcenter(m_tgtCenter);
		fVector2 _p;
		gm->GetAbsolutePos(_p);
		m_tgtCenter.sub(_p);
		m_tgtCenter.div(gm->GetCurrentZoom( ));
	}
	else
	{
		if (bZoomIn && fsimilar(GlobalMap( )->GetCurrentZoom( ), GlobalMap( )->GetMinZoom( ), EPSILON_3))
		{
			SetZoom(GlobalMap( )->GetMaxZoom( ));
		}

		m_tgtCenter = m->ConvertRealToLocalNoTransform(pos);
		m_tgtCenter.add(m->GetWndPos( )).div(GlobalMap( )->GetCurrentZoom( ));
	}

	ResetActionPlanner( );
}

void CUIMapWnd::Draw( )
{
	inherited::Draw( );
	m_text_hint->Draw( );
	if (m_hint->GetOwner( ))
	{
		m_hint->Draw_( );
	}
}

bool CUIMapWnd::OnKeyboardHold(s32 dik)
{
	switch (dik)
	{
		case DIK_UP:
		case DIK_DOWN:
		case DIK_LEFT:
		case DIK_RIGHT:
		{
			fVector2 pos_delta;
			pos_delta.set(0.0f, 0.0f);

			if (dik == DIK_UP)
			{
				pos_delta.y += 1.0f;
			}

			if (dik == DIK_DOWN)
			{
				pos_delta.y -= 1.0f;
			}

			if (dik == DIK_LEFT)
			{
				pos_delta.x += 1.0f;
			}

			if (dik == DIK_RIGHT)
			{
				pos_delta.x -= 1.0f;
			}

			GlobalMap( )->MoveWndDelta(pos_delta);
			UpdateScroll( );
			m_hint->SetOwner(NULL);
			return true;
		}
		break;
	}

	return false;
}

bool CUIMapWnd::OnKeyboard(s32 dik, EUIMessages keyboard_action)
{
	switch (dik)
	{
		case DIK_NUMPADMINUS:
		{
			SetZoom(GetZoom( ) / 1.5f);
			ResetActionPlanner( );
			return true;
		}
		break;
		case DIK_NUMPADPLUS:
		{
			SetZoom(GetZoom( ) * 1.5f);
			ResetActionPlanner( );
			return true;
		}
		break;
	}

	return inherited::OnKeyboard(dik, keyboard_action);
}

bool CUIMapWnd::OnMouse(f32 x, f32 y, EUIMessages mouse_action)
{
	if (inherited::OnMouse(x, y, mouse_action))
	{
		return true;
	}

	fVector2 cursor_pos = GetUICursor( )->GetCursorPosition( );

	if (GlobalMap( ) && !GlobalMap( )->Locked( ) && ActiveMapRect( ).in(cursor_pos))
	{
		switch (mouse_action)
		{
			case WINDOW_MOUSE_MOVE:
			{
				if (pInput->iGetAsyncBtnState(0))
				{
					GlobalMap( )->MoveWndDelta(GetUICursor( )->GetCursorPositionDelta( ));
					UpdateScroll( );
					m_hint->SetOwner(NULL);
					return true;
				}
			}
			break;
		}
	}

	if (((mouse_action == WINDOW_LBUTTON_DOWN) && (m_flags.is_any(lmZoomIn + lmZoomOut))) ||
		(mouse_action == WINDOW_MOUSE_WHEEL_DOWN) ||
		(mouse_action == WINDOW_MOUSE_WHEEL_UP)
		)
	{
		bool b_zoom_in = (mouse_action == WINDOW_LBUTTON_DOWN && m_flags.test(lmZoomIn)) ||
			(mouse_action == WINDOW_MOUSE_WHEEL_DOWN);

		if (mouse_action == WINDOW_MOUSE_WHEEL_UP)
		{
//.				Msg("up");
		}

		if (mouse_action == WINDOW_MOUSE_WHEEL_DOWN)
		{
//.				Msg("down");
		}

		CUIGlobalMap* gm = GlobalMap( );
		f32 _prev_zoom = GetZoom( );
		if (b_zoom_in)
		{
			SetZoom(GetZoom( ) * 1.5f);
		}
		else
		{
			SetZoom(GetZoom( ) / 1.5f);
		}

		if (!fsimilar(_prev_zoom, GetZoom( )))
		{
			m_tgtCenter = cursor_pos;
			fVector2						_p;
			gm->GetAbsolutePos(_p);
			m_tgtCenter.sub(_p);
			m_tgtCenter.div(gm->GetCurrentZoom( ));
			ResetActionPlanner( );
			m_hint->SetOwner(NULL);
		}

		return true;
	}

	return false;
}

void CUIMapWnd::SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

CUICustomMap* CUIMapWnd::GetMapByIdx(u16 idx)
{
	VERIFY(idx != u16(-1));
	GameMapsPairIt it = m_GameMaps.begin( );
	std::advance(it, idx);
	return it->second;
}

u16 CUIMapWnd::GetIdxByName(const shared_str& map_name)
{
	GameMapsPairIt it = m_GameMaps.find(map_name);
	if (it == m_GameMaps.end( ))
	{
		Msg("~ Level Map '%s' not registered", map_name.c_str( ));
		return u16(-1);
	}

	return (u16)std::distance(m_GameMaps.begin( ), it);
}

void CUIMapWnd::UpdateScroll( )
{
	fVector2 w_pos = GlobalMap( )->GetWndPos( );
	m_UIMainScrollV->SetRange(0, iFloor(GlobalMap( )->GetHeight( )));
	m_UIMainScrollH->SetRange(0, iFloor(GlobalMap( )->GetWidth( )));

	m_UIMainScrollV->SetScrollPos(iFloor(-w_pos.y));
	m_UIMainScrollH->SetScrollPos(iFloor(-w_pos.x));
}

void CUIMapWnd::OnScrollV(CUIWindow*, pvoid)
{
	if (GlobalMap( ))
	{
		s32 s_pos = m_UIMainScrollV->GetScrollPos( );
		fVector2 w_pos = GlobalMap( )->GetWndPos( );
		GlobalMap( )->SetWndPos(w_pos.x, f32(-s_pos));
	}
}

void CUIMapWnd::OnScrollH(CUIWindow*, pvoid)
{
	if (GlobalMap( ))
	{
		s32 s_pos = m_UIMainScrollH->GetScrollPos( );
		fVector2 w_pos = GlobalMap( )->GetWndPos( );
		GlobalMap( )->SetWndPos(f32(-s_pos), w_pos.y);
	}
}

void CUIMapWnd::Update( )
{
	if (m_GlobalMap)
	{
		m_GlobalMap->SetClipRect(ActiveMapRect( ));
	}

	inherited::Update( );
	m_ActionPlanner->update( );
}

void CUIMapWnd::SetZoom(f32 value)
{
//	f32 _prev_zoom = m_currentZoom;
	m_currentZoom = value;
	clamp(m_currentZoom, GlobalMap( )->GetMinZoom( ), GlobalMap( )->GetMaxZoom( ));
}

void CUIMapWnd::OnToolGlobalMapClicked(CUIWindow* w, pvoid)
{
	if (GlobalMap( )->Locked( ))
	{
		return;
	}

	SetTargetMap(GlobalMap( ));
}

void CUIMapWnd::ResetActionPlanner( )
{
	if (MAP_FLY_MODE)
	{
		m_ActionPlanner->m_storage.set_property(1, false);
		m_ActionPlanner->m_storage.set_property(2, false);
		m_ActionPlanner->m_storage.set_property(3, false);
	}
	else
	{
		fRect m_desiredMapRect;
		GlobalMap( )->CalcOpenRect(m_tgtCenter, m_desiredMapRect, GetZoom( ));
		GlobalMap( )->SetWndRect(m_desiredMapRect);
		UpdateScroll( );
	}
}

void CUIMapWnd::OnToolZoomInClicked(CUIWindow* w, pvoid)
{
	if (GlobalMap( )->Locked( ))
	{
		return;
	}

	m_flags.zero( );

	CUI3tButton* btn = smart_cast<CUI3tButton*>(w);
	bool bPushed = btn->GetCheck( );
	m_flags.set(lmZoomIn, bPushed);
	ValidateToolBar( );
}

void CUIMapWnd::OnToolZoomOutClicked(CUIWindow* w, pvoid)
{
	if (GlobalMap( )->Locked( ))
	{
		return;
	}

	m_flags.zero( );

	CUI3tButton* btn = smart_cast<CUI3tButton*>(w);
	bool bPushed = btn->GetCheck( );
	m_flags.set(lmZoomOut, bPushed);
	ValidateToolBar( );
}

void CUIMapWnd::ValidateToolBar( )
{
	CUI3tButton* btn = NULL;
	btn = m_ToolBar[eZoomIn];
	if (btn)
	{
		btn->SetCheck(!!m_flags.test(lmZoomIn));
	}

	btn = m_ToolBar[eZoomOut];
	if (btn)
	{
		btn->SetCheck(!!m_flags.test(lmZoomOut));
	}
}

void CUIMapWnd::OnToolActorClicked(CUIWindow*, pvoid)
{
	if (GlobalMap( )->Locked( ))
	{
		return;
	}

	fVector3 v = Level( ).CurrentEntity( )->Position( );
	fVector2 v2;
	v2.set(v.x, v.z);

	CUICustomMap* lm = NULL;
	u16	idx = GetIdxByName(Level( ).name( ));
	if (idx != u16(-1))
	{
		lm = GetMapByIdx(idx);
	}
	else
	{
		lm = GlobalMap( );
	}

	SetTargetMap(lm, v2, true);
}

bool is_in(const fRect& b1, const fRect& b2)
{
	return (b1.x1 < b2.x1) && (b1.x2 > b2.x2) && (b1.y1 < b2.y1) && (b1.y2 > b2.y2);
}

void CUIMapWnd::ShowHint(CUIWindow* parent, pcstr text)
{
	if (m_hint->GetOwner( ))
	{
		return;
	}

	if (!text)
	{
		return;
	}

	fVector2 c_pos = GetUICursor( )->GetCursorPosition( );
	fRect vis_rect = ActiveMapRect( );
	if (FALSE == vis_rect.in(c_pos))
	{
		return;
	}

	m_hint->SetOwner(parent);
	m_hint->SetText(text);

	//select appropriate position
	fRect r;
	r.set(0.0f, 0.0f, m_hint->GetWidth( ), m_hint->GetHeight( ));
	r.add(c_pos.x, c_pos.y);

	r.sub(0.0f, r.height( ));
	if (false == is_in(vis_rect, r))
	{
		r.sub(r.width( ), 0.0f);
	}

	if (false == is_in(vis_rect, r))
	{
		r.add(0.0f, r.height( ));
	}

	if (false == is_in(vis_rect, r))
	{
		r.add(r.width( ), 45.0f);
	}

	m_hint->SetWndPos(r.lt);
}

void CUIMapWnd::HideHint(CUIWindow* parent)
{
	if (m_hint->GetOwner( ) == parent)
	{
		m_hint->SetOwner(NULL);
	}
}

void CUIMapWnd::Hint(const shared_str& text)
{
	m_text_hint->SetTextST(*text);
}

void CUIMapWnd::Reset( )
{
	inherited::Reset( );
	ResetActionPlanner( );
}
