#pragma once

#include "WeaponMagazined.h"
#include "script_export_space.h"

class CWeaponVal : public CWeaponMagazined
{
	typedef CWeaponMagazined inherited;

public:
	CWeaponVal( );
	virtual ~CWeaponVal( );

public:
	static void script_register(lua_State*);
};

add_to_type_list(CWeaponVal)
#undef script_type_list
#define script_type_list save_type_list(CWeaponVal)
