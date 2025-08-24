// PostProcessEffector.cpp: implementation of the CPostProcessEffector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PostProcessEffector.h"
#include "CameraManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPostProcessEffector::CPostProcessEffector(EEffectorPPType type, f32 lifeTime, bool free_on_remove)
{
	eType = type;
	fLifeTime = lifeTime;
	bFreeOnRemove = free_on_remove;
}

CPostProcessEffector::~CPostProcessEffector( )
{ }

BOOL CPostProcessEffector::Process(SPPInfo& PPInfo)
{
	fLifeTime -= Device.fTimeDelta;
	return TRUE;
}
