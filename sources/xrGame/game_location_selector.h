////////////////////////////////////////////////////////////////////////////
//	Module 		: game_location_selector.h
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game location selector
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "abstract_location_selector.h"
#include "game_graph.h"
#include "location_manager.h"

enum ESelectionType {
	eSelectionTypeMask = u32(0),
	eSelectionTypeRandomBranching,
	eSelectionTypeDummy = u32(-1),
};

template <
	typename _VertexEvaluator,
	typename _vertex_id_type
>
class 
	CBaseLocationSelector<
		CGameGraph,
		_VertexEvaluator,
		_vertex_id_type
	> :
	public CAbstractLocationSelector <
		CGameGraph,
		_VertexEvaluator,
		_vertex_id_type
	>
{
	typedef CGameGraph _Graph;
	typedef CAbstractLocationSelector <
		CGameGraph,
		_VertexEvaluator,
		_vertex_id_type
	> inherited;
private:
	ESelectionType			m_selection_type;
	GameGraph::_GRAPH_ID	m_previous_vertex_id;
	CLocationManager		*m_location_manager;

protected:
	inline			void		select_random_location	(const _vertex_id_type start_vertex_id, _vertex_id_type &dest_vertex_id);
	inline			bool		accessible				(const _vertex_id_type vertex_id) const;

public:
	inline						CBaseLocationSelector	(CRestrictedObject *object, CLocationManager *location_manager);
	inline	virtual				~CBaseLocationSelector	();
	inline			void		init					();
	inline	virtual void		reinit					(const _Graph *graph = 0);
	inline			void		set_selection_type		(const ESelectionType selection_type);
	inline			void		selection_type			() const;
	inline			bool		actual					(const _vertex_id_type start_vertex_id, bool path_completed);
	inline			void		select_location			(const _vertex_id_type start_vertex_id, _vertex_id_type &dest_vertex_id);
};

#include "game_location_selector_inline.h"