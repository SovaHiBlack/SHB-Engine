#pragma once

#include "WeaponPistol.h"
#include "script_export_space.h"

class CWeaponPM : public CWeaponPistol
{
private:
	using inherited = CWeaponPistol;

public:
	CWeaponPM( );
	virtual			~CWeaponPM( );

	static void script_register(lua_State*);
};

add_to_type_list(CWeaponPM)
#undef script_type_list
#define script_type_list save_type_list(CWeaponPM)
