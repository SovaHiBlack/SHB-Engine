#pragma once

#include "script_export_space.h"

struct UIRegistrator{

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(UIRegistrator)
#undef script_type_list
#define script_type_list save_type_list(UIRegistrator)
