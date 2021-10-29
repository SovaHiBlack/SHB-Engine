#pragma once

#include "../BaseMonster/BaseMonster.h"
#include "..\ControlledEntity.h"
#include "../../../script_export_space.h"

class CBoar : public CBaseMonster, public CControlledEntity<CBoar>
{
	using inherited = CBaseMonster;
	using CControlled = CControlledEntity<CBoar>;

public:
	CBoar( );
	virtual			~CBoar( );

	virtual void	Load(const char* section);
	virtual BOOL	net_Spawn(CSE_Abstract* DC);
	virtual void	reinit( );

	virtual void	UpdateCL( );

	virtual bool	CanExecRotationJump( )
	{
		return true;
	}
	virtual void	CheckSpecParams(u32 spec_params);

	// look at enemy
	static void		BoneCallback(CBoneInstance* B);

	float									_velocity;
	float									_cur_delta;
	float									_target_delta;
	bool									look_at_enemy;

	virtual bool	ability_can_drag( )
	{
		return true;
	}

public:
	static void script_register(lua_State*);
};

add_to_type_list(CBoar)
#undef script_type_list
#define script_type_list save_type_list(CBoar)
