#pragma once

#include "CameraDefs.h"

class ENGINE_API		CEffectorCam
{
protected:
	ECamEffectorType	eType;
	
	friend class		CCameraManager;
	F32				fLifeTime;
public:
						CEffectorCam	(ECamEffectorType type, F32 tm)	{eType=type; fLifeTime=tm;};
						CEffectorCam	()									{eType=(ECamEffectorType)0; fLifeTime=0.0f;};
	virtual				~CEffectorCam	()									{};
			void		SetType			(ECamEffectorType type)				{eType=type;}
	IC ECamEffectorType	GetType			()									{return eType;}
	virtual	BOOL		Valid			()									{return fLifeTime>0.0f;}
	IC virtual BOOL		Overlapped		()									{return FALSE;}

	virtual	BOOL		Process			(Fvector &p, Fvector &d, Fvector &n, F32& fFov, F32& fFar, F32& fAspect){fLifeTime-=Device.fTimeDelta; return Valid();};

	virtual	void		ProcessIfInvalid(Fvector &p, Fvector &d, Fvector &n, F32& fFov, F32& fFar, F32& fAspect){};
	virtual BOOL		AllowProcessingIfInvalid()							{return FALSE;}
};
