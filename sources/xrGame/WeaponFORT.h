#pragma once

#include "WeaponPistol.h"
#include "script_export_space.h"

//#define SND_RIC_COUNT 5

class CWeaponFORT : public CWeaponPistol
{
private:
	typedef CWeaponPistol inherited;

public:
	CWeaponFORT( );
	virtual			~CWeaponFORT( );

	static void script_register(lua_State*);
};

add_to_type_list(CWeaponFORT)
#undef script_type_list
#define script_type_list save_type_list(CWeaponFORT)
