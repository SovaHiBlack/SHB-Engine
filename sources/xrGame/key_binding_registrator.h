#pragma once

#include "script_export_space.h"

struct key_binding_registrator{


public:
	static void script_register(lua_State*);
};

add_to_type_list(key_binding_registrator)
#undef script_type_list
#define script_type_list save_type_list(key_binding_registrator)
