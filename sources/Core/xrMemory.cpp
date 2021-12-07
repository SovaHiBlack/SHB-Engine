#include "stdafx.h"

#include "xrMemory_pure.h"
#include <malloc.h>

xrMemory	Memory;
bool		mem_initialized = false;
bool		shared_str_initialized = false;

// Processor specific implementations
extern pso_MemCopy		xrMemCopy_MMX;
extern pso_MemCopy		xrMemCopy_x86;
extern pso_MemFill		xrMemFill_x86;
extern pso_MemFill32	xrMemFill32_MMX;
extern pso_MemFill32	xrMemFill32_x86;

xrMemory::xrMemory( )
{
	mem_copy = xrMemCopy_x86;
	mem_fill = xrMemFill_x86;
	mem_fill32 = xrMemFill32_x86;
}

void xrMemory::_initialize(bool bDebug)
{
	stat_calls = 0;
	stat_counter = 0;

	unsigned int features = CPU::ID.feature & CPU::ID.os_support;
	if (features & _CPU_FEATURE_MMX)
	{
		mem_copy = xrMemCopy_MMX;
		mem_fill = xrMemFill_x86;
		mem_fill32 = xrMemFill32_MMX;
	}
	else
	{
		mem_copy = xrMemCopy_x86;
		mem_fill = xrMemFill_x86;
		mem_fill32 = xrMemFill32_x86;
	}

	if (!strstr(Core.Params, "-pure_alloc"))
	{
		// initialize POOLs
		unsigned int element = mem_pools_ebase;
		unsigned int sector = mem_pools_ebase * 1024;
		for (unsigned int pid = 0; pid < mem_pools_count; pid++)
		{
			mem_pools[pid]._initialize(element, sector, 0x1);
			element += mem_pools_ebase;
		}
	}

	mem_initialized = true;

	g_pStringContainer = xr_new<CStringContainer>( );
	shared_str_initialized = true;

	g_pSharedMemoryContainer = xr_new<smem_container>( );
}

void xrMemory::_destroy( )
{
	xr_delete(g_pSharedMemoryContainer);
	xr_delete(g_pStringContainer);

	mem_initialized = false;
}

void xrMemory::mem_compact( )
{
	RegFlushKey(HKEY_CLASSES_ROOT);
	RegFlushKey(HKEY_CURRENT_USER);
	_heapmin( );
	HeapCompact(GetProcessHeap( ), 0);
	if (g_pStringContainer)
	{
		g_pStringContainer->clean( );
	}

	if (g_pSharedMemoryContainer)
	{
		g_pSharedMemoryContainer->clean( );
	}

	if (strstr(Core.Params, "-swap_on_compact"))
	{
		SetProcessWorkingSetSize(GetCurrentProcess( ), size_t(-1), size_t(-1));
	}
}

char* xr_strdup(const char* string)
{
	VERIFY(string);
	unsigned int len = unsigned int(xr_strlen(string)) + 1;
	char* memory = (char*) Memory.mem_alloc(len);
	CopyMemory(memory, string, len);
	return	memory;
}

CORE_API		BOOL			is_stack_ptr(void* _ptr)
{
	int			local_value = 0;
	void* ptr_refsound = _ptr;
	void* ptr_local = &local_value;
	ptrdiff_t	difference = (ptrdiff_t) _abs(signed __int64(ptrdiff_t(ptr_local) - ptrdiff_t(ptr_refsound)));
	return		(difference < (512 * 1024));
}
