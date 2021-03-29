#pragma once

#include "UIButton.h"//

class CUIScrollBox : public CUIStatic
{
	using inherited						= CUIStatic;

public:
						CUIScrollBox	( );

	void 				SetHorizontal	( );
	void 				SetVertical		( );

	virtual bool		OnMouse			(float x, float y, EUIMessages mouse_action);
	virtual void		Draw			( );

protected:
	bool								m_bIsHorizontal;
};
