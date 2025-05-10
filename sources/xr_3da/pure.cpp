#include "stdafx.h"

#include "pure.h"

ENGINE_API s32	__cdecl	_REG_Compare(pcvoid e1, pcvoid e2)
{
	_REG_INFO *p1 = (_REG_INFO *)e1;
	_REG_INFO *p2 = (_REG_INFO *)e2;
	return (p2->Prio - p1->Prio);
}

DECLARE_RP(Frame);
DECLARE_RP(Render);
DECLARE_RP(AppActivate);
DECLARE_RP(AppDeactivate);
DECLARE_RP(AppStart);
DECLARE_RP(AppEnd);
DECLARE_RP(DeviceReset);
