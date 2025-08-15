#include "stdafx.h"

#include "xrMemory_align.h"
#include "xrMemory_pure.h"

#define	debug_mode 0

MEMPOOL		mem_pools[mem_pools_count];

// MSVC
ICF	u8* acc_header(pvoid P)
{
	u8* _P = (u8*)P;	return	_P - 1;
}
ICF	u32		get_header(pvoid P)
{
	return	(u32)*acc_header(P);
}
ICF	u32		get_pool(size_t size)
{
	u32		pid = u32(size / mem_pools_ebase);
	if (pid >= mem_pools_count)	return mem_generic;
	else						return pid;
}

#ifdef PURE_ALLOC
bool	g_use_pure_alloc = false;
#endif // PURE_ALLOC

pvoid xrMemory::mem_alloc(size_t size
#	ifdef DEBUG_MEMORY_NAME
						  , pcstr _name
#	endif // DEBUG_MEMORY_NAME
)
{
	stat_calls++;

#ifdef PURE_ALLOC
	static bool g_use_pure_alloc_initialized = false;
	if (!g_use_pure_alloc_initialized)
	{
		g_use_pure_alloc_initialized = true;
		g_use_pure_alloc = !!strstr(GetCommandLine(), "-pure_alloc");
	}

	if (g_use_pure_alloc)
	{
		pvoid result = malloc(size);
#ifdef USE_MEMORY_MONITOR
		memory_monitor::monitor_alloc(result, size, _name);
#endif // USE_MEMORY_MONITOR
		return							(result);
	}
#endif // PURE_ALLOC

	u32		_footer = debug_mode ? 4 : 0;
	pvoid _ptr = 0;

	//
	if (!mem_initialized /*|| debug_mode*/)
	{
		// generic
		//	Igor: Reserve 1 byte for xrMemory header
		pvoid _real = xr_aligned_offset_malloc(1 + size + _footer, 16, 0x1);
		//pvoid	_real			=	xr_aligned_offset_malloc	(size + _footer, 16, 0x1);
		_ptr = (pvoid)(((u8*)_real) + 1);
		*acc_header(_ptr) = mem_generic;
	}
	else
	{
		//	accelerated
		//	Igor: Reserve 1 byte for xrMemory header
		u32	pool = get_pool(1 + size + _footer);
		//u32	pool				=	get_pool	(size+_footer);
		if (mem_generic == pool)
		{
			// generic
			//	Igor: Reserve 1 byte for xrMemory header
			pvoid _real = xr_aligned_offset_malloc(1 + size + _footer, 16, 0x1);
			//pvoid	_real		=	xr_aligned_offset_malloc	(size + _footer,16,0x1);
			_ptr = (pvoid)(((u8*)_real) + 1);
			*acc_header(_ptr) = mem_generic;
		}
		else
		{
			// pooled
			//	Igor: Reserve 1 byte for xrMemory header
			//	Already reserved when getting pool id
			pvoid _real = mem_pools[pool].create();
			_ptr = (pvoid)(((u8*)_real) + 1);
			*acc_header(_ptr) = (u8)pool;
		}
	}

#ifdef USE_MEMORY_MONITOR
	memory_monitor::monitor_alloc(_ptr, size, _name);
#endif // USE_MEMORY_MONITOR
	return	_ptr;
}

void	xrMemory::mem_free(pvoid P)
{
	stat_calls++;
#ifdef USE_MEMORY_MONITOR
	memory_monitor::monitor_free(P);
#endif // USE_MEMORY_MONITOR

#ifdef PURE_ALLOC
	if (g_use_pure_alloc)
	{
		free(P);
		return;
	}
#endif // PURE_ALLOC

	if (debug_mode)		dbg_unregister(P);
	u32	pool = get_header(P);
	pvoid _real = (pvoid)(((u8*)P) - 1);
	if (mem_generic == pool)
	{
		// generic
		xr_aligned_free(_real);
	}
	else
	{
		// pooled
		VERIFY2(pool < mem_pools_count, "Memory corruption");
		mem_pools[pool].destroy(_real);
	}
}

extern BOOL	g_bDbgFillMemory;

pvoid xrMemory::mem_realloc(pvoid P, size_t size
#ifdef DEBUG_MEMORY_NAME
							, pcstr _name
#endif // DEBUG_MEMORY_NAME
)
{
	stat_calls++;
#ifdef PURE_ALLOC
	if (g_use_pure_alloc)
	{
		pvoid result = realloc(P, size);
#	ifdef USE_MEMORY_MONITOR
		memory_monitor::monitor_free(P);
		memory_monitor::monitor_alloc(result, size, _name);
#	endif // USE_MEMORY_MONITOR
		return							(result);
	}
#endif // PURE_ALLOC
	if (0 == P)
	{
		return mem_alloc(size
#	ifdef DEBUG_MEMORY_NAME
						 , _name
#	endif // DEBUG_MEMORY_NAME
		);
	}

	u32		p_current = get_header(P);
	//	Igor: Reserve 1 byte for xrMemory header
	u32		p_new = get_pool(1 + size + (debug_mode ? 4 : 0));
	//u32		p_new				= get_pool	(size+(debug_mode?4:0));
	u32		p_mode;

	if (mem_generic == p_current)
	{
		if (p_new < p_current)		p_mode = 2;
		else						p_mode = 0;
	}
	else 							p_mode = 1;

	pvoid _real = (pvoid)(((u8*)P) - 1);
	pvoid _ptr = NULL;
	if (0 == p_mode)
	{
		u32		_footer = debug_mode ? 4 : 0;
		//	Igor: Reserve 1 byte for xrMemory header
		pvoid _real2 = xr_aligned_offset_realloc(_real, 1 + size + _footer, 16, 0x1);
		//pvoid	_real2			=	xr_aligned_offset_realloc	(_real,size+_footer,16,0x1);
		_ptr = (pvoid)(((u8*)_real2) + 1);
		*acc_header(_ptr) = mem_generic;

#ifdef USE_MEMORY_MONITOR
		memory_monitor::monitor_free(P);
		memory_monitor::monitor_alloc(_ptr, size, _name);
#endif // USE_MEMORY_MONITOR
	}
	else if (1 == p_mode)
	{
		// pooled realloc
		R_ASSERT2(p_current < mem_pools_count, "Memory corruption");
		u32		s_current = mem_pools[p_current].get_element();
		u32		s_dest = (u32)size;
		pvoid p_old = P;

		pvoid p_new = mem_alloc(size
#ifdef DEBUG_MEMORY_NAME
								, _name
#endif // DEBUG_MEMORY_NAME
		);
		//	Igor: Reserve 1 byte for xrMemory header
		//	Don't bother in this case?
		mem_copy(p_new, p_old, _min(s_current - 1, s_dest));
		//mem_copy				(p_new,p_old,_min(s_current,s_dest));
		mem_free(p_old);
		_ptr = p_new;
	}
	else if (2 == p_mode)
	{
		// relocate into another mmgr(pooled) from real
		pvoid p_old = P;
		pvoid p_new = mem_alloc(size
#	ifdef DEBUG_MEMORY_NAME
								, _name
#	endif // DEBUG_MEMORY_NAME
		);
		mem_copy(p_new, p_old, (u32)size);
		mem_free(p_old);
		_ptr = p_new;
	}
	return	_ptr;
}
