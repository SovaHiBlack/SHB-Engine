#pragma once

#include "PHShellHolder.h"
#include "PHSkeleton.h"

class CSE_ALifePHSkeletonObject;

class CPHSkeletonObject : public CPHShellHolder, public CPHSkeleton
{
	using inherited										= CPHShellHolder;

public:
								CPHSkeletonObject		( );
	virtual						~CPHSkeletonObject		( );

	virtual BOOL				net_Spawn				(CSE_Abstract* DC);
	virtual void				net_Destroy				( );
	virtual void				Load					(const char* section);
	virtual void				UpdateCL				( );// Called each frame, so no need for dt
	virtual void				shedule_Update			(u32 dt);
	virtual void				net_Save				(CNetPacket& P);
	virtual BOOL				net_SaveRelevant		( );
	virtual BOOL				UsedAI_Locations		( );

protected:
	virtual CPHShellHolder*		PPhysicsShellHolder		( )
	{
		return PhysicsShellHolder( );
	}
	virtual CPHSkeleton*		PHSkeleton				( )
	{
		return this;
	}
	virtual void				SpawnInitPhysics		(CSE_Abstract* D);
	virtual void				PHObjectPositionUpdate	( );
	virtual void				CreatePhysicsShell		(CSE_Abstract* e);
};
