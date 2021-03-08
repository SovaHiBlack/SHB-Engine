// Scope - апгрейд оружия оптический прицел

#pragma once

#include "InventoryItemObject.h"
#include "script_export_space.h"

class CScope : public CInventoryItemObject
{
private:
	using inherited					= CInventoryItemObject;

public:
					CScope			( );
	virtual			~CScope			( );

	static void		script_register	(lua_State*);
};

add_to_type_list(CScope)
#undef script_type_list
#define script_type_list save_type_list(CScope)
