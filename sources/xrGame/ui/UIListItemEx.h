// File:        UIListWndEx.cpp
// Description: Extended ListItem
//              Requiered to use feature "Selected Item"
// Created:     
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua

// Copyright:   2004 GSC Game World

#pragma once
#include "uilistitem.h"

class CUIListItemEx : public CUIListItem
{
private:
	typedef CUIListItem inherited;

public:
	CUIListItemEx( );
	virtual ~CUIListItemEx( );
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData);
	virtual void SetSelectionColor(u32 dwColor);
	virtual void Draw( );
	virtual void dummy( )
	{ }

protected:
	u32 m_dwSelectionColor;
};
