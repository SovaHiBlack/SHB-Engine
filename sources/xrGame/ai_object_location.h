////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_object_location.h
//	Created 	: 27.11.2003
//  Modified 	: 27.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI object location
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "game_graph_space.h"

namespace LevelGraph {
	class CVertex;
};

class CAI_ObjectLocation {
private:
	u32										m_level_vertex_id;
	GameGraph::_GRAPH_ID					m_game_vertex_id;

public:
	inline										CAI_ObjectLocation	();
	inline			void						init				();
	inline	virtual	void						reinit				();
	inline			void						game_vertex			(const GameGraph::CVertex	*game_vertex);
	inline			void						game_vertex			(const GameGraph::_GRAPH_ID	game_vertex_id);
	inline			const GameGraph::CVertex	*game_vertex		() const;
	inline			const GameGraph::_GRAPH_ID	game_vertex_id		() const;
	inline			void						level_vertex		(const LevelGraph::CVertex	*level_vertex);
	inline			void						level_vertex		(const u32					level_vertex_id);
	inline			const LevelGraph::CVertex	*level_vertex		() const;
	inline			const u32					level_vertex_id		() const;
};

#include "ai_object_location_inline.h"
