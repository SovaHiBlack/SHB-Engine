// File:        UIListWndEx.cpp
// Description: Extended ListItem
//              Requiered to use feature "Selected Item"

#pragma once

#include "UIListItem.h"//

class CUIListItemEx : public CUIListItem
{
private:
	using inherited = CUIListItem;

public:
	CUIListItemEx( );
	virtual ~CUIListItemEx( );
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);
	virtual void SetSelectionColor(u32 dwColor);
	virtual void Draw( );
	virtual void dummy( )
	{ }

protected:
	u32 m_dwSelectionColor;
};
