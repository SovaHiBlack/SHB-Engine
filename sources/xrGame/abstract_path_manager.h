////////////////////////////////////////////////////////////////////////////
//	Module 		: abstract_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Abstract path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "restricted_object.h"

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type,
	typename _index_type
>
class CAbstractPathManager {
public:
	typedef xr_vector<_vertex_id_type> PATH;

private:
	const _Graph		*m_graph;
	_VertexEvaluator	*m_evaluator;

protected:
	_index_type			m_current_index;
	_index_type			m_intermediate_index;
	_vertex_id_type		m_dest_vertex_id;
	bool				m_actuality;
	bool				m_failed;
	PATH				m_path;
	CRestrictedObject	*m_object;

protected:
	_vertex_id_type		m_failed_start_vertex_id;
	_vertex_id_type		m_failed_dest_vertex_id;

protected:
	inline	_vertex_id_type	intermediate_vertex_id		() const;

	inline			void	build_path					(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id);
	inline	const _VertexEvaluator	*evaluator			() const;
	inline			void	make_inactual				();
	inline	virtual	void	before_search				(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id);
	inline	virtual	void	after_search				();
	inline	virtual	bool	check_vertex				(const _vertex_id_type vertex_id) const;

public:
	inline					CAbstractPathManager		(CRestrictedObject *object);
	inline	virtual			~CAbstractPathManager		();
	inline			void	reinit						(const _Graph *graph = 0);
	inline			bool	actual						(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id) const;
	inline			void	set_evaluator				(_VertexEvaluator *evaluator);
	inline			void	set_dest_vertex				(const _vertex_id_type vertex_id);
	inline	_vertex_id_type	dest_vertex_id				() const;
	inline	virtual	bool	completed					() const;
	inline			bool	failed						() const;
	inline			void	reset						();
	inline	virtual	void	select_intermediate_vertex	();
	inline	CRestrictedObject &object					() const;
	
public:
	inline	const PATH		&path						() const;
	inline			u32		intermediate_index			() const;

	friend class CMovementManager;
};

#include "abstract_path_manager_inline.h"

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type,
	typename _index_type
>
class CBasePathManager :
	public CAbstractPathManager<
		_Graph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	> 
{
};