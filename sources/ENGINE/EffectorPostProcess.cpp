#include "stdafx.h"

#include "EffectorPostProcess.h"
//#include "CameraManager.h"

CEffectorPostProcess::CEffectorPostProcess(EEffectorPostProcessType type, float lifeTime, bool free_on_remove)
{
	eType			= type;
	fLifeTime		= lifeTime;
	bFreeOnRemove	= free_on_remove;
}

CEffectorPostProcess::~CEffectorPostProcess() {}

BOOL CEffectorPostProcess::Process(SPostProcessInfo& PPInfo)
{
	fLifeTime		-= Device.fTimeDelta;
	return TRUE;
}
