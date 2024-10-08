// CameraBase.h: interface for the CCameraBase class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "CameraDefs.h"

// refs
class CObject;

class ENGINE_API	CCameraBase
{
protected:
	CObject*		parent;

public:
	BOOL			bClampYaw, bClampPitch, bClampRoll;
	F32				yaw;
	F32				pitch;
	F32				roll;

	enum{
		flRelativeLink		= (1<<0),
		flPositionRigid		= (1<<1),
		flDirectionRigid	= (1<<2),
	};
	Flags32			m_Flags;

	ECameraStyle	style;
	Fvector2		lim_yaw,lim_pitch,lim_roll;
	Fvector			rot_speed;

	Fvector			vPosition;
	Fvector			vDirection;
	Fvector			vNormal;
	F32				f_fov;
	F32				f_aspect;

	int				tag;
public:
					CCameraBase		( CObject* p, u32 flags );
	virtual			~CCameraBase	( );
	virtual void	Load			(pcstr section);
	void			SetParent		( CObject* p )								{parent=p; VERIFY(p);}
	virtual	void	OnActivate		( CCameraBase* old_cam )					{;}
	virtual	void	OnDeactivate	( )											{;}
	virtual void	Move			( int cmd, F32 val=0.0f, F32 factor=1.0f)	{;}
	virtual void	Update			( Fvector& point, Fvector& noise_angle )	{;}
	virtual void	Get				( Fvector& P, Fvector& D, Fvector& N )		{P.set(vPosition);D.set(vDirection);N.set(vNormal);}
	virtual void	Set				( const Fvector& P, const Fvector& D, const Fvector& N ){vPosition.set(P);vDirection.set(D);vNormal.set(N);}
	virtual void	Set				(F32 Y, F32 P, F32 R )				{yaw=Y;pitch=P;roll=R;}
	
	virtual F32	GetWorldYaw		( )	{ return 0.0f; };
	virtual F32	GetWorldPitch	( )	{ return 0.0f; };

	virtual F32	CheckLimYaw		( );
	virtual F32	CheckLimPitch	( );
	virtual F32	CheckLimRoll	( );
};
