#pragma once

typedef void	__stdcall	pso_MemFill(pvoid dest, int value, u32 count);
typedef void	__stdcall	pso_MemFill32(pvoid dest, u32 value, u32 count);
typedef void	__stdcall	pso_MemCopy(pvoid dest, pcvoid src, u32 count);
