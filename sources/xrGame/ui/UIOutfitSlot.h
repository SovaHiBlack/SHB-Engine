#pragma once
#include "UIDragDropListEx.h"

class CUIOutfitDragDropList : public CUIDragDropListEx
{
	typedef CUIDragDropListEx						inherited;
	CUIStatic*										m_background;
	shared_str										m_default_outfit;
	void					SetOutfit				(CUICellItem* itm);

public:
							CUIOutfitDragDropList	( );
	virtual					~CUIOutfitDragDropList	( );

	virtual void			SetItem					(CUICellItem* itm);						//auto
	virtual void			SetItem					(CUICellItem* itm, fVector2 abs_pos);	// start at cursor pos
	virtual void			SetItem					(CUICellItem* itm, iVector2 cell_pos);	// start at cell
	virtual CUICellItem*	RemoveItem				(CUICellItem* itm, bool force_root);
	virtual void			Draw					( );
	void					SetDefaultOutfit		(pcstr default_outfit);
};
