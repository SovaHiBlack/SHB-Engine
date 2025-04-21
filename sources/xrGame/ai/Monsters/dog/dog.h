#pragma once

#include "../BaseMonster/base_monster.h"
#include "../controlled_entity.h"
#include "../../../script_export_space.h"

class CAI_Dog : public CBaseMonster, 
				public CControlledEntity<CAI_Dog> {
	
	typedef		CBaseMonster				inherited;
	typedef		CControlledEntity<CAI_Dog>	CControlled;

public:
					CAI_Dog				();
	virtual			~CAI_Dog			();	

	virtual void	Load				(pcstr section);
	virtual void	reinit				();

	virtual void	CheckSpecParams		(u32 spec_params);

	virtual bool	ability_can_drag		() {return true;}

private:
#ifdef _DEBUG
	virtual void	debug_on_key		(s32 key);
#endif

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CAI_Dog)
#undef script_type_list
#define script_type_list save_type_list(CAI_Dog)
