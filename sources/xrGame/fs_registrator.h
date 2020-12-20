#pragma once

#include "script_export_space.h"

struct fs_registrator{
public:
	static void script_register(lua_State*);
};

add_to_type_list(fs_registrator)
#undef script_type_list
#define script_type_list save_type_list(fs_registrator)
