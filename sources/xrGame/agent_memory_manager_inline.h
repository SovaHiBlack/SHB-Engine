////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_memory_manager.h
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Agent memory manager
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CAgentMemoryManager::CAgentMemoryManager						(CAgentManager *object)
{
	VERIFY						(object);
	m_object					= object;
}

inline	CAgentManager &CAgentMemoryManager::object						() const
{
	VERIFY				(m_object);
	return *m_object;
}

inline	void CAgentMemoryManager::set_squad_objects						(VISIBLES *objects)
{
	m_visible_objects			= objects;
}

inline	void CAgentMemoryManager::set_squad_objects						(SOUNDS *objects)
{
	m_sound_objects				= objects;
}

inline	void CAgentMemoryManager::set_squad_objects						(HITS *objects)
{
	m_hit_objects				= objects;
}

inline	CAgentMemoryManager::VISIBLES &CAgentMemoryManager::visibles	() const
{
	VERIFY						(m_visible_objects);
	return *m_visible_objects;
}

inline	CAgentMemoryManager::SOUNDS &CAgentMemoryManager::sounds		() const
{
	VERIFY						(m_sound_objects);
	return *m_sound_objects;
}

inline	CAgentMemoryManager::HITS &CAgentMemoryManager::hits			() const
{
	VERIFY						(m_hit_objects);
	return *m_hit_objects;
}

inline	void CAgentMemoryManager::update_memory_mask	(const squad_mask_type &mask, squad_mask_type &current)
{
	// this function removes specified bit and shifts all the others
	current					= (((mask ^ squad_mask_type(-1) ^ (mask - 1)) & current) >> 1) | (current & (mask - 1));
}
