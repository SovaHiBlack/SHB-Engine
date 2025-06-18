////////////////////////////////////////////////////////////////////////////
//	Module 		: QuadTree.h
//	Description : Quadtree class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Profiler.h"

template <typename _object_type>
class CQuadTree
{
public:
	struct SQuadNode
	{
		SQuadNode* m_neighbours[4];
		IC SQuadNode*& next( )
		{
			return m_neighbours[0];
		}
	};

	struct SListItem
	{
		_object_type* m_object;
		SListItem* m_next;
		IC SListItem*& next( )
		{
			return m_next;
		}
	};

	template <typename T>
	struct SFixedStorage
	{
		T* m_objects;
		T* m_free;
		u32						m_max_object_count;

		IC SFixedStorage(u32 max_object_count) : m_max_object_count(max_object_count)
		{
			m_objects = xr_alloc<T>(m_max_object_count);
			T* B = 0;
			T* I = m_objects;
			T* E = m_objects + m_max_object_count;
			for (; I != E; B = I, ++I)
			{
				I->next( ) = B;
			}

			m_free = E - 1;
		}

		virtual		~SFixedStorage( )
		{
			xr_free(m_objects);
		}

		IC	T* get_object( )
		{
			VERIFY(m_free);
			T* node = m_free;
			m_free = m_free->next( );
			ZeroMemory(node, sizeof(T));
			return node;
		}

		IC	void	clear( )
		{
			T* B = 0;
			T* I = m_objects;
			T* E = m_objects + m_max_object_count;
			m_free = E - 1;
			for (; I != E; ++I)
			{
				I->next( ) = B;
			}
		}

		IC	void	remove(T*& node)
		{
			node->next( ) = m_free;
			m_free = node;
			node = 0;
		}
	};

	typedef SFixedStorage<SQuadNode> CQuadNodeStorage;
	typedef SFixedStorage<SListItem> CListItemStorage;

protected:
	fVector3					m_center;
	f32							m_radius;
	s32							m_max_depth;
	SQuadNode*					m_root;
	CQuadNodeStorage*			m_nodes;
	CListItemStorage*			m_list_items;
	size_t						m_leaf_count;

protected:
	IC		u32					neighbour_index(const fVector3& position, fVector3& center, f32 distance) const;
	IC		void				nearest(const fVector3& position, f32 radius, xr_vector<_object_type*>& objects, SQuadNode* node, fVector3 center, f32 distance, s32 depth) const;
	IC		_object_type*		remove(const _object_type* object, SQuadNode*& node, fVector3 center, f32 distance, s32 depth);
	IC		void				all(xr_vector<_object_type*>& objects, SQuadNode* node, s32 depth) const;

public:
	IC							CQuadTree(const fBox3& box, f32 min_cell_size, u32 max_node_count, u32 max_list_item_count);
	virtual						~CQuadTree( );
	IC		void				clear( );
	IC		void				insert(_object_type* object);
	IC		_object_type*		remove(const _object_type* object);
	IC		_object_type*		find(const fVector3& position);
	IC		void				nearest(const fVector3& position, f32 radius, xr_vector<_object_type*>& objects, bool clear = true) const;
	IC		void				all(xr_vector<_object_type*>& objects, bool clear = true) const;
	IC		size_t				size( ) const;
};

#include "QuadTree_inline.h"
