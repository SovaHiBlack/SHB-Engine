// File:		UIDebugFonts.h
// Description:	Output list of all fonts
// Created:		22.03.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#pragma once

#include "UIListWnd.h"
#include "UIDialogWnd.h"

class CUIDebugFonts : public CUIDialogWnd
{
public:
					CUIDebugFonts	( );
	virtual			~CUIDebugFonts	( );

	virtual void	Init			(f32 x, f32 y, f32 width, f32 height);
	virtual bool	OnKeyboard		(s32 dik, EUIMessages keyboard_action);
	void			FillUpList		( );

protected:
	CUIStatic		m_background;
};
