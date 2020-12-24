// File:        UITabButton.h
// Description: 

#pragma once

#include "UI3tButton.h"//

class CUITabButton : public CUI3tButton
{
	typedef CUI3tButton inherited;

public:
	CUITabButton( );
	virtual ~CUITabButton( );

	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = nullptr);
	virtual bool OnMouse(float x, float y, EUIMessages mouse_action);
	virtual bool OnMouseDown(int mouse_btn);
	virtual void Update( );
};
