//=============================================================================
//  Filename:   UIGameLog.h
//	Created by Vitaly 'Mad Max' Maximov, mad-max@gsc-game.kiev.ua
//	Copyright 2005. GSC Game World
//	---------------------------------------------------------------------------
//  Multiplayer game log window
//=============================================================================
#pragma once

#include "UIDialogWnd.h"
#include "UIScrollView.h"

class CUIXml;
class CUIPdaMsgListItem;
class CUIStatic;

class CUIGameLog : public CUIScrollView
{
public:
	CUIGameLog( );
	virtual ~CUIGameLog( );
	CUIStatic* AddLogMessage(pcstr msg);
	CUIPdaMsgListItem* AddPdaMessage(pcstr msg, f32 delay);
	virtual void			Update( );

	void					SetTextAtrib(CGameFont* pFont, u32 color);
	u32						GetTextColor( );

private:
	xr_vector<CUIWindow*>		toDelList;
	f32						kill_msg_height;
	u32							txt_color;
};
