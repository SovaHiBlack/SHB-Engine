////////////////////////////////////////////////////////////////////////////
//	Module 		: InventoryItem_impl.h
//	Description : inventory item implementation functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Inventory.h"

IC	CInventoryOwner &CInventoryItem::inventory_owner	() const
{
	VERIFY				(m_pCurrentInventory);
	VERIFY				(m_pCurrentInventory->GetOwner());
	return				(*m_pCurrentInventory->GetOwner());
}
