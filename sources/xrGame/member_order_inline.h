////////////////////////////////////////////////////////////////////////////
//	Module 		: member_order_inline.h
//	Created 	: 26.05.2004
//  Modified 	: 26.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Member order inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CMemberOrder::CMemberOrder					(CStalker *object) :
	m_object		(object),
	m_initialized	(true)
{
	VERIFY					(m_object);
	m_cover					= 0;
	m_probability			= 1.f;
	m_processed				= false;
	m_selected_enemy		= 0;
	m_detour				= false;
}

inline	bool CMemberOrder::initialized				() const
{
	return			(m_initialized);
}

inline	CStalker &CMemberOrder::object			() const
{
	VERIFY			(m_object);
	return			(*m_object);
}

inline	float CMemberOrder::probability				() const
{
	return			(m_probability);
}

inline	void CMemberOrder::probability				(float probability)
{
	m_probability	= probability;
}

inline	xr_vector<u32> &CMemberOrder::enemies		()
{
	return			(m_enemies);
}

inline	bool CMemberOrder::processed				() const
{
	return			(m_processed);
}

inline	void CMemberOrder::processed				(bool processed)
{
	m_processed		= processed;
}

inline	u32	 CMemberOrder::selected_enemy			() const
{
	return			(m_selected_enemy);
}

inline	void CMemberOrder::selected_enemy			(u32 selected_enemy)
{
	m_selected_enemy = selected_enemy;
}

inline	void CMemberOrder::cover					(const CCoverPoint *object_cover) const
{
	m_cover			= object_cover;
}

inline	const CCoverPoint *CMemberOrder::cover		() const
{
	return			(m_cover);
}

inline	CMemberOrder::CMemberDeathReaction &CMemberOrder::member_death_reaction	()
{
	return			(m_member_death_reaction);
}

inline	CMemberOrder::CGrenadeReaction &CMemberOrder::grenade_reaction			()
{
	return			(m_grenade_reaction);
}

inline	bool CMemberOrder::detour					() const
{
	return			(m_detour);
}

inline	void CMemberOrder::detour					(const bool &value)
{
	m_detour		= value;
}
