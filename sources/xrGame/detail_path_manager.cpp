////////////////////////////////////////////////////////////////////////////
//	Module 		: detailed_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Detail path manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "detail_path_manager.h"
#include "AISpace.h"
#include "ai_object_location.h"
#include "level_graph.h"

#ifdef DEBUG
#	include "custommonster.h"
#endif

#include "GameObject.h"

CDetailPathManager::CDetailPathManager	(CRestrictedObject *object)
{
	m_restricted_object		= object;
}

CDetailPathManager::~CDetailPathManager	()
{
}

void CDetailPathManager::reinit			()
{
	m_actuality				= false;
	m_failed				= false;
	m_start_position		= fVector3().set(0.0f,0.0f,0.0f);
	m_dest_position			= fVector3().set(0.0f,0.0f,0.0f);
	m_current_travel_point	= u32(-1);
	m_path_type				= eDetailPathTypeSmooth;
	m_path.clear			();
	m_desirable_mask		= u32(-1);
	m_velocity_mask			= 0;
	m_try_min_time			= false;
	m_use_dest_orientation	= false;
	m_state_patrol_path		= false;
	m_time_path_built		= 0;
	m_extrapolate_length	= 8.0f;
	m_distance_to_target	= flt_max;
	m_distance_to_target_actual	= false;
}

bool CDetailPathManager::valid			() const
{
	bool					b = true;
	b						= b && !m_path.empty();
	if (m_state_patrol_path)
		b					= b && (fis_zero(m_path[m_last_patrol_point].position.distance_to_xz_sqr(m_corrected_dest_position)));
	else
		b					= b && fis_zero(m_path.back().position.distance_to_xz_sqr(m_corrected_dest_position));
//		b					= b && fis_zero(m_path.back().position.distance_to_xz_sqr(m_dest_position));
	return					(b);
}

bool CDetailPathManager::valid			(const fVector3& position) const
{
	return					(!!_valid(position));
}

fVector3 CDetailPathManager::direction() const
{
	if ((m_path.size() < 2) || (m_path.size() <= m_current_travel_point + 1))
		return				fVector3().set(0.0f,0.0f,1.0f);
	
	fVector3					direction;
	direction.sub			(m_path[m_current_travel_point + 1].position, m_path[m_current_travel_point].position);

	if (direction.square_magnitude() < EPSILON_3)
		direction.set		(0.0f,0.0f,1.0f);
	else
		direction.normalize	();

	return					(direction);
}

void CDetailPathManager::build_path(const xr_vector<u32> &level_path, u32 intermediate_index)
{
	if (valid(m_start_position) && valid(m_dest_position)) {
		switch (m_path_type) {
			case eDetailPathTypeSmooth : {
				build_smooth_path(level_path,intermediate_index);
				break;
			}
			case eDetailPathTypeSmoothDodge : {
				build_smooth_path(level_path,intermediate_index);
				break;
			}
			case eDetailPathTypeSmoothCriteria : {
				build_smooth_path(level_path,intermediate_index);
				break;
			}
			default : NODEFAULT;
		}
		if (failed()) {
			Msg						("! DetailPathManager has failed : from [%f,%f,%f] to [%f,%f,%f]", VPUSH(ai().level_graph().vertex_position(level_path.front())), VPUSH(ai().level_graph().vertex_position(level_path.back())));
#ifdef DEBUG
			Msg						("! DetailPathManager has failed for object %s : from [%f,%f,%f] to [%f,%f,%f]", m_restricted_object ? *m_restricted_object->object().cName() : "unknown", VPUSH(ai().level_graph().vertex_position(level_path.front())), VPUSH(ai().level_graph().vertex_position(level_path.back())));
			Msg						("List of available velocities :");
			xr_vector<STravelParamsIndex>::const_iterator	I = m_start_params.begin();
			xr_vector<STravelParamsIndex>::const_iterator	E = m_start_params.end();
			for ( ; I != E; ++I)
				Msg					("[%d] : [%f][%f]",(*I).index,(*I).linear_velocity,(*I).angular_velocity);

//			for (;;)
//				build_smooth_path(level_path,intermediate_index);
#endif
		}

		if (valid()) {
			m_actuality				= true;
			m_current_travel_point	= 0;
			m_time_path_built		= Device.dwTimeGlobal;
		}
	}
}

bool CDetailPathManager::actual() const
{
	return					(m_actuality);
}

void CDetailPathManager::update_distance_to_target	()
{
	m_distance_to_target_actual	= true;
	m_distance_to_target		= 0.f;

	if (!actual())
		return;

	if (path().empty())
		return;

	if (curr_travel_point_index() >= path().size() - 1)
		return;

	xr_vector<STravelPathPoint>::const_iterator	I = path().begin() + curr_travel_point_index() + 1;
	xr_vector<STravelPathPoint>::const_iterator	E = path().end();
	for ( ; I != E; ++I)
		m_distance_to_target	+= (*(I - 1)).position.distance_to((*I).position);
}

void CDetailPathManager::on_travel_point_change	(const u32 &previous_travel_point_index)
{
	m_distance_to_target_actual	= false;
}

u32 CDetailPathManager::location_on_path		(const CGameObject *object, f32 distance, fVector3& result) const
{
	VERIFY						(m_restricted_object);
	result						= object->Position();
	u32							vertex_result = object->ai_location().level_vertex_id();
	if (!actual())
		return					(vertex_result);

	if (path().empty())
		return					(vertex_result);

	if (curr_travel_point_index() >= path().size() - 1)
		return					(vertex_result);

	f32						current_distance = 0.0f;
	xr_vector<STravelPathPoint>::const_iterator	I = path().begin() + curr_travel_point_index() + 1;
	xr_vector<STravelPathPoint>::const_iterator	E = path().end();
	for ( ; I != E; ++I) {
		f32					next = (*(I - 1)).position.distance_to((*I).position);
		if (current_distance + next > distance) {
			result				= (*I).position;
			return				((*I).vertex_id);
		}
	}

	result						= path().back().position;
	return						(path().back().vertex_id);
}
