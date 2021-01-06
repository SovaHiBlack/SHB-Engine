#pragma once

#include "../BaseMonster/BaseMonster.h"
#include "../../../script_export_space.h"

class CCat : public CBaseMonster
{
	using inherited = CBaseMonster;

public:
					CCat				();
	virtual			~CCat				();	

	virtual void	Load				(const char* section);
	virtual void	reinit				();

	virtual	void	UpdateCL			();

	virtual void	CheckSpecParams		(u32 spec_params);

			void	try_to_jump			();

	virtual	void	HitEntityInJump		(const CEntity *pEntity);

public:
	static void script_register(lua_State*);
};

add_to_type_list(CCat)
#undef script_type_list
#define script_type_list save_type_list(CCat)
