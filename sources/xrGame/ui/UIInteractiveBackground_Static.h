// File:		UIInteractiveBackground_Static.h
// Description:	Inheritance of UIInteractiveBackground template class with some

#pragma once

#include "UIInteractiveBackground.h"//
#include "UIStatic.h"//

class CUIInteractiveBackground_Static : public CUIInteractiveBackground<CUIStatic>
{
public:
	virtual void SetTextureOffset(float x, float y);
};
