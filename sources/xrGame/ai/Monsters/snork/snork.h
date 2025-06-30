#pragma once
#include "../BaseMonster/base_monster.h"
#include "../../../script_export_space.h"

class CSnork : public CBaseMonster
{
	typedef		CBaseMonster		inherited;

	SVelocityParam	m_fsVelocityJumpPrepare;
	SVelocityParam	m_fsVelocityJumpGround;

public:
	CSnork( );
	virtual			~CSnork( );

	virtual void	Load(pcstr section);
	virtual void	reinit( );
	virtual void	UpdateCL( );
	virtual void	CheckSpecParams(u32 spec_params);
	virtual void	jump(const fVector3& position, f32 factor);
	virtual bool	ability_jump_over_physics( )
	{
		return true;
	}
	virtual bool	ability_distant_feel( )
	{
		return true;
	}
	virtual void	HitEntityInJump(const CEntity* pEntity);

	bool	find_geometry(fVector3& dir);
	f32	trace(const fVector3& dir);

	bool	trace_geometry(const fVector3& d, f32& range);

	virtual bool	check_start_conditions(ControlCom::EControlType type);
	virtual void	on_activate_control(ControlCom::EControlType);

private:
#ifdef _DEBUG	
	virtual void	debug_on_key(s32 key);
#endif

public:
	u32		m_target_node;
	bool	start_threaten;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CSnork)
#undef script_type_list
#define script_type_list save_type_list(CSnork)
