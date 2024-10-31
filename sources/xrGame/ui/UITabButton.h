// File:        UITabButton.cpp
// Description: 
// Created:     19.11.2004
// Last Change: 19.11.2004
// Author:      Serhiy Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
//
// Copyright: 2004 GSC Game World

#pragma once
#include "UI3tButton.h"

class CUITabButton : public CUI3tButton
{
	typedef CUI3tButton inherited;

public:
	CUITabButton( );
	virtual ~CUITabButton( );

	virtual void SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData = 0);
	virtual bool OnMouse(f32 x, f32 y, EUIMessages mouse_action);
	virtual bool OnMouseDown(s32 mouse_btn);
	virtual void Update( );
};
