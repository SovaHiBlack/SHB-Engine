#include "stdafx.h"

void	__stdcall	xrSlerp_x86		(fQuaternion* D, fQuaternion* Q1, fQuaternion* Q2, f32 t)
{
	D->slerp(*Q1,*Q2,t);
}
