#include "stdafx.h"

void	__stdcall	xrMemFill_x86(pvoid dest, int value, u32 count)
{
	memset(dest, int(value), count);
}
