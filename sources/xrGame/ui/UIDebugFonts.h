// File:		UIDebugFonts.h
// Description:	Output list of all fonts

#pragma once

#include "UIListWnd.h"//
#include "UIDialogWnd.h"//

class CUIDebugFonts : public CUIDialogWnd
{
public:
					CUIDebugFonts	( );
	virtual			~CUIDebugFonts	( );

	virtual void	Init			(float x, float y, float width, float height);
	virtual bool	OnKeyboard		(int dik, EUIMessages keyboard_action);
	void			FillUpList		( );

protected:
	CUIStatic						m_background;
};