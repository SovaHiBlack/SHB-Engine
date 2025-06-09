#include "stdafx.h"

#include "WeaponSVU.h"

CWeaponSVU::CWeaponSVU( ) : CWeaponCustomPistol("SVU")
{ }

CWeaponSVU::~CWeaponSVU( )
{ }

using namespace luabind;

#pragma optimize("s",on)
void CWeaponSVU::script_register(lua_State* L)
{
	module(L)
		[
			class_<CWeaponSVU, CGameObject>("CWeaponSVU")
				.def(constructor<>( ))
		];
}
