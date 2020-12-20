#pragma once

#include "WeaponPistol.h"
#include "script_export_space.h"

class CWeaponHPSA : public CWeaponPistol
{
private:
	typedef CWeaponPistol inherited;

public:
	CWeaponHPSA( );
	virtual			~CWeaponHPSA( );

public:
	static void script_register(lua_State*);
};

add_to_type_list(CWeaponHPSA)
#undef script_type_list
#define script_type_list save_type_list(CWeaponHPSA)
