#include "stdafx.h"

#include "xrMemory_align.h"
#include "xrMemory_pure.h"

#define	debug_mode 0

MEMPOOL		mem_pools[mem_pools_count];

// MSVC
__forceinline	unsigned char* acc_header(void* P)
{
	unsigned char* _P = (unsigned char*) P;
	return	_P - 1;
}
__forceinline	U32		get_header(void* P)
{
	return	(U32) *acc_header(P);
}
__forceinline	U32		get_pool(size_t size)
{
	U32		pid = U32(size / mem_pools_ebase);
	if (pid >= mem_pools_count)	return mem_generic;
	else						return pid;
}

#ifdef PURE_ALLOC
bool	g_use_pure_alloc = false;
#endif // PURE_ALLOC

void* xrMemory::mem_alloc(size_t size)
{
	stat_calls++;

#ifdef PURE_ALLOC
	static bool g_use_pure_alloc_initialized = false;
	if (!g_use_pure_alloc_initialized)
	{
		g_use_pure_alloc_initialized = true;
		g_use_pure_alloc = !!strstr(GetCommandLine( ), "-pure_alloc");
	}

	if (g_use_pure_alloc)
	{
		void* result = malloc(size);
		return							(result);
	}
#endif // PURE_ALLOC

	U32		_footer = debug_mode ? 4 : 0;
	void* _ptr = nullptr;

	//
	if (!mem_initialized)
	{
		// generic
		// Reserve 1 byte for xrMemory header
		void* _real = xr_aligned_offset_malloc(1 + size + _footer, 16, 0x1);
		_ptr = (void*) (((unsigned char*) _real) + 1);
		*acc_header(_ptr) = mem_generic;
	}
	else
	{
		// accelerated
		// Reserve 1 byte for xrMemory header
		U32 pool = get_pool(1 + size + _footer);
		//U32	pool				=	get_pool	(size+_footer);
		if (mem_generic == pool)
		{
			// generic
			// Reserve 1 byte for xrMemory header
			void* _real = xr_aligned_offset_malloc(1 + size + _footer, 16, 0x1);
			_ptr = (void*) (((unsigned char*) _real) + 1);
			*acc_header(_ptr) = mem_generic;
		}
		else
		{
			// pooled
			// Reserve 1 byte for xrMemory header
			// Already reserved when getting pool id
			void* _real = mem_pools[pool].create( );
			_ptr = (void*) (((unsigned char*) _real) + 1);
			*acc_header(_ptr) = (unsigned char) pool;
		}
	}

	return	_ptr;
}

void	xrMemory::mem_free(void* P)
{
	stat_calls++;

#ifdef PURE_ALLOC
	if (g_use_pure_alloc)
	{
		free(P);
		return;
	}
#endif // PURE_ALLOC

	if (debug_mode)
	{
		dbg_unregister(P);
	}

	U32 pool = get_header(P);
	void* _real = (void*) (((unsigned char*) P) - 1);
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

extern BOOL g_bDbgFillMemory;

void* xrMemory::mem_realloc(void* P, size_t size)
{
	stat_calls++;

#ifdef PURE_ALLOC
	if (g_use_pure_alloc)
	{
		void* result = realloc(P, size);
		return result;
	}
#endif // def PURE_ALLOC

	if (0 == P)
	{
		return mem_alloc(size);
	}

	U32 p_current = get_header(P);
	// Reserve 1 byte for xrMemory header
	U32 p_new = get_pool(1 + size + (debug_mode ? 4 : 0));
//	U32 p_new = get_pool(size + (debug_mode ? 4 : 0));
	U32 p_mode;

	if (mem_generic == p_current)
	{
		if (p_new < p_current)
		{
			p_mode = 2;
		}
		else
		{
			p_mode = 0;
		}
	}
	else
	{
		p_mode = 1;
	}

	void* _real = (void*) (((unsigned char*) P) - 1);
	void* _ptr = nullptr;
	if (0 == p_mode)
	{
		U32 _footer = debug_mode ? 4 : 0;
		// Reserve 1 byte for xrMemory header
		void* _real2 = xr_aligned_offset_realloc(_real, 1 + size + _footer, 16, 0x1);
		_ptr = (void*) (((unsigned char*) _real2) + 1);
		*acc_header(_ptr) = mem_generic;
	}
	else if (1 == p_mode)
	{
		// pooled realloc
		R_ASSERT2(p_current < mem_pools_count, "Memory corruption");
		U32		s_current = mem_pools[p_current].get_element( );
		U32		s_dest = (U32) size;
		void* p_old = P;

		void* p_new = mem_alloc(size);
		// Reserve 1 byte for xrMemory header
		// Don't bother in this case?
		mem_copy(p_new, p_old, _min(s_current - 1, s_dest));
		mem_free(p_old);
		_ptr = p_new;
	}
	else if (2 == p_mode)
	{
		// relocate into another mmgr(pooled) from real
		void* p_old = P;
		void* p_new = mem_alloc(size);
		mem_copy(p_new, p_old, (U32) size);
		mem_free(p_old);
		_ptr = p_new;
	}
	return	_ptr;
}
