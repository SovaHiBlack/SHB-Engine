////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_memory_manager_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Sound memory manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CSoundMemoryManager::CSoundMemoryManager						(CCustomMonster *object, CStalker *stalker, CSound_UserDataVisitor *visitor)
{
	VERIFY						(object);
	m_object					= object;
	VERIFY						(visitor);
	m_visitor					= visitor;
	m_stalker					= stalker;
	m_max_sound_count			= 0;
#ifdef USE_SELECTED_SOUND
	m_selected_sound			= 0;
#endif
}

inline	const CSoundMemoryManager::SOUNDS &CSoundMemoryManager::objects	() const
{
	VERIFY						(m_sounds);
	return						(*m_sounds);
}

inline	void CSoundMemoryManager::priority								(const ESoundTypes &sound_type, u32 priority)
{
	PRIORITIES::const_iterator	I = m_priorities.find(sound_type);
	VERIFY						(m_priorities.end() == I);
	m_priorities.insert			(std::make_pair(sound_type,priority));
}

#ifdef USE_SELECTED_SOUND
inline	const MemorySpace::CSoundObject *CSoundMemoryManager::sound		() const
{
	return						(m_selected_sound);
}
#endif

inline	void CSoundMemoryManager::set_squad_objects						(SOUNDS *squad_objects)
{
	m_sounds					= squad_objects;
}

inline	void CSoundMemoryManager::set_threshold							(float threshold)
{
	m_sound_threshold			= threshold;
	VERIFY						(_valid(m_sound_threshold));
}

inline	void CSoundMemoryManager::restore_threshold						()
{
	m_sound_threshold			= m_min_sound_threshold;
	VERIFY						(_valid(m_sound_threshold));
}
