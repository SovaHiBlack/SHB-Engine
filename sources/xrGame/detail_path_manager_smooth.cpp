////////////////////////////////////////////////////////////////////////////
//	Module 		: detailed_path_manager_criteria.h
//	Created 	: 04.12.2003
//  Modified 	: 04.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Detail path manager criteria path builder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "detail_path_manager.h"
#include "AISpace.h"
#include "Profiler.h"
#include "level_graph.h"

#ifdef DEBUG
#	include "custommonster.h"
extern bool show_restrictions(CRestrictedObject* object);
#endif

template <typename T>
IC	T sin_apb(T sina, T cosa, T sinb, T cosb)
{
	return				(sina * cosb + cosa * sinb);
}

template <typename T>
IC	T cos_apb(T sina, T cosa, T sinb, T cosb)
{
	return				(cosa * cosb - sina * sinb);
}

IC	bool is_negative(f32 a)
{
	return				(!fis_zero(a) && (a < 0.f));
}

IC	bool coincide_directions(
	const fVector2& start_circle_center,
	const fVector2& start_tangent_point,
	f32			start_cross_product,
	const fVector2& dest_circle_center,
	const fVector2& dest_tangent_point,
	f32			dest_cross_product
)
{
	if (fis_zero(start_cross_product))
	{
		fVector2		circle_tangent_point_direction = fVector2( ).sub(dest_tangent_point, dest_circle_center);
		fVector2		start_tangent_dest_tangent_direction = fVector2( ).sub(dest_tangent_point, start_tangent_point);
		f32				cp1 = start_tangent_dest_tangent_direction.crossproduct(circle_tangent_point_direction);
		return			(dest_cross_product * cp1 >= 0.f);
	}

	fVector2			circle_tangent_point_direction = fVector2( ).sub(start_tangent_point, start_circle_center);
	fVector2			start_tangent_dest_tangent_direction = fVector2( ).sub(dest_tangent_point, start_tangent_point);
	f32				cp1 = start_tangent_dest_tangent_direction.crossproduct(circle_tangent_point_direction);
	return				(start_cross_product * cp1 >= 0.f);
}

bool CDetailPathManager::compute_tangent(
	const STrajectoryPoint& start,
	const SCirclePoint& start_circle,
	const STrajectoryPoint& dest,
	const SCirclePoint& dest_circle,
	SCirclePoint* tangents,
	const EDirectionType	direction_type
)
{
	f32				start_cp;
	f32				dest_cp;
	f32				distance;
	f32				alpha;
	f32				start_yaw;
	f32				dest_yaw;
	f32				yaw1;
	f32				yaw2;
	fVector2			direction;

	// computing 2D cross product for start point
	direction.sub(start.position, start_circle.center);
	if (fis_zero(direction.square_magnitude( )))
		direction = start.direction;

	start_yaw = direction.getH( );
	start_yaw = start_yaw >= 0.f ? start_yaw : start_yaw + PI_MUL_2;
	start_cp = start.direction.crossproduct(direction);

	// computing 2D cross product for dest point
	direction.sub(dest.position, dest_circle.center);
	if (fis_zero(direction.square_magnitude( )))
		direction = dest.direction;

	dest_yaw = direction.getH( );
	dest_yaw = dest_yaw >= 0.f ? dest_yaw : dest_yaw + PI_MUL_2;
	dest_cp = dest.direction.crossproduct(direction);

	// direction from the first circle to the second one
	direction.sub(dest_circle.center, start_circle.center);
	yaw1 = direction.getH( );
	yaw1 = yaw2 = yaw1 >= 0.f ? yaw1 : yaw1 + PI_MUL_2;

	if (start_cp * dest_cp >= 0.f)
	{
// so, our tangents are outside
		if (start_circle.center.similar(dest_circle.center, EPSILON_7))
		{
			if (fsimilar(start_circle.radius, dest_circle.radius, EPSILON_7))
			{
// so, our circles are equal
				tangents[0] = tangents[1] = start_circle;
				adjust_point(start_circle.center, dest_yaw, start_circle.radius, tangents[0].point);
				if (start_cp >= 0.f)
					assign_angle(tangents[0].angle, start_yaw, dest_yaw, true, direction_type);
				else
					assign_angle(tangents[0].angle, start_yaw, dest_yaw, false, direction_type);

				tangents[1].point = tangents[0].point;
				tangents[1].angle = 0.f;
				return				(true);
			}
			else
				return				(false);
		}
		else
		{
	  // distance between circle centers
			distance = start_circle.center.distance_to(dest_circle.center);
			// radius difference
			f32			r_diff = start_circle.radius - dest_circle.radius;
			f32			r_diff_abs = _abs(r_diff);
			if ((r_diff_abs > distance) && !fsimilar(r_diff_abs, distance, EPSILON_7))
				return		(false);
			// angle between external tangents and circle centers segment
			f32			temp = r_diff / distance;
			clamp(temp, -.99999f, .99999f);
			alpha = acosf(temp);
			alpha = alpha >= 0.f ? alpha : alpha + PI_MUL_2;
		}
	}
	else
	{
		distance = start_circle.center.distance_to(dest_circle.center);
		// so, our tangents are inside (crossing)
		if ((start_circle.radius + dest_circle.radius > distance) && !fsimilar(start_circle.radius + dest_circle.radius, distance, EPSILON_7))
			return		(false);

		// angle between internal tangents and circle centers segment
		f32			temp = (start_circle.radius + dest_circle.radius) / distance;
		clamp(temp, -.99999f, .99999f);
		alpha = acosf(temp);
		alpha = alpha >= 0.f ? alpha : alpha + PI_MUL_2;
		yaw2 = yaw1 < PI ? yaw1 + PI : yaw1 - PI;
	}

	tangents[0] = start_circle;
	tangents[1] = dest_circle;

	// compute external tangent points
	adjust_point(start_circle.center, yaw1 + alpha, start_circle.radius, tangents[0].point);
	adjust_point(dest_circle.center, yaw2 + alpha, dest_circle.radius, tangents[1].point);

	if (coincide_directions(start_circle.center, tangents[0].point, start_cp, dest_circle.center, tangents[1].point, dest_cp))
	{
		assign_angle(tangents[0].angle, start_yaw, yaw1 + alpha < PI_MUL_2 ? yaw1 + alpha : yaw1 + alpha - PI_MUL_2, start_cp >= 0, direction_type);
		assign_angle(tangents[1].angle, dest_yaw, yaw2 + alpha < PI_MUL_2 ? yaw2 + alpha : yaw2 + alpha - PI_MUL_2, dest_cp >= 0, direction_type, false);
		return			(true);
	}

	// compute external tangent points
	adjust_point(start_circle.center, yaw1 - alpha, start_circle.radius, tangents[0].point);
	adjust_point(dest_circle.center, yaw2 - alpha, dest_circle.radius, tangents[1].point);
	assign_angle(tangents[0].angle, start_yaw, yaw1 - alpha >= 0.f ? yaw1 - alpha : yaw1 - alpha + PI_MUL_2, start_cp >= 0, direction_type);
	assign_angle(tangents[1].angle, dest_yaw, yaw2 - alpha >= 0.f ? yaw2 - alpha : yaw2 - alpha + PI_MUL_2, dest_cp >= 0, direction_type, false);

	return				(true);
}

bool CDetailPathManager::build_circle_trajectory(
	const STrajectoryPoint& position,
	xr_vector<STravelPathPoint>* path,
	u32* vertex_id,
	const u32					velocity
)
{
	const f32			min_dist = 0.1f;
	STravelPathPoint	t;
	t.velocity = velocity;
	if (position.radius * _abs(position.angle) <= min_dist)
	{
		if (!path)
		{
			if (vertex_id)
				*vertex_id = position.vertex_id;
			return			(true);
		}
		if (vertex_id)
			*vertex_id = position.vertex_id;

		t.position = ai( ).level_graph( ).v3d(position.position);
		if (vertex_id || (!path->empty( ) && !path->back( ).position.similar(t.position, EPSILON_7)))
		{
			VERIFY(t.velocity != u32(-1));
			t.vertex_id = position.vertex_id;
			path->push_back(t);
		}
		return			(true);
	}

	fVector2			direction;
	fVector3				curr_pos;
	u32					curr_vertex_id;
	direction.sub(position.position, position.center);
	curr_pos.set(position.position.x, 0.f, position.position.y);
	curr_vertex_id = position.vertex_id;
	f32				angle = position.angle;
	s32					size = path ? (s32)path->size( ) : -1;

	if (!fis_zero(direction.square_magnitude( )))
		direction.normalize( );
	else
		direction.set(1.f, 0.f);

	f32				sina;
	f32				cosa;
	f32				sinb;
	f32				cosb;
	f32				sini;
	f32				cosi;
	f32				temp;
	s32					n;
	if (fis_zero(position.angular_velocity))
	{
		n = 1;
	}
	else
	{
		s32 m = _min(iFloor(_abs(angle) / position.angular_velocity * 10.0f + 1.5f), iFloor(position.radius * _abs(angle) / min_dist + 1.5f));

#ifdef DEBUG
		if (m >= 10000)
		{
			Msg("! [position.radius=%f],[angle=%f],[m=%d]", position.radius, angle, m);
			VERIFY(m < 10000);
		}
#endif

		n = !m ? 1 : m;
	}

	s32 k = vertex_id ? 0 : -1;

	if (path)
	{
		path->reserve(size + n + k);
	}

	sina = -direction.x;
	cosa = direction.y;
	sinb = _sin(angle / f32(n));
	cosb = _cos(angle / f32(n));
	sini = 0.0f;
	cosi = 1.0f;

	for (s32 i = 0; i <= n + k; ++i)
	{
		VERIFY(t.velocity != u32(-1));
		t.position.x = -sin_apb(sina, cosa, sini, cosi) * position.radius + position.center.x;
		t.position.z = cos_apb(sina, cosa, sini, cosi) * position.radius + position.center.y;

		curr_vertex_id = ai( ).level_graph( ).check_position_in_direction(curr_vertex_id, curr_pos, t.position);
		if (!ai( ).level_graph( ).valid_vertex_id(curr_vertex_id))
			return		(false);

		if (path)
		{
			t.vertex_id = curr_vertex_id;
			path->push_back(t);
		}

		temp = sin_apb(sinb, cosb, sini, cosi);
		cosi = cos_apb(sinb, cosb, sini, cosi);
		sini = temp;
		curr_pos = t.position;
	}

	if (vertex_id)
		*vertex_id = curr_vertex_id;
	else
		if (path)
			std::reverse(path->begin( ) + size, path->end( ));

	return				(true);
}

bool CDetailPathManager::build_line_trajectory(
	const STrajectoryPoint& start,
	const STrajectoryPoint& dest,
	u32							vertex_id,
	xr_vector<STravelPathPoint>* path,
	u32							velocity
)
{
	VERIFY(ai( ).level_graph( ).valid_vertex_id(vertex_id));
	STravelPathPoint	t;
	t.velocity = velocity;
	if (ai( ).level_graph( ).inside(vertex_id, dest.point))
	{
		if (path)
		{
			t.position = ai( ).level_graph( ).v3d(dest.point);
			t.vertex_id = vertex_id;
			path->push_back(t);
		}
		return			(true);
	}

//	VERIFY				(ai().level_graph().check_position_in_direction(vertex_id,start.point,dest.point));
	return				(path ? ai( ).level_graph( ).create_straight_path<false>(vertex_id, start.point, dest.point, *path, t, false, false) : ai( ).level_graph( ).valid_vertex_id(ai( ).level_graph( ).check_position_in_direction(vertex_id, start.point, dest.point)));
}

bool CDetailPathManager::build_trajectory(
	const STrajectoryPoint& start,
	const STrajectoryPoint& dest,
	xr_vector<STravelPathPoint>* path,
	const u32					velocity1,
	const u32					velocity2,
	const u32					velocity3
)
{

	u32					vertex_id;
	if (!build_circle_trajectory(start, path, &vertex_id, velocity1))
		return			(false);

	if (!build_line_trajectory(start, dest, vertex_id, path, velocity2))
		return			(false);

	if (!build_circle_trajectory(dest, path, 0, velocity3))
		return			(false);

	return				(true);
}

bool CDetailPathManager::build_trajectory(
	STrajectoryPoint& start,
	STrajectoryPoint& dest,
	const SCirclePoint			tangents[4][2],
	const u32					tangent_count,
	xr_vector<STravelPathPoint>* path,
	f32& time,
	const u32					velocity1,
	const u32					velocity2,
	const u32					velocity3
)
{
	time = flt_max;
	SDist			dist[4];
	f32			straight_velocity = _abs(velocity(velocity2).linear_velocity);
	{
		for (u32 i = 0; i < tangent_count; ++i)
		{
			dist[i].index = i;
			dist[i].time =
				_abs(tangents[i][0].angle) / start.angular_velocity +
				_abs(tangents[i][1].angle) / dest.angular_velocity +
				tangents[i][0].point.distance_to(tangents[i][1].point) *
				(fis_zero(straight_velocity) ? 0 : 1.f / straight_velocity);
		}
	}

	std::sort(dist, dist + tangent_count);

	{
		for (u32 i = 0, j = path ? path->size( ) : 0; i < tangent_count; ++i)
		{
			(SCirclePoint&)(start) = tangents[dist[i].index][0];
			(SCirclePoint&)(dest) = tangents[dist[i].index][1];
			if (build_trajectory(start, dest, path, velocity1, velocity2, velocity3))
			{
				time = dist[i].time;
				return	(true);
			}
			else
				if (path)
					path->resize(j);
		}
	}

	return		(false);
}

bool CDetailPathManager::compute_trajectory(
	STrajectoryPoint& start,
	STrajectoryPoint& dest,
	xr_vector<STravelPathPoint>* path,
	f32& time,
	const u32					velocity1,
	const u32					velocity2,
	const u32					velocity3,
	const EDirectionType		direction_type
)
{
	SCirclePoint				start_circles[2], dest_circles[2];
	compute_circles(start, start_circles);
	compute_circles(dest, dest_circles);

	u32							tangent_count = 0;
	SCirclePoint				tangent_points[4][2];
	for (u32 i = 0; i < 2; ++i)
		for (u32 j = 0; j < 2; ++j)
			if (compute_tangent(
				start,
				start_circles[i],
				dest,
				dest_circles[j],
				tangent_points[tangent_count],
				direction_type
				)
				)
			{
				if (!ai( ).level_graph( ).valid_vertex_position(ai( ).level_graph( ).v3d(tangent_points[tangent_count][0].point)))
					continue;

				if (!ai( ).level_graph( ).valid_vertex_position(ai( ).level_graph( ).v3d(tangent_points[tangent_count][1].point)))
					continue;

				++tangent_count;
			}

	return			(build_trajectory(start, dest, tangent_points, tangent_count, path, time, velocity1, velocity2, velocity3));
}

bool CDetailPathManager::compute_path(
	STrajectoryPoint& _start,
	STrajectoryPoint& _dest,
	xr_vector<STravelPathPoint>* m_tpTravelLine,
	const xr_vector<STravelParamsIndex>& start_params,
	const xr_vector<STravelParamsIndex>& dest_params,
	const u32					straight_line_index,
	const u32					straight_line_index_negative
)
{
	STrajectoryPoint			start = _start;
	STrajectoryPoint			dest = _dest;
	f32							min_time = flt_max;
	f32							time;
	u32							size = m_tpTravelLine ? m_tpTravelLine->size( ) : 0;
	u32							real_straight_line_index;
	xr_vector<STravelParamsIndex>::const_iterator I = start_params.begin( );
	xr_vector<STravelParamsIndex>::const_iterator E = start_params.end( );
	for (; I != E; ++I)
	{
		EDirectionType				direction_type = eDirectionTypePP;
		start = _start;
		(STravelParams&)start = (*I);
		real_straight_line_index = straight_line_index;
		if (is_negative(start.linear_velocity))
		{
			real_straight_line_index = straight_line_index_negative;
			direction_type = EDirectionType(direction_type | eDirectionTypeFN);
			start.direction.mul(-1.f);
		}
		xr_vector<STravelParamsIndex>::const_iterator i = dest_params.begin( );
		xr_vector<STravelParamsIndex>::const_iterator e = dest_params.end( );
		for (; i != e; ++i)
		{
			dest = _dest;
			(STravelParams&)dest = (*i);

			if (is_negative(dest.linear_velocity))
				direction_type = EDirectionType(direction_type | eDirectionTypeSN);

			if (direction_type & eDirectionTypeFN)
				dest.direction.mul(-1.f);

			m_temp_path.clear( );
			if (compute_trajectory(start, dest, m_tpTravelLine ? &m_temp_path : 0, time, (*I).index, real_straight_line_index, (*i).index, direction_type))
			{
				if (!m_try_min_time || (time < min_time))
				{
					min_time = time;
					if (m_tpTravelLine)
					{
						m_tpTravelLine->resize(size);
						m_tpTravelLine->insert(m_tpTravelLine->end( ), m_temp_path.begin( ), m_temp_path.end( ));

						if (!m_try_min_time)
							return	(true);
					}
					else
						return		(true);
				}
			}
		}
	}

	if (fsimilar(min_time, flt_max))
		return				(false);

	return					(true);
}

void CDetailPathManager::validate_vertex_position(STrajectoryPoint& point) const
{
	if (ai( ).level_graph( ).valid_vertex_position(ai( ).level_graph( ).v3d(point.position)) && ai( ).level_graph( ).inside(point.vertex_id, point.position))
		return;

	CLevelGraph::SContour	contour;
	fVector3				position;
	fVector3				center;
	ai( ).level_graph( ).contour(contour, point.vertex_id);
	ai( ).level_graph( ).nearest(position, ai( ).level_graph( ).v3d(point.position), contour);
	center.add(contour.v1, contour.v3);
	center.mul(.5f);
	center.sub(position);
	center.normalize( );
	center.mul(EPSILON_3);
	position.add(center);
	point.position = ai( ).level_graph( ).v2d(position);
	VERIFY(ai( ).level_graph( ).inside(point.vertex_id, point.position));
}

bool CDetailPathManager::init_build(
	const xr_vector<u32>& level_path,
	u32						intermediate_index,
	STrajectoryPoint& start,
	STrajectoryPoint& dest,
	u32& straight_line_index,
	u32& straight_line_index_negative
)
{
	VERIFY(!level_path.empty( ));
	VERIFY(level_path.size( ) > intermediate_index);

	m_current_travel_point = 0;
	m_path.clear( );

	start.position = ai( ).level_graph( ).v2d(m_start_position);
	start.direction = ai( ).level_graph( ).v2d(m_start_direction);
	start.vertex_id = level_path.front( );

	validate_vertex_position(start);

	dest.position = ai( ).level_graph( ).v2d(m_dest_position);
	if (m_use_dest_orientation)
		dest.direction = ai( ).level_graph( ).v2d(m_dest_direction);
	else
		dest.direction.set(0.f, 1.f);
	dest.vertex_id = level_path.back( );

	validate_vertex_position(dest);
	m_corrected_dest_position.x = dest.position.x;
	m_corrected_dest_position.y = ai( ).level_graph( ).vertex_plane_y(dest.vertex_id, dest.position.x, dest.position.y);
	m_corrected_dest_position.z = dest.position.y;

	if (start.direction.square_magnitude( ) < EPSILON_3)
		start.direction.set(0.f, 1.f);
	else
		start.direction.normalize( );

	if (dest.direction.square_magnitude( ) < EPSILON_3)
		dest.direction.set(0.f, 1.f);
	else
		dest.direction.normalize( );

	// filling velocity parameters
	f32								max_linear_velocity = -flt_max;
	f32								min_linear_velocity = flt_max;
	straight_line_index = u32(-1);
	straight_line_index_negative = u32(-1);
	m_start_params.clear( );
	VELOCITIES::const_iterator			I = m_movement_params.begin( );
	VELOCITIES::const_iterator			E = m_movement_params.end( );
	for (; I != E; ++I)
	{
		if (!check_mask(m_velocity_mask, (*I).first))
			continue;
		STravelParamsIndex				temp;
		(STravelParams&)temp = (*I).second;
		temp.index = (*I).first;
		if (check_mask(m_desirable_mask, (*I).first))
		{
			m_start_params.insert(m_start_params.begin( ), temp);
			if (max_linear_velocity < temp.linear_velocity)
			{
				straight_line_index = temp.index;
				max_linear_velocity = temp.linear_velocity;
			}
			if (min_linear_velocity > temp.linear_velocity)
			{
				straight_line_index_negative = temp.index;
				min_linear_velocity = temp.linear_velocity;
			}
		}
		else
			m_start_params.push_back(temp);
	}

	if (m_start_params.empty( ))
		return							(false);

	m_dest_params.clear( );
	m_dest_params.push_back(STravelParamsIndex(0.f, PI_MUL_2, u32(-1)));

	return								(true);
}

bool CDetailPathManager::fill_key_points(
	const xr_vector<u32>& level_path,
	u32						intermediate_index,
	STrajectoryPoint& start,
	STrajectoryPoint& dest
)
{
	STravelPoint						start_point;
	start_point.vertex_id = start.vertex_id;
	start_point.position = start.position;
	m_key_points.clear( );

	for (s32 _i = 0, i = 0, n = (s32)level_path.size( ) - 1, j = n, m = j; _i < n; )
	{
		if (!ai( ).level_graph( ).check_vertex_in_direction(start_point.vertex_id, start_point.position, level_path[j]))
		{
			m = j;
			j = (i + j) / 2;
		}
		else
		{
			if ((j == n) && !ai( ).level_graph( ).valid_vertex_id(ai( ).level_graph( ).check_position_in_direction(start_point.vertex_id, start_point.position, dest.position)))
			{
				m = j;
				j = (i + j) / 2;
			}
			else
			{
				i = j;
				j = (i + m) / 2;
			}
		}
		if (i >= m - 1)
		{
			if (i <= _i)
				return					(false);
			_i = i;
			m_key_points.push_back(start_point);
			if (i == n)
			{
				if (ai( ).level_graph( ).valid_vertex_id(ai( ).level_graph( ).check_position_in_direction(start_point.vertex_id, start_point.position, dest.position)))
				{
					m_key_points.push_back(dest);
					break;
				}
				else
					return				(false);
			}
			start_point.vertex_id = level_path[_i];
			start_point.position = ai( ).level_graph( ).v2d(ai( ).level_graph( ).vertex_position(start_point.vertex_id));
			j = m = n;
		}
	}
	return								(true);
}

IC	CDetailPathManager::STravelPoint CDetailPathManager::compute_better_key_point(
	const STravelPoint& point0,
	const STravelPoint& point1,
	const STravelPoint& point2,
	bool				reverse_order
)
{
	CDetailPathManager::STravelPoint		result = point1;
	f32						dist02 = point2.position.distance_to(point0.position);
	f32						dist12 = point2.position.distance_to(point1.position);
	fVector2					direction21 = fVector2( ).sub(point1.position, point2.position);
	fVector2					direction20 = fVector2( ).sub(point0.position, point2.position);
	direction21.normalize( );
	direction20.normalize( );
	f32						cos_alpha = direction21.dot(direction20);
	clamp(cos_alpha, -.99999f, .99999f);
	direction20 = direction21;
	direction20.mul(-1.f);
	f32						a = 0.0f;
	f32						b = 1.0f;
	f32						c = 1.0f;
	f32						d = dist12 - dist02 / cos_alpha * 0.5f;
	u32							vertex_id;

	do
	{
		direction21 = direction20;
		direction21.mul(d * c);
		direction21.add(point1.position);
//.		
		if (!ai( ).level_graph( ).valid_vertex_position(ai( ).level_graph( ).v3d(direction21)))
			return				(point1);

		if (!reverse_order)
			vertex_id = ai( ).level_graph( ).check_position_in_direction(point0.vertex_id, point0.position, direction21);
		else
			vertex_id = ai( ).level_graph( ).check_position_in_direction(point2.vertex_id, point2.position, direction21);
		if (ai( ).level_graph( ).valid_vertex_id(vertex_id))
		{
			VERIFY(ai( ).level_graph( ).inside(vertex_id, direction21));
			u32					test_vertex_id;
			if (!reverse_order)
				test_vertex_id = ai( ).level_graph( ).check_position_in_direction(vertex_id, direction21, point2.position);
			else
				test_vertex_id = ai( ).level_graph( ).check_position_in_direction(vertex_id, direction21, point0.position);
			if (!ai( ).level_graph( ).valid_vertex_id(test_vertex_id))
			{
				b = c;
			}
			else
			{
				a = c;
				result.position = direction21;
				result.vertex_id = vertex_id;
			}
		}
		else
			b = c;
		c = (a + b) * .5f;
	}
	while (!fsimilar(a, b, .01f));

	return						(result);
}

IC	bool CDetailPathManager::better_key_point(
	const STravelPoint& point0,
	const STravelPoint& point2,
	const STravelPoint& point10,
	const STravelPoint& point11
)
{
	fVector2				direction100 = fVector2( ).sub(point0.position, point10.position);
	fVector2				direction120 = fVector2( ).sub(point2.position, point10.position);
	fVector2				direction101 = fVector2( ).sub(point0.position, point11.position);
	fVector2				direction121 = fVector2( ).sub(point2.position, point11.position);
	direction100.normalize( );
	direction120.normalize( );
	direction101.normalize( );
	direction121.normalize( );
	f32					cos_alpha0 = direction100.dot(direction120);
	f32					cos_alpha1 = direction101.dot(direction121);
	return					(cos_alpha0 < cos_alpha1);
}

void CDetailPathManager::build_path_via_key_points(
	STrajectoryPoint& start,
	STrajectoryPoint& dest,
	xr_vector<STravelParamsIndex>& finish_params,
	const u32				straight_line_index,
	const u32				straight_line_index_negative
)
{
	STrajectoryPoint					s, d, p;
	s = start;
	xr_vector<STravelPoint>::const_iterator	I = m_key_points.begin( ), B = I;
	xr_vector<STravelPoint>::const_iterator	E = m_key_points.end( );
	for (B != E ? ++I : I; I != E; ++I)
	{
		VERIFY(ai( ).level_graph( ).inside((*I).vertex_id, (*I).position));

		bool							last_point = (I + 1) == E;
		if (!last_point)
		{
			(STravelPoint&)d = *I;
			d.direction.sub((I + 1)->position, d.position);
			VERIFY(!fis_zero(d.direction.magnitude( )));
			d.direction.normalize( );
		}
		else
			d = dest;

		bool							succeed = compute_path(s, d, &m_path, m_start_params, last_point ? finish_params : m_start_params, straight_line_index, straight_line_index_negative);
		if (!succeed)
		{
			m_path.clear( );
			return;
		}

		if (last_point)
			break;

		s = d;
		VERIFY(m_path.size( ) > 1);
		s.direction.sub(
			ai( ).level_graph( ).v2d(m_path[m_path.size( ) - 1].position),
			ai( ).level_graph( ).v2d(m_path[m_path.size( ) - 2].position)
		);

		if (fis_zero(s.direction.magnitude( )))
		{
			m_path.pop_back( );
			VERIFY(m_path.size( ) > 1);
			s.direction.sub(
				ai( ).level_graph( ).v2d(m_path[m_path.size( ) - 1].position),
				ai( ).level_graph( ).v2d(m_path[m_path.size( ) - 2].position)
			);
		}

		VERIFY(!fis_zero(s.direction.magnitude( )));
		s.direction.normalize( );
		m_path.pop_back( );

		d = p;
		if (!m_path.empty( ))
		{
			if (is_negative(velocity(m_path.back( ).velocity).linear_velocity))
				s.direction.mul(-1.f);
		}

		VERIFY(!fis_zero(s.direction.magnitude( )));
	}

	if ((B == E) && !compute_path(s, dest, &m_path, m_start_params, finish_params, straight_line_index, straight_line_index_negative))
	{
		m_path.clear( );
		return;
	}

	add_patrol_point( );
	ai( ).level_graph( ).assign_y_values(m_path);
	m_failed = false;
}

void CDetailPathManager::postprocess_key_points(
	const xr_vector<u32>& level_path,
	u32						intermediate_index,
	STrajectoryPoint& start,
	STrajectoryPoint& dest,
	xr_vector<STravelParamsIndex>& finish_params,
	u32						straight_line_index,
	u32						straight_line_index_negative
)
{
	if (m_key_points.size( ) < 3)
	{
		return;
	}

	if (m_key_points[m_key_points.size( ) - 2].position.similar(m_key_points[m_key_points.size( ) - 1].position, EPSILON_7))
	{
		m_key_points.pop_back( );
	}

	for (s32 i = 1, n = (s32)m_key_points.size( ) - 1; i < n; ++i)
	{
		STravelPoint		key_point0 = compute_better_key_point(m_key_points[i - 1], m_key_points[i], m_key_points[i + 1], false);
		STravelPoint		key_point1 = compute_better_key_point(m_key_points[i + 1], m_key_points[i], m_key_points[i - 1], true);
		{
			u32				vertex_id = ai( ).level_graph( ).check_position_in_direction(m_key_points[i - 1].vertex_id, m_key_points[i - 1].position, key_point0.position);
			if (!ai( ).level_graph( ).valid_vertex_id(vertex_id))
			{
				vertex_id = vertex_id;
			}
		}
		{
			u32				vertex_id = ai( ).level_graph( ).check_position_in_direction(m_key_points[i - 1].vertex_id, m_key_points[i - 1].position, key_point1.position);
			if (!ai( ).level_graph( ).valid_vertex_id(vertex_id))
			{
				vertex_id = vertex_id;
			}
		}
		{
			u32				vertex_id = ai( ).level_graph( ).check_position_in_direction(key_point0.vertex_id, key_point0.position, m_key_points[i + 1].position);
			if (!ai( ).level_graph( ).valid_vertex_id(vertex_id))
			{
				vertex_id = vertex_id;
			}
		}
		{
			u32				vertex_id = ai( ).level_graph( ).check_position_in_direction(key_point1.vertex_id, key_point1.position, m_key_points[i + 1].position);
			if (!ai( ).level_graph( ).valid_vertex_id(vertex_id))
			{
				vertex_id = vertex_id;
			}
		}

		if (better_key_point(m_key_points[i - 1], m_key_points[i + 1], key_point0, key_point1))
		{
			m_key_points[i] = key_point0;
		}
		else
		{
			m_key_points[i] = key_point1;
		}

		VERIFY(!m_key_points[i].position.similar(m_key_points[i - 1].position, EPSILON_7));
	}
}

void CDetailPathManager::add_patrol_point( )
{
	m_last_patrol_point = m_path.size( ) - 1;
	if ((m_path.size( ) > 1) && m_state_patrol_path && !fis_zero(extrapolate_length( )))
	{
		STravelPathPoint				t;
		fVector3							v;
		v.sub(m_path.back( ).position, m_path[m_last_patrol_point - 1].position);
		v.y = 0.f;
		if (v.magnitude( ) > EPSILON_7)
			v.normalize( );
		else
			return;

		v.mul(extrapolate_length( ));
		t = m_path.back( );
		t.position.add(v);
		ai( ).level_graph( ).create_straight_path<false>(
			m_path.back( ).vertex_id,
			ai( ).level_graph( ).v2d(m_path.back( ).position),
			ai( ).level_graph( ).v2d(t.position),
			m_path,
			m_path.back( ),
			false,
			false
		);
	}
}

void CDetailPathManager::build_smooth_path(
	const xr_vector<u32>& level_path,
	u32						intermediate_index
)
{
	START_PROFILE("Build Path/Detail Path");

	m_failed = true;

	u32									straight_line_index, straight_line_index_negative;
	STrajectoryPoint					start, dest;

	if (!init_build(level_path, intermediate_index, start, dest, straight_line_index, straight_line_index_negative))
	{
		return;
	}

	if (m_restricted_object)
	{

#ifdef DEBUG
		fVector3							start_pos = ai( ).level_graph( ).v3d(start.position);
		start_pos.y = ai( ).level_graph( ).vertex_plane_y(start.vertex_id, start_pos.x, start_pos.z);
		bool							alvi = m_restricted_object->accessible(start.vertex_id);
		bool							asp = m_restricted_object->accessible(start_pos);
		VERIFY(ai( ).level_graph( ).inside(start.vertex_id, start_pos));
		if (!((alvi && asp) || (!asp && !alvi)))
		{
			Msg("! vertex [%d], position [%f][%f][%f]", start.vertex_id, VPUSH(start_pos));
		}
		VERIFY3((alvi && asp) || (!asp && !alvi) || show_restrictions(m_restricted_object), "Invalid restrictions (see log for details) for object ", *m_restricted_object->object( ).cName( ));
#endif

		m_restricted_object->add_border(start.vertex_id, dest.vertex_id);
	}

	xr_vector<STravelParamsIndex>& finish_params = m_use_dest_orientation ? m_start_params : m_dest_params;

	if (!fill_key_points(level_path, intermediate_index, start, dest))
	{
		if (m_restricted_object)
			m_restricted_object->remove_border( );
		return;
	}

	postprocess_key_points(level_path, intermediate_index, start, dest, finish_params, straight_line_index, straight_line_index_negative);

	build_path_via_key_points(start, dest, finish_params, straight_line_index, straight_line_index_negative);

	if (m_restricted_object)
		m_restricted_object->remove_border( );

	STOP_PROFILE;
}
