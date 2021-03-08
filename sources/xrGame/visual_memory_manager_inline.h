////////////////////////////////////////////////////////////////////////////
//	Module 		: visual_memory_manager_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Visual memory manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	const CVisualMemoryManager::VISIBLES &CVisualMemoryManager::objects() const
{
	return							(*m_objects);
}

inline	const CVisualMemoryManager::RAW_VISIBLES &CVisualMemoryManager::raw_objects() const
{
	return							(m_visible_objects);
}

inline	const CVisualMemoryManager::NOT_YET_VISIBLES &CVisualMemoryManager::not_yet_visible_objects() const
{
	return							(m_not_yet_visible_objects);
}

inline	void CVisualMemoryManager::set_squad_objects(VISIBLES *squad_objects)
{
	m_objects						= squad_objects;
	if (!m_objects)
		m_not_yet_visible_objects.clear	();
}

inline	float CVisualMemoryManager::visibility_threshold	() const
{
	return							(current_state().m_visibility_threshold);
}

inline	float CVisualMemoryManager::transparency_threshold	() const
{
	return							(current_state().m_transparency_threshold);
}

inline	bool CVisualMemoryManager::enabled					() const
{
	return							(m_enabled);
}

inline	void CVisualMemoryManager::enable					(bool value)
{
	m_enabled						= value;
}
