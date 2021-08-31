//	Module 		: SpaceRestriction_inline.h
//	Description : Space restriction inline functions

#pragma once

inline CSpaceRestriction::CSpaceRestriction(CSpaceRestrictionManager* space_restriction_manager, CSharedString out_restrictions, CSharedString in_restrictions)
{
	VERIFY(space_restriction_manager);
	m_space_restriction_manager = space_restriction_manager;
	m_out_restrictions = out_restrictions;
	m_in_restrictions = in_restrictions;
	m_initialized = false;
	m_applied = false;
}

inline bool CSpaceRestriction::initialized( ) const
{
	return m_initialized;
}

inline CSharedString CSpaceRestriction::out_restrictions( ) const
{
	return m_out_restrictions;
}

inline CSharedString CSpaceRestriction::in_restrictions( ) const
{
	return m_in_restrictions;
}

template <typename T1, typename T2>
inline void CSpaceRestriction::add_border(T1 p1, T2 p2)
{
	if (!initialized( ))
	{
		return;
	}

	VERIFY(!m_applied);

	m_applied = true;

	if (m_out_space_restriction)
	{
		ai( ).level_graph( ).set_mask(border( ));
		return;
	}

#ifdef USE_FREE_IN_RESTRICTIONS
	FREE_IN_RESTRICTIONS::iterator I = m_free_in_restrictions.begin( );
	FREE_IN_RESTRICTIONS::iterator E = m_free_in_restrictions.end( );
	for (; I != E; ++I)
	{
		if (affect((*I).m_restriction, p1, p2))
		{
			VERIFY(!(*I).m_enabled);
			(*I).m_enabled = true;
			ai( ).level_graph( ).set_mask((*I).m_restriction->border( ));
		}
	}
#else
	ai( ).level_graph( ).set_mask(m_in_space_restriction->border( ));
#endif

}

inline bool CSpaceRestriction::applied( ) const
{
	return m_applied;
}

inline bool CSpaceRestriction::inside(const Fsphere& sphere)
{
	return accessible(sphere);
}

inline bool CSpaceRestriction::inside(unsigned int level_vertex_id, bool partially_inside)
{
	return ((m_out_space_restriction ? m_out_space_restriction->inside(level_vertex_id, partially_inside) : true) &&
		(m_in_space_restriction ? !m_in_space_restriction->inside(level_vertex_id, !partially_inside) : true));
}
