#pragma once

#define DUMP_PHASE	do {} while (0)

#include "xrMemory_pso.h"
#include "xrMemory_POOL.h"

class CORE_API		xrMemory
{
public:
	struct				mdbg
	{
		Pvoid _p;
		size_t 			_size;
		const char* _name;
		U32				_dummy;
	};

	xrMemory( );
	void				_initialize(bool bDebug = false);
	void				_destroy( );

	U32					stat_calls;
	int					stat_counter;

	void				dbg_register(Pvoid _p, size_t _size, const char* _name);
	void				dbg_unregister(Pvoid _p);
	void				dbg_check( );

	U32					mem_usage(U32* pBlocksUsed = nullptr, U32* pBlocksFree = nullptr);
	void				mem_compact( );
	void				mem_counter_set(U32 _val)
	{
		stat_counter = _val;
	}
	U32					mem_counter_get( )
	{
		return stat_counter;
	}

	Pvoid mem_alloc(size_t	size);
	Pvoid mem_realloc(Pvoid p, size_t size);
	void				mem_free(Pvoid p);

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
inline T* xr_alloc(U32 count)
{
	return  (T*) Memory.mem_alloc(count * sizeof(T));
}

template <class T>
inline void		xr_free(T*& P)
{
	if (P)
	{
		Memory.mem_free((Pvoid) P);
		P = nullptr;
	}
}

inline Pvoid xr_malloc(size_t size)
{
	return	Memory.mem_alloc(size);
}

inline Pvoid xr_realloc(Pvoid P, size_t size)
{
	return Memory.mem_realloc(P, size);
}

CORE_API	char* xr_strdup(const char* string);

inline Pvoid operator new		(size_t size)
{
	return Memory.mem_alloc(size ? size : 1);
}

inline void		operator delete		(Pvoid p)
{
	xr_free(p);
}

inline Pvoid operator new[ ] (size_t size)
{
	return Memory.mem_alloc(size ? size : 1);
}

inline void		operator delete[ ] (Pvoid p)
{
	xr_free(p);
}

// POOL-ing
const		U32			mem_pools_count = 54;
const		U32			mem_pools_ebase = 16;
const		U32			mem_generic = mem_pools_count + 1;
extern		MEMPOOL		mem_pools[mem_pools_count];
extern		bool		mem_initialized;

CORE_API void vminfo(size_t* _free, size_t* reserved, size_t* committed);
CORE_API void log_vminfo( );

CORE_API U32	mem_usage_impl(U32* pBlocksUsed, U32* pBlocksFree);
