#pragma once

#include "UI3tButton.h"//

class CUITabButton : public CUI3tButton
{
	using inherited					= CUI3tButton;

public:
					CUITabButton	( );
	virtual			~CUITabButton	( );

	virtual void	SendMessage		(CUIWindow* pWnd, S16 msg, void* pData = nullptr);
	virtual bool	OnMouse			(float x, float y, EUIMessages mouse_action);
	virtual bool	OnMouseDown		(int mouse_btn);
	virtual void	Update			( );
};
