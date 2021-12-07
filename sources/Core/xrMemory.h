#pragma once

#define DUMP_PHASE	do {} while (0)

#include "xrMemory_pso.h"
#include "xrMemory_POOL.h"

class CORE_API		xrMemory
{
public:
	struct				mdbg
	{
		void* _p;
		size_t 			_size;
		const char* _name;
		unsigned int				_dummy;
	};

	xrMemory( );
	void				_initialize(bool bDebug = false);
	void				_destroy( );

	unsigned int					stat_calls;
	int					stat_counter;

	void				dbg_register(void* _p, size_t _size, const char* _name);
	void				dbg_unregister(void* _p);
	void				dbg_check( );

	unsigned int					mem_usage(unsigned int* pBlocksUsed = nullptr, unsigned int* pBlocksFree = nullptr);
	void				mem_compact( );
	void				mem_counter_set(unsigned int _val)
	{
		stat_counter = _val;
	}
	unsigned int					mem_counter_get( )
	{
		return stat_counter;
	}

	void* mem_alloc(size_t	size);
	void* mem_realloc(void* p, size_t size);
	void				mem_free(void* p);

	pso_MemCopy* mem_copy;
	pso_MemFill* mem_fill;
	pso_MemFill32* mem_fill32;
};

extern CORE_API	xrMemory	Memory;

#undef	ZeroMemory
#undef	CopyMemory
#undef	FillMemory
#define ZeroMemory(a,b)		Memory.mem_fill(a,0,b)
#define CopyMemory(a,b,c)	memcpy(a,b,c)			//. CopyMemory(a,b,c)
#define FillMemory(a,b,c)	Memory.mem_fill(a,c,b)

// delete
#include "xrMemory_subst_msvc.h"

// generic "C"-like allocations/deallocations
template <class T>
inline T* xr_alloc(unsigned int count)
{
	return  (T*) Memory.mem_alloc(count * sizeof(T));
}

template <class T>
inline void		xr_free(T*& P)
{
	if (P)
	{
		Memory.mem_free((void*) P);
		P = nullptr;
	}
}

inline void* xr_malloc(size_t size)
{
	return	Memory.mem_alloc(size);
}

inline void* xr_realloc(void* P, size_t size)
{
	return Memory.mem_realloc(P, size);
}

CORE_API	char* xr_strdup(const char* string);

inline void* operator new		(size_t size)
{
	return Memory.mem_alloc(size ? size : 1);
}

inline void		operator delete		(void* p)
{
	xr_free(p);
}

inline void* operator new[ ] (size_t size)
{
	return Memory.mem_alloc(size ? size : 1);
}

inline void		operator delete[ ] (void* p)
{
	xr_free(p);
}

// POOL-ing
const		unsigned int			mem_pools_count = 54;
const		unsigned int			mem_pools_ebase = 16;
const		unsigned int			mem_generic = mem_pools_count + 1;
extern		MEMPOOL		mem_pools[mem_pools_count];
extern		bool		mem_initialized;

CORE_API void vminfo(size_t* _free, size_t* reserved, size_t* committed);
CORE_API void log_vminfo( );

CORE_API unsigned int	mem_usage_impl(unsigned int* pBlocksUsed, unsigned int* pBlocksFree);
