#pragma once

#include "WeaponMagazined.h"
#include "script_export_space.h"

class CWeaponLR300: public CWeaponMagazined
{
private:
	typedef CWeaponMagazined inherited;
public:
	/*
	virtual	void	UpdateCL			();
	virtual void	renderable_Render	();
	virtual void	spatial_move		();
	virtual void	spatial_register	();
	virtual void	spatial_unregister	();
	*/

					CWeaponLR300		();
	virtual			~CWeaponLR300		();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponLR300)
#undef script_type_list
#define script_type_list save_type_list(CWeaponLR300)
