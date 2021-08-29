#pragma once

#include "WeaponCustomPistol.h"
#include "script_export_space.h"

class CWeaponSVU : public CWeaponCustomPistol
{
	using inherited = CWeaponCustomPistol;

public:
	CWeaponSVU( );
	virtual ~CWeaponSVU( );

	static void script_register(lua_State*);
};

add_to_type_list(CWeaponSVU)
#undef script_type_list
#define script_type_list save_type_list(CWeaponSVU)
