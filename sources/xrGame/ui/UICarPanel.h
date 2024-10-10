#pragma once

#include "uiwindow.h"

class CUICarPanel : public CUIWindow
{
private:
	typedef CUIWindow inherited;

	CUIStatic			UIStaticCarHealth;
	CUIProgressBar		UICarHealthBar;

public:
	// Установить 
	void				SetCarHealth	(f32 value);
	void				SetSpeed		(f32 speed);
	void				SetRPM			(f32 rmp);
	void				Init			(f32 x, f32 y, f32 width, f32 height);
};