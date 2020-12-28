#pragma once

#include "UIWindow.h"//

class CUICarPanel : public CUIWindow
{
private:
	using inherited							= CUIWindow;

	CUIStatic								UIStaticCarHealth;
	CUIProgressBar							UICarHealthBar;

public:
	// Установить
	void				SetCarHealth		(float value);
	void				SetSpeed			(float speed);
	void				SetRPM				(float rmp);
	void				Init				(float x, float y, float width, float height);
};
