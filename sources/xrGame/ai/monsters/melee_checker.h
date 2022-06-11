#pragma once
class CBaseMonster;
class CEntity;

#define HIT_STACK_SIZE	2

class CMeleeChecker {
private:
	collide::rq_results	r_res;

private:
	CBaseMonster	*m_object;

	// ltx parameters
	F32			m_min_attack_distance;
	F32			m_max_attack_distance;
	F32			m_as_min_dist;
	F32			m_as_step;

	bool			m_hit_stack[HIT_STACK_SIZE];

	F32			m_current_min_distance;
	
public:
			void	init_external			(CBaseMonster *obj) {m_object = obj;}
	IC		void	load					(pcstr section);
			
	// инициализировано состо€ние атаки
	IC		void	init_attack				();
			void	on_hit_attempt			(bool hit_success);

			// ѕолучить рассто€ние от fire_bone до врага
			// ¬ыполнить RayQuery от fire_bone в enemy.center
			F32	distance_to_enemy		(const CEntity *enemy);

	IC		F32	get_min_distance		();
	IC		F32	get_max_distance		();

	IC		bool	can_start_melee			(const CEntity *enemy);
	IC		bool	should_stop_melee		(const CEntity *enemy);

#ifdef DEBUG
	IC		F32	dbg_as_min_dist			(){return m_as_min_dist;}
	IC		F32	dbg_as_step				(){return m_as_step;}
#endif

};

#include "melee_checker_inline.h"