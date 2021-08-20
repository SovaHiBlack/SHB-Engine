#pragma once

#include "Camera_defs.h"

struct SPostProcessInfo;

// постпроцесс
class ENGINE_API CEffectorPostProcess
{
	EEffectorPostProcessType								eType;
	bool													bFreeOnRemove;

protected:
	float													fLifeTime;

public:
									CEffectorPostProcess	(EEffectorPostProcessType type, float lifeTime, bool free_on_remove = true);
									CEffectorPostProcess	( ) :bFreeOnRemove(true), fLifeTime(0.0f)
	{ }
	virtual							~CEffectorPostProcess	( );
	virtual BOOL					Process					(SPostProcessInfo& PPInfo);
	virtual BOOL					Valid					( )
	{
		return fLifeTime > 0.0f;
	}
	inline EEffectorPostProcessType	Type					( ) const
	{
		return eType;
	}
	inline bool						FreeOnRemove			( ) const
	{
		return bFreeOnRemove;
	}
	inline void						SetType					(EEffectorPostProcessType t)
	{
		eType = t;
	}
	virtual void					Stop					(float speed)
	{
		fLifeTime = 0.0f;
	}
};
