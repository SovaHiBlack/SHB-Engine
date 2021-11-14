////////////////////////////////////////////////////////////////////////////
//	Module 		: vertex_manager_fixed.h
//	Created 	: 21.03.2002
//  Modified 	: 01.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Fixed vertex manager
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _path_id_type,
	typename _index_type,
	unsigned char mask
>
struct CVertexManagerFixed {

	template <template <typename _T> class T1>
	struct VertexManager {
		template<typename T2>
		struct _vertex : public T1<T2> {
			typedef _index_type _index_type;
			_index_type	_index  : 8*sizeof(_index_type) - mask;
			_index_type	_opened : mask;

			inline	_index_type index() const
			{
				return	(_index);
			}

			inline	_index_type opened() const
			{
				return	(_opened);
			}
		};
	};

	template <
		template <typename _T> class _vertex = CEmptyClassTemplate,
		template <typename _T1, typename _T2> class _index_vertex = CEmptyClassTemplate2,
		typename _data_storage = CBuilderAllocatorConstructor
	> 
	class CDataStorage : public _data_storage::template CDataStorage<VertexManager<_vertex>::_vertex> {
	public:
		typedef typename _data_storage::template CDataStorage<
			VertexManager<_vertex>::_vertex
		>												inherited;
		typedef typename inherited::CGraphVertex		CGraphVertex;
		typedef typename CGraphVertex::_index_type		_index_type;

#pragma pack(push,1)
		template <typename _path_id_type>
		struct SGraphIndexVertex : public _index_vertex<CGraphVertex,SGraphIndexVertex<_path_id_type> > {
			_path_id_type	m_path_id;
			CGraphVertex	*m_vertex;
		};
#pragma pack(pop)

		typedef _path_id_type							_path_id_type;
		typedef SGraphIndexVertex<_path_id_type>		CGraphIndexVertex;

	protected:
		_path_id_type			m_current_path_id;
		u32						m_max_node_count;
		CGraphIndexVertex		*m_indexes;

	public:
		inline						CDataStorage	(const u32 vertex_count);
		virtual					~CDataStorage	();
		inline		void			init			();
		inline		bool			is_opened		(const CGraphVertex &vertex) const;
		inline		bool			is_visited		(const _index_type &vertex_id) const;
		inline		bool			is_closed		(const CGraphVertex &vertex) const;
		inline		CGraphVertex	&get_node		(const _index_type &vertex_id) const;
		inline		CGraphVertex	&create_vertex	(CGraphVertex &vertex, const _index_type &vertex_id);
		inline		void			add_opened		(CGraphVertex &vertex);
		inline		void			add_closed		(CGraphVertex &vertex);
		inline		_path_id_type	current_path_id	() const;
	};
};

#include "vertex_manager_fixed_inline.h"