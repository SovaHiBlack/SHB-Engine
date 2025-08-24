#pragma once

#include "Camera_defs.h"
struct SPPInfo;

// постпроцесс
class ENGINE_API CPostProcessEffector
{
	EEffectorPPType		eType;
	bool				bFreeOnRemove;

protected:
	f32				fLifeTime;

public:
	CPostProcessEffector(EEffectorPPType type, f32 lifeTime, bool free_on_remove = true);
	CPostProcessEffector( ) : bFreeOnRemove(true), fLifeTime(0.0f)
	{ }
	virtual				~CPostProcessEffector( );
	virtual BOOL		Process(SPPInfo& PPInfo);
	virtual BOOL		Valid( )
	{
		return fLifeTime > 0.0f;
	}
	IC EEffectorPPType	Type( ) const
	{
		return eType;
	}
	IC bool				FreeOnRemove( ) const
	{
		return bFreeOnRemove;
	}
	IC void				SetType(EEffectorPPType t)
	{
		eType = t;
	}
	virtual void		Stop(f32 speed)
	{
		fLifeTime = 0.0f;
	}
};
