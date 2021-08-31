//	Module 		: SpaceRestrictionBridge_inline.h
//	Description : Space restriction bridge inline functions

#pragma once

inline CSpaceRestrictionBridge::CSpaceRestrictionBridge(CSpaceRestrictionBase* object)
{
	VERIFY(object);
	m_object = object;
}

inline CSpaceRestrictionBase& CSpaceRestrictionBridge::object( ) const
{
	VERIFY(m_object);
	return *m_object;
}

template <typename T>
inline u32	CSpaceRestrictionBridge::accessible_nearest(T& restriction, const Fvector3& position, Fvector3& result, bool out_restriction)
{
#pragma todo("Dima to Dima : _Warning : this place can be optimized in case of a slowdown")
	VERIFY(initialized( ));
	VERIFY(!restriction->border( ).empty( ));
	VERIFY(!restriction->accessible_neighbour_border(restriction, out_restriction).empty( ));

	float min_dist_sqr = flt_max;
	u32 selected = u32(-1);
	xr_vector<u32>::const_iterator	I = restriction->accessible_neighbour_border(restriction, out_restriction).begin( );
	xr_vector<u32>::const_iterator	E = restriction->accessible_neighbour_border(restriction, out_restriction).end( );
	for (; I != E; ++I)
	{
		float distance_sqr = ai( ).level_graph( ).vertex_position(*I).distance_to_sqr(position);
		if (distance_sqr < min_dist_sqr)
		{
			min_dist_sqr = distance_sqr;
			selected = *I;
		}
	}

	VERIFY2(ai( ).level_graph( ).valid_vertex_id(selected), *name( ));

	{
		min_dist_sqr = flt_max;
		u32	new_selected = u32(-1);
		CLevelGraph::const_iterator	I, E;
		ai( ).level_graph( ).begin(selected, I, E);
		for (; I != E; ++I)
		{
			u32	current = ai( ).level_graph( ).value(selected, I);
			if (!ai( ).level_graph( ).valid_vertex_id(current))
			{
				continue;
			}

			if (restriction->inside(current, !out_restriction) != out_restriction)
			{
				continue;
			}

			float distance_sqr = ai( ).level_graph( ).vertex_position(current).distance_to_sqr(position);
			if (distance_sqr < min_dist_sqr)
			{
				min_dist_sqr = distance_sqr;
				new_selected = current;
			}
		}

		selected = new_selected;
	}

	VERIFY(ai( ).level_graph( ).valid_vertex_id(selected));

	{
		Fvector3 center = ai( ).level_graph( ).vertex_position(selected);
		float offset = ai( ).level_graph( ).header( ).cell_size( ) * 0.5f - EPS_L;
		bool found = false;
		min_dist_sqr = flt_max;
		for (u32 i = 0; i < 5; ++i)
		{
			Fsphere current;
			current.R = EPS_L;

#ifdef DEBUG
			current.P = Fvector3( ).set(flt_max, flt_max, flt_max);
#endif

			switch (i)
			{
				case 0:
				{
					current.P.set(center.x + offset, center.y, center.z + offset);
				}
				break;
				case 1:
				{
					current.P.set(center.x + offset, center.y, center.z - offset);
				}
				break;
				case 2:
				{
					current.P.set(center.x - offset, center.y, center.z + offset);
				}
				break;
				case 3:
				{
					current.P.set(center.x - offset, center.y, center.z - offset);
				}
				break;
				case 4:
				{
					current.P.set(center.x, center.y, center.z);
				}
				break;
				default:
				{
					NODEFAULT;
				}
			}

			if (i < 4)
			{
				current.P.y = ai( ).level_graph( ).vertex_plane_y(selected, current.P.x, current.P.z);
			}

			VERIFY(ai( ).level_graph( ).inside(selected, current.P));
			VERIFY(restriction->inside(selected, !out_restriction) == out_restriction);
			VERIFY(restriction->inside(current) == out_restriction);
			float distance_sqr = current.P.distance_to(position);
			if (distance_sqr < min_dist_sqr)
			{
				min_dist_sqr = distance_sqr;
				result = current.P;
				found = true;
			}
		}

		VERIFY(found);
	}

	VERIFY(ai( ).level_graph( ).valid_vertex_id(selected));

	return selected;
}

template <typename T>
inline const xr_vector<u32>& CSpaceRestrictionBridge::accessible_neighbour_border(T& restriction, bool out_restriction)
{
	return object( ).accessible_neighbour_border(restriction, out_restriction);
}
