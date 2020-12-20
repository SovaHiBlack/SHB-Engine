#pragma once

#include "WeaponMagazinedWGrenade.h"
#include "script_export_space.h"

class CWeaponGroza : public CWeaponMagazinedWGrenade
{
	typedef CWeaponMagazinedWGrenade inherited;

public:
	CWeaponGroza( );
	virtual ~CWeaponGroza( );

public:
	static void script_register(lua_State*);
};

add_to_type_list(CWeaponGroza)
#undef script_type_list
#define script_type_list save_type_list(CWeaponGroza)
