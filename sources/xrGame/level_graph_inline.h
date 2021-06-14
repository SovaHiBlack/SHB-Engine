////////////////////////////////////////////////////////////////////////////
//	Module 		: level_graph_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 11.11.2003
//	Author		: Oles Shihkovtsov, Dmitriy Iassenev
//	Description : Level graph inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CLevelGraph::const_vertex_iterator CLevelGraph::begin	() const
{
	return				(m_nodes);
}

inline	CLevelGraph::const_vertex_iterator CLevelGraph::end		() const
{
	return				(m_nodes + header().vertex_count());
}

inline const CLevelGraph::CHeader &CLevelGraph::header	() const
{
	return				(*m_header);
}

__forceinline bool CLevelGraph::valid_vertex_id	(u32 id) const
{
	bool				b = id < header().vertex_count();
	return				(b);
}

__forceinline	CLevelGraph::CVertex	*CLevelGraph::vertex(const u32 vertex_id) const
{
	VERIFY				(valid_vertex_id(vertex_id));
	return				(m_nodes + vertex_id);
}

__forceinline	u32	CLevelGraph::vertex	(const CVertex *vertex_p) const
{
	VERIFY				((vertex_p >= m_nodes) && valid_vertex_id(u32(vertex_p - m_nodes)));
	return				(u32(vertex_p - m_nodes));
}

__forceinline	u32	CLevelGraph::vertex	(const CVertex &vertex_r) const
{
	return				(vertex(&vertex_r));
}

inline	void CLevelGraph::unpack_xz(const CLevelGraph::CPosition &vertex_position, u32 &x, u32 &z) const
{
	VERIFY				(vertex_position.xz() < (1 << MAX_NODE_BIT_COUNT) - 1);
	x					= vertex_position.xz() / m_row_length;
	z					= vertex_position.xz() % m_row_length;
}

inline	void CLevelGraph::unpack_xz(const CLevelGraph::CPosition &vertex_position, int &x, int &z) const
{
	u32					_x, _z;
	unpack_xz			(vertex_position,_x,_z);
	x					= (int)_x;
	z					= (int)_z;
}

inline	void CLevelGraph::unpack_xz(const CLevelGraph::CPosition &vertex_position, float &x, float &z) const
{
	u32					_x,_z;
	unpack_xz			(vertex_position,_x,_z);
	x					= float(_x)*header().cell_size() + header().box().min.x;
	z 					= float(_z)*header().cell_size() + header().box().min.z;
}

template <typename T>
inline	void CLevelGraph::unpack_xz(const CLevelGraph::CVertex &vertex, T &x, T &z) const
{
	unpack_xz			(vertex.position(),x,z);
}

template <typename T>
inline	void CLevelGraph::unpack_xz(const CLevelGraph::CVertex *vertex, T &x, T &z) const
{
	unpack_xz			(*vertex,x,z);
}

__forceinline	const Fvector3 CLevelGraph::vertex_position	(const CLevelGraph::CPosition &source_position) const
{
	Fvector3				dest_position;
	unpack_xz			(source_position,dest_position.x,dest_position.z);
	dest_position.y 	= (float(source_position.y())/65535)*header().factor_y() + header().box().min.y;
	return				(dest_position);
}

__forceinline	const Fvector3& CLevelGraph::vertex_position	(Fvector3& dest_position, const CLevelGraph::CPosition &source_position) const
{
	return				(dest_position = vertex_position(source_position));
}

inline	const CLevelGraph::CPosition &CLevelGraph::vertex_position	(CLevelGraph::CPosition &dest_position, const Fvector3& source_position) const
{
	VERIFY				(iFloor((source_position.z - header().box().min.z)/header().cell_size() + .5f) < (int)m_row_length);
	int					pxz	= iFloor(((source_position.x - header().box().min.x)/header().cell_size() + .5f))*m_row_length + iFloor((source_position.z - header().box().min.z)/header().cell_size() + .5f);
	int					py	= iFloor(65535.f*(source_position.y - header().box().min.y)/header().factor_y() + EPS_S);
	VERIFY				(pxz < (1 << MAX_NODE_BIT_COUNT) - 1);
	dest_position.xz	(u32(pxz));
	clamp				(py,0,65535);
	dest_position.y		(U16(py));
	return				(dest_position);
}

inline const Fvector3 CLevelGraph::vertex_position(u32 vertex_id) const
{
	Fvector3				t = vertex_position(vertex(vertex_id));
	return				(t);
}

inline const Fvector3 CLevelGraph::vertex_position(const CLevelGraph::CVertex &vertex) const
{
	Fvector3				position;
	vertex_position		(position, vertex.position());
	return				(position);
}

inline const Fvector3 CLevelGraph::vertex_position(const CLevelGraph::CVertex *vertex) const
{
	return				(vertex_position(*vertex));
}

inline	const CLevelGraph::CPosition	CLevelGraph::vertex_position	(const Fvector3& position) const
{
	CLevelGraph::CPosition	_vertex_position;
	return				(vertex_position(_vertex_position,position));
}

inline bool	CLevelGraph::inside				(const CLevelGraph::CVertex &vertex, const CLevelGraph::CPosition &vertex_position) const
{
	return 				(vertex_position.xz() == vertex.position().xz());
}

inline bool	CLevelGraph::inside				(const CLevelGraph::CVertex &vertex, const Fvector3& position) const
{
	if (!valid_vertex_position(position))
		return			(false);
	return				(inside(vertex,vertex_position(position)));
}

inline bool	CLevelGraph::inside				(const CVertex *vertex, const CLevelGraph::CPosition &vertex_position) const
{
	return				(inside(*vertex,vertex_position));
}

inline bool CLevelGraph::inside				(const CVertex *vertex, const Fvector3& position) const
{
	return				(inside(*vertex,position));
}

inline bool	CLevelGraph::inside				(const u32 vertex_id, const CLevelGraph::CPosition &vertex_position) const
{
	return				(inside(vertex(vertex_id),vertex_position));
}

inline bool CLevelGraph::inside				(const u32 vertex_id, const Fvector3& position) const
{
	bool				b = inside(vertex(vertex_id),position);
	return				(b);
}

inline bool	CLevelGraph::inside				(const CLevelGraph::CVertex &vertex, const CLevelGraph::CPosition &_vertex_position, const float epsilon) const
{
	return				(inside(vertex,_vertex_position) && (_abs(vertex_position(vertex).y - vertex_position(_vertex_position).y) <= epsilon));
}

inline bool	CLevelGraph::inside				(const CLevelGraph::CVertex &vertex, const Fvector3& position, const float epsilon) const
{
	if (!valid_vertex_position(position))
		return			(false);
	return				(inside(vertex,vertex_position(position),epsilon));
}

inline bool	CLevelGraph::inside				(const CVertex *vertex, const CLevelGraph::CPosition &vertex_position, const float epsilon) const
{
	return				(inside(*vertex,vertex_position,epsilon));
}

inline bool CLevelGraph::inside				(const CVertex *vertex, const Fvector3& position, const float epsilon) const
{
	return				(inside(*vertex,position,epsilon));
}

inline bool	CLevelGraph::inside				(const u32 vertex_id, const CLevelGraph::CPosition &vertex_position, const float epsilon) const
{
	return				(inside(vertex(vertex_id),vertex_position,epsilon));
}

inline bool CLevelGraph::inside				(const u32 vertex_id, const Fvector3& position, const float epsilon) const
{
	return				(inside(vertex(vertex_id),position,epsilon));
}

inline bool	CLevelGraph::inside				(const u32 vertex_id,	const Fvector2 &position) const
{
	int					pxz	= iFloor(((position.x - header().box().min.x)/header().cell_size() + .5f))*m_row_length + iFloor((position.y - header().box().min.z)/header().cell_size() + .5f);
	VERIFY				(pxz < (1 << MAX_NODE_BIT_COUNT) - 1);
	bool				b = vertex(vertex_id)->position().xz() == u32(pxz);
	return				(b);
}

inline float CLevelGraph::vertex_plane_y	(const CLevelGraph::CVertex &vertex, const float X, const float Z) const
{
	Fvector3				DUP;
	Fvector3 normal;
	Fvector3 v;
	Fvector3 v1;
	Fvector3 P;
	Fplane				PL; 

	DUP.set				(0,1,0);
	pvDecompress		(normal,vertex.plane());
	vertex_position		(P,vertex.position());
	PL.build			(P,normal);
	v.set				(X,P.y,Z);	
	PL.intersectRayPoint(v,DUP,v1);	
	return				(v1.y);
}

inline float CLevelGraph::vertex_plane_y	(const CLevelGraph::CVertex &vertex) const
{
	float				x,z;
	unpack_xz			(vertex,x,z);
	return				(vertex_plane_y(vertex,x,z));
}

inline float CLevelGraph::vertex_plane_y	(const CLevelGraph::CVertex *vertex, const float X, const float Z) const
{
	return				(vertex_plane_y(*vertex,X,Z));
}

inline float CLevelGraph::vertex_plane_y	(const CLevelGraph::CVertex *vertex) const
{
	return				(vertex_plane_y(*vertex));
}

inline float CLevelGraph::vertex_plane_y	(const u32 vertex_id, const float X, const float Z) const
{
	return				(vertex_plane_y(vertex(vertex_id),X,Z));
}

inline float CLevelGraph::vertex_plane_y	(const u32 vertex_id) const
{
	return				(vertex_plane_y(vertex(vertex_id)));
}

__forceinline u32	CLevelGraph::CHeader::version() const
{
	return				(hdrNODES::version);
}

__forceinline u32	CLevelGraph::CHeader::vertex_count() const
{
	return				(count);
}

__forceinline float	CLevelGraph::CHeader::cell_size() const
{
	return				(size);
}

__forceinline float	CLevelGraph::CHeader::factor_y() const
{
	return				(size_y);
}

__forceinline const Fbox3& CLevelGraph::CHeader::box() const
{
	return				(aabb);
}

__forceinline const xrGUID &CLevelGraph::CHeader::guid() const
{
	return				(hdrNODES::guid);
}

__forceinline U8	CLevelGraph::CVertex::light() const
{
	return				(NodeCompressed::light());
}

__forceinline u32	CLevelGraph::CVertex::link(int index) const
{
	return				(NodeCompressed::link(U8(index)));
}

__forceinline U16	CLevelGraph::CVertex::cover(U8 index) const
{
	return				(NodeCompressed::cover(index));
}

__forceinline U16	CLevelGraph::CVertex::plane() const
{
	return				(NodeCompressed::plane);
}

__forceinline const CLevelGraph::CPosition &CLevelGraph::CVertex::position() const
{
	return				(p);
}

__forceinline bool CLevelGraph::CVertex::operator<	(const CLevelGraph::CVertex &vertex) const
{
	return				(position().xz() < vertex.position().xz());
}

__forceinline bool CLevelGraph::CVertex::operator>	(const CLevelGraph::CVertex &vertex) const
{
	return				(position().xz() > vertex.position().xz());
}

__forceinline bool CLevelGraph::CVertex::operator==	(const CLevelGraph::CVertex &vertex) const
{
	return				(position().xz() == vertex.position().xz());
}

inline	const GameGraph::_LEVEL_ID &CLevelGraph::level_id() const
{
	return				(m_level_id);
}

inline	void CLevelGraph::level_id(const GameGraph::_LEVEL_ID &level_id)
{
	m_level_id			= level_id;
}

inline	void CLevelGraph::begin(const CVertex &vertex, const_iterator &begin, const_iterator &end) const
{
	begin				= 0;
	end					= 4;
}

inline	void CLevelGraph::begin(const CVertex *_vertex, const_iterator &_begin, const_iterator &end) const
{
	begin				(*_vertex,_begin,end);
}

inline	void CLevelGraph::begin(const u32 vertex_id, const_iterator &_begin, const_iterator &end) const
{
	begin				(vertex(vertex_id),_begin,end);
}

inline	u32	CLevelGraph::value(const CVertex &vertex, const_iterator &i) const
{
	return				(vertex.link(i));
}

inline	u32	CLevelGraph::value(const CVertex *vertex, const_iterator &i) const
{
	return				(value(*vertex,i));
}

inline	u32	CLevelGraph::value(const u32 vertex_id, const_iterator &i) const
{
	return				(value(vertex(vertex_id),i));
}

inline	bool CLevelGraph::is_accessible		(const u32 vertex_id) const
{
	return				(valid_vertex_id(vertex_id) && m_access_mask[vertex_id]);
}

inline	void CLevelGraph::set_invalid_vertex(u32 &vertex_id, CVertex **vertex) const
{
	vertex_id			= u32(-1);
	VERIFY				(!valid_vertex_id(vertex_id));
	if (vertex)
		*vertex			= NULL;
}

inline	const u32 CLevelGraph::vertex_id(const CLevelGraph::CVertex *vertex) const
{
	VERIFY				(valid_vertex_id(u32(vertex - m_nodes)));
	return				(u32(vertex - m_nodes));
}

inline Fvector3 CLevelGraph::v3d(const Fvector2 &vector2d) const
{
	return				(Fvector3().set(vector2d.x,0.f,vector2d.y));
}

inline  Fvector2 CLevelGraph::v2d(const Fvector3& vector3d) const
{
	return				(Fvector2().set(vector3d.x,vector3d.z));
}

template <bool bAssignY, typename T>
inline	bool	CLevelGraph::create_straight_path	(u32 start_vertex_id, const Fvector2 &start_point, const Fvector2 &finish_point, xr_vector<T> &tpaOutputPoints, const T &example, bool bAddFirstPoint, bool bClearPath) const
{
	if (!valid_vertex_position(v3d(finish_point)))
		return				(false);

	u32						cur_vertex_id = start_vertex_id, prev_vertex_id = start_vertex_id;
	Fbox2					box;
	Fvector2				identity, start, dest, dir;
	T						path_node = example;

	identity.x = identity.y	= header().cell_size()/2.f;
	start					= start_point;
	dest					= finish_point;
	dir.sub					(dest,start);
	u32						dest_xz = vertex_position(v3d(dest)).xz();
	Fvector2				temp;
	Fvector3					pos3d;
	unpack_xz				(vertex(start_vertex_id),temp.x,temp.y);

	if (bClearPath)
		tpaOutputPoints.clear	();
	
	if (bAddFirstPoint) {
		pos3d				= v3d(start_point);
		if (bAssignY)
			pos3d.y			= vertex_plane_y(start_vertex_id,start_point.x,start_point.y);
		path_node.set_position(pos3d);
		path_node.set_vertex_id(start_vertex_id);
		tpaOutputPoints.push_back(path_node);
	}

	if (vertex(start_vertex_id)->position().xz() == dest_xz) {
		Fvector3						tIntersectPoint	= v3d(dest);
		if (bAssignY)
			tIntersectPoint.y		= vertex_plane_y(vertex(cur_vertex_id),tIntersectPoint.x,tIntersectPoint.z);
		path_node.set_position		(tIntersectPoint);
		path_node.set_vertex_id		(start_vertex_id);
		tpaOutputPoints.push_back	(path_node);
		return						(true);
	}


	float					cur_sqr = _sqr(temp.x - dest.x) + _sqr(temp.y - dest.y);
	for (;;) {
		const_iterator		I,E;
		begin				(cur_vertex_id,I,E);
		bool				found = false;
		for ( ; I != E; ++I) {
			u32				next_vertex_id = value(cur_vertex_id,I);
			if ((next_vertex_id == prev_vertex_id) || !valid_vertex_id(next_vertex_id))
				continue;
			CVertex			*v = vertex(next_vertex_id);
			unpack_xz		(v,temp.x,temp.y);
			box.min			= box.max = temp;
			box.grow		(identity);
			if (box.pick_exact(start,dir)) {
				Fvector2		temp;
				temp.add		(box.min,box.max);
				temp.mul		(.5f);
				float			dist = _sqr(temp.x - dest.x) + _sqr(temp.y - dest.y);
				if ((dist > cur_sqr) && (dest_xz != v->position().xz()))
					continue;

				if (!is_accessible(next_vertex_id))
					return		(false);

				cur_sqr			= dist;

				Fvector2		next1, next2;
#ifdef DEBUG
				next1			= next2 = Fvector2().set(0.f,0.f);
#endif
				Fvector3			tIntersectPoint;

				switch (I) {
					case 0 : {
						next1		= box.max;
						next2.set	(box.max.x,box.min.y);
						break;
					}
					case 1 : {
						next1		= box.min;
						next2.set	(box.max.x,box.min.y);
						break;
					}
					case 2 : {
						next1		= box.min;
						next2.set	(box.min.x,box.max.y);
						break;
					}
					case 3 : {
						next1		= box.max;
						next2.set	(box.min.x,box.max.y);
						break;
					}
					default : NODEFAULT;
				}
#ifdef DEBUG
				VERIFY			(_valid(next1));
				VERIFY			(_valid(next2));
				u32				dwIntersect = 
#endif
					intersect_no_check(start_point.x,start_point.y,finish_point.x,finish_point.y,next1.x,next1.y,next2.x,next2.y,&tIntersectPoint.x,&tIntersectPoint.z);
#ifdef DEBUG
				VERIFY			(dwIntersect);
				VERIFY			(_valid(tIntersectPoint.x));
				VERIFY			(_valid(tIntersectPoint.z));
#endif

				clamp			(tIntersectPoint.x,_min(next1.x,next2.x),_max(next1.x,next2.x));
				clamp			(tIntersectPoint.z,_min(next1.y,next2.y),_max(next1.y,next2.y));
				if (bAssignY)
					tIntersectPoint.y = vertex_plane_y(vertex(cur_vertex_id),tIntersectPoint.x,tIntersectPoint.z);
				path_node.set_position(tIntersectPoint);
				path_node.set_vertex_id(next_vertex_id);
				tpaOutputPoints.push_back(path_node);

				if (dest_xz == v->position().xz()/**box.contains(dest)/**/) {
					tIntersectPoint = v3d(dest);
					if (bAssignY)
						tIntersectPoint.y = vertex_plane_y(vertex(cur_vertex_id),tIntersectPoint.x,tIntersectPoint.z);
					path_node.set_position(tIntersectPoint);
					path_node.set_vertex_id(next_vertex_id);
					tpaOutputPoints.push_back(path_node);
					return		(true);
				}
				found			= true;
				prev_vertex_id	= cur_vertex_id;
				cur_vertex_id	= next_vertex_id;
#ifdef DEBUG
				if (tpaOutputPoints.size() > 100000) {
					Msg			(
						"CLevelGraph::create_straight_path : Loop became infinite (%d,[%f][%f][%f],[%f][%f][%f])",
						start_vertex_id,
						VPUSH(v3d(start_point)),
						VPUSH(v3d(finish_point))
					);
					FlushLog	();
					R_ASSERT2	(false,"Loop became infinite :-( call Dima and SAVE YOUR LOG!");
				}
#endif
				break;
			}
		}
		if (!found)
			return			(false);
	}
}

template<typename T>
inline	void CLevelGraph::assign_y_values		(xr_vector<T> &path)
{
	Fvector3						DUP = { 0,1,0 };
	Fvector3 normal;
	Fvector3 v1;
	Fvector3 P = { 0,0,0 };
	Fplane						PL; 
	const CVertex				*_vertex;
	u32							prev_id = u32(-1);

	xr_vector<T>::iterator		I = path.begin();
	xr_vector<T>::iterator		E = path.end();
	for ( ; I != E; ++I) {
		if (prev_id != (*I).get_vertex_id()) {
			_vertex				= vertex((*I).get_vertex_id());
			pvDecompress		(normal,_vertex->plane());
			vertex_position		(P,_vertex->position());
			PL.build			(P,normal);
			prev_id				= (*I).get_vertex_id();
		}
		(*I).get_position().y	= P.y;
		PL.intersectRayPoint	((*I).get_position(),DUP,v1);	
		(*I).get_position().y	= v1.y;
	}
}

inline	u32	CLevelGraph::row_length				() const
{
	return						(m_row_length);
}

inline	bool CLevelGraph::valid_vertex_position	(const Fvector3& position) const
{
	if ((position.x < header().box().min.x - header().cell_size()*.5f) || (position.x > header().box().max.x + header().cell_size()*.5f) || (position.z < header().box().min.z - header().cell_size()*.5f) || (position.z > header().box().max.z + header().cell_size()*.5f))
		return			(false);

	if (!(iFloor((position.z - header().box().min.z)/header().cell_size() + .5f) < (int)m_row_length))
		return			(false);
	
	if (!(iFloor((position.x - header().box().min.x)/header().cell_size() + .5f) < (int)m_column_length))
		return			(false);

	return				((vertex_position(position).xz() < (1 << MAX_NODE_BIT_COUNT) - 1));
}

inline	void CLevelGraph::set_mask				(const xr_vector<u32> &mask)
{
	xr_vector<u32>::const_iterator	I = mask.begin();
	xr_vector<u32>::const_iterator	E = mask.end();
	for ( ; I != E; ++I)
		set_mask		(*I);
}

inline	void CLevelGraph::clear_mask			(const xr_vector<u32> &mask)
{
	xr_vector<u32>::const_iterator	I = mask.begin();
	xr_vector<u32>::const_iterator	E = mask.end();
	for ( ; I != E; ++I)
		clear_mask		(*I);
}

inline	void CLevelGraph::set_mask				(u32 vertex_id)
{
	VERIFY						(m_access_mask[vertex_id]);
	m_access_mask[vertex_id]	= false;
}

inline	void CLevelGraph::clear_mask			(u32 vertex_id)
{
	VERIFY						(!m_access_mask[vertex_id]);
	m_access_mask[vertex_id]	= true;
}

template<typename P>
inline	void CLevelGraph::iterate_vertices		(const Fvector3& min_position, const Fvector3& max_position, const P &predicate) const
{
	CVertex						*I, *E;

	if (valid_vertex_position(min_position))
		I						= std::lower_bound(m_nodes,m_nodes + header().vertex_count(),vertex_position(min_position).xz(),&vertex::predicate2);
	else
		I						= m_nodes;

	if (valid_vertex_position(max_position)) {
		E						= std::upper_bound(m_nodes,m_nodes + header().vertex_count(),vertex_position(max_position).xz(),&vertex::predicate);
		if (E != (m_nodes + header().vertex_count()))
			++E;
	}
	else
		E						= m_nodes + header().vertex_count();

	for ( ; I != E; ++I)
		predicate				(*I);
}

inline	u32	CLevelGraph::max_x					() const
{
	return						(m_max_x);
}

inline	u32	CLevelGraph::max_z					() const
{
	return						(m_max_z);
}

inline	bool operator<		(const CLevelGraph::CVertex &vertex, const u32 &vertex_xz)
{
	return						(vertex.position().xz() < vertex_xz);
}

inline	bool operator>		(const CLevelGraph::CVertex &vertex, const u32 &vertex_xz)
{
	return						(vertex.position().xz() > vertex_xz);
}

inline	bool operator==		(const CLevelGraph::CVertex &vertex, const u32 &vertex_xz)
{
	return						(vertex.position().xz() == vertex_xz);
}

inline	bool operator<		(const u32 &vertex_xz, const CLevelGraph::CVertex &vertex)
{
	return						(vertex_xz < vertex.position().xz());
}

inline	bool operator>		(const u32 &vertex_xz, const CLevelGraph::CVertex &vertex)
{
	return						(vertex_xz > vertex.position().xz());
}

inline	bool operator==		(const u32 &vertex_xz, const CLevelGraph::CVertex &vertex)
{
	return						(vertex_xz == vertex.position().xz());
}
