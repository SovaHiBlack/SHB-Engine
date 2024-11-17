#pragma once

#include "WeaponMagazined.h"

class CWeaponCustomPistol : public CWeaponMagazined
{
private:
	typedef CWeaponMagazined inherited;

public:
	CWeaponCustomPistol(pcstr name);
	virtual ~CWeaponCustomPistol( );
	virtual s32 GetCurrentFireMode( )
	{
		return 1;
	}

protected:
	virtual void FireEnd( );
	virtual void switch2_Fire( );
};
