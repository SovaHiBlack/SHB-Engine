#pragma once

typedef void	__stdcall	pso_MemFill(void* dest, int value, U32 count);
typedef void	__stdcall	pso_MemFill32(void* dest, U32 value, U32 count);
typedef void	__stdcall	pso_MemCopy(void* dest, const void* src, U32 count);
