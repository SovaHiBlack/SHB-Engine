////////////////////////////////////////////////////////////////////////////
//	Module 		: associative_vector.h
//	Created 	: 14.10.2005
//  Modified 	: 14.10.2005
//	Author		: Dmitriy Iassenev
//	Description : associative vector container
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "associative_vector_compare_predicate.h"

template <
	typename _key_type,
	typename _data_type,
	typename _compare_predicate_type = std::less<_key_type>
>
class associative_vector :
	protected xr_vector<std::pair<_key_type, _data_type> >,
	protected associative_vector_compare_predicate<_key_type, _data_type, _compare_predicate_type>
{
private:
	typedef
		associative_vector<
		_key_type,
		_data_type,
		_compare_predicate_type
		>												self_type;

	typedef
		xr_vector<
		std::pair<
		_key_type,
		_data_type
		>
		>												inherited;

public:
	typedef
		associative_vector_compare_predicate<
		_key_type,
		_data_type,
		_compare_predicate_type
		>												value_compare;

	typedef typename inherited::allocator_type			allocator_type;
	typedef typename inherited::const_pointer			const_pointer;
	typedef typename inherited::const_reference			const_reference;
	typedef typename inherited::const_iterator			const_iterator;
	typedef typename inherited::const_reverse_iterator	const_reverse_iterator;
	typedef typename inherited::pointer					pointer;
	typedef typename inherited::reference				reference;
	typedef typename inherited::iterator				iterator;
	typedef typename inherited::reverse_iterator		reverse_iterator;
	typedef typename allocator_type::difference_type	difference_type;
	typedef _compare_predicate_type						key_compare;
	typedef _key_type									key_type;
	typedef _data_type									mapped_type;
	typedef typename inherited::size_type				size_type;
	typedef typename inherited::value_type				value_type;
	typedef std::pair<iterator, bool>					insert_result;
	typedef std::pair<iterator, iterator>				equal_range_result;
	typedef std::pair<const_iterator, const_iterator>	const_equal_range_result;

private:
	inline void						actualize( ) const;//.

public:
	template <typename _iterator_type>
	inline									associative_vector(_iterator_type first, _iterator_type last, const key_compare& predicate = key_compare( ), const allocator_type& allocator = allocator_type( ));
	inline									associative_vector(const key_compare& predicate = key_compare( ), const allocator_type& allocator = allocator_type( ));
	inline explicit	associative_vector(const key_compare& predicate);
	inline iterator					begin( );
	inline iterator					end( );
	inline reverse_iterator			rbegin( );
	inline iterator					rend( );
	inline insert_result				insert(const value_type& value);
	inline iterator					insert(iterator where, const value_type& value);
	template <class _iterator_type>
	inline void						insert(_iterator_type first, _iterator_type last);
	inline void						erase(iterator element);
	inline void						erase(iterator first, iterator last);
	inline size_type					erase(const key_type& key);
	inline void						clear( );
	inline iterator					find(const key_type& key);
	inline iterator					lower_bound(const key_type& key);
	inline iterator					upper_bound(const key_type& key);
	inline equal_range_result			equal_range(const key_type& key);
	inline void						swap(self_type& object);

	inline const_iterator				begin( ) const;
	inline const_iterator				end( ) const;
	inline const_reverse_iterator		rbegin( ) const;
	inline const_reverse_iterator		rend( ) const;
	inline const_iterator				find(const key_type& key) const;
	inline const_iterator				lower_bound(const key_type& key) const;
	inline const_iterator				upper_bound(const key_type& key) const;
	inline const_equal_range_result	equal_range(const key_type& key) const;
	inline size_type					count(const key_type& key) const;
	inline size_type					max_size( ) const;
//	inline size_type					size				() const;
	inline u32							size( ) const;
	inline bool						empty( ) const;
	inline key_compare					key_comp( ) const;
	inline value_compare				value_comp( ) const;
	inline allocator_type				get_allocator( ) const;

	inline mapped_type& operator[]			(const key_type& key);
	inline self_type& operator=			(const self_type& right);
	inline bool						operator<			(const self_type& right) const;
	inline bool						operator<=			(const self_type& right) const;
	inline bool						operator>			(const self_type& right) const;
	inline bool						operator>=			(const self_type& right) const;
	inline bool						operator==			(const self_type& right) const;
	inline bool						operator!=			(const self_type& right) const;
};

template <
	typename _key_type,
	typename _data_type,
	typename _compare_predicate_type
>
inline			void					swap(
	associative_vector<
	_key_type,
	_data_type,
	_compare_predicate_type
	>& left,
	associative_vector<
	_key_type,
	_data_type,
	_compare_predicate_type
	>& right
);

#include "associative_vector_inline.h"
