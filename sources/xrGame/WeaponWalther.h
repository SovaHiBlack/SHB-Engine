#pragma once

#include "WeaponPistol.h"
#include "script_export_space.h"

class CWeaponWalther : public CWeaponPistol
{
	typedef CWeaponPistol inherited;

public:
	CWeaponWalther( );
	virtual ~CWeaponWalther( );

public:
	static void script_register(lua_State*);
};

add_to_type_list(CWeaponWalther)
#undef script_type_list
#define script_type_list save_type_list(CWeaponWalther)
