#pragma once

#include "WeaponMagazined.h"
#include "script_export_space.h"

class CWeaponFN2000: public CWeaponMagazined
{
private:
	typedef CWeaponMagazined inherited;
public:
	CWeaponFN2000	();
	virtual			~CWeaponFN2000	();
/*	virtual void	Load			(pcstr section);
	virtual void			Fire2Start			();
	virtual void			Fire2End			();

	virtual bool Action(s32 cmd, u32 flags);*/

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponFN2000)
#undef script_type_list
#define script_type_list save_type_list(CWeaponFN2000)
