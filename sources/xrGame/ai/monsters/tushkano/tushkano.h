#pragma once

#include "../BaseMonster/BaseMonster.h"
#include "../controlled_entity.h"
#include "../../../script_export_space.h"

class CTushkano :	public CBaseMonster,
					public CControlledEntity<CTushkano> {


	typedef		CBaseMonster					inherited;
	typedef		CControlledEntity<CTushkano>	CControlled;

public:
					CTushkano 			();
	virtual			~CTushkano 			();	

	virtual void	Load				(const char* section);
	virtual void	CheckSpecParams		(u32 spec_params);

public:
	static void script_register(lua_State*);
};

add_to_type_list(CTushkano)
#undef script_type_list
#define script_type_list save_type_list(CTushkano)
