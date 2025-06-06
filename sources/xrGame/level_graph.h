////////////////////////////////////////////////////////////////////////////
//	Module 		: level_graph.h
//	Created 	: 02.10.2001
//  Modified 	: 11.11.2003
//	Author		: Oles Shihkovtsov, Dmitriy Iassenev
//	Description : Level graph
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\XR_3DA\xrLevel.h"

#include "alife_space.h"
#include "level_graph_space.h"
#include "game_graph_space.h"

namespace LevelGraph
{
	class	CHeader;
	class	CVertex;
	struct	SSegment;
	struct	SContour;
};

class CCoverPoint;

class CLevelGraph
{
private:
	friend class CRenumbererConverter;

public:
	typedef LevelGraph::CPosition	CPosition;
	typedef LevelGraph::CHeader		CHeader;
	typedef LevelGraph::CVertex		CVertex;
	typedef LevelGraph::SSegment	SSegment;
	typedef LevelGraph::SContour	SContour;

private:
	enum ELineIntersections
	{
		eLineIntersectionNone = u32(0),
		eLineIntersectionCollinear = u32(0),
		eLineIntersectionIntersect = u32(1),
		eLineIntersectionEqual = u32(2)
	};

private:
	IReader* m_reader;		// level graph virtual storage
	CHeader* m_header;		// level graph header
	CVertex* m_nodes;		// nodes array
	xr_vector<bool>			m_access_mask;
	GameGraph::_LEVEL_ID	m_level_id;		// unique level identifier
	u32						m_row_length;
	u32						m_column_length;
	u32						m_max_x;
	u32						m_max_z;

protected:
	u32		vertex(const fVector3& position) const;

public:
	typedef u32 const_iterator;
	typedef u32 const_spawn_iterator;
	typedef u32 const_death_iterator;
	typedef const CVertex* const_vertex_iterator;

private:
	struct vertex
	{
		static IC	bool	predicate(const u32& value, const CVertex& vertex)
		{
			return	(value < vertex.position( ).xz( ));
		}

		static IC	bool	predicate2(const CVertex& vertex, const u32& value)
		{
			return	(vertex.position( ).xz( ) < value);
		}
	};

public:
	CLevelGraph( );

	virtual			~CLevelGraph( );
	IC		const_vertex_iterator begin( ) const;
	IC		const_vertex_iterator end( ) const;
	IC		void	set_mask(const xr_vector<u32>& mask);
	IC		void	set_mask(u32 vertex_id);
	IC		void	clear_mask(const xr_vector<u32>& mask);
	IC		void	clear_mask(u32 vertex_id);
	IC		bool	is_accessible(const u32 vertex_id) const;
	IC		void	level_id(const GameGraph::_LEVEL_ID& level_id);
	IC		u32		max_x( ) const;
	IC		u32		max_z( ) const;
	IC		void	begin(const CVertex& vertex, const_iterator& begin, const_iterator& end) const;
	IC		void	begin(const CVertex* vertex, const_iterator& begin, const_iterator& end) const;
	IC		void	begin(u32 vertex_id, const_iterator& begin, const_iterator& end) const;
	IC		u32		value(const CVertex& vertex, const_iterator& i) const;
	IC		u32		value(const CVertex* vertex, const_iterator& i) const;
	IC		u32		value(const u32 vertex_id, const_iterator& i) const;
	IC		const CHeader& header( ) const;
	ICF		bool	valid_vertex_id(u32 vertex_id) const;
	IC		const GameGraph::_LEVEL_ID& level_id( ) const;
	IC		void	unpack_xz(const CLevelGraph::CPosition& vertex_position, u32& x, u32& z) const;
	IC		void	unpack_xz(const CLevelGraph::CPosition& vertex_position, s32& x, s32& z) const;
	IC		void	unpack_xz(const CLevelGraph::CPosition& vertex_position, f32& x, f32& z) const;
	template <typename T>
	IC		void	unpack_xz(const CLevelGraph::CVertex& vertex, T& x, T& z) const;
	template <typename T>
	IC		void	unpack_xz(const CLevelGraph::CVertex* vertex, T& x, T& z) const;
	ICF		CVertex* vertex(u32 vertex_id) const;
	ICF		u32		vertex(const CVertex* vertex_p) const;
	ICF		u32		vertex(const CVertex& vertex_r) const;
	IC		const	fVector3						vertex_position(const CLevelGraph::CPosition& source_position) const;
	IC		const	fVector3& vertex_position(fVector3& dest_position, const CLevelGraph::CPosition& source_position) const;
	IC		const	CLevelGraph::CPosition& vertex_position(CLevelGraph::CPosition& dest_position, const fVector3& source_position) const;
	IC		const	CLevelGraph::CPosition		vertex_position(const fVector3& position) const;
	IC		const	fVector3						vertex_position(u32 vertex_id) const;
	IC		const	fVector3						vertex_position(const CVertex& vertex) const;
	IC		const	fVector3						vertex_position(const CVertex* vertex) const;
	IC		f32	vertex_plane_y(const CVertex& vertex, const f32 X, const f32 Z) const;
	IC		f32	vertex_plane_y(const CVertex* vertex, const f32 X, const f32 Z) const;
	IC		f32	vertex_plane_y(const u32 vertex_id, const f32 X, const f32 Z) const;
	IC		f32	vertex_plane_y(const CVertex& vertex) const;
	IC		f32	vertex_plane_y(const CVertex* vertex) const;
	IC		f32	vertex_plane_y(const u32 vertex_id) const;
	IC		bool	inside(const CVertex& vertex, const CLevelGraph::CPosition& vertex_position) const;
	IC		bool	inside(const CVertex& vertex, const fVector3& vertex_position) const;
	IC		bool	inside(const CVertex* vertex, const CLevelGraph::CPosition& vertex_position) const;
	IC		bool	inside(const CVertex* vertex, const fVector3& vertex_position) const;
	IC		bool	inside(const u32 vertex_id, const CLevelGraph::CPosition& vertex_position) const;
	IC		bool	inside(const u32 vertex_id, const fVector3& position) const;
	IC		bool	inside(const u32 vertex_id, const fVector2& position) const;
	IC		bool	inside(const CVertex& vertex, const CLevelGraph::CPosition& vertex_position, const f32 epsilon) const;
	IC		bool	inside(const CVertex& vertex, const fVector3& vertex_position, const f32 epsilon) const;
	IC		bool	inside(const CVertex* vertex, const CLevelGraph::CPosition& vertex_position, const f32 epsilon) const;
	IC		bool	inside(const CVertex* vertex, const fVector3& vertex_position, const f32 epsilon) const;
	IC		bool	inside(const u32 vertex_id, const CLevelGraph::CPosition& vertex_position, const f32 epsilon) const;
	IC		bool	inside(const u32 vertex_id, const fVector3& position, const f32 epsilon) const;
	IC		void	project_point(const fPlane3& plane, fVector3& point) const;
	IC		u32		row_length( ) const;
	f32	distance(const fVector3& position, const CVertex* vertex) const;
	f32	distance(const fVector3& position, const u32 vertex_id) const;
	f32	distance(const u32 vertex_id, const fVector3& position) const;
	IC		f32	distance(const fVector3& position, const fVector3& point0, const fVector3& point1) const;
	IC		f32	distance(u32 vertex_id0, u32 vertex_id1) const;
	IC		f32	distance(const CVertex* tpNode0, u32 vertex_id1) const;
	IC		f32	distance(u32 vertex_id0, const CVertex* vertex) const;
	IC		f32	distance(const CVertex* node0, const CVertex* node1) const;
	IC		f32	distance(const u32 vertex_id, const CPosition& position) const;
	IC		f32	distance(const CPosition& position, const u32 vertex_id) const;
	IC		ELineIntersections	intersect(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4, f32* x, f32* y) const;
	IC		ELineIntersections	intersect_no_check(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4, f32* x, f32* y) const;
	IC		bool	similar(const fVector3& point0, const fVector3& point1) const;
	IC		bool	inside(const fVector3& point, const SContour& contour) const;
	IC		void	intersect(SSegment& segment, const SContour& contour0, const SContour& contour1) const;
	IC		f32	nearest(fVector3& destination, const fVector3& position, const fVector3& point0, const fVector3& point1) const;
	IC		void	contour(SContour& contour, u32 vertex_id) const;
	IC		void	contour(SContour& contour, const CVertex* vertex) const;
	IC		void	nearest(fVector3& destination, const fVector3& position, const SContour& contour) const;
	IC		bool	intersect(fVector3& destination, const fVector3& v1, const fVector3& v2, const fVector3& v3, const fVector3& v4) const;
	IC		f32	square(f32 a1, f32 b1, f32 alpha = PI_DIV_2) const;
	IC		f32	compute_square(f32 angle, f32 AOV, f32 b0, f32 b1, f32 b2, f32 b3) const;
	IC		f32	compute_square(f32 angle, f32 AOV, const CVertex* vertex) const;
	IC		f32	compute_square(f32 angle, f32 AOV, u32 dwNodeID) const;
	IC		f32	vertex_cover(const CLevelGraph::CVertex* vertex) const;
	IC		f32	vertex_cover(const u32 vertex_id) const;
	f32	cover_in_direction(f32 angle, f32 b0, f32 b1, f32 b2, f32 b3) const;
	IC		f32	cover_in_direction(f32 angle, const CVertex* vertex) const;
	IC		f32	cover_in_direction(f32 angle, u32 vertex_id) const;

	template <class _predicate>
	IC		f32	vertex_cover_angle(u32 vertex_id, f32 inc_angle, _predicate compare_predicate) const;
	IC		void	set_invalid_vertex(u32& vertex_id, CVertex** vertex = NULL) const;
	IC		const u32 vertex_id(const CLevelGraph::CVertex* vertex) const;
	u32		vertex_id(const fVector3& position) const;
	u32		vertex(u32 current_vertex_id, const fVector3& position) const;
	void	choose_point(const fVector3& start_point, const fVector3& finish_point, const SContour& contour, s32 vertex_id, fVector3& temp_point, s32& saved_index) const;
	IC		bool	check_vertex_in_direction(u32 start_vertex_id, const fVector3& start_position, u32 finish_vertex_id) const;
	IC		u32		check_position_in_direction(u32 start_vertex_id, const fVector3& start_position, const fVector3& finish_position) const;
	f32	check_position_in_direction(u32 start_vertex_id, const fVector3& start_position, const fVector3& finish_position, const f32 max_distance) const;
	f32	mark_nodes_in_direction(u32 start_vertex_id, const fVector3& start_position, const fVector3& direction, f32 distance, xr_vector<u32>& vertex_stack, xr_vector<bool>* vertex_marks) const;
	f32	mark_nodes_in_direction(u32 start_vertex_id, const fVector3& start_position, u32 finish_node, xr_vector<u32>& vertex_stack, xr_vector<bool>* vertex_marks) const;
	f32	mark_nodes_in_direction(u32 start_vertex_id, const fVector3& start_position, const fVector3& finish_point, xr_vector<u32>& vertex_stack, xr_vector<bool>* vertex_marks) const;
	f32	farthest_vertex_in_direction(u32 start_vertex_id, const fVector3& start_point, const fVector3& finish_point, u32& finish_vertex_id, xr_vector<bool>* tpaMarks, bool check_accessability = false) const;
	bool	create_straight_path(u32 start_vertex_id, const fVector3& start_point, const fVector3& finish_point, xr_vector<fVector3>& tpaOutputPoints, xr_vector<u32>& tpaOutputNodes, bool bAddFirstPoint, bool bClearPath = true) const;
	bool	create_straight_path(u32 start_vertex_id, const fVector2& start_point, const fVector2& finish_point, xr_vector<fVector3>& tpaOutputPoints, xr_vector<u32>& tpaOutputNodes, bool bAddFirstPoint, bool bClearPath = true) const;
	template <bool bAssignY, typename T>
	IC		bool	create_straight_path(u32 start_vertex_id, const fVector2& start_point, const fVector2& finish_point, xr_vector<T>& tpaOutputPoints, const T& example, bool bAddFirstPoint, bool bClearPath = true) const;
	template<typename T>
	IC		void	assign_y_values(xr_vector<T>& path);
	template<typename P>
	IC		void	iterate_vertices(const fVector3& min_position, const fVector3& max_position, const P& predicate) const;
	IC		bool	check_vertex_in_direction(u32 start_vertex_id, const fVector2& start_position, u32 finish_vertex_id) const;
	IC		u32		check_position_in_direction(u32 start_vertex_id, const fVector2& start_position, const fVector2& finish_position) const;
	bool	check_vertex_in_direction_slow(u32 start_vertex_id, const fVector2& start_position, u32 finish_vertex_id) const;
	u32		check_position_in_direction_slow(u32 start_vertex_id, const fVector2& start_position, const fVector2& finish_position) const;
	IC		fVector3 v3d(const fVector2& vector2d) const;
	IC		fVector2 v2d(const fVector3& vector3d) const;
	IC		bool	valid_vertex_position(const fVector3& position) const;
	bool	neighbour_in_direction(const fVector3& direction, u32 start_vertex_id) const;

#ifdef DEBUG
private:
	ref_shader			sh_debug;

private:
	s32					m_current_level_id;
	bool				m_current_actual;
	fVector3				m_current_center;
	fVector3				m_current_radius;

public:
	void		setup_current_level(const s32& level_id);

private:
	fVector3		convert_position(const fVector3& position);
	void		draw_edge(const s32& vertex_id0, const s32& vertex_id1);
	void		draw_vertex(const s32& vertex_id);
	void		draw_stalkers(const s32& vertex_id);
	void		draw_objects(const s32& vertex_id);
	void		update_current_info( );

private:
	void		draw_nodes( );
	void		draw_restrictions( );
	void		draw_covers( );
	void		draw_game_graph( );
	void		draw_objects( );
	void		draw_debug_node( );

public:
	void		render( );
#endif
};

IC	bool operator<		(const CLevelGraph::CVertex& vertex, const u32& vertex_xz);
IC	bool operator>		(const CLevelGraph::CVertex& vertex, const u32& vertex_xz);
IC	bool operator==		(const CLevelGraph::CVertex& vertex, const u32& vertex_xz);
IC	bool operator<		(const u32& vertex_xz, const CLevelGraph::CVertex& vertex);
IC	bool operator>		(const u32& vertex_xz, const CLevelGraph::CVertex& vertex);
IC	bool operator==		(const u32& vertex_xz, const CLevelGraph::CVertex& vertex);

#ifdef DEBUG
extern BOOL	g_bDebugNode;
extern u32	g_dwDebugNodeSource;
extern u32	g_dwDebugNodeDest;
#endif

#include "level_graph_inline.h"
#include "level_graph_vertex_inline.h"