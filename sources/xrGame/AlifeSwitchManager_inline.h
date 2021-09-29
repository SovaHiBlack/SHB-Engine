//	Module 		: AlifeSwitchManager_inline.h
//	Description : ALife Simulator switch manager inline functions

#pragma once

inline	CAlifeSwitchManager::CAlifeSwitchManager(CServer* server, const char* section) :
	inherited(server, section)
{
	m_switch_distance = pSettings->r_float(section, "switch_distance");
	m_switch_factor = pSettings->r_float(section, "switch_factor");
	set_switch_distance(m_switch_distance);
	seed(u32(CPU::QPC( ) & 0xffffffff));
}

inline	float CAlifeSwitchManager::online_distance( ) const
{
	return				(m_online_distance);
}

inline	float CAlifeSwitchManager::offline_distance( ) const
{
	return				(m_offline_distance);
}

inline	float CAlifeSwitchManager::switch_distance( ) const
{
	return				(m_switch_distance);
}

inline	void CAlifeSwitchManager::set_switch_distance(float switch_distance)
{
	m_switch_distance = switch_distance;
	m_online_distance = m_switch_distance * (1.f - m_switch_factor);
	m_offline_distance = m_switch_distance * (1.f + m_switch_factor);
}

inline	void CAlifeSwitchManager::set_switch_factor(float switch_factor)
{
	m_switch_factor = switch_factor;
	set_switch_distance(switch_distance( ));
}
