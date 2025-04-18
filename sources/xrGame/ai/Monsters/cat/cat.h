#pragma once
#include "../BaseMonster/base_monster.h"
#include "../../../script_export_space.h"

class CCat : public CBaseMonster{
	typedef		CBaseMonster	inherited;
public:
					CCat				();
	virtual			~CCat				();	

	virtual void	Load				(pcstr section);
	virtual void	reinit				();

	virtual	void	UpdateCL			();

	virtual void	CheckSpecParams		(u32 spec_params);

			void	try_to_jump			();

	virtual	void	HitEntityInJump		(const CEntity *pEntity);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CCat)
#undef script_type_list
#define script_type_list save_type_list(CCat)


