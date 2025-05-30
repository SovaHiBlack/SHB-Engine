#pragma once

using std::swap;

#include "_type_traits.h"

#define	M_DONTDEFERCLEAR_EXT		//. for mem-debug only

//--------	
template <class T>
class	xalloc
{
public:
	typedef	size_t		size_type;
	typedef ptrdiff_t	difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T			value_type;

	template<class _Other>
	struct rebind
	{
		typedef xalloc<_Other> other;
	};

	pointer					address(reference _Val) const
	{
		return (&_Val);
	}
	const_pointer			address(const_reference _Val) const
	{
		return (&_Val);
	}
	xalloc( )
	{ }
	xalloc(const xalloc<T>&)
	{ }
	template<class _Other>							xalloc(const xalloc<_Other>&)
	{ }
	template<class _Other>	xalloc<T>& operator=		(const xalloc<_Other>&)
	{
		return (*this);
	}
	pointer					allocate(size_type n, pcvoid p = 0) const
	{
		return xr_alloc<T>((u32)n);
	}
	pstr _charalloc(size_type n)
	{
		return (pstr)allocate(n);
	}
	void					deallocate(pointer p, size_type n) const
	{
		xr_free(p);
	}
	void					deallocate(pvoid p, size_type n) const
	{
		xr_free(p);
	}
	void					construct(pointer p, const T& _Val)
	{
		std::_Construct(p, _Val);
	}
	void					destroy(pointer p)
	{
		std::_Destroy(p);
	}
	size_type				max_size( ) const
	{
		size_type _Count = (size_type)(-1) / sizeof(T);
		return (0 < _Count ? _Count : 1);
	}
};

struct xr_allocator
{
	template <typename T>
	struct helper
	{
		typedef xalloc<T>	result;
	};

	static	pvoid alloc(const u32& n)
	{
		return xr_malloc((u32)n);
	}
	template <typename T>
	static	void	dealloc(T*& p)
	{
		xr_free(p);
	}
};

template<class _Ty, class _Other>	IC	bool operator==(const xalloc<_Ty>&, const xalloc<_Other>&)
{
	return (true);
}
template<class _Ty, class _Other>	IC	bool operator!=(const xalloc<_Ty>&, const xalloc<_Other>&)
{
	return (false);
}

namespace std
{
	template<class _Tp1, class _Tp2>	IC	xalloc<_Tp2>& __stl_alloc_rebind(xalloc<_Tp1>& __a, const _Tp2*)
	{
		return (xalloc<_Tp2>&)(__a);
	}
	template<class _Tp1, class _Tp2>	IC	xalloc<_Tp2> __stl_alloc_create(xalloc<_Tp1>&, const _Tp2*)
	{
		return xalloc<_Tp2>( );
	}
};

// string(char)
typedef std::basic_string<char, std::char_traits<char>, xalloc<char>> xr_string;

// vector
template	<typename T, typename allocator = xalloc<T> >
class xr_vector : public std::vector<T, allocator>
{
private:
	typedef std::vector<T, allocator>	inherited;

public:
	typedef allocator					allocator_type;

public:
	xr_vector( ) : inherited( )
	{ }
	xr_vector(size_t _count, const T& _value) : inherited(_count, _value)
	{ }
	explicit xr_vector(size_t _count) : inherited(_count)
	{ }
	u32		size( ) const
	{
		return (u32)inherited::size( );
	}

	void	clear_and_free( )
	{
		inherited::clear( );
	}
	void	clear_not_free( )
	{
		erase(begin( ), end( ));
	}
	void	clear_and_reserve( )
	{
		if (capacity( ) <= (size( ) + size( ) / 4))
		{
			clear_not_free( );
		}
		else
		{
			u32 old = size( );
			clear_and_free( );
			reserve(old);
		}
	}

#ifdef M_DONTDEFERCLEAR_EXT
	void	clear( )
	{
		clear_and_free( );
	}
#else
	void	clear( )
	{
		clear_not_free( );
	}
#endif

	const_reference operator[]	(size_type _Pos) const
	{
		{
			VERIFY(_Pos < size( ));
		} return (*(begin( ) + _Pos));
	}
	reference operator[]		(size_type _Pos)
	{
		{
			VERIFY(_Pos < size( ));
		} return (*(begin( ) + _Pos));
	}
};

// vector<bool>
template <>
class xr_vector<bool, xalloc<bool>> : public std::vector<bool, xalloc<bool>>
{
private:
	typedef std::vector<bool, xalloc<bool>> inherited;

public:
	u32		size( ) const
	{
		return (u32)inherited::size( );
	}
	void	clear( )
	{
		erase(begin( ), end( ));
	}
};

template <typename allocator>
class xr_vector<bool, allocator> : public std::vector<bool, allocator>
{
private:
	typedef std::vector<bool, allocator> inherited;

public:
	u32		size( ) const
	{
		return (u32)inherited::size( );
	}
	void	clear( )
	{
		erase(begin( ), end( ));
	}
};

// deque
template <typename T, typename allocator = xalloc<T> >
class xr_deque : public std::deque<T, allocator>
{
public:
	typedef typename allocator								allocator_type;
	typedef typename allocator_type::value_type				value_type;
	typedef typename allocator_type::size_type				size_type;
	u32		size( ) const
	{
		return (u32)__super::size( );
	}
};

// stack
template <typename _Ty, class _C = xr_vector<_Ty> >
class xr_stack
{
public:
	typedef typename _C::allocator_type						allocator_type;
	typedef typename allocator_type::value_type				value_type;
	typedef typename allocator_type::size_type				size_type;

	//explicit			stack(const allocator_type& _Al = allocator_type()) : c(_Al) {}
	allocator_type		get_allocator( ) const
	{
		return c.get_allocator( );
	}
	bool				empty( ) const
	{
		return c.empty( );
	}
	u32					size( ) const
	{
		return c.size( );
	}
	value_type& top( )
	{
		return c.back( );
	}
	const value_type& top( ) const
	{
		return c.back( );
	}
	void				push(const value_type& _X)
	{
		c.push_back(_X);
	}
	void				pop( )
	{
		c.pop_back( );
	}
	bool				operator==		(const xr_stack<_Ty, _C>& _X) const
	{
		return (c == _X.c);
	}
	bool				operator!=		(const xr_stack<_Ty, _C>& _X) const
	{
		return (!(*this == _X));
	}
	bool				operator<		(const xr_stack<_Ty, _C>& _X) const
	{
		return (c < _X.c);
	}
	bool				operator>		(const xr_stack<_Ty, _C>& _X) const
	{
		return (_X < *this);
	}
	bool				operator<=		(const xr_stack<_Ty, _C>& _X) const
	{
		return (!(_X < *this));
	}
	bool				operator>=		(const xr_stack<_Ty, _C>& _X) const
	{
		return (!(*this < _X));
	}

protected:
	_C c;
};

template	<typename T, typename allocator = xalloc<T> >									class	xr_list : public std::list<T, allocator>
{
public: u32 size( ) const
{
	return (u32)__super::size( );
}
};
template	<typename K, class P = std::less<K>, typename allocator = xalloc<K> >				class	xr_set : public std::set<K, P, allocator>
{
public: u32 size( ) const
{
	return (u32)__super::size( );
}
};
template	<typename K, class P = std::less<K>, typename allocator = xalloc<K> >				class	xr_multiset : public std::multiset<K, P, allocator>
{
public: u32 size( ) const
{
	return (u32)__super::size( );
}
};
template	<typename K, class V, class P = std::less<K>, typename allocator = xalloc<std::pair<K, V> > >	class	xr_map : public std::map<K, V, P, allocator>
{
public: u32 size( ) const
{
	return (u32)__super::size( );
}
};
template	<typename K, class V, class P = std::less<K>, typename allocator = xalloc<std::pair<K, V> > >	class	xr_multimap : public std::multimap<K, V, P, allocator>
{
public: u32 size( ) const
{
	return (u32)__super::size( );
}
};


template	<class _Ty1, class _Ty2> inline	std::pair<_Ty1, _Ty2>		mk_pair(_Ty1 _Val1, _Ty2 _Val2)
{
	return (std::pair<_Ty1, _Ty2>(_Val1, _Val2));
}

struct pred_str : public std::binary_function<pstr, pstr, bool>
{
	IC bool operator()(pcstr x, pcstr y) const
	{
		return xr_strcmp(x, y) < 0;
	}
};
struct pred_stri : public std::binary_function<pstr, pstr, bool>
{
	IC bool operator()(pcstr x, pcstr y) const
	{
		return stricmp(x, y) < 0;
	}
};

// STL extensions
#define DEF_VECTOR(N,T)				typedef xr_vector< T > N;		typedef N::iterator N##_it;
#define DEF_LIST(N,T)				typedef xr_list< T > N;			typedef N::iterator N##_it;
#define DEF_DEQUE(N,T)				typedef xr_deque< T > N;		typedef N::iterator N##_it;
#define DEF_MAP(N,K,T)				typedef xr_map< K, T > N;		typedef N::iterator N##_it;

#define DEFINE_DEQUE(T,N,I)			typedef xr_deque< T > N;		typedef N::iterator I;
#define DEFINE_LIST(T,N,I)			typedef xr_list< T > N;			typedef N::iterator I;
#define DEFINE_VECTOR(T,N,I)		typedef xr_vector< T > N;		typedef N::iterator I;
#define DEFINE_MAP(K,T,N,I)			typedef xr_map< K , T > N;		typedef N::iterator I;
#define DEFINE_MAP_PRED(K,T,N,I,P)	typedef xr_map< K, T, P > N;	typedef N::iterator I;
#define DEFINE_MMAP(K,T,N,I)		typedef xr_multimap< K, T > N;	typedef N::iterator I;
#define DEFINE_SVECTOR(T,C,N,I)		typedef svector< T, C > N;		typedef N::iterator I;
#define DEFINE_SET(T,N,I)			typedef xr_set< T > N;			typedef N::iterator I;
#define DEFINE_SET_PRED(T,N,I,P)	typedef xr_set< T, P > N;		typedef N::iterator I;
#define DEFINE_STACK(T,N)			typedef xr_stack< T > N;

#include "FixedVector.h"

// auxilary definition
DEFINE_VECTOR(bool, boolVec, boolIt);
DEFINE_VECTOR(BOOL, BOOLVec, BOOLIt);
DEFINE_VECTOR(BOOL*, LPBOOLVec, LPBOOLIt);
DEFINE_VECTOR(fRect, FrectVec, FrectIt);
DEFINE_VECTOR(iRect, IrectVec, IrectIt);
DEFINE_VECTOR(fPlane3, PlaneVec, PlaneIt);
DEFINE_VECTOR(fVector2, Fvector2Vec, Fvector2It);
DEFINE_VECTOR(fVector3, FvectorVec, FvectorIt);
DEFINE_VECTOR(fVector3*, LPFvectorVec, LPFvectorIt);
DEFINE_VECTOR(fColor, FcolorVec, FcolorIt);
DEFINE_VECTOR(fColor*, LPFcolorVec, LPFcolorIt);
DEFINE_VECTOR(pstr, LPSTRVec, LPSTRIt);
DEFINE_VECTOR(pcstr, LPCSTRVec, LPCSTRIt);
//DEFINE_VECTOR(string64,string64Vec,string64It);
DEFINE_VECTOR(xr_string, SStringVec, SStringVecIt);

DEFINE_VECTOR(s8, S8Vec, S8It);
DEFINE_VECTOR(s8*, LPS8Vec, LPS8It);
DEFINE_VECTOR(s16, S16Vec, S16It);
DEFINE_VECTOR(s16*, LPS16Vec, LPS16It);
DEFINE_VECTOR(s32, S32Vec, S32It);
DEFINE_VECTOR(s32*, LPS32Vec, LPS32It);
DEFINE_VECTOR(u8, U8Vec, U8It);
DEFINE_VECTOR(u8*, LPU8Vec, LPU8It);
DEFINE_VECTOR(u16, U16Vec, U16It);
DEFINE_VECTOR(u16*, LPU16Vec, LPU16It);
DEFINE_VECTOR(u32, U32Vec, U32It);
DEFINE_VECTOR(u32*, LPU32Vec, LPU32It);
DEFINE_VECTOR(f32, FloatVec, FloatIt);
DEFINE_VECTOR(f32*, LPFloatVec, LPFloatIt);
DEFINE_VECTOR(int, IntVec, IntIt);
DEFINE_VECTOR(int*, LPIntVec, LPIntIt);
