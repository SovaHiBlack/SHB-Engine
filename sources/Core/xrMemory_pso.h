#pragma once

typedef void	__stdcall	pso_MemFill(void* dest, int value, unsigned int count);
typedef void	__stdcall	pso_MemFill32(void* dest, unsigned int value, unsigned int count);
typedef void	__stdcall	pso_MemCopy(void* dest, const void* src, unsigned int count);
