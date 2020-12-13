#pragma once

inline void CMonsterMorale::set_despondent( )
{
	m_state = eDespondent;
}

inline void CMonsterMorale::set_take_heart( )
{
	m_state = eTakeHeart;
}

inline void CMonsterMorale::set_normal_state( )
{
	m_state = eStable;
}

inline void CMonsterMorale::change(float value)
{
	m_morale += value;
	clamp(m_morale, 0.0f, 1.0f);
}

inline bool CMonsterMorale::is_despondent( )
{
	return ((m_state == eDespondent) || (m_morale < m_despondent_threshold));
}

inline float CMonsterMorale::get_morale( )
{
	return m_morale;
}
