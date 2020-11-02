// File:		UI_IB_Static.cpp
// Description:	Inheritance of UIInteractiveBackground template class with some

#include "stdafx.h"

#include "UIInteractiveBackground_Static.h"//

void CUIInteractiveBackground_Static::SetTextureOffset(float x, float y)
{
	if (m_stateCurrent)
	{
		m_stateCurrent->SetTextureOffset(x, y);
	}

	if (m_stateEnabled)
	{
		m_stateEnabled->SetTextureOffset(x, y);
	}

	if (m_stateDisabled)
	{
		m_stateDisabled->SetTextureOffset(x, y);
	}

	if (m_stateHighlighted)
	{
		m_stateHighlighted->SetTextureOffset(x, y);
	}

	if (m_stateTouched)
	{
		m_stateTouched->SetTextureOffset(x, y);
	}
}
