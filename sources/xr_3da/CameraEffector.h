#pragma once

#include "Camera_defs.h"

class ENGINE_API		CCameraEffector
{
protected:
	ECamEffectorType	eType;

	friend class		CCameraManager;
	f32				fLifeTime;

public:
	CCameraEffector(ECamEffectorType type, f32 tm)
	{
		eType = type;
		fLifeTime = tm;
	}
	CCameraEffector( )
	{
		eType = (ECamEffectorType)0;
		fLifeTime = 0.0f;
	}
	virtual				~CCameraEffector( )
	{ }
	void		SetType(ECamEffectorType type)
	{
		eType = type;
	}
	IC ECamEffectorType	GetType( )
	{
		return eType;
	}
	virtual BOOL		Valid( )
	{
		return fLifeTime > 0.0f;
	}
	IC virtual BOOL		Overlapped( )
	{
		return FALSE;
	}

	virtual	BOOL		Process(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect)
	{
		fLifeTime -= Device.fTimeDelta;
		return Valid( );
	}

	virtual	void		ProcessIfInvalid(fVector3& p, fVector3& d, fVector3& n, f32& fFov, f32& fFar, f32& fAspect)
	{ }
	virtual BOOL		AllowProcessingIfInvalid( )
	{
		return FALSE;
	}
};
