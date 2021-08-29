#pragma once

#include "WeaponMagazined.h"
#include "script_export_space.h"

class CWeaponVal : public CWeaponMagazined
{
	using inherited = CWeaponMagazined;

public:
	CWeaponVal( );
	virtual			~CWeaponVal( );

	static void		script_register(lua_State*);
};

add_to_type_list(CWeaponVal)
#undef script_type_list
#define script_type_list save_type_list(CWeaponVal)
