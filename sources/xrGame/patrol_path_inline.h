////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_inline.h
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	const CPatrolPath::CVertex* CPatrolPath::point(CSharedString name) const
{
	const_vertex_iterator		I = vertices( ).begin( );
	const_vertex_iterator		E = vertices( ).end( );
	for (; I != E; ++I)
	{
		if ((*I).second->data( ).name( ) == name)
		{
			return				((*I).second);
		}
	}

	return nullptr;
}

template <typename T>
inline	const CPatrolPath::CVertex* CPatrolPath::point(const Fvector3& position, const T& evaluator) const
{
	const CPatrolPath::CVertex* nearest = 0;
	float						best_distance = flt_max;
	const_vertex_iterator		I = vertices( ).begin( );
	const_vertex_iterator		E = vertices( ).end( );
	for (; I != E; ++I)
	{
		if (!evaluator((*I).second->data( ).position( )))
		{
			continue;
		}

		float					distance = (*I).second->data( ).position( ).distance_to_sqr(position);
		if (distance < best_distance)
		{
			best_distance = distance;
			nearest = (*I).second;
		}
	}

	return nearest;
}

inline	const CPatrolPath::CVertex* CPatrolPath::point(const Fvector3& position) const
{
	return point(position, CAlwaysTrueEvaluator( ));
}

#ifdef DEBUG
inline	void CPatrolPath::name(const CSharedString& name)
{
	VERIFY(!m_name.size( ));
	VERIFY(name.size( ));
	m_name = name;
}
#endif
