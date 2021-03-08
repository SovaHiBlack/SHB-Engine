//	Module 		: stalker_animation_manager_inline.h
//	Description : Stalker animation manager inline functions

#pragma once

inline	const CStalkerAnimationManager::SCRIPT_ANIMATIONS &CStalkerAnimationManager::script_animations	() const
{
	return							(m_script_animations);
}

inline	CStalkerAnimationPair &CStalkerAnimationManager::global	()
{
	return							(m_global);
}

inline	CStalkerAnimationPair &CStalkerAnimationManager::head	()
{
	return							(m_head);
}

inline	CStalkerAnimationPair &CStalkerAnimationManager::torso	()
{
	return							(m_torso);
}

inline	CStalkerAnimationPair &CStalkerAnimationManager::legs	()
{
	return							(m_legs);
}

inline	CStalkerAnimationPair &CStalkerAnimationManager::script	()
{
	return							(m_script);
}

inline	CStalker	&CStalkerAnimationManager::object			() const
{
	VERIFY							(m_object);
	return							(*m_object);
}

inline	void CStalkerAnimationManager::pop_script_animation		()
{
	VERIFY							(!script_animations().empty());
	m_script_animations.pop_front	();
	script().reset					();
}

inline	void CStalkerAnimationManager::clear_script_animations	()
{
	m_script_animations.clear		();
	script().reset					();
}

inline	bool CStalkerAnimationManager::non_script_need_update	() const
{
	return							(
		m_global.need_update() ||
		m_head.need_update() ||
		m_torso.need_update() ||
		m_legs.need_update()
	);
}
