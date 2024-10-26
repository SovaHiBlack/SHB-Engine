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
	f32				yaw;
	f32				pitch;
	f32				roll;

	enum{
		flRelativeLink		= (1<<0),
		flPositionRigid		= (1<<1),
		flDirectionRigid	= (1<<2),
	};
	flags32			m_Flags;

	ECameraStyle	style;
	fVector2		lim_yaw;
	fVector2		lim_pitch;
	fVector2		lim_roll;
	Fvector			rot_speed;

	Fvector			vPosition;
	Fvector			vDirection;
	Fvector			vNormal;
	f32				f_fov;
	f32				f_aspect;

	int				tag;

public:
					CCameraBase		( CObject* p, u32 flags );
	virtual			~CCameraBase	( );
	virtual void	Load			(pcstr section);
	void			SetParent		( CObject* p )								{parent=p; VERIFY(p);}
	virtual	void	OnActivate		( CCameraBase* old_cam )					{;}
	virtual	void	OnDeactivate	( )											{;}
	virtual void	Move			( int cmd, f32 val=0.0f, f32 factor=1.0f)	{;}
	virtual void	Update			( Fvector& point, Fvector& noise_angle )	{;}
	virtual void	Get				( Fvector& P, Fvector& D, Fvector& N )		{P.set(vPosition);D.set(vDirection);N.set(vNormal);}
	virtual void	Set				( const Fvector& P, const Fvector& D, const Fvector& N ){vPosition.set(P);vDirection.set(D);vNormal.set(N);}
	virtual void	Set				(f32 Y, f32 P, f32 R )				{yaw=Y;pitch=P;roll=R;}
	
	virtual f32	GetWorldYaw		( )	{ return 0.0f; };
	virtual f32	GetWorldPitch	( )	{ return 0.0f; };

	virtual f32	CheckLimYaw		( );
	virtual f32	CheckLimPitch	( );
	virtual f32	CheckLimRoll	( );
};
