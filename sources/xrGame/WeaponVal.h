#pragma once

#include "WeaponMagazined.h"
#include "script_export_space.h"

class CWeaponVal : public CWeaponMagazined
{
	typedef CWeaponMagazined inherited;

public:
	CWeaponVal( );
	virtual ~CWeaponVal( );

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CWeaponVal)
#undef script_type_list
#define script_type_list save_type_list(CWeaponVal)
