#pragma once

#include "../BaseMonster/base_monster.h"
#include "../controlled_entity.h"
#include "../../../script_export_space.h"

class CBoar : public CBaseMonster, 				 public CControlledEntity<CBoar> {

	typedef		CBaseMonster	inherited;
	typedef		CControlledEntity<CBoar>	CControlled;

public:
	CBoar();
	virtual			~CBoar();

	virtual void	Load				(pcstr section);
	virtual BOOL	net_Spawn			(CSE_Abstract* DC);
	virtual void	reinit				();

	virtual void	UpdateCL			();

	virtual bool	CanExecRotationJump	() {return true;}
	virtual void	CheckSpecParams		(u32 spec_params);

	// look at enemy
	static void		BoneCallback	(CBoneInstance *B);
	
	f32	_velocity;
	f32	_cur_delta;
	f32 _target_delta;
			bool	look_at_enemy;
	
	virtual bool	ability_can_drag	() {return true;}
	
	DECLARE_SCRIPT_REGISTER_FUNCTION

};

add_to_type_list(CBoar)
#undef script_type_list
#define script_type_list save_type_list(CBoar)