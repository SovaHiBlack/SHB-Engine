#pragma once

#include "../BaseMonster/base_monster.h"
#include "../controlled_entity.h"
#include "../../../script_export_space.h"

class CDog : public CBaseMonster, public CControlledEntity<CDog>
{
	typedef		CBaseMonster				inherited;
	typedef		CControlledEntity<CDog>	CControlled;

public:
	CDog();
	virtual			~CDog();

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

add_to_type_list(CDog)
#undef script_type_list
#define script_type_list save_type_list(CDog)
