#pragma once

#include "../BaseMonster/BaseMonster.h"
#include "..\ControlledEntity.h"
#include "../../../script_export_space.h"

class CDog : public CBaseMonster, public CControlledEntity<CDog>
{
	using inherited = CBaseMonster;
	using CControlled = CControlledEntity<CDog>;

public:
	CDog();
	virtual			~CDog();

	virtual void	Load				(const char* section);
	virtual void	reinit				();

	virtual void	CheckSpecParams		(u32 spec_params);

	virtual bool	ability_can_drag		() {return true;}

private:
#ifdef _DEBUG
	virtual void	debug_on_key		(int key);
#endif

public:
	static void script_register(lua_State*);
};

add_to_type_list(CDog)
#undef script_type_list
#define script_type_list save_type_list(CDog)
