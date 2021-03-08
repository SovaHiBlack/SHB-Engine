////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_abstract.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph class template
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_vertex.h"
#include "graph_edge.h"
#include "object_broker.h"

template <
	typename _data_type = Loki::EmptyType,
	typename _edge_weight_type = float,
	typename _vertex_id_type = u32
>
class CGraphAbstract {
public:
	typedef CVertex<
		_data_type,
		_vertex_id_type,
		CGraphAbstract
	>												CVertex;

	typedef CEdge<
		_edge_weight_type,
		CVertex
	>												CEdge;

public:
	typedef xr_map<_vertex_id_type,CVertex*>		VERTICES;
	typedef typename CVertex::EDGES					EDGES;

public:
	typedef typename VERTICES::const_iterator		const_vertex_iterator;
	typedef typename VERTICES::iterator				vertex_iterator;
	typedef typename EDGES::const_iterator			const_iterator;
	typedef typename EDGES::iterator				iterator;

private:
	VERTICES					m_vertices;
	u32							m_edge_count;

public:
	inline									CGraphAbstract	();
	virtual								~CGraphAbstract	();
	inline		bool						operator==		(const CGraphAbstract &obj) const;
	inline		void						clear			();
	inline		void						add_vertex		(const _data_type &data, const _vertex_id_type &vertex_id);
	inline		void						remove_vertex	(const _vertex_id_type &vertex_id);
	inline		void						add_edge		(const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1, const _edge_weight_type &edge_weight);
	inline		void						add_edge		(const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1, const _edge_weight_type &edge_weight0, const _edge_weight_type &edge_weight1);
	inline		void						remove_edge		(const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1);
	inline		u32							vertex_count	() const;
	inline		u32							edge_count		() const;
	inline		bool						empty			() const;
	inline		const VERTICES				&vertices		() const;
	inline		VERTICES					&vertices		();
	inline		const CVertex				*vertex			(const _vertex_id_type &vertex_id) const;
	inline		CVertex						*vertex			(const _vertex_id_type &vertex_id);
	inline		const CEdge					*edge			(const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1) const;
	inline		CEdge						*edge			(const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1);
	inline		const CGraphAbstract		&header			() const;
	inline		const _edge_weight_type		get_edge_weight	(const _vertex_id_type vertex_index0, const _vertex_id_type vertex_index1, const_iterator i) const;
	inline		bool						is_accessible	(const _vertex_id_type vertex_index) const;
	inline		const CVertex 				*value			(const _vertex_id_type vertex_index, const_iterator i) const;
	inline		void						begin			(const CVertex *vertex, const_iterator &b, const_iterator &e) const;
};

template <
	typename _data_type = Loki::EmptyType,
	typename _edge_weight_type = float,
	typename _vertex_id_type = u32
>
class CGraphAbstractSerialize : public CGraphAbstract<_data_type,_edge_weight_type,_vertex_id_type>, public IPureSerializeObject<IReader,IWriter> {
public:
	virtual void						save			(IWriter &stream);
	virtual void						load			(IReader &stream);
};

#include "graph_abstract_inline.h"