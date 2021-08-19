////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph.h
//	Created 	: 18.02.2003
//  Modified 	: 13.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game graph class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "game_graph_space.h"
#include "script_export_space.h"
#include "GameLevelCrossTable.h"

#define GRAPH_NAME			"game.graph"

class CGameGraph {
//private:
//	friend class CRenumbererConverter;

public:
	typedef GameGraph::_GRAPH_ID    _GRAPH_ID;
	typedef GameGraph::_LEVEL_ID	_LEVEL_ID;
	typedef GameGraph::_LOCATION_ID	_LOCATION_ID;
	typedef GameGraph::SLevel		SLevel;
	typedef GameGraph::CEdge		CEdge;
	typedef GameGraph::CVertex		CVertex;
	typedef GameGraph::CHeader		CHeader;
	typedef GameGraph::CLevelPoint	CLevelPoint;

public:		
	typedef const CEdge				*const_iterator;
	typedef const CLevelPoint		*const_spawn_iterator;
	typedef xr_vector<CLevelPoint>	LEVEL_POINT_VECTOR;
	typedef xr_vector<bool>			ENABLED;

private:
	CHeader							m_header;
	IReader							*m_reader;
	CVertex							*m_nodes;
	mutable ENABLED					m_enabled;
	_GRAPH_ID						m_current_level_some_vertex_id;

public:
	inline 								CGameGraph				();

public:
	inline virtual						~CGameGraph				();
	inline		const CHeader			&header					() const;
	inline		bool					mask					(const svector<_LOCATION_ID,GameGraph::LOCATION_TYPE_COUNT> &M, const _LOCATION_ID E[GameGraph::LOCATION_TYPE_COUNT]) const;
	inline		bool					mask					(const _LOCATION_ID M[GameGraph::LOCATION_TYPE_COUNT], const _LOCATION_ID E[GameGraph::LOCATION_TYPE_COUNT]) const;
	inline		float					distance				(const _GRAPH_ID tGraphID0, const _GRAPH_ID tGraphID1) const;
	inline		bool					accessible				(const u32 &vertex_id) const;
	inline		void					accessible				(const u32 &vertex_id, bool value) const;
	inline		bool					valid_vertex_id			(const u32 &vertex_id) const;
	inline		void					begin					(const u32 &vertex_id, const_iterator &start, const_iterator &end) const;
	inline		void					begin_spawn				(const u32 &vertex_id, const_spawn_iterator &start, const_spawn_iterator &end) const;
	inline		const _GRAPH_ID			&value					(const u32 &vertex_id, const_iterator &i) const;
	inline		const float				&edge_weight			(const_iterator i) const;
	inline		const CVertex			*vertex					(const u32 &vertex_id) const;
	inline		void					set_invalid_vertex		(_GRAPH_ID &vertex_id) const;
	inline		_GRAPH_ID				vertex_id				(const CVertex *vertex) const;
	inline		void					set_current_level		(const u32 &level_id);
	inline		const _GRAPH_ID			&current_level_vertex	() const;
public:
	static void script_register(lua_State*);
};

add_to_type_list(CGameGraph)
#undef script_type_list
#define script_type_list save_type_list(CGameGraph)

#include "game_graph_inline.h"
