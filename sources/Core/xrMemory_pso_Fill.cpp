#include "stdafx.h"

void	__stdcall	xrMemFill_x86(void* dest, int value, unsigned int count)
{
	memset(dest, int(value), count);
}
