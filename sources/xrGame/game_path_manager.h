////////////////////////////////////////////////////////////////////////////
//	Module 		: game_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "abstract_path_manager.h"
#include "game_graph.h"

template <
	typename _VertexEvaluator,
	typename _vertex_id_type,
	typename _index_type
>
class 
	CBasePathManager<
		CGameGraph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	> :
	public CAbstractPathManager<
		CGameGraph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	>
{
	typedef CAbstractPathManager<
		CGameGraph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	> inherited;
protected:
	inline	virtual	void	before_search				(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id);
	inline	virtual	void	after_search				();

public:
	inline					CBasePathManager			(CRestrictedObject *object);
	inline	virtual	void	reinit						(const CGameGraph *graph = 0);
	inline			bool	actual						() const;
	inline	virtual	void	select_intermediate_vertex	();
	inline	virtual	bool	completed					() const;
};

#include "game_path_manager_inline.h"
