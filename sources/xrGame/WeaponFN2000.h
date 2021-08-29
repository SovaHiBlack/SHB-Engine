#pragma once

#include "WeaponMagazined.h"
#include "script_export_space.h"

class CWeaponFN2000 : public CWeaponMagazined
{
private:
	using inherited = CWeaponMagazined;

public:
	CWeaponFN2000( );
	virtual			~CWeaponFN2000( );

	static void script_register(lua_State*);
};

add_to_type_list(CWeaponFN2000)
#undef script_type_list
#define script_type_list save_type_list(CWeaponFN2000)
