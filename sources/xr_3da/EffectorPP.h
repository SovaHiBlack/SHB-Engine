#pragma once

#include "CameraDefs.h"
struct SPPInfo;

// �����������
class ENGINE_API CEffectorPP
{
	EEffectorPPType		eType;
	bool				bFreeOnRemove;

protected:
	f32				fLifeTime;

public:
	CEffectorPP(EEffectorPPType type, f32 lifeTime, bool free_on_remove = true);
	CEffectorPP( ) : bFreeOnRemove(true), fLifeTime(0.0f)
	{ }
	virtual				~CEffectorPP( );
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
