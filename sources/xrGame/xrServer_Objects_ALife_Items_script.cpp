//	Module 		: xrServer_Objects_ALife_Items_script.cpp
//	Description : Server items for ALife simulator, script export

#include "stdafx.h"

#include "xrServer_Objects_ALife_Items.h"
#include "xrServer_script_macroses.h"

using namespace luabind;

#pragma optimize("s",on)
void CSE_ALifeInventoryItem::script_register(lua_State* L)
{
	module(L)[
		class_<CSE_ALifeInventoryItem>
			("cse_alife_inventory_item")
//			.def(		constructor<const char*>())
	];
}

void CSE_ALifeItem::script_register(lua_State* L)
{
	module(L)[
//		luabind_class_item2(
		luabind_class_abstract2(
			CSE_ALifeItem,
			"cse_alife_item",
			CSE_ALifeDynamicObjectVisual,
			CSE_ALifeInventoryItem
		)
	];
}

void CSE_ALifeItemTorch::script_register(lua_State* L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemTorch,
			"cse_alife_item_torch",
			CSE_ALifeItem
		)
	];
}

void CSE_ALifeItemAmmo::script_register(lua_State* L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemAmmo,
			"cse_alife_item_ammo",
			CSE_ALifeItem
		)
	];
}

void CSE_ALifeItemWeapon::script_register(lua_State* L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemWeapon,
			"cse_alife_item_weapon",
			CSE_ALifeItem
		)
	];
}

void CSE_ALifeItemWeaponShotGun::script_register(lua_State* L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemWeaponShotGun,
			"cse_alife_item_weapon_shotgun",
			CSE_ALifeItemWeapon
		)
	];
}

void CSE_ALifeItemDetector::script_register(lua_State* L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemDetector,
			"cse_alife_item_detector",
			CSE_ALifeItem
		)
	];
}

void CSE_ALifeItemArtefact::script_register(lua_State* L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemArtefact,
			"cse_alife_item_artefact",
			CSE_ALifeItem
		)
	];
}

void CSE_ALifeItemPDA::script_register(lua_State* L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemPDA,
			"cse_alife_item_pda",
			CSE_ALifeItem
		)
	];
}

void CSE_ALifeItemDocument::script_register(lua_State* L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemDocument,
			"cse_alife_item_document",
			CSE_ALifeItem
		)
	];
}

void CSE_ALifeItemGrenade::script_register(lua_State* L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemGrenade,
			"cse_alife_item_grenade",
			CSE_ALifeItem
		)
	];
}

void CSE_ALifeItemExplosive::script_register(lua_State* L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemExplosive,
			"cse_alife_item_explosive",
			CSE_ALifeItem
		)
	];
}

void CSE_ALifeItemBolt::script_register(lua_State* L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemBolt,
			"cse_alife_item_bolt",
			CSE_ALifeItem
		)
	];
}

void CSE_ALifeItemCustomOutfit::script_register(lua_State* L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemCustomOutfit,
			"cse_alife_item_custom_outfit",
			CSE_ALifeItem
		)
	];
}

void CSE_ALifeItemWeaponMagazined::script_register(lua_State* L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemWeaponMagazined,
			"cse_alife_item_weapon_magazined",
			CSE_ALifeItemWeapon
		)
	];
}

void CSE_ALifeItemWeaponMagazinedWGL::script_register(lua_State* L)
{
	module(L)[
		luabind_class_item1(
			CSE_ALifeItemWeaponMagazinedWGL,
			"cse_alife_item_weapon_magazined_w_gl",
			CSE_ALifeItemWeaponMagazined
		)
	];
}