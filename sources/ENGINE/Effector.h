#pragma once

#include "Camera_defs.h"

class ENGINE_API		CEffectorCam
{
protected:
	ECameraEffectorType	eType;
	
	friend class		CCameraManager;
	float				fLifeTime;

public:
						CEffectorCam	(ECameraEffectorType type, float tm)	{eType=type; fLifeTime=tm;};
						CEffectorCam	()									{eType=(ECameraEffectorType)0; fLifeTime=0.0f;};
	virtual				~CEffectorCam	()									{};
			void		SetType			(ECameraEffectorType type)				{eType=type;}
	IC ECameraEffectorType	GetType			()									{return eType;}
	virtual	BOOL		Valid			()									{return fLifeTime>0.0f;}
	IC virtual BOOL		Overlapped		()									{return FALSE;}

	virtual	BOOL		Process			(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect){fLifeTime-=Device.fTimeDelta; return Valid();};

	virtual	void		ProcessIfInvalid(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect){};
	virtual BOOL		AllowProcessingIfInvalid()							{return FALSE;}
};
