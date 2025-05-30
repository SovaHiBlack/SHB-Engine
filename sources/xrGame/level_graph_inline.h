////////////////////////////////////////////////////////////////////////////
//	Module 		: level_graph_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 11.11.2003
//	Author		: Oles Shihkovtsov, Dmitriy Iassenev
//	Description : Level graph inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CLevelGraph::const_vertex_iterator CLevelGraph::begin( ) const
{
	return				(m_nodes);
}

IC	CLevelGraph::const_vertex_iterator CLevelGraph::end( ) const
{
	return				(m_nodes + header( ).vertex_count( ));
}

IC const CLevelGraph::CHeader& CLevelGraph::header( ) const
{
	return				(*m_header);
}

ICF bool CLevelGraph::valid_vertex_id(u32 id) const
{
	bool				b = id < header( ).vertex_count( );
	return				(b);
}

ICF	CLevelGraph::CVertex* CLevelGraph::vertex(const u32 vertex_id) const
{
	VERIFY(valid_vertex_id(vertex_id));
	return				(m_nodes + vertex_id);
}

ICF	u32	CLevelGraph::vertex(const CVertex* vertex_p) const
{
	VERIFY((vertex_p >= m_nodes) && valid_vertex_id(u32(vertex_p - m_nodes)));
	return				(u32(vertex_p - m_nodes));
}

ICF	u32	CLevelGraph::vertex(const CVertex& vertex_r) const
{
	return				(vertex(&vertex_r));
}

IC	void CLevelGraph::unpack_xz(const CLevelGraph::CPosition& vertex_position, u32& x, u32& z) const
{
	VERIFY(vertex_position.xz( ) < (1 << MAX_NODE_BIT_COUNT) - 1);
	x = vertex_position.xz( ) / m_row_length;
	z = vertex_position.xz( ) % m_row_length;
}

IC	void CLevelGraph::unpack_xz(const CLevelGraph::CPosition& vertex_position, s32& x, s32& z) const
{
	u32					_x, _z;
	unpack_xz(vertex_position, _x, _z);
	x = (s32)_x;
	z = (s32)_z;
}

IC	void CLevelGraph::unpack_xz(const CLevelGraph::CPosition& vertex_position, f32& x, f32& z) const
{
	u32					_x, _z;
	unpack_xz(vertex_position, _x, _z);
	x = f32(_x) * header( ).cell_size( ) + header( ).box( ).min.x;
	z = f32(_z) * header( ).cell_size( ) + header( ).box( ).min.z;
}

template <typename T>
IC	void CLevelGraph::unpack_xz(const CLevelGraph::CVertex& vertex, T& x, T& z) const
{
	unpack_xz(vertex.position( ), x, z);
}

template <typename T>
IC	void CLevelGraph::unpack_xz(const CLevelGraph::CVertex* vertex, T& x, T& z) const
{
	unpack_xz(*vertex, x, z);
}

ICF	const fVector3 CLevelGraph::vertex_position(const CLevelGraph::CPosition& source_position) const
{
	fVector3				dest_position;
	unpack_xz(source_position, dest_position.x, dest_position.z);
	dest_position.y = (f32(source_position.y( )) / 65535) * header( ).factor_y( ) + header( ).box( ).min.y;
	return				(dest_position);
}

ICF	const fVector3& CLevelGraph::vertex_position(fVector3& dest_position, const CLevelGraph::CPosition& source_position) const
{
	return				(dest_position = vertex_position(source_position));
}

IC	const CLevelGraph::CPosition& CLevelGraph::vertex_position(CLevelGraph::CPosition& dest_position, const fVector3& source_position) const
{
	VERIFY(iFloor((source_position.z - header( ).box( ).min.z) / header( ).cell_size( ) + .5f) < (s32)m_row_length);
	s32					pxz = iFloor(((source_position.x - header( ).box( ).min.x) / header( ).cell_size( ) + .5f)) * m_row_length + iFloor((source_position.z - header( ).box( ).min.z) / header( ).cell_size( ) + .5f);
	s32					py = iFloor(65535.f * (source_position.y - header( ).box( ).min.y) / header( ).factor_y( ) + EPSILON_7);
	VERIFY(pxz < (1 << MAX_NODE_BIT_COUNT) - 1);
	dest_position.xz(u32(pxz));
	clamp(py, 0, 65535);
	dest_position.y(u16(py));
	return				(dest_position);
}

IC  const fVector3 CLevelGraph::vertex_position(u32 vertex_id) const
{
	fVector3				t = vertex_position(vertex(vertex_id));
	return				(t);
}

IC  const fVector3 CLevelGraph::vertex_position(const CLevelGraph::CVertex& vertex) const
{
	fVector3				position;
	vertex_position(position, vertex.position( ));
	return				(position);
}

IC  const fVector3 CLevelGraph::vertex_position(const CLevelGraph::CVertex* vertex) const
{
	return				(vertex_position(*vertex));
}

IC	const CLevelGraph::CPosition	CLevelGraph::vertex_position(const fVector3& position) const
{
	CLevelGraph::CPosition	_vertex_position;
	return				(vertex_position(_vertex_position, position));
}

IC bool	CLevelGraph::inside(const CLevelGraph::CVertex& vertex, const CLevelGraph::CPosition& vertex_position) const
{
	return 				(vertex_position.xz( ) == vertex.position( ).xz( ));
}

IC bool	CLevelGraph::inside(const CLevelGraph::CVertex& vertex, const fVector3& position) const
{
	if (!valid_vertex_position(position))
		return			(false);
	return				(inside(vertex, vertex_position(position)));
}

IC bool	CLevelGraph::inside(const CVertex* vertex, const CLevelGraph::CPosition& vertex_position) const
{
	return				(inside(*vertex, vertex_position));
}

IC bool CLevelGraph::inside(const CVertex* vertex, const fVector3& position) const
{
	return				(inside(*vertex, position));
}

IC bool	CLevelGraph::inside(const u32 vertex_id, const CLevelGraph::CPosition& vertex_position) const
{
	return				(inside(vertex(vertex_id), vertex_position));
}

IC bool CLevelGraph::inside(const u32 vertex_id, const fVector3& position) const
{
	bool				b = inside(vertex(vertex_id), position);
	return				(b);
}

IC bool	CLevelGraph::inside(const CLevelGraph::CVertex& vertex, const CLevelGraph::CPosition& _vertex_position, const f32 epsilon) const
{
	return				(inside(vertex, _vertex_position) && (_abs(vertex_position(vertex).y - vertex_position(_vertex_position).y) <= epsilon));
}

IC bool	CLevelGraph::inside(const CLevelGraph::CVertex& vertex, const fVector3& position, const f32 epsilon) const
{
	if (!valid_vertex_position(position))
		return			(false);
	return				(inside(vertex, vertex_position(position), epsilon));
}

IC bool	CLevelGraph::inside(const CVertex* vertex, const CLevelGraph::CPosition& vertex_position, const f32 epsilon) const
{
	return				(inside(*vertex, vertex_position, epsilon));
}

IC bool CLevelGraph::inside(const CVertex* vertex, const fVector3& position, const f32 epsilon) const
{
	return				(inside(*vertex, position, epsilon));
}

IC bool	CLevelGraph::inside(const u32 vertex_id, const CLevelGraph::CPosition& vertex_position, const f32 epsilon) const
{
	return				(inside(vertex(vertex_id), vertex_position, epsilon));
}

IC bool CLevelGraph::inside(const u32 vertex_id, const fVector3& position, const f32 epsilon) const
{
	return				(inside(vertex(vertex_id), position, epsilon));
}

IC bool	CLevelGraph::inside(const u32 vertex_id, const fVector2& position) const
{
	s32					pxz = iFloor(((position.x - header( ).box( ).min.x) / header( ).cell_size( ) + .5f)) * m_row_length + iFloor((position.y - header( ).box( ).min.z) / header( ).cell_size( ) + .5f);
	VERIFY(pxz < (1 << MAX_NODE_BIT_COUNT) - 1);
	bool				b = vertex(vertex_id)->position( ).xz( ) == u32(pxz);
	return				(b);
}

IC f32 CLevelGraph::vertex_plane_y(const CLevelGraph::CVertex& vertex, const f32 X, const f32 Z) const
{
	fVector3				DUP;
	fVector3 normal;
	fVector3 v;
	fVector3 v1;
	fVector3 P;
	fPlane3				PL;

	DUP.set(0, 1, 0);
	pvDecompress(normal, vertex.plane( ));
	vertex_position(P, vertex.position( ));
	PL.build(P, normal);
	v.set(X, P.y, Z);
	PL.intersectRayPoint(v, DUP, v1);
	return				(v1.y);
}

IC f32 CLevelGraph::vertex_plane_y(const CLevelGraph::CVertex& vertex) const
{
	f32				x, z;
	unpack_xz(vertex, x, z);
	return				(vertex_plane_y(vertex, x, z));
}

IC f32 CLevelGraph::vertex_plane_y(const CLevelGraph::CVertex* vertex, const f32 X, const f32 Z) const
{
	return				(vertex_plane_y(*vertex, X, Z));
}

IC f32 CLevelGraph::vertex_plane_y(const CLevelGraph::CVertex* vertex) const
{
	return				(vertex_plane_y(*vertex));
}

IC f32 CLevelGraph::vertex_plane_y(const u32 vertex_id, const f32 X, const f32 Z) const
{
	return				(vertex_plane_y(vertex(vertex_id), X, Z));
}

IC f32 CLevelGraph::vertex_plane_y(const u32 vertex_id) const
{
	return				(vertex_plane_y(vertex(vertex_id)));
}

ICF u32	CLevelGraph::CHeader::version( ) const
{
	return				(hdrNODES::version);
}

ICF u32	CLevelGraph::CHeader::vertex_count( ) const
{
	return				(count);
}

ICF f32	CLevelGraph::CHeader::cell_size( ) const
{
	return				(size);
}

ICF f32	CLevelGraph::CHeader::factor_y( ) const
{
	return				(size_y);
}

ICF const fBox3& CLevelGraph::CHeader::box( ) const
{
	return				(aabb);
}

ICF const xrGUID& CLevelGraph::CHeader::guid( ) const
{
	return				(hdrNODES::guid);
}

ICF u8	CLevelGraph::CVertex::light( ) const
{
	return				(NodeCompressed::light( ));
}

ICF u32	CLevelGraph::CVertex::link(s32 index) const
{
	return				(NodeCompressed::link(u8(index)));
}

ICF u16	CLevelGraph::CVertex::cover(u8 index) const
{
	return				(NodeCompressed::cover(index));
}

ICF u16	CLevelGraph::CVertex::plane( ) const
{
	return				(NodeCompressed::plane);
}

ICF const CLevelGraph::CPosition& CLevelGraph::CVertex::position( ) const
{
	return				(p);
}

ICF bool CLevelGraph::CVertex::operator<	(const CLevelGraph::CVertex& vertex) const
{
	return				(position( ).xz( ) < vertex.position( ).xz( ));
}

ICF bool CLevelGraph::CVertex::operator>	(const CLevelGraph::CVertex& vertex) const
{
	return				(position( ).xz( ) > vertex.position( ).xz( ));
}

ICF bool CLevelGraph::CVertex::operator==	(const CLevelGraph::CVertex& vertex) const
{
	return				(position( ).xz( ) == vertex.position( ).xz( ));
}

IC	const GameGraph::_LEVEL_ID& CLevelGraph::level_id( ) const
{
	return				(m_level_id);
}

IC	void CLevelGraph::level_id(const GameGraph::_LEVEL_ID& level_id)
{
	m_level_id = level_id;
}

IC	void CLevelGraph::begin(const CVertex& vertex, const_iterator& begin, const_iterator& end) const
{
	begin = 0;
	end = 4;
}

IC	void CLevelGraph::begin(const CVertex* _vertex, const_iterator& _begin, const_iterator& end) const
{
	begin(*_vertex, _begin, end);
}

IC	void CLevelGraph::begin(const u32 vertex_id, const_iterator& _begin, const_iterator& end) const
{
	begin(vertex(vertex_id), _begin, end);
}

IC	u32	CLevelGraph::value(const CVertex& vertex, const_iterator& i) const
{
	return				(vertex.link(i));
}

IC	u32	CLevelGraph::value(const CVertex* vertex, const_iterator& i) const
{
	return				(value(*vertex, i));
}

IC	u32	CLevelGraph::value(const u32 vertex_id, const_iterator& i) const
{
	return				(value(vertex(vertex_id), i));
}

IC	bool CLevelGraph::is_accessible(const u32 vertex_id) const
{
	return				(valid_vertex_id(vertex_id) && m_access_mask[vertex_id]);
}

IC	void CLevelGraph::set_invalid_vertex(u32& vertex_id, CVertex** vertex) const
{
	vertex_id = u32(-1);
	VERIFY(!valid_vertex_id(vertex_id));
	if (vertex)
		*vertex = NULL;
}

IC	const u32 CLevelGraph::vertex_id(const CLevelGraph::CVertex* vertex) const
{
	VERIFY(valid_vertex_id(u32(vertex - m_nodes)));
	return				(u32(vertex - m_nodes));
}

IC  fVector3 CLevelGraph::v3d(const fVector2& vector2d) const
{
	return				(fVector3( ).set(vector2d.x, 0.0f, vector2d.y));
}

IC  fVector2 CLevelGraph::v2d(const fVector3& vector3d) const
{
	return				(fVector2( ).set(vector3d.x, vector3d.z));
}

template <bool bAssignY, typename T>
IC	bool	CLevelGraph::create_straight_path(u32 start_vertex_id, const fVector2& start_point, const fVector2& finish_point, xr_vector<T>& tpaOutputPoints, const T& example, bool bAddFirstPoint, bool bClearPath) const
{
	if (!valid_vertex_position(v3d(finish_point)))
		return				(false);

	u32						cur_vertex_id = start_vertex_id, prev_vertex_id = start_vertex_id;
	fBox2					box;
	fVector2				identity;
	fVector2				start;
	fVector2				dest;
	fVector2				dir;
	T						path_node = example;

	identity.x = identity.y = header( ).cell_size( ) / 2.f;
	start = start_point;
	dest = finish_point;
	dir.sub(dest, start);
	u32						dest_xz = vertex_position(v3d(dest)).xz( );
	fVector2				temp;
	fVector3					pos3d;
	unpack_xz(vertex(start_vertex_id), temp.x, temp.y);

	if (bClearPath)
		tpaOutputPoints.clear( );

	if (bAddFirstPoint)
	{
		pos3d = v3d(start_point);
		if (bAssignY)
			pos3d.y = vertex_plane_y(start_vertex_id, start_point.x, start_point.y);
		path_node.set_position(pos3d);
		path_node.set_vertex_id(start_vertex_id);
		tpaOutputPoints.push_back(path_node);
	}

	if (vertex(start_vertex_id)->position( ).xz( ) == dest_xz)
	{
		fVector3						tIntersectPoint = v3d(dest);
		if (bAssignY)
			tIntersectPoint.y = vertex_plane_y(vertex(cur_vertex_id), tIntersectPoint.x, tIntersectPoint.z);
		path_node.set_position(tIntersectPoint);
		path_node.set_vertex_id(start_vertex_id);
		tpaOutputPoints.push_back(path_node);
		return						(true);
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
				if ((dist > cur_sqr) && (dest_xz != v->position( ).xz( )))
					continue;

				if (!is_accessible(next_vertex_id))
					return		(false);

				cur_sqr = dist;

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

#ifdef DEBUG
				VERIFY(_valid(next1));
				VERIFY(_valid(next2));
				u32				dwIntersect =
#endif

					intersect_no_check(start_point.x, start_point.y, finish_point.x, finish_point.y, next1.x, next1.y, next2.x, next2.y, &tIntersectPoint.x, &tIntersectPoint.z);

#ifdef DEBUG
				VERIFY(dwIntersect);
				VERIFY(_valid(tIntersectPoint.x));
				VERIFY(_valid(tIntersectPoint.z));
#endif

				clamp(tIntersectPoint.x, _min(next1.x, next2.x), _max(next1.x, next2.x));
				clamp(tIntersectPoint.z, _min(next1.y, next2.y), _max(next1.y, next2.y));
				if (bAssignY)
				{
					tIntersectPoint.y = vertex_plane_y(vertex(cur_vertex_id), tIntersectPoint.x, tIntersectPoint.z);
				}

				path_node.set_position(tIntersectPoint);
				path_node.set_vertex_id(next_vertex_id);
				tpaOutputPoints.push_back(path_node);

				if (dest_xz == v->position( ).xz( )/**box.contains(dest)/**/)
				{
					tIntersectPoint = v3d(dest);
					if (bAssignY)
					{
						tIntersectPoint.y = vertex_plane_y(vertex(cur_vertex_id), tIntersectPoint.x, tIntersectPoint.z);
					}

					path_node.set_position(tIntersectPoint);
					path_node.set_vertex_id(next_vertex_id);
					tpaOutputPoints.push_back(path_node);
					return		(true);
				}
				found = true;
				prev_vertex_id = cur_vertex_id;
				cur_vertex_id = next_vertex_id;

#ifdef DEBUG
				if (tpaOutputPoints.size( ) > 100000)
				{
					Msg(
						"CLevelGraph::create_straight_path : Loop became infinite (%d,[%f][%f][%f],[%f][%f][%f])",
						start_vertex_id,
						VPUSH(v3d(start_point)),
						VPUSH(v3d(finish_point))
					);
					FlushLog( );
					R_ASSERT2(false, "Loop became infinite :-( call Dima and SAVE YOUR LOG!");
				}
#endif

				break;
			}
		}

		if (!found)
		{
			return			(false);
		}
	}
}

template<typename T>
IC	void CLevelGraph::assign_y_values(xr_vector<T>& path)
{
	fVector3						DUP = {0.0f,1.0f,0.0f};
	fVector3 normal;
	fVector3 v1;
	fVector3 P = {0.0f,0.0f,0.0f};
	fPlane3						PL;
	const CVertex* _vertex;
	u32							prev_id = u32(-1);

	xr_vector<T>::iterator		I = path.begin( );
	xr_vector<T>::iterator		E = path.end( );
	for (; I != E; ++I)
	{
		if (prev_id != (*I).get_vertex_id( ))
		{
			_vertex = vertex((*I).get_vertex_id( ));
			pvDecompress(normal, _vertex->plane( ));
			vertex_position(P, _vertex->position( ));
			PL.build(P, normal);
			prev_id = (*I).get_vertex_id( );
		}
		(*I).get_position( ).y = P.y;
		PL.intersectRayPoint((*I).get_position( ), DUP, v1);
		(*I).get_position( ).y = v1.y;
	}
}

IC	u32	CLevelGraph::row_length( ) const
{
	return						(m_row_length);
}

IC	bool CLevelGraph::valid_vertex_position(const fVector3& position) const
{
	if ((position.x < header( ).box( ).min.x - header( ).cell_size( ) * .5f) || (position.x > header( ).box( ).max.x + header( ).cell_size( ) * .5f) || (position.z < header( ).box( ).min.z - header( ).cell_size( ) * .5f) || (position.z > header( ).box( ).max.z + header( ).cell_size( ) * .5f))
		return			(false);

	if (!(iFloor((position.z - header( ).box( ).min.z) / header( ).cell_size( ) + .5f) < (s32)m_row_length))
		return			(false);

	if (!(iFloor((position.x - header( ).box( ).min.x) / header( ).cell_size( ) + .5f) < (s32)m_column_length))
		return			(false);

	return				((vertex_position(position).xz( ) < (1 << MAX_NODE_BIT_COUNT) - 1));
}

IC	void CLevelGraph::set_mask(const xr_vector<u32>& mask)
{
	xr_vector<u32>::const_iterator	I = mask.begin( );
	xr_vector<u32>::const_iterator	E = mask.end( );
	for (; I != E; ++I)
		set_mask(*I);
}

IC	void CLevelGraph::clear_mask(const xr_vector<u32>& mask)
{
	xr_vector<u32>::const_iterator	I = mask.begin( );
	xr_vector<u32>::const_iterator	E = mask.end( );
	for (; I != E; ++I)
		clear_mask(*I);
}

IC	void CLevelGraph::set_mask(u32 vertex_id)
{
	VERIFY(m_access_mask[vertex_id]);
	m_access_mask[vertex_id] = false;
}

IC	void CLevelGraph::clear_mask(u32 vertex_id)
{
	VERIFY(!m_access_mask[vertex_id]);
	m_access_mask[vertex_id] = true;
}

template<typename P>
IC	void CLevelGraph::iterate_vertices(const fVector3& min_position, const fVector3& max_position, const P& predicate) const
{
	CVertex* I, * E;

	if (valid_vertex_position(min_position))
		I = std::lower_bound(m_nodes, m_nodes + header( ).vertex_count( ), vertex_position(min_position).xz( ), &vertex::predicate2);
	else
		I = m_nodes;

	if (valid_vertex_position(max_position))
	{
		E = std::upper_bound(m_nodes, m_nodes + header( ).vertex_count( ), vertex_position(max_position).xz( ), &vertex::predicate);
		if (E != (m_nodes + header( ).vertex_count( )))
			++E;
	}
	else
		E = m_nodes + header( ).vertex_count( );

	for (; I != E; ++I)
		predicate(*I);
}

IC	u32	CLevelGraph::max_x( ) const
{
	return						(m_max_x);
}

IC	u32	CLevelGraph::max_z( ) const
{
	return						(m_max_z);
}

IC	bool operator<		(const CLevelGraph::CVertex& vertex, const u32& vertex_xz)
{
	return						(vertex.position( ).xz( ) < vertex_xz);
}

IC	bool operator>		(const CLevelGraph::CVertex& vertex, const u32& vertex_xz)
{
	return						(vertex.position( ).xz( ) > vertex_xz);
}

IC	bool operator==		(const CLevelGraph::CVertex& vertex, const u32& vertex_xz)
{
	return						(vertex.position( ).xz( ) == vertex_xz);
}

IC	bool operator<		(const u32& vertex_xz, const CLevelGraph::CVertex& vertex)
{
	return						(vertex_xz < vertex.position( ).xz( ));
}

IC	bool operator>		(const u32& vertex_xz, const CLevelGraph::CVertex& vertex)
{
	return						(vertex_xz > vertex.position( ).xz( ));
}

IC	bool operator==		(const u32& vertex_xz, const CLevelGraph::CVertex& vertex)
{
	return						(vertex_xz == vertex.position( ).xz( ));
}
