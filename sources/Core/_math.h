#pragma once

#include "cpuid.h"

namespace FPU
{
	CORE_API void	 m24(void);
	CORE_API void	 m24r(void);
	CORE_API void	 m53(void);
	CORE_API void	 m53r(void);
	CORE_API void	 m64(void);
	CORE_API void	 m64r(void);
};
namespace CPU
{
	CORE_API extern u64				clk_per_second;
	CORE_API extern u64				clk_per_milisec;
	CORE_API extern u64				clk_per_microsec;
	CORE_API extern u64				clk_overhead;
	CORE_API extern float			clk_to_seconds;
	CORE_API extern float			clk_to_milisec;
	CORE_API extern float			clk_to_microsec;

	CORE_API extern u64				qpc_freq;
	CORE_API extern u64				qpc_overhead;
	CORE_API extern U32				qpc_counter;

	CORE_API extern	_processor_info	ID;
	CORE_API extern	u64				QPC( );

#pragma warning(disable:4035)
	inline u64	GetCLK(void)
	{
		_asm    _emit 0x0F;
		_asm    _emit 0x31;
	}
#pragma warning(default:4035)
};

extern CORE_API	void	_initialize_cpu( );
extern CORE_API	void	_initialize_cpu_thread( );

// threading
typedef				void	thread_t(Pvoid);
extern CORE_API	void	thread_name(const char* name);
extern CORE_API	void	thread_spawn(
	thread_t* entry,
	const char* name,
	unsigned	stack,
	Pvoid arglist
);
