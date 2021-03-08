////////////////////////////////////////////////////////////////////////////
//	Module 		: level_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Level path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "abstract_path_manager.h"

template <
	typename _VertexEvaluator,
	typename _vertex_id_type,
	typename _index_type
>
class 
	CBasePathManager<
		CLevelGraph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	> :
	public CAbstractPathManager<
		CLevelGraph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	>
{
	typedef CAbstractPathManager<
		CLevelGraph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	> inherited;
private:
	friend class CMovementManager;
	friend class CLevelPathBuilder;

protected:
	inline			void	build_path					(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id);
	inline	virtual	void	before_search				(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id);
	inline	virtual	void	after_search				();
	inline	virtual	bool	check_vertex				(const _vertex_id_type vertex_id) const;

public:
	inline					CBasePathManager			(CRestrictedObject *object);
	inline			void	reinit						(const CLevelGraph *graph = 0);
	inline			bool	actual						() const;
	inline			void	on_restrictions_change		();
};

#include "level_path_manager_inline.h"
