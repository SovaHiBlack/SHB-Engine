#pragma once

#include "WeaponMagazinedWGrenade.h"
#include "script_export_space.h"

class CWeaponAK74 : public CWeaponMagazinedWGrenade
{
private:
	using inherited = CWeaponMagazinedWGrenade;

public:
	CWeaponAK74(const char* name = "AK74", ESoundTypes eSoundType = SOUND_TYPE_WEAPON_SUBMACHINEGUN);
	virtual			~CWeaponAK74( );

	static void		script_register(lua_State*);
};

add_to_type_list(CWeaponAK74)
#undef script_type_list
#define script_type_list save_type_list(CWeaponAK74)
