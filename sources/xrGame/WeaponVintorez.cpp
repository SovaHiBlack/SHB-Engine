#include "stdafx.h"

#include "WeaponVintorez.h"

CWeaponVintorez::CWeaponVintorez( ) : CWeaponMagazined("VINTOREZ", eST_WEAPON_SNIPERRIFLE)
{
	m_weight = 1.5f;
	m_slot = 2;
}

CWeaponVintorez::~CWeaponVintorez( )
{ }

using namespace luabind;

#pragma optimize("s",on)
void CWeaponVintorez::script_register(lua_State* L)
{
	module(L)
		[
			class_<CWeaponVintorez, CGameObject>("CWeaponVintorez")
				.def(constructor<>( ))
		];
}
