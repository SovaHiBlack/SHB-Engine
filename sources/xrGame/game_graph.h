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
#include "game_level_cross_table.h"

#define GRAPH_NAME			"game.graph"

class CGameGraph
{
private:
	friend class CRenumbererConverter;

public:
	using _GRAPH_ID = GameGraph::_GRAPH_ID;
	using _LEVEL_ID = GameGraph::_LEVEL_ID;
	using _LOCATION_ID = GameGraph::_LOCATION_ID;
	using SLevel = GameGraph::SLevel;
	using CEdge = GameGraph::CEdge;
	using CVertex = GameGraph::CVertex;
	using CHeader = GameGraph::CHeader;
	using CLevelPoint = GameGraph::CLevelPoint;

public:
	using const_iterator = const CEdge*;
	using const_spawn_iterator = const CLevelPoint*;
	using LEVEL_POINT_VECTOR = xr_vector<CLevelPoint>;
	using ENABLED = xr_vector<bool>;

private:
	CHeader							m_header;
	IReader* m_reader;
	CVertex* m_nodes;
	mutable ENABLED					m_enabled;
	_GRAPH_ID						m_current_level_some_vertex_id;

public:
	IC 								CGameGraph();

public:
	IC virtual						~CGameGraph();
	IC		const CHeader& header() const;
	IC		bool					mask(const svector<_LOCATION_ID, GameGraph::LOCATION_TYPE_COUNT>& M, const _LOCATION_ID E[GameGraph::LOCATION_TYPE_COUNT]) const;
	IC		bool					mask(const _LOCATION_ID M[GameGraph::LOCATION_TYPE_COUNT], const _LOCATION_ID E[GameGraph::LOCATION_TYPE_COUNT]) const;
	IC		f32					distance(const _GRAPH_ID tGraphID0, const _GRAPH_ID tGraphID1) const;
	IC		bool					accessible(const u32& vertex_id) const;
	IC		void					accessible(const u32& vertex_id, bool value) const;
	IC		bool					valid_vertex_id(const u32& vertex_id) const;
	IC		void					begin(const u32& vertex_id, const_iterator& start, const_iterator& end) const;
	IC		void					begin_spawn(const u32& vertex_id, const_spawn_iterator& start, const_spawn_iterator& end) const;
	IC		const _GRAPH_ID& value(const u32& vertex_id, const_iterator& i) const;
	IC		const float& edge_weight(const_iterator i) const;
	IC		const CVertex* vertex(const u32& vertex_id) const;
	IC		void					set_invalid_vertex(_GRAPH_ID& vertex_id) const;
	IC		_GRAPH_ID				vertex_id(const CVertex* vertex) const;
	IC		void					set_current_level(const u32& level_id);
	IC		const _GRAPH_ID& current_level_vertex() const;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CGameGraph)
#undef script_type_list
#define script_type_list save_type_list(CGameGraph)

#include "game_graph_inline.h"