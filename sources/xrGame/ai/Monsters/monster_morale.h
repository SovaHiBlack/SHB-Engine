#pragma once

class CBaseMonster;

class CMonsterMorale {

	// external parameters
	F32			m_hit_quant;
	F32			m_attack_success_quant;
	F32			m_team_mate_die;
	F32			m_v_taking_heart;
	F32			m_v_despondent;
	F32			m_v_stable;
	F32			m_despondent_threshold;

	
	CBaseMonster	*m_object;

	enum EState {
		eStable,
		eTakeHeart,
		eDespondent
	};

	EState			m_state;
	
	F32			m_morale;

public:
				CMonsterMorale		(){}
				~CMonsterMorale		(){}
	
		void	init_external		(CBaseMonster *obj);
		void	load				(pcstr section);
		void	reinit				();

		void	on_hit				();
		void	on_attack_success	();

		void	update_schedule		(u32 dt);

	IC	void	set_despondent		();
	IC	void	set_take_heart		();
	IC	void	set_normal_state	();

	IC	bool	is_despondent		();

	IC	F32	get_morale			();

private:
	IC	void	change				(F32 value);
};

#include "monster_morale_inline.h"
