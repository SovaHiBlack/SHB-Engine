#pragma once

#include "../BaseMonster/BaseMonster.h"
#include "../../../script_export_space.h"

class CStateManagerFracture;

class CFracture : public CBaseMonster {
	typedef		CBaseMonster		inherited;
	
public:
					CFracture 			();
	virtual			~CFracture 			();	

	virtual void	Load				(const char* section);
	virtual void	CheckSpecParams		(u32 spec_params);

public:
	static void script_register(lua_State*);
};

add_to_type_list(CFracture)
#undef script_type_list
#define script_type_list save_type_list(CFracture)
