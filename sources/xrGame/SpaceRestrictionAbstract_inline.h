//	Module 		: SpaceRestrictionAbstract_inline.h
//	Description : Space restriction abstract inline functions

#pragma once

inline CSpaceRestrictionAbstract::CSpaceRestrictionAbstract( )
{
	m_initialized = false;
	m_accessible_neighbour_border_actual = false;
}

inline const xr_vector<unsigned int>& CSpaceRestrictionAbstract::border( )
{
	if (!initialized( ))
	{
		initialize( );
	}

	THROW(initialized( ));
	THROW3(!m_border.empty( ), "Space restrictor has no border!", *name( ));
	return m_border;
}

inline bool CSpaceRestrictionAbstract::initialized( ) const
{
	return m_initialized;
}

template <typename T>
inline const xr_vector<unsigned int>& CSpaceRestrictionAbstract::accessible_neighbour_border(T& restriction, bool out_restriction)
{
	if (!m_accessible_neighbour_border_actual)
	{
		prepare_accessible_neighbour_border(restriction, out_restriction);
	}

	THROW3(!m_accessible_neighbour_border.empty( ), "Space restrictor has no accessible neighbours", *name( ));
	return m_accessible_neighbour_border;
}

template <typename T>
inline bool CSpaceRestrictionAbstract::accessible_neighbours(T& restriction, unsigned int level_vertex_id, bool out_restriction)
{
	CLevelGraph::const_iterator I;
	CLevelGraph::const_iterator E;
	ai( ).level_graph( ).begin(level_vertex_id, I, E);
	for (; I != E; ++I)
	{
		unsigned int current = ai( ).level_graph( ).value(level_vertex_id, I);
		if (!ai( ).level_graph( ).valid_vertex_id(current))
		{
			continue;
		}

		if (restriction->inside(current, !out_restriction) != out_restriction)
		{
			continue;
		}

		return true;
	}

	return false;
}

template <typename T>
inline void CSpaceRestrictionAbstract::prepare_accessible_neighbour_border(T& restriction, bool out_restriction)
{
	VERIFY(!m_accessible_neighbour_border_actual);
	m_accessible_neighbour_border_actual = true;

	VERIFY(!border( ).empty( ));
	m_accessible_neighbour_border.reserve(border( ).size( ));

	xr_vector<unsigned int>::const_iterator I = border( ).begin( );
	xr_vector<unsigned int>::const_iterator E = border( ).end( );
	for (; I != E; ++I)
	{
		if (accessible_neighbours(restriction, *I, out_restriction))
		{
			m_accessible_neighbour_border.push_back(*I);
		}
	}
}
