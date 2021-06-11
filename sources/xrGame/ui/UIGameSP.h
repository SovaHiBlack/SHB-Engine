#pragma once

#include "UIGameCustom.h"//
#include "UIDialogWnd.h"//
#include "..\..\ENGINE\NetPacket.h"
#include "..\game_graph_space.h"

class CUIInventoryWnd;
class CUITradeWnd;
class CUIPdaWnd;
class CUITalkWnd;
class CUICarBodyWnd;
class CInventory;

class game_cl_Single;
class CChangeLevelWnd;
class CUIMessageBox;
class CInventoryBox;
class CInventoryOwner;

class CUIGame : public CUIGameCustom
{
private:
	game_cl_Single*										m_game;
	using inherited										= CUIGameCustom;

public:
							CUIGame						( );
	virtual					~CUIGame					( );

	virtual void			reset_ui					( );
	virtual void			shedule_Update				(u32 dt);
	virtual void			SetClGame					(game_cl_GameState* g);
	virtual bool			IR_OnKeyboardPress			(int dik);
	virtual bool			IR_OnKeyboardRelease		(int dik);

	void					StartTalk					( );
	void					StartCarBody				(CInventoryOwner* pOurInv, CInventoryOwner* pOthers);
	void					StartCarBody				(CInventoryOwner* pOurInv, CInventoryBox* pBox);
	virtual void			ReInitShownUI				( );
	void					ChangeLevel					(GameGraph::_GRAPH_ID game_vert_id, u32 level_vert_id, Fvector3 pos, Fvector3 ang, Fvector3 pos2, Fvector3 ang2, bool b);

	virtual void			HideShownDialogs			( );

	CUIInventoryWnd*									InventoryMenu;
	CUIPdaWnd*											PdaMenu;
	CUITalkWnd*											TalkMenu;
	CUICarBodyWnd*										UICarBodyMenu;
	CChangeLevelWnd*									UIChangeLevelWnd;
};

class CChangeLevelWnd : public CUIDialogWnd
{
	CUIMessageBox*										m_messageBox;
	using inherited										= CUIDialogWnd;

	void					OnCancel					( );
	void					OnOk						( );

public:
	GameGraph::_GRAPH_ID								m_game_vertex_id;
	u32													m_level_vertex_id;
	Fvector3											m_position;
	Fvector3											m_angles;
	Fvector3											m_position_cancel;
	Fvector3											m_angles_cancel;
	bool												m_b_position_cancel;

							CChangeLevelWnd				( );
	virtual					~CChangeLevelWnd			( )
	{ }
	virtual void			SendMessage					(CUIWindow* pWnd, S16 msg, void* pData);
	virtual bool			WorkInPause					( ) const
	{
		return true;
	}
	virtual void			Show						( );
	virtual void			Hide						( );
	virtual bool			OnKeyboard					(int dik, EUIMessages keyboard_action);
};
