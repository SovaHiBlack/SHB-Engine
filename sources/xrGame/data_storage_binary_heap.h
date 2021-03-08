////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_binary_heap.h
//	Created 	: 21.03.2002
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Binary heap data storage
////////////////////////////////////////////////////////////////////////////

#pragma once

struct CDataStorageBinaryHeap {

	template <
		typename _data_storage,
		template <typename _T> class _vertex = CEmptyClassTemplate
	>
	class CDataStorage : public _data_storage::template CDataStorage<_vertex> {
	public:
		typedef typename _data_storage::template CDataStorage<_vertex>	inherited;
		typedef typename inherited::CGraphVertex				CGraphVertex;
		typedef typename CGraphVertex::_dist_type				_dist_type;
		typedef typename CGraphVertex::_index_type				_index_type;

		struct CGraphNodePredicate {
			inline			bool	operator()(CGraphVertex *node1, CGraphVertex *node2)
			{
				return				(node1->f() > node2->f());
			};
		};

	protected:
		CGraphVertex			**m_heap;
		CGraphVertex			**m_heap_head;
		CGraphVertex			**m_heap_tail;

	public:
		inline						CDataStorage		(const u32 vertex_count);
		virtual					~CDataStorage		();
		inline		void			init				();
		inline		bool			is_opened_empty		() const;
		inline		void			add_opened			(CGraphVertex &vertex);
		inline		void			decrease_opened		(CGraphVertex &vertex, const _dist_type value);
		inline		void			remove_best_opened	();
		inline		void			add_best_closed		();
		inline		CGraphVertex	&get_best			() const;
	};
};

#include "data_storage_binary_heap_inline.h"