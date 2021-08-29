#pragma once

#include "WeaponCustomPistol.h"
#include "script_export_space.h"

class CWeaponSVD : public CWeaponCustomPistol
{
	using inherited = CWeaponCustomPistol;

protected:
	virtual void switch2_Fire( );
	virtual void OnAnimationEnd(unsigned int state);

public:
	CWeaponSVD( );
	virtual ~CWeaponSVD( );

	static void script_register(lua_State*);
};

add_to_type_list(CWeaponSVD)
#undef script_type_list
#define script_type_list save_type_list(CWeaponSVD)
