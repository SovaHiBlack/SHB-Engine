////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_bridge_inline.h
//	Created 	: 27.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction bridge inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CSpaceRestrictionBridge::CSpaceRestrictionBridge		(CSpaceRestrictionBase *object)
{
	VERIFY		(object);
	m_object	= object;
}

IC	CSpaceRestrictionBase &CSpaceRestrictionBridge::object	() const
{
	VERIFY		(m_object);
	return		(*m_object);
}

template <typename T>
IC	u32	CSpaceRestrictionBridge::accessible_nearest	(T &restriction, const fVector3& position, fVector3& result, bool out_restriction)
{
#pragma todo("Dima to Dima : _Warning : this place can be optimized in case of a slowdown")
	VERIFY							(initialized());
	VERIFY							(!restriction->border().empty());
	VERIFY							(!restriction->accessible_neighbour_border(restriction,out_restriction).empty());

	f32							min_dist_sqr = flt_max;
	u32								selected = u32(-1);
	xr_vector<u32>::const_iterator	I = restriction->accessible_neighbour_border(restriction,out_restriction).begin();
	xr_vector<u32>::const_iterator	E = restriction->accessible_neighbour_border(restriction,out_restriction).end();
	for ( ; I != E; ++I) {
		f32	distance_sqr = ai().level_graph().vertex_position(*I).distance_to_sqr(position);
		if (distance_sqr < min_dist_sqr) {
			min_dist_sqr			= distance_sqr;
			selected				= *I;
		}
	}
	VERIFY2							(ai().level_graph().valid_vertex_id(selected),*name());

	{
		min_dist_sqr = flt_max;
		u32	new_selected = u32(-1);
		CLevelGraph::const_iterator	I, E;
		ai().level_graph().begin(selected,I,E);
		for ( ; I != E; ++I) {
			u32	current = ai().level_graph().value(selected,I);
			if (!ai().level_graph().valid_vertex_id(current))
				continue;
			// if (out_restriction)
			//		check if node is completely inside
			// else
			//		check if node is completely outside
			if (restriction->inside(current,!out_restriction) != out_restriction)
				continue;
			
			f32	distance_sqr = ai().level_graph().vertex_position(current).distance_to_sqr(position);
			if (distance_sqr < min_dist_sqr) {
				min_dist_sqr = distance_sqr;
				new_selected = current;
			}
		}
		selected	= new_selected;
	}
	VERIFY	(ai().level_graph().valid_vertex_id(selected));

	{
		fVector3		center = ai().level_graph().vertex_position(selected);
		f32		offset = ai().level_graph().header().cell_size()*.5f - EPSILON_3;
		bool		found = false;
		min_dist_sqr = flt_max;
		for (u32 i=0; i<5; ++i) {
			fSphere		current;
			current.R	= EPSILON_3;
#ifdef DEBUG
			current.P	= fVector3().set(flt_max,flt_max,flt_max);
#endif
			switch (i) {
				case 0 : current.P.set(center.x + offset,center.y,center.z + offset); break;
				case 1 : current.P.set(center.x + offset,center.y,center.z - offset); break;
				case 2 : current.P.set(center.x - offset,center.y,center.z + offset); break;
				case 3 : current.P.set(center.x - offset,center.y,center.z - offset); break;
				case 4 : current.P.set(center.x,center.y,center.z); break;
				default : NODEFAULT;
			}
			if (i < 4)
				current.P.y = ai().level_graph().vertex_plane_y(selected,current.P.x,current.P.z);

			VERIFY	(ai().level_graph().inside(selected,current.P));
			VERIFY	(restriction->inside(selected,!out_restriction) == out_restriction);
			VERIFY	(restriction->inside(current) == out_restriction);
			f32	distance_sqr = current.P.distance_to(position);
			if (distance_sqr < min_dist_sqr) {
				min_dist_sqr = distance_sqr;
				result = current.P;
				found = true;
			}
		}
		VERIFY	(found);
	}
	VERIFY		(ai().level_graph().valid_vertex_id(selected));
	
	return		(selected);
}

template <typename T>
IC	const xr_vector<u32> &CSpaceRestrictionBridge::accessible_neighbour_border(T &restriction, bool out_restriction)
{
	return		(object().accessible_neighbour_border(restriction,out_restriction));
}
