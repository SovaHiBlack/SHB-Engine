#pragma once

#include "WeaponMagazined.h"
#include "script_export_space.h"

class CWeaponVintorez : public CWeaponMagazined
{
	using inherited = CWeaponMagazined;

public:
	CWeaponVintorez( );
	virtual			~CWeaponVintorez( );

	static void		script_register(lua_State*);
};

add_to_type_list(CWeaponVintorez)
#undef script_type_list
#define script_type_list save_type_list(CWeaponVintorez)
