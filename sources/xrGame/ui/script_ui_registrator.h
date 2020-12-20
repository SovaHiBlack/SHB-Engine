#pragma once

#include "..\script_export_space.h"

struct UIRegistrator{

public:
	static void script_register(lua_State*);
};

add_to_type_list(UIRegistrator)
#undef script_type_list
#define script_type_list save_type_list(UIRegistrator)
