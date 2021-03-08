////////////////////////////////////////////////////////////////////////////
//	Module 		: CEdge.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph edge class template
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _edge_weight_type,
	typename _vertex_type
>
class CEdge {
public:
	typedef _edge_weight_type						_edge_weight_type;
	typedef _vertex_type							_vertex_type;
	typedef typename _vertex_type::_vertex_id_type	_vertex_id_type;

private:
	_edge_weight_type				m_weight;
	_vertex_type					*m_vertex;

public:
	inline								CEdge		(const _edge_weight_type &weight, _vertex_type *vertex);
	inline		const _edge_weight_type	&weight		() const;
	inline		_vertex_type			*vertex		() const;
	inline		const _vertex_id_type	&vertex_id	() const;
	inline		bool					operator==	(const _vertex_id_type &vertex_id) const;
	inline		bool					operator==	(const CEdge &obj) const;
};

#include "graph_edge_inline.h"