////////////////////////////////////////////////////////////////////////////
//	Module 		: level_graph_vertex.cpp
//	Created 	: 02.10.2001
//  Modified 	: 11.11.2003
//	Author		: Oles Shihkovtsov, Dmitriy Iassenev
//	Description : Level graph vertex functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level_graph.h"
#include "game_level_cross_table.h"

#include "AISpace.h"

f32 CLevelGraph::distance(const fVector3& position, const CLevelGraph::CVertex* vertex) const
{
	SContour				_contour;
	contour(_contour, vertex);

	// calculate minimal distance
	f32					best, dist;
	best = distance(position, _contour.v1, _contour.v2);
	dist = distance(position, _contour.v2, _contour.v3);
	if (dist < best)
		best = dist;

	dist = distance(position, _contour.v3, _contour.v4);
	if (dist < best)
		best = dist;

	dist = distance(position, _contour.v4, _contour.v1);
	if (dist < best)
		best = dist;

	return					(best);
}

void CLevelGraph::choose_point(const fVector3& start_point, const fVector3& finish_point, const SContour& _contour, s32 node_id, fVector3& temp_point, s32& saved_index) const
{
	SContour					tNextContour;
	SSegment					tNextSegment;
	fVector3					tCheckPoint1 = start_point;
	fVector3					tCheckPoint2 = start_point;
	fVector3					tIntersectPoint;
	contour(tNextContour, node_id);
	intersect(tNextSegment, tNextContour, _contour);
	u32							dwIntersect = intersect(start_point.x, start_point.z, finish_point.x, finish_point.z, tNextSegment.v1.x, tNextSegment.v1.z, tNextSegment.v2.x, tNextSegment.v2.z, &tIntersectPoint.x, &tIntersectPoint.z);
	if (!dwIntersect)
		return;
	for (s32 i = 0; i < 4; ++i)
	{
		switch (i)
		{
			case 0:
			{
				tCheckPoint1 = tNextContour.v1;
				tCheckPoint2 = tNextContour.v2;
				break;
			}
			case 1:
			{
				tCheckPoint1 = tNextContour.v2;
				tCheckPoint2 = tNextContour.v3;
				break;
			}
			case 2:
			{
				tCheckPoint1 = tNextContour.v3;
				tCheckPoint2 = tNextContour.v4;
				break;
			}
			case 3:
			{
				tCheckPoint1 = tNextContour.v4;
				tCheckPoint2 = tNextContour.v1;
				break;
			}
			default: NODEFAULT;
		}

		dwIntersect = intersect(start_point.x, start_point.z, finish_point.x, finish_point.z, tCheckPoint1.x, tCheckPoint1.z, tCheckPoint2.x, tCheckPoint2.z, &tIntersectPoint.x, &tIntersectPoint.z);
		if (dwIntersect == eLineIntersectionIntersect)
		{
			if (finish_point.distance_to_xz(tIntersectPoint) < finish_point.distance_to_xz(temp_point) + EPSILON_3)
			{
				temp_point = tIntersectPoint;
				saved_index = node_id;
			}
		}
		else if (dwIntersect == eLineIntersectionEqual)
		{
			if (start_point.distance_to_xz(tCheckPoint1) > start_point.distance_to_xz(temp_point))
			{
				if (start_point.distance_to_xz(tCheckPoint1) > start_point.distance_to_xz(tCheckPoint2))
				{
					temp_point = tCheckPoint1;
					saved_index = node_id;
				}
				else
				{
					temp_point = tCheckPoint2;
					saved_index = node_id;
				}
			}
			else if (start_point.distance_to_xz(tCheckPoint2) > start_point.distance_to_xz(temp_point))
			{
				temp_point = tCheckPoint2;
				saved_index = node_id;
			}
		}
	}
}

f32 CLevelGraph::check_position_in_direction(u32 start_vertex_id, const fVector3& start_position, const fVector3& finish_position, const f32 max_distance) const
{
	SContour				_contour;
	const_iterator			I, E;
	s32						saved_index, iPrevIndex = -1, iNextNode;
	fVector3					start_point = start_position;
	fVector3			temp_point = start_position;
	fVector3			finish_point = finish_position;
	f32					fCurDistance = 0.f, fDistance = start_position.distance_to_xz(finish_position);
	u32						dwCurNode = start_vertex_id;

	while (!inside(vertex(dwCurNode), finish_position) && (fCurDistance < (fDistance + EPSILON_3)))
	{
		begin(dwCurNode, I, E);
		saved_index = -1;
		contour(_contour, dwCurNode);
		for (; I != E; ++I)
		{
			iNextNode = value(dwCurNode, I);
			if (valid_vertex_id(iNextNode) && (iPrevIndex != iNextNode))
				choose_point(start_point, finish_point, _contour, iNextNode, temp_point, saved_index);
		}

		if (saved_index > -1)
		{
			fCurDistance = start_point.distance_to_xz(temp_point);
			iPrevIndex = dwCurNode;
			dwCurNode = saved_index;
		}
		else
			return			(max_distance);
	}

	if (inside(vertex(dwCurNode), finish_position) && (_abs(vertex_plane_y(*vertex(dwCurNode), finish_position.x, finish_position.z) - finish_position.y) < .5f))
		return				(start_point.distance_to_xz(finish_position));
	else
		return				(max_distance);
}

f32 CLevelGraph::mark_nodes_in_direction(u32 start_vertex_id, const fVector3& start_position, const fVector3& tDirection, f32 fDistance, xr_vector<u32>& tpaStack, xr_vector<bool>* tpaMarks) const
{
	fVector3					finish_point;
	fVector3 direction = tDirection;
	direction.normalize( );
	finish_point.mul(direction, fDistance);
	finish_point.add(start_position);
	return					(mark_nodes_in_direction(start_vertex_id, start_position, finish_point, tpaStack, tpaMarks));
}

f32 CLevelGraph::mark_nodes_in_direction(u32 start_vertex_id, const fVector3& start_position, u32 finish_vertex_id, xr_vector<u32>& tpaStack, xr_vector<bool>* tpaMarks) const
{
	return					(mark_nodes_in_direction(start_vertex_id, start_position, vertex_position(finish_vertex_id), tpaStack, tpaMarks));
}

f32 CLevelGraph::mark_nodes_in_direction(u32 start_vertex_id, const fVector3& start_point, const fVector3& finish_point, xr_vector<u32>& tpaStack, xr_vector<bool>* tpaMarks) const
{
	SContour				_contour;
	const_iterator			I, E;
	s32						saved_index, iPrevIndex = -1, iNextNode;
	fVector3					temp_point = start_point;
	f32					fDistance = start_point.distance_to(finish_point), fCurDistance = 0.f;
	u32						dwCurNode = start_vertex_id;

	while (!inside(vertex(dwCurNode), finish_point) && (fCurDistance < (fDistance + EPSILON_3)))
	{
		begin(dwCurNode, I, E);
		saved_index = -1;
		contour(_contour, dwCurNode);
		for (; I != E; ++I)
		{
			iNextNode = value(dwCurNode, I);
			if (valid_vertex_id(iNextNode) && (iPrevIndex != iNextNode))
				choose_point(start_point, finish_point, _contour, iNextNode, temp_point, saved_index);
		}

		if (saved_index > -1)
		{
			fCurDistance = start_point.distance_to_xz(temp_point);
			iPrevIndex = dwCurNode;
			dwCurNode = saved_index;
		}
		else
			return(fCurDistance);

		if (tpaMarks)
			(*tpaMarks)[dwCurNode] = true;
		tpaStack.push_back(dwCurNode);
	}

	return					(fCurDistance);
}

f32 CLevelGraph::farthest_vertex_in_direction(u32 start_vertex_id, const fVector3& start_point, const fVector3& finish_point, u32& finish_vertex_id, xr_vector<bool>* tpaMarks, bool check_accessability) const
{
	SContour				_contour;
	const_iterator			I, E;
	s32						saved_index, iPrevIndex = -1, iNextNode;
	fVector3					temp_point = start_point;
	f32					fDistance = start_point.distance_to(finish_point), fCurDistance = 0.f;
	u32						dwCurNode = start_vertex_id;

	while (!inside(vertex(dwCurNode), finish_point) && (fCurDistance < (fDistance + EPSILON_3)))
	{
		begin(dwCurNode, I, E);
		saved_index = -1;
		contour(_contour, dwCurNode);
		for (; I != E; ++I)
		{
			iNextNode = value(dwCurNode, I);
			if (valid_vertex_id(iNextNode) && (iPrevIndex != iNextNode))
				choose_point(start_point, finish_point, _contour, iNextNode, temp_point, saved_index);
		}

		if (saved_index > -1)
		{
			if (check_accessability && !is_accessible(saved_index))
				return		(fCurDistance);

			fCurDistance = start_point.distance_to_xz(temp_point);
			iPrevIndex = dwCurNode;
			dwCurNode = saved_index;
		}
		else
			return			(fCurDistance);

		if (tpaMarks)
			(*tpaMarks)[dwCurNode] = true;
		finish_vertex_id = dwCurNode;
	}
	return					(fCurDistance);
}

bool CLevelGraph::create_straight_path(u32 start_vertex_id, const fVector3& start_point, const fVector3& finish_point, xr_vector<fVector3>& tpaOutputPoints, xr_vector<u32>& tpaOutputNodes, bool bAddFirstPoint, bool bClearPath) const
{
	return					(create_straight_path(start_vertex_id, v2d(start_point), v2d(finish_point), tpaOutputPoints, tpaOutputNodes, bAddFirstPoint, bClearPath));
}

u32	 CLevelGraph::check_position_in_direction_slow(u32 start_vertex_id, const fVector2& start_position, const fVector2& finish_position) const
{
	if (!valid_vertex_position(v3d(finish_position)))
		return				(u32(-1));

	u32						cur_vertex_id = start_vertex_id, prev_vertex_id = u32(-1);
	fBox2					box;
	fVector2				identity;
	fVector2				start;
	fVector2				dest;
	fVector2				dir;

	identity.x = identity.y = header( ).cell_size( ) * .5f;
	start = start_position;
	dest = finish_position;
	dir.sub(dest, start);
	u32						dest_xz = vertex_position(v3d(dest)).xz( );
	fVector2				temp;
	unpack_xz(vertex(start_vertex_id), temp.x, temp.y);

	f32					cur_sqr = _sqr(temp.x - dest.x) + _sqr(temp.y - dest.y);
	for (;;)
	{
		const_iterator		I, E;
		begin(cur_vertex_id, I, E);
		bool				found = false;
		for (; I != E; ++I)
		{
			u32				next_vertex_id = value(cur_vertex_id, I);
			if ((next_vertex_id == prev_vertex_id) || !valid_vertex_id(next_vertex_id))
				continue;
			CVertex* v = vertex(next_vertex_id);
			unpack_xz(v, temp.x, temp.y);
			box.min = box.max = temp;
			box.grow(identity);
			if (box.pick_exact(start, dir))
			{

				if (dest_xz == v->position( ).xz( ))
				{
					return	(is_accessible(next_vertex_id) ? next_vertex_id : u32(-1));
				}
				fVector2		temp;
				temp.add(box.min, box.max);
				temp.mul(.5f);
				f32			dist = _sqr(temp.x - dest.x) + _sqr(temp.y - dest.y);
				if (dist > cur_sqr)
					continue;

				if (!is_accessible(next_vertex_id))
					return		(u32(-1));

				cur_sqr = dist;
				found = true;
				prev_vertex_id = cur_vertex_id;
				cur_vertex_id = next_vertex_id;
				break;
			}
		}
		if (!found)
		{
			return			(u32(-1));
		}
	}
}

bool CLevelGraph::check_vertex_in_direction_slow(u32 start_vertex_id, const fVector2& start_position, u32 finish_vertex_id) const
{
	fVector3					finish_position = vertex_position(finish_vertex_id);
	u32						cur_vertex_id = start_vertex_id, prev_vertex_id = u32(-1);
	fBox2					box;
	fVector2				identity;
	fVector2				start;
	fVector2				dest;
	fVector2				dir;

	identity.x = identity.y = header( ).cell_size( ) * .5f;
	start = start_position;
	dest.set(finish_position.x, finish_position.z);
	dir.sub(dest, start);
	fVector2				temp;
	unpack_xz(vertex(start_vertex_id), temp.x, temp.y);

	f32					cur_sqr = _sqr(temp.x - dest.x) + _sqr(temp.y - dest.y);
	for (;;)
	{
		const_iterator		I, E;
		begin(cur_vertex_id, I, E);
		bool				found = false;
		for (; I != E; ++I)
		{
			u32				next_vertex_id = value(cur_vertex_id, I);
			if ((next_vertex_id == prev_vertex_id) || !valid_vertex_id(next_vertex_id))
				continue;
			unpack_xz(vertex(next_vertex_id), temp.x, temp.y);
			box.min = box.max = temp;
			box.grow(identity);
			if (box.pick_exact(start, dir))
			{
				if (next_vertex_id == finish_vertex_id)
				{
					return		(is_accessible(next_vertex_id));
				}
				fVector2		temp;
				temp.add(box.min, box.max);
				temp.mul(.5f);
				f32			dist = _sqr(temp.x - dest.x) + _sqr(temp.y - dest.y);
				if (dist > cur_sqr)
					continue;

				if (!is_accessible(next_vertex_id))
					return		(false);

				cur_sqr = dist;
				found = true;
				prev_vertex_id = cur_vertex_id;
				cur_vertex_id = next_vertex_id;
				break;
			}
		}
		if (!found)
		{
			return			(false);
		}
	}
}

IC  fVector3 v3d(const fVector2& vector2d)
{
	return			(fVector3( ).set(vector2d.x, 0.0f, vector2d.y));
}

IC  fVector2 v2d(const fVector3& vector3d)
{
	return			(fVector2( ).set(vector3d.x, vector3d.z));
}

bool CLevelGraph::create_straight_path(u32 start_vertex_id, const fVector2& start_point, const fVector2& finish_point, xr_vector<fVector3>& tpaOutputPoints, xr_vector<u32>& tpaOutputNodes, bool bAddFirstPoint, bool bClearPath) const
{
	if (!valid_vertex_position(v3d(finish_point)))
		return				(false);

	u32						cur_vertex_id = start_vertex_id, prev_vertex_id = start_vertex_id;
	fBox2					box;
	fVector2				identity;
	fVector2				start;
	fVector2				dest;
	fVector2				dir;

	identity.x = identity.y = header( ).cell_size( ) * .5f;
	start = start_point;
	dest = finish_point;
	dir.sub(dest, start);
	u32						dest_xz = vertex_position(v3d(dest)).xz( );
	fVector2				temp;
	fVector3					pos3d;
	unpack_xz(vertex(start_vertex_id), temp.x, temp.y);

	if (bClearPath)
	{
		tpaOutputPoints.clear( );
		tpaOutputNodes.clear( );
	}
	if (bAddFirstPoint)
	{
		pos3d = v3d(start_point);
		pos3d.y = vertex_plane_y(start_vertex_id, start_point.x, start_point.y);
		tpaOutputPoints.push_back(pos3d);
		tpaOutputNodes.push_back(start_vertex_id);
	}

	f32					cur_sqr = _sqr(temp.x - dest.x) + _sqr(temp.y - dest.y);
	for (;;)
	{
		const_iterator		I, E;
		begin(cur_vertex_id, I, E);
		bool				found = false;
		for (; I != E; ++I)
		{
			u32				next_vertex_id = value(cur_vertex_id, I);
			if ((next_vertex_id == prev_vertex_id) || !valid_vertex_id(next_vertex_id))
				continue;
			CVertex* v = vertex(next_vertex_id);
			unpack_xz(v, temp.x, temp.y);
			box.min = box.max = temp;
			box.grow(identity);
			if (box.pick_exact(start, dir))
			{
				fVector2		temp;
				temp.add(box.min, box.max);
				temp.mul(.5f);
				f32			dist = _sqr(temp.x - dest.x) + _sqr(temp.y - dest.y);
				if (dist > cur_sqr)
					continue;

				fVector2		next1;
				fVector2		next2;

#ifdef DEBUG
				next1 = next2 = fVector2( ).set(0.0f, 0.0f);
#endif

				fVector3			tIntersectPoint;

				switch (I)
				{
					case 0:
					{
						next1 = box.max;
						next2.set(box.max.x, box.min.y);
						break;
					}
					case 1:
					{
						next1 = box.min;
						next2.set(box.max.x, box.min.y);
						break;
					}
					case 2:
					{
						next1 = box.min;
						next2.set(box.min.x, box.max.y);
						break;
					}
					case 3:
					{
						next1 = box.max;
						next2.set(box.min.x, box.max.y);
						break;
					}
					default: NODEFAULT;
				}
				VERIFY(_valid(next1));
				VERIFY(_valid(next2));
				u32				dwIntersect = intersect(start_point.x, start_point.y, finish_point.x, finish_point.y, next1.x, next1.y, next2.x, next2.y, &tIntersectPoint.x, &tIntersectPoint.z);
				if (!dwIntersect)
					continue;
				tIntersectPoint.y = vertex_plane_y(vertex(cur_vertex_id), tIntersectPoint.x, tIntersectPoint.z);

				tpaOutputPoints.push_back(tIntersectPoint);
				tpaOutputNodes.push_back(cur_vertex_id);

				if (dest_xz == v->position( ).xz( ))
					return		(true);

				cur_sqr = dist;
				found = true;
				prev_vertex_id = cur_vertex_id;
				cur_vertex_id = next_vertex_id;
				break;
			}
		}
		if (!found)
			return				(false);
	}
}

f32 CLevelGraph::cover_in_direction(f32 fAngle, f32 b1, f32 b0, f32 b3, f32 b2) const
{
	f32 fResult;

	if (fAngle < PI_DIV_2)
		;
	else
		if (fAngle < PI)
		{
			fAngle -= PI_DIV_2;
			b0 = b1;
			b1 = b2;
		}
		else
			if (fAngle < 3 * PI_DIV_2)
			{
				fAngle -= PI;
				b0 = b2;
				b1 = b3;
			}
			else
			{
				fAngle -= 3 * PI_DIV_2;
				b1 = b0;
				b0 = b3;
			}
	fResult = (b1 - b0) * fAngle / PI_DIV_2 + b0;
	return(fResult);
}

bool CLevelGraph::neighbour_in_direction(const fVector3& direction, u32 start_vertex_id) const
{
	u32						cur_vertex_id = start_vertex_id, prev_vertex_id = u32(-1);
	fBox2					box;
	fVector2				identity;
	fVector2				start;
	fVector2				dest;
	fVector2				dir;

	identity.x = identity.y = header( ).cell_size( ) * .5f;
	start = v2d(vertex_position(start_vertex_id));
	dir = v2d(direction);
	dir.normalize_safe( );
	dest = dir;
	dest.mul(header( ).cell_size( ) * 4.f);
	dest.add(start);
	fVector2				temp;
	unpack_xz(vertex(start_vertex_id), temp.x, temp.y);

	f32					cur_sqr = _sqr(temp.x - dest.x) + _sqr(temp.y - dest.y);
	const_iterator			I, E;
	begin(cur_vertex_id, I, E);
	for (; I != E; ++I)
	{
		u32					next_vertex_id = value(cur_vertex_id, I);
		if ((next_vertex_id == prev_vertex_id) || !is_accessible(next_vertex_id))
			continue;
		unpack_xz(vertex(next_vertex_id), temp.x, temp.y);
		box.min = box.max = temp;
		box.grow(identity);
		if (box.pick_exact(start, dir))
		{
			fVector2		temp;
			temp.add(box.min, box.max);
			temp.mul(.5f);
			f32			dist = _sqr(temp.x - dest.x) + _sqr(temp.y - dest.y);
			if (dist > cur_sqr)
				continue;
			return			(true);
		}
	}
	return					(false);
}
