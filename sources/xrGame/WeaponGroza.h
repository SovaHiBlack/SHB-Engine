#pragma once

#include "WeaponMagazinedWGrenade.h"
#include "script_export_space.h"

class CWeaponGroza : public CWeaponMagazinedWGrenade
{
	using inherited = CWeaponMagazinedWGrenade;

public:
	CWeaponGroza( );
	virtual ~CWeaponGroza( );

	static void script_register(lua_State*);
};

add_to_type_list(CWeaponGroza)
#undef script_type_list
#define script_type_list save_type_list(CWeaponGroza)
