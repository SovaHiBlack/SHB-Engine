#include "stdafx.h"

void	__stdcall	xrMemFill_x86(void* dest, int value, U32 count)
{
	memset(dest, int(value), count);
}
