#pragma once

#include "Camera_defs.h"

struct SPPInfo;

// постпроцесс
class ENGINE_API CEffectorPP
{
	EEffectorPostProcessType		eType;
	bool				bFreeOnRemove;
protected:
	float				fLifeTime;
public:
						CEffectorPP		(EEffectorPostProcessType type, float lifeTime, bool free_on_remove=true);
						CEffectorPP		():bFreeOnRemove(true),fLifeTime(0.0f){};
	virtual				~CEffectorPP	();
	virtual	BOOL		Process			(SPPInfo &PPInfo);
	virtual	BOOL		Valid			()							{return fLifeTime>0.0f;}
	IC EEffectorPostProcessType	Type			()	const					{return eType;}
	IC bool				FreeOnRemove	()	const					{return bFreeOnRemove;}
	IC void				SetType			(EEffectorPostProcessType t)			{eType=t;}
	virtual void		Stop            (float speed)				{fLifeTime=0.0f;};
};
