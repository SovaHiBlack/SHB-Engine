#include "stdafx.h"

#include "PHSkeletonObject.h"
#include "PhysicsShell.h"
#include "PHSynchronize.h"
#include "xrserver_objects_alife.h"
#include "..\ENGINE\skeletoncustom.h"
#include "..\ENGINE\xr_collide_form.h"

CPHSkeletonObject::CPHSkeletonObject( )
{ }

CPHSkeletonObject::~CPHSkeletonObject( )
{ }

BOOL CPHSkeletonObject::net_Spawn(CSE_Abstract* DC)
{
	CSE_Abstract* e = (CSE_Abstract*) (DC);

	inherited::net_Spawn(DC);
	xr_delete(collidable.model);
	collidable.model = xr_new<CCF_Skeleton>(this);
	CPHSkeleton::Spawn(e);
	setVisible(TRUE);
	setEnabled(TRUE);
	if (!PPhysicsShell( )->isBreakable( ))
	{
		SheduleUnregister( );
	}

	return TRUE;
}

void CPHSkeletonObject::SpawnInitPhysics(CSE_Abstract* D)
{
	CreatePhysicsShell(D);
	CKinematics* K = smart_cast<CKinematics*>(Visual( ));
	if (K)
	{
		K->CalculateBones_Invalidate( );
		K->CalculateBones( );
	}
}

void CPHSkeletonObject::net_Destroy( )
{
	inherited::net_Destroy( );
	CPHSkeleton::RespawnInit( );
}

void CPHSkeletonObject::Load(const char* section)
{
	inherited::Load(section);
	CPHSkeleton::Load(section);
}

void CPHSkeletonObject::CreatePhysicsShell(CSE_Abstract* e)
{
	CSE_PHSkeleton* po = smart_cast<CSE_PHSkeleton*>(e);
	if (m_pPhysicsShell)
	{
		return;
	}

	if (!Visual( ))
	{
		return;
	}

	m_pPhysicsShell = P_build_Shell(this, !po->_flags.test(CSE_PHSkeleton::flActive));
}

void CPHSkeletonObject::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);

	CPHSkeleton::Update(dt);
}

void CPHSkeletonObject::net_Save(NET_Packet& P)
{
	inherited::net_Save(P);
	CPHSkeleton::SaveNetState(P);
}

BOOL CPHSkeletonObject::net_SaveRelevant( )
{
	return TRUE;//!m_flags.test(CSE_ALifeObjectPhysic::flSpawnCopy);
}

BOOL CPHSkeletonObject::UsedAI_Locations( )
{
	return FALSE;
}

void CPHSkeletonObject::UpdateCL( )
{
	inherited::UpdateCL( );
	PHObjectPositionUpdate( );
}

void CPHSkeletonObject::PHObjectPositionUpdate( )
{
	if (m_pPhysicsShell)
	{
		m_pPhysicsShell->InterpolateGlobalTransform(&XFORM( ));
	}
}
