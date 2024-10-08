#pragma once

#include "memory_monitor.h"

#ifdef USE_MEMORY_MONITOR
#	define DEBUG_MEMORY_NAME
#endif // USE_MEMORY_MONITOR

#define DUMP_PHASE	do {} while (0)

#include "xrMemory_pso.h"
#include "xrMemory_POOL.h"

class XRCORE_API		xrMemory
{
public:
	struct				mdbg
	{
		pvoid _p;
		size_t 			_size;
		pcstr		_name;
		u32				_dummy;
	};
public:
	xrMemory();
	void				_initialize(BOOL _debug_mode = FALSE);
	void				_destroy();

	u32					stat_calls;
	s32					stat_counter;
public:
	void				dbg_register(pvoid _p, size_t _size, pcstr _name);
	void				dbg_unregister(pvoid _p);
	void				dbg_check();

	u32					mem_usage(u32* pBlocksUsed = NULL, u32* pBlocksFree = NULL);
	void				mem_compact();
	void				mem_counter_set(u32 _val)
	{
		stat_counter = _val;
	}
	u32					mem_counter_get()
	{
		return stat_counter;
	}

#ifdef DEBUG_MEMORY_NAME
	void				mem_statistic(pcstr fn);
	pvoid mem_alloc(size_t	size, pcstr _name);
	pvoid mem_realloc(pvoid p, size_t size, pcstr _name);
#else // DEBUG_MEMORY_NAME
	pvoid mem_alloc(size_t	size);
	pvoid mem_realloc(pvoid p, size_t size);
#endif // DEBUG_MEMORY_NAME
	void				mem_free(void* p);

	pso_MemCopy* mem_copy;
	pso_MemFill* mem_fill;
	pso_MemFill32* mem_fill32;
};

extern XRCORE_API	xrMemory	Memory;

#undef	ZeroMemory
#undef	CopyMemory
#undef	FillMemory
#define ZeroMemory(a,b)		Memory.mem_fill(a,0,b)
#define CopyMemory(a,b,c)	memcpy(a,b,c)			//. CopyMemory(a,b,c)
#define FillMemory(a,b,c)	Memory.mem_fill(a,c,b)

// delete
#include "xrMemory_subst_msvc.h"

// generic "C"-like allocations/deallocations
#ifdef DEBUG_MEMORY_NAME
template <class T>
IC T* xr_alloc(u32 count)
{
	return  (T*)Memory.mem_alloc(count * sizeof(T), typeid(T).name());
}
template <class T>
IC void		xr_free(T*& P)
{
	if (P)
	{
		Memory.mem_free((pvoid)P); P = NULL;
	};
}
IC pvoid xr_malloc(size_t size)
{
	return	Memory.mem_alloc(size, "xr_malloc");
}
IC pvoid xr_realloc(pvoid P, size_t size)
{
	return Memory.mem_realloc(P, size, "xr_realloc");
}
#else // DEBUG_MEMORY_NAME
template <class T>
IC T* xr_alloc(u32 count)
{
	return  (T*)Memory.mem_alloc(count * sizeof(T));
}
template <class T>
IC void		xr_free(T*& P)
{
	if (P)
	{
		Memory.mem_free((pvoid)P); P = NULL;
	};
}
IC pvoid xr_malloc(size_t size)
{
	return	Memory.mem_alloc(size);
}
IC pvoid xr_realloc(pvoid P, size_t size)
{
	return Memory.mem_realloc(P, size);
}
#endif // DEBUG_MEMORY_NAME

XRCORE_API	pstr xr_strdup(pcstr string);

#ifdef DEBUG_MEMORY_NAME
// Global new/delete override
IC pvoid operator new		(size_t size)
{
	return Memory.mem_alloc(size ? size : 1, "C++ NEW");
}
IC void		operator delete		(pvoid p)
{
	xr_free(p);
}
IC pvoid operator new[](size_t size)
{
	return Memory.mem_alloc(size ? size : 1, "C++ NEW");
}
IC void		operator delete[](pvoid p)
{
	xr_free(p);
}
#else // DEBUG_MEMORY_NAME
IC pvoid operator new		(size_t size)
{
	return Memory.mem_alloc(size ? size : 1);
}
IC void		operator delete		(pvoid p)
{
	xr_free(p);
}
IC pvoid operator new[](size_t size)
{
	return Memory.mem_alloc(size ? size : 1);
}
IC void		operator delete[](pvoid p)
{
	xr_free(p);
}
#endif // DEBUG_MEMORY_NAME

// POOL-ing
const		u32			mem_pools_count = 54;
const		u32			mem_pools_ebase = 16;
const		u32			mem_generic = mem_pools_count + 1;
extern		MEMPOOL		mem_pools[mem_pools_count];
extern		BOOL		mem_initialized;

XRCORE_API void vminfo(size_t* _free, size_t* reserved, size_t* committed);
XRCORE_API void log_vminfo();

XRCORE_API u32	mem_usage_impl(u32* pBlocksUsed, u32* pBlocksFree);
