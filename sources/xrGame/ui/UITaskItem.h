#pragma once

#include "UIDialogWnd.h"//
#include "UIListItem.h"//
#include "UIWndCallback.h"

class CGameTask;
class CUIStatic;
class CUIButton;
class SGameTaskObjective;
class CUIEventsWnd;
class CUIEditBoxEx;
class CUIEditBox;

class CUITaskItem : public CUIListItem, public CUIWndCallback
{
	using inherited											= CUIListItem;

protected:
	CGameTask*												m_GameTask;
	U16														m_TaskObjectiveIdx;
	void __stdcall			OnItemClicked					(CUIWindow*, void*);
	void					Init							( );

public:
							CUITaskItem						(CUIEventsWnd* w);
	virtual					~CUITaskItem					( );
	virtual void			SendMessage						(CUIWindow* pWnd, signed short msg, void* pData = nullptr);

	virtual void			SetGameTask						(CGameTask* gt, U16 obj_idx);

	CGameTask*				GameTask						( )
	{
		return m_GameTask;
	}
	U16						ObjectiveIdx					( )
	{
		return m_TaskObjectiveIdx;
	}
	SGameTaskObjective*		Objective						( );

	CUIEventsWnd*											m_EventsWnd;
};

class CUITaskRootItem : public CUITaskItem
{
	using inherited											= CUITaskItem;

protected:
	CUIStatic*												m_taskImage;
	CUIStatic*												m_captionStatic;
	CUIStatic*												m_captionTime;
	CUIStatic*												m_remTimeStatic;
	CUI3tButton*											m_switchDescriptionBtn;
	bool													m_curr_descr_mode;
	void					Init							( );

public:
							CUITaskRootItem					(CUIEventsWnd* w);
	virtual					~CUITaskRootItem				( );
	virtual void			Update							( );
	virtual void			SetGameTask						(CGameTask* gt, U16 obj_idx);
	void __stdcall			OnSwitchDescriptionClicked		(CUIWindow*, void*);

	virtual void			MarkSelected					(bool b);
	virtual bool			OnDbClick						( );
};

class CUITaskSubItem : public CUITaskItem
{
	using inherited											= CUITaskItem;

	u32														m_active_color;
	u32														m_failed_color;
	u32														m_accomplished_color;

protected:
	CUIStatic*												m_ActiveObjectiveStatic;
	CUI3tButton*											m_showDescriptionBtn;
	CUIStatic*												m_descriptionStatic;
	CUIStatic*												m_stateStatic;

	void					Init							( );

public:
							CUITaskSubItem					(CUIEventsWnd* w);
	virtual					~CUITaskSubItem					( );
	virtual void			Update							( );
	virtual void			SetGameTask						(CGameTask* gt, U16 obj_idx);
	void					OnActiveObjectiveClicked		( );
	void __stdcall			OnShowDescriptionClicked		(CUIWindow*, void*);
	virtual void			MarkSelected					(bool b);
	virtual bool			OnDbClick						( );
};
