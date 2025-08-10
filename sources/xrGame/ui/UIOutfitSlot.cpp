#include "stdafx.h"

#include "UIOutfitSlot.h"//
#include "UIStatic.h"
#include "UICellItem.h"
#include "../CustomOutfit.h"
#include "../Actor.h"
#include "UIInventoryUtilities.h"//
#include "../Level.h"
#include "../game_base_space.h"

CUIOutfitDragDropList::CUIOutfitDragDropList( )
{
	m_background = xr_new<CUIStatic>( );
	m_background->SetAutoDelete(true);
	AttachChild(m_background);
	m_default_outfit = "npc_icon_without_outfit";
}

CUIOutfitDragDropList::~CUIOutfitDragDropList( )
{ }

void CUIOutfitDragDropList::SetOutfit(CUICellItem* itm)
{
	m_background->Init(0.0f, 0.0f, GetWidth( ), GetHeight( ));
	m_background->SetStretchTexture(true);
	if (itm)
	{
		PIItem _iitem = (PIItem)itm->m_pData;
		CCustomOutfit* pOutfit = smart_cast<CCustomOutfit*>(_iitem); VERIFY(pOutfit);
		m_background->InitTexture(pOutfit->GetFullIconName( ).c_str( ));
	}
	else
	{
		m_background->InitTexture("npc_icon_without_outfit");
	}

	m_background->TextureAvailable(true);
	m_background->TextureOn( );
}

void CUIOutfitDragDropList::SetDefaultOutfit(pcstr default_outfit)
{
	m_default_outfit = default_outfit;
}

void CUIOutfitDragDropList::SetItem(CUICellItem* itm)
{
	if (itm)
	{
		inherited::SetItem(itm);
	}

	SetOutfit(itm);
}

void CUIOutfitDragDropList::SetItem(CUICellItem* itm, fVector2 abs_pos)
{
	if (itm)
	{
		inherited::SetItem(itm, abs_pos);
	}

	SetOutfit(itm);
}

void CUIOutfitDragDropList::SetItem(CUICellItem* itm, iVector2 cell_pos)
{
	if (itm)
	{
		inherited::SetItem(itm, cell_pos);
	}

	SetOutfit(itm);
}

CUICellItem* CUIOutfitDragDropList::RemoveItem(CUICellItem* itm, bool force_root)
{
	VERIFY(!force_root);
	CUICellItem* ci = inherited::RemoveItem(itm, force_root);
	SetOutfit(NULL);
	return ci;
}

void CUIOutfitDragDropList::Draw( )
{
	m_background->Draw( );
}
