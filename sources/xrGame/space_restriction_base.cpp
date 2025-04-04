////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_base.cpp
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction base
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "space_restriction_base.h"
#include "AISpace.h"
#include "level_graph.h"

#define DEFAULT_RADIUS EPSILON_3

bool CSpaceRestrictionBase::inside	(u32 level_vertex_id, bool partially_inside)
{
	return							(inside(level_vertex_id,partially_inside,DEFAULT_RADIUS));
}

IC	fVector3 construct_position		(u32 level_vertex_id, f32 x, f32 z)
{
	return							(fVector3().set(x,ai().level_graph().vertex_plane_y(level_vertex_id,x,z),z));
}

IC	bool CSpaceRestrictionBase_inside	(CSpaceRestrictionBase *self, const fVector3& position, const f32& radius)
{
	fSphere							sphere;
	sphere.P						= position;
	sphere.R						= radius;
	return							(self->inside(sphere));
}

bool CSpaceRestrictionBase::inside	(u32 level_vertex_id, bool partially_inside, f32 radius)
{
	const fVector3& position = ai().level_graph().vertex_position(level_vertex_id);
	f32							offset = ai().level_graph().header().cell_size()*.5f - EPSILON_3;
	if (partially_inside)
		return						(
			CSpaceRestrictionBase_inside(this,construct_position(level_vertex_id,position.x + offset,position.z + offset),radius) || 
			CSpaceRestrictionBase_inside(this,construct_position(level_vertex_id,position.x + offset,position.z - offset),radius) ||
			CSpaceRestrictionBase_inside(this,construct_position(level_vertex_id,position.x - offset,position.z + offset),radius) || 
			CSpaceRestrictionBase_inside(this,construct_position(level_vertex_id,position.x - offset,position.z - offset),radius) ||
			CSpaceRestrictionBase_inside(this, fVector3().set(position.x,position.y,position.z),radius)
		);
	else
		return						(
			CSpaceRestrictionBase_inside(this,construct_position(level_vertex_id,position.x + offset,position.z + offset),radius) && 
			CSpaceRestrictionBase_inside(this,construct_position(level_vertex_id,position.x + offset,position.z - offset),radius) && 
			CSpaceRestrictionBase_inside(this,construct_position(level_vertex_id,position.x - offset,position.z + offset),radius) && 
			CSpaceRestrictionBase_inside(this,construct_position(level_vertex_id,position.x - offset,position.z - offset),radius) &&
			CSpaceRestrictionBase_inside(this, fVector3().set(position.x,position.y,position.z),radius)
		);
}

struct SortByXZ_predicate {
	IC	bool	operator()	(u32 v0, u32 v1) const
	{
		return						(ai().level_graph().vertex(v0)->position().xz() < ai().level_graph().vertex(v1)->position().xz());
	}
};

void CSpaceRestrictionBase::process_borders			()
{
	std::sort					(m_border.begin(),m_border.end());
	m_border.erase				(
		std::unique(
			m_border.begin(),
			m_border.end()
		),
		m_border.end()
	);
	std::sort					(m_border.begin(),m_border.end(),SortByXZ_predicate());
}
