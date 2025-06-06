#include "stdafx.h"
#include "UIInventoryWnd.h"//
#include "../Actor.h"
#include "../Silencer.h"
#include "../Scope.h"
#include "../GrenadeLauncher.h"
#include "../Artefact.h"
#include "../EatableItem.h"
#include "../BottleItem.h"
#include "../WeaponMagazined.h"
#include "../Inventory.h"
#include "../game_base.h"
#include "../game_cl_base.h"
#include "../xr_level_controller.h"
#include "UICellItem.h"
#include "UIListBoxItem.h"
#include "../CustomOutfit.h"
#include "../Medkit.h"
#include "../Antirad.h"

void CUIInventoryWnd::EatItem(PIItem itm)
{
	SetCurrentItem(NULL);
	if (!itm->Useful( ))
	{
		return;
	}

	SendEvent_Item_Eat(itm);

	PlaySnd(eInvItemUse);
}

void CUIInventoryWnd::ActivatePropertiesBox( )
{
	// ����-������� ��� ���������� ������ ������������ ����: Dreess Outfit, ���� ������ ��� �����
	bool bAlreadyDressed = false;

	UIPropertiesBox.RemoveAll( );

	CMedkit* pMedkit = smart_cast<CMedkit*>			(CurrentIItem( ));
	CAntirad* pAntirad = smart_cast<CAntirad*>			(CurrentIItem( ));
	CEatableItem* pEatableItem = smart_cast<CEatableItem*>		(CurrentIItem( ));
	CCustomOutfit* pOutfit = smart_cast<CCustomOutfit*>	(CurrentIItem( ));
	CWeapon* pWeapon = smart_cast<CWeapon*>			(CurrentIItem( ));
	CScope* pScope = smart_cast<CScope*>			(CurrentIItem( ));
	CSilencer* pSilencer = smart_cast<CSilencer*>		(CurrentIItem( ));
	CGrenadeLauncher* pGrenadeLauncher = smart_cast<CGrenadeLauncher*>	(CurrentIItem( ));
	CBottleItem* pBottleItem = smart_cast<CBottleItem*>		(CurrentIItem( ));

	bool b_show = false;

	if (!pOutfit && CurrentIItem( )->GetSlot( ) != NO_ACTIVE_SLOT && !m_pInv->m_slots[CurrentIItem( )->GetSlot( )].m_bPersistent && m_pInv->CanPutInSlot(CurrentIItem( )))
	{
		UIPropertiesBox.AddItem("st_move_to_slot", NULL, INVENTORY_TO_SLOT_ACTION);
		b_show = true;
	}

	if (CurrentIItem( )->Belt( ) && m_pInv->CanPutInBelt(CurrentIItem( )))
	{
		UIPropertiesBox.AddItem("st_move_on_belt", NULL, INVENTORY_TO_BELT_ACTION);
		b_show = true;
	}

	if (CurrentIItem( )->Ruck( ) && m_pInv->CanPutInRuck(CurrentIItem( )) && (CurrentIItem( )->GetSlot( ) == u32(-1) || !m_pInv->m_slots[CurrentIItem( )->GetSlot( )].m_bPersistent))
	{
		if (!pOutfit)
		{
			UIPropertiesBox.AddItem("st_move_to_bag", NULL, INVENTORY_TO_BAG_ACTION);
		}
		else
		{
			UIPropertiesBox.AddItem("st_undress_outfit", NULL, INVENTORY_TO_BAG_ACTION);
		}

		bAlreadyDressed = true;
		b_show = true;
	}

	if (pOutfit && !bAlreadyDressed)
	{
		UIPropertiesBox.AddItem("st_dress_outfit", NULL, INVENTORY_TO_SLOT_ACTION);
		b_show = true;
	}

	// ������������ ������� �� ����
	if (pWeapon)
	{
		if (pWeapon->GrenadeLauncherAttachable( ) && pWeapon->IsGrenadeLauncherAttached( ))
		{
			UIPropertiesBox.AddItem("st_detach_gl", NULL, INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON);
			b_show = true;
		}

		if (pWeapon->ScopeAttachable( ) && pWeapon->IsScopeAttached( ))
		{
			UIPropertiesBox.AddItem("st_detach_scope", NULL, INVENTORY_DETACH_SCOPE_ADDON);
			b_show = true;
		}

		if (pWeapon->SilencerAttachable( ) && pWeapon->IsSilencerAttached( ))
		{
			UIPropertiesBox.AddItem("st_detach_silencer", NULL, INVENTORY_DETACH_SILENCER_ADDON);
			b_show = true;
		}

		if (smart_cast<CWeaponMagazined*>(pWeapon))
		{
			bool b = (0 != pWeapon->GetAmmoElapsed( ));

			if (!b)
			{
				CUICellItem* itm = CurrentItem( );
				for (u32 i = 0; i < itm->ChildsCount( ); ++i)
				{
					pWeapon = smart_cast<CWeaponMagazined*>((CWeapon*)itm->Child(i)->m_pData);
					if (pWeapon->GetAmmoElapsed( ))
					{
						b = true;
						break;
					}
				}
			}

			if (b)
			{
				UIPropertiesBox.AddItem("st_unload_magazine", NULL, INVENTORY_UNLOAD_MAGAZINE);
				b_show = true;
			}
		}
	}

	// ������������� ������� � ��������� ����� (2 ��� 3)
	if (pScope)
	{
		if (m_pInv->m_slots[PISTOL_SLOT].m_pIItem != NULL && m_pInv->m_slots[PISTOL_SLOT].m_pIItem->CanAttach(pScope))
		{
			PIItem tgt = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_scope_to_pistol", (pvoid)tgt, INVENTORY_ATTACH_ADDON);
			b_show = true;
		}

		if (m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL && m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pScope))
		{
			PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_scope_to_rifle", (pvoid)tgt, INVENTORY_ATTACH_ADDON);
			b_show = true;
		}
	}
	else if (pSilencer)
	{
		if (m_pInv->m_slots[PISTOL_SLOT].m_pIItem != NULL && m_pInv->m_slots[PISTOL_SLOT].m_pIItem->CanAttach(pSilencer))
		{
			PIItem tgt = m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_silencer_to_pistol", (pvoid)tgt, INVENTORY_ATTACH_ADDON);
			b_show = true;
		}

		if (m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL && m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pSilencer))
		{
			PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_silencer_to_rifle", (pvoid)tgt, INVENTORY_ATTACH_ADDON);
			b_show = true;
		}
	}
	else if (pGrenadeLauncher)
	{
		if (m_pInv->m_slots[RIFLE_SLOT].m_pIItem != NULL && m_pInv->m_slots[RIFLE_SLOT].m_pIItem->CanAttach(pGrenadeLauncher))
		{
			PIItem tgt = m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_gl_to_rifle", (pvoid)tgt, INVENTORY_ATTACH_ADDON);
			b_show = true;
		}
	}

	pcstr _action = NULL;
	if (pMedkit || pAntirad)
	{
		_action = "st_use";
	}
	else if (pEatableItem)
	{
		if (pBottleItem)
		{
			_action = "st_drink";
		}
		else
		{
			_action = "st_eat";
		}
	}

	if (_action)
	{
		UIPropertiesBox.AddItem(_action, NULL, INVENTORY_EAT_ACTION);
		b_show = true;
	}

	bool disallow_drop = (pOutfit && bAlreadyDressed);
	disallow_drop |= !!CurrentIItem( )->IsQuestItem( );

	if (!disallow_drop)
	{
		UIPropertiesBox.AddItem("st_drop", NULL, INVENTORY_DROP_ACTION);
		b_show = true;

		if (CurrentItem( )->ChildsCount( ))
		{
			UIPropertiesBox.AddItem("st_drop_all", (pvoid)33, INVENTORY_DROP_ACTION);
		}
	}

	if (b_show)
	{
		UIPropertiesBox.AutoUpdateSize( );
		UIPropertiesBox.BringAllToTop( );

		fVector2						cursor_pos;
		fRect							vis_rect;
		GetAbsoluteRect(vis_rect);
		cursor_pos = GetUICursor( )->GetCursorPosition( );
		cursor_pos.sub(vis_rect.lt);
		UIPropertiesBox.Show(vis_rect, cursor_pos);
		PlaySnd(eInvProperties);
	}
}

void CUIInventoryWnd::ProcessPropertiesBoxClicked( )
{
	if (UIPropertiesBox.GetClickedItem( ))
	{
		switch (UIPropertiesBox.GetClickedItem( )->GetTAG( ))
		{
			case INVENTORY_TO_SLOT_ACTION:
			{
				ToSlot(CurrentItem( ), true);
			}
			break;
			case INVENTORY_TO_BELT_ACTION:
			{
				ToBelt(CurrentItem( ), false);
			}
			break;
			case INVENTORY_TO_BAG_ACTION:
			{
				ToBag(CurrentItem( ), false);
			}
			break;
			case INVENTORY_DROP_ACTION:
			{
				pvoid d = UIPropertiesBox.GetClickedItem( )->GetData( );
				bool b_all = (d == (pvoid)33);

				DropCurrentItem(b_all);
			}
			break;
			case INVENTORY_EAT_ACTION:
			{
				EatItem(CurrentIItem( ));
			}
			break;
			case INVENTORY_ATTACH_ADDON:
			{
				AttachAddon((PIItem)(UIPropertiesBox.GetClickedItem( )->GetData( )));
			}
			break;
			case INVENTORY_DETACH_SCOPE_ADDON:
			{
				DetachAddon(*(smart_cast<CWeapon*>(CurrentIItem( )))->GetScopeName( ));
			}
			break;
			case INVENTORY_DETACH_SILENCER_ADDON:
			{
				DetachAddon(*(smart_cast<CWeapon*>(CurrentIItem( )))->GetSilencerName( ));
			}
			break;
			case INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON:
			{
				DetachAddon(*(smart_cast<CWeapon*>(CurrentIItem( )))->GetGrenadeLauncherName( ));
			}
			break;
			case INVENTORY_RELOAD_MAGAZINE:
			{
				(smart_cast<CWeapon*>(CurrentIItem( )))->Action(kWPN_RELOAD, CMD_START);
			}
			break;
			case INVENTORY_UNLOAD_MAGAZINE:
			{
				CUICellItem* itm = CurrentItem( );
				(smart_cast<CWeaponMagazined*>((CWeapon*)itm->m_pData))->UnloadMagazine( );
				for (u32 i = 0; i < itm->ChildsCount( ); ++i)
				{
					CUICellItem* child_itm = itm->Child(i);
					(smart_cast<CWeaponMagazined*>((CWeapon*)child_itm->m_pData))->UnloadMagazine( );
				}
			}
			break;
		}
	}
}

bool CUIInventoryWnd::TryUseItem(PIItem itm)
{
	CBottleItem* pBottleItem = smart_cast<CBottleItem*>		(itm);
	CMedkit* pMedkit = smart_cast<CMedkit*>			(itm);
	CAntirad* pAntirad = smart_cast<CAntirad*>			(itm);
	CEatableItem* pEatableItem = smart_cast<CEatableItem*>		(itm);

	if (pMedkit || pAntirad || pEatableItem || pBottleItem)
	{
		EatItem(itm);
		return true;
	}

	return false;
}

bool CUIInventoryWnd::DropItem(PIItem itm, CUIDragDropListEx* lst)
{
	if (lst == m_pUIOutfitList)
	{
		return TryUseItem(itm);
	}

	CUICellItem* _citem = lst->ItemsCount( ) ? lst->GetItemIdx(0) : NULL;
	PIItem _iitem = _citem ? (PIItem)_citem->m_pData : NULL;

	if (!_iitem)
	{
		return false;
	}

	if (!_iitem->CanAttach(itm))
	{
		return false;
	}

	AttachAddon(_iitem);

	return true;
}
