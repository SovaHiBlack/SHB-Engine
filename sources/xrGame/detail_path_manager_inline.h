////////////////////////////////////////////////////////////////////////////
//	Module 		: detailed_path_manager_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Detail path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	bool CDetailPathManager::failed( ) const
{
	return					(m_failed);
}

IC	bool CDetailPathManager::completed(const fVector3& position, bool bRealCompleted) const
{
	return					(m_path.empty( ) || ((bRealCompleted || !m_state_patrol_path) ? (curr_travel_point_index( ) == m_path.size( ) - 1) : curr_travel_point_index( ) >= m_last_patrol_point));
}

IC	const xr_vector<DetailPathManager::STravelPathPoint>& CDetailPathManager::path( ) const
{
	return					(m_path);
}

IC	const DetailPathManager::STravelPathPoint& CDetailPathManager::curr_travel_point( ) const
{
	return					(m_path[curr_travel_point_index( )]);
}

IC	u32	 CDetailPathManager::curr_travel_point_index( ) const
{
	VERIFY(!m_path.empty( ) && (m_current_travel_point < m_path.size( )));
	return					(m_current_travel_point);
}

IC	void CDetailPathManager::set_start_position(const fVector3& start_position)
{
	m_start_position = start_position;
}

IC	void CDetailPathManager::set_start_direction(const fVector3& start_direction)
{
	m_start_direction = start_direction;
}

IC	void CDetailPathManager::set_dest_position(const fVector3& dest_position)
{

#ifdef DEBUG
	if (!(!m_restricted_object || m_restricted_object->accessible(dest_position)))
	{
		LogStackTrace("error call stack");
	}
#endif // DEBUG

	THROW2(!m_restricted_object || m_restricted_object->accessible(dest_position), "Old movement destination is not accessible after changing restrictions!");

	bool					value = !!m_dest_position.similar(dest_position, .1f);
	if (!value)
		m_corrected_dest_position = dest_position;

	m_actuality = m_actuality && value;
	m_dest_position = dest_position;
}

IC	void CDetailPathManager::set_dest_direction(const fVector3& dest_direction)
{
	m_actuality = m_actuality && m_dest_direction.similar(dest_direction);
	m_dest_direction = dest_direction;
}

IC	const fVector3& CDetailPathManager::start_position( ) const
{
	return					(m_start_position);
}

IC	const fVector3& CDetailPathManager::start_direction( ) const
{
	return					(m_start_direction);
}

IC	const fVector3& CDetailPathManager::dest_position( ) const
{
	return					(m_dest_position);
}

IC	const fVector3& CDetailPathManager::dest_direction( ) const
{
	return					(m_dest_direction);
}

IC	void CDetailPathManager::set_path_type(const EDetailPathType path_type)
{
	m_actuality = m_actuality && (path_type == m_path_type);
	m_path_type = path_type;
}

IC	void CDetailPathManager::adjust_point(
	const fVector2& source,
	f32				yaw,
	f32				magnitude,
	fVector2& dest
) const
{
	dest.x = -_sin(yaw);
	dest.y = _cos(yaw);
	dest.mad(source, dest, magnitude);
}

IC	void CDetailPathManager::assign_angle(
	f32& angle,
	const f32				start_yaw,
	const f32				dest_yaw,
	const bool				positive,
	const EDirectionType	direction_type,
	const bool				start
) const
{
	if (positive)
		if (dest_yaw >= start_yaw)
			angle = dest_yaw - start_yaw;
		else
			angle = PI_MUL_2 - start_yaw + dest_yaw;
	else
		if (dest_yaw <= start_yaw)
			angle = dest_yaw - start_yaw;
		else
			angle = dest_yaw - start_yaw - PI_MUL_2;

	if (!start && ((direction_type == eDirectionTypePP) || (direction_type == eDirectionTypeNN)))
		if (angle <= 0.f)
			angle = angle + PI_MUL_2;
		else
			angle = angle - PI_MUL_2;

	VERIFY(_valid(angle));
}

IC	void CDetailPathManager::compute_circles(
	STrajectoryPoint& point,
	SCirclePoint* circles
)
{
	VERIFY(!fis_zero(point.angular_velocity));
	point.radius = _abs(point.linear_velocity) / point.angular_velocity;
	circles[0].radius = circles[1].radius = point.radius;
	VERIFY(fsimilar(point.direction.square_magnitude( ), 1.f));
	circles[0].center.x = point.direction.y * point.radius + point.position.x;
	circles[0].center.y = -point.direction.x * point.radius + point.position.y;
	circles[1].center.x = -point.direction.y * point.radius + point.position.x;
	circles[1].center.y = point.direction.x * point.radius + point.position.y;
}

IC	void CDetailPathManager::set_velocity_mask(const u32 velocity_mask)
{
	m_actuality = m_actuality && (velocity_mask == m_velocity_mask);
	m_velocity_mask = velocity_mask;
}

IC	const u32 CDetailPathManager::velocity_mask( ) const
{
	return					(m_velocity_mask);
}

IC	void CDetailPathManager::set_desirable_mask(const u32 desirable_mask)
{
	m_actuality = m_actuality && (desirable_mask == m_desirable_mask);
	m_desirable_mask = desirable_mask;
}

IC	const u32 CDetailPathManager::desirable_mask( ) const
{
	return					(m_desirable_mask);
}

IC	void CDetailPathManager::set_try_min_time(const bool try_min_time)
{
	m_actuality = m_actuality && (try_min_time == m_try_min_time);
	m_try_min_time = try_min_time;
}

IC	const bool CDetailPathManager::try_min_time( ) const
{
	return					(m_try_min_time);
}

IC	void CDetailPathManager::set_use_dest_orientation(const bool use_dest_orientation)
{
	m_actuality = m_actuality && (use_dest_orientation == m_use_dest_orientation);
	m_use_dest_orientation = use_dest_orientation;
}

IC	const bool CDetailPathManager::use_dest_orientation( ) const
{
	return					(m_use_dest_orientation);
}

IC	bool CDetailPathManager::check_mask(u32 mask, u32 test) const
{
	return					((mask & test) == test);
}

IC	void CDetailPathManager::set_state_patrol_path(const bool state_patrol_path)
{
	m_actuality = m_actuality && (state_patrol_path == m_state_patrol_path);
	m_state_patrol_path = state_patrol_path;
}

IC	bool CDetailPathManager::state_patrol_path( ) const
{
	return					(m_state_patrol_path);
}

IC	const u32 CDetailPathManager::time_path_built( ) const
{
	return	(m_time_path_built);
}

IC	const CDetailPathManager::STravelParams& CDetailPathManager::velocity(const u32& velocity_id) const
{
	VELOCITIES::const_iterator	I = m_movement_params.find(velocity_id);
	VERIFY(m_movement_params.end( ) != I);
	return						((*I).second);
}

IC	void CDetailPathManager::add_velocity(const u32& velocity_id, const STravelParams& params)
{
	m_movement_params.insert(std::make_pair(velocity_id, params));
}

IC	void CDetailPathManager::extrapolate_length(f32 extrapolate_length)
{
	m_actuality = m_actuality && (fsimilar(m_extrapolate_length, extrapolate_length));
	m_extrapolate_length = extrapolate_length;
}

IC	f32 CDetailPathManager::extrapolate_length( ) const
{
	return					(m_extrapolate_length);
}

IC	const CDetailPathManager::VELOCITIES& CDetailPathManager::velocities( ) const
{
	return					(m_movement_params);
}

IC	const f32& CDetailPathManager::distance_to_target( )
{
	if (m_distance_to_target_actual)
		return					(m_distance_to_target);

	update_distance_to_target( );
	return						(m_distance_to_target);
}
