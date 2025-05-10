#pragma once

#include "UIWindow.h"
#include "UIWndCallback.h"

class CUICustomMap;
class CUIGlobalMap;
class CUIFrameWindow;
class CUIScrollBar;
class CUIFrameLineWnd;
class CMapActionPlanner;
class CUITabControl;
class CUIStatic;
class CUI3tButton;
class CUILevelMap;
class CUIMapHint;
class CMapLocation;

DEFINE_MAP(shared_str, CUICustomMap*, GameMaps, GameMapsPairIt);

class CUIMapWnd : public CUIWindow, public CUIWndCallback
{
	typedef CUIWindow inherited;
	enum EMapToolBtn
	{
		eGlobalMap = 0,
		eZoomIn,
		eZoomOut,
		eActor,
		eMaxBtn
	};

public:
	enum lmFlags
	{
		lmZoomIn = (1 << 3),
		lmZoomOut = (1 << 4),
		lmFirst = (1 << 5)
	};
	flags32						m_flags;

private:
	f32						m_currentZoom;
	CUIGlobalMap* m_GlobalMap;
	GameMaps					m_GameMaps;

	CUIFrameWindow* m_UIMainFrame;
	CUIScrollBar* m_UIMainScrollV;
	CUIScrollBar* m_UIMainScrollH;
	CUIWindow* m_UILevelFrame;
	CMapActionPlanner* m_ActionPlanner;
	CUIFrameLineWnd* UIMainMapHeader;
	CUI3tButton* m_ToolBar[eMaxBtn];
	CUIMapHint* m_hint;
	CUIStatic* m_text_hint;

	void __stdcall				OnScrollV(CUIWindow*, pvoid);
	void __stdcall				OnScrollH(CUIWindow*, pvoid);
	void __stdcall				OnToolGlobalMapClicked(CUIWindow*, pvoid);
	void __stdcall				OnToolActorClicked(CUIWindow*, pvoid);
	void __stdcall				OnToolZoomInClicked(CUIWindow*, pvoid);
	void __stdcall				OnToolZoomOutClicked(CUIWindow*, pvoid);
	void						ValidateToolBar( );

	void						ResetActionPlanner( );

public:
	CUICustomMap* m_tgtMap;
	fVector2					m_tgtCenter;

public:
	CUIMapWnd( );
	virtual						~CUIMapWnd( );

	virtual void				Init(pcstr xml_name, pcstr start_from);
	virtual void				Show(bool status);
	virtual void				Draw( );
	virtual void				Reset( );
	virtual void				Update( );
	f32				GetZoom( )
	{
		return m_currentZoom;
	}
	void				SetZoom(f32 value);

	void				ShowHint(CUIWindow* parent, pcstr text);
	void				HideHint(CUIWindow* parent);
	void				Hint(const shared_str& text);
	virtual bool				OnMouse(f32 x, f32 y, EUIMessages mouse_action);
	virtual bool				OnKeyboard(s32 dik, EUIMessages keyboard_action);
	virtual bool				OnKeyboardHold(s32 dik);

	virtual void				SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData = NULL);

	void						SetTargetMap(CUICustomMap* m, bool bZoomIn = false);
	void						SetTargetMap(CUICustomMap* m, const fVector2& pos, bool bZoomIn = false);
	void						SetTargetMap(const shared_str& name, const fVector2& pos, bool bZoomIn = false);
	void						SetTargetMap(const shared_str& name, bool bZoomIn = false);

	fRect						ActiveMapRect( )
	{
		fRect r;
		m_UILevelFrame->GetAbsoluteRect(r);
		return r;
	}
	void						AddMapToRender(CUICustomMap*);
	void						RemoveMapToRender(CUICustomMap*);
	CUIGlobalMap* GlobalMap( )
	{
		return m_GlobalMap;
	}
	const GameMaps& GameMaps( )
	{
		return m_GameMaps;
	}
	CUICustomMap* GetMapByIdx(u16 idx);
	u16							GetIdxByName(const shared_str& map_name);
	void						UpdateScroll( );
	shared_str					cName( ) const
	{
		return "ui_map_wnd";
	}
};
