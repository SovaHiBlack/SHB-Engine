#pragma once

#include "WeaponPistol.h"
#include "script_export_space.h"

class CWeaponPM : public CWeaponPistol
{
private:
	typedef CWeaponPistol inherited;

public:
	CWeaponPM( );
	virtual			~CWeaponPM( );

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CWeaponPM)
#undef script_type_list
#define script_type_list save_type_list(CWeaponPM)
