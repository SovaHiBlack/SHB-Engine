#pragma once

inline void CMeleeChecker::load(const char* section)
{
	m_as_min_dist			= pSettings->r_float(section, "as_min_dist");
	m_as_step				= pSettings->r_float(section, "as_step");

	m_min_attack_distance	= pSettings->r_float(section, "MinAttackDist");
	m_max_attack_distance	= pSettings->r_float(section, "MaxAttackDist");
}

inline void CMeleeChecker::init_attack( )
{
	// инициализировать стек
	for (u32 i = 0; i < HIT_STACK_SIZE; i++)
	{
		m_hit_stack[i] = true;
	}

	m_current_min_distance = m_min_attack_distance;
}

inline float CMeleeChecker::get_min_distance( )
{
	return m_current_min_distance;
}

inline float CMeleeChecker::get_max_distance( )
{
	return (m_max_attack_distance - (m_min_attack_distance - m_current_min_distance));
}

inline bool CMeleeChecker::can_start_melee(const CEntity* enemy)
{
	return (distance_to_enemy(enemy) < get_min_distance( ));
}

inline bool CMeleeChecker::should_stop_melee(const CEntity* enemy)
{
	return (distance_to_enemy(enemy) > get_max_distance( ));
}
