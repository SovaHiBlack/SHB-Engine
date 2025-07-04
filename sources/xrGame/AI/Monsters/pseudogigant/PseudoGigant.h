#pragma once
#include "../BaseMonster/base_monster.h"
#include "../controlled_entity.h"
#include "../../../script_export_space.h"

class CPseudoGigant : public CBaseMonster, public CControlledEntity<CPseudoGigant>
{
	typedef		CBaseMonster						inherited;
	typedef		CControlledEntity<CPseudoGigant>	CControlled;

private:
	xr_vector<CObject*>		m_nearest;

	// step_effector
	struct
	{
		f32 time;
		f32 amplitude;
		f32 period_number;
	} step_effector;

	SAttackEffector m_threaten_effector;
	ref_sound		m_sound_threaten_hit;		// ����, ������� �������� � ������ � ������
	ref_sound		m_sound_start_threaten;		// ����, ������� �������� � ������ � ������

	u32				m_time_next_threaten;

	u32				m_threaten_delay_min;
	u32				m_threaten_delay_max;
	f32			m_threaten_dist_min;
	f32			m_threaten_dist_max;

	f32			m_kick_damage;

	u32				m_time_kick_actor_slow_down;

	SVelocityParam	m_fsVelocityJumpPrepare;
	SVelocityParam	m_fsVelocityJumpGround;

	pcstr			m_kick_particles;

public:
	CPseudoGigant( );
	virtual			~CPseudoGigant( );

	virtual void	Load(pcstr section);
	virtual void	reinit( );

	virtual bool	ability_earthquake( )
	{
		return true;
	}
	virtual void	event_on_step( );

	virtual bool	check_start_conditions(ControlCom::EControlType type);
	virtual void	on_activate_control(ControlCom::EControlType);

	virtual void	on_threaten_execute( );

	virtual void	HitEntityInJump(const CEntity* pEntity);
	virtual void	TranslateActionToPathParams( );

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPseudoGigant)
#undef script_type_list
#define script_type_list save_type_list(CPseudoGigant)
