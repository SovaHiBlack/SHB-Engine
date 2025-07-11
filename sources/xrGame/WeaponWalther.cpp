#include "stdafx.h"

#include "WeaponWalther.h"

CWeaponWalther::CWeaponWalther( ) : CWeaponPistol("WALTHER")
{
	m_weight = 0.5f;
	m_slot = 1;
}

CWeaponWalther::~CWeaponWalther( )
{ }

using namespace luabind;

#pragma optimize("s",on)
void CWeaponWalther::script_register(lua_State* L)
{
	module(L)
		[
			class_<CWeaponWalther, CGameObject>("CWeaponWalther")
				.def(constructor<>( ))
		];
}
