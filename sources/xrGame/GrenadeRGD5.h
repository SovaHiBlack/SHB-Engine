#pragma once

#include "Grenade.h"
#include "script_export_space.h"

class CGrenadeRGD5 : public CGrenade
{
	typedef CGrenade inherited;

public:
	CGrenadeRGD5( );
	virtual ~CGrenadeRGD5( );

public:
	static void script_register(lua_State*);
};

add_to_type_list(CGrenadeRGD5)
#undef script_type_list
#define script_type_list save_type_list(CGrenadeRGD5)
