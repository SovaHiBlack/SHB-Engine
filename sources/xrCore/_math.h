#pragma once

#include "cpuid.h"

namespace FPU
{
	CORE_API void	 m24( );
	CORE_API void	 m24r( );
	CORE_API void	 m53( );
	CORE_API void	 m53r( );
	CORE_API void	 m64( );
	CORE_API void	 m64r( );
};
namespace CPU
{
	CORE_API extern u64				clk_per_second;
	CORE_API extern u64				clk_per_milisec;
	CORE_API extern u64				clk_per_microsec;
	CORE_API extern u64				clk_overhead;
	CORE_API extern f32				clk_to_seconds;
	CORE_API extern f32				clk_to_milisec;
	CORE_API extern f32				clk_to_microsec;

	CORE_API extern u64				qpc_freq;
	CORE_API extern u64				qpc_overhead;
	CORE_API extern u32				qpc_counter;

	CORE_API extern	_processor_info	ID;
	CORE_API extern	u64				QPC();

#pragma warning(disable:4035)
	IC u64	GetCLK( )
	{
		_asm    _emit 0x0F;
		_asm    _emit 0x31;
	}
#pragma warning(default:4035)
};

extern CORE_API	void	_initialize_cpu();
extern CORE_API	void	_initialize_cpu_thread();

// threading
typedef				void	thread_t(pvoid);
extern CORE_API	void	thread_name(pcstr name);
extern CORE_API	void	thread_spawn(
	thread_t* entry,
	pcstr	name,
	unsigned	stack,
	pvoid arglist
);
