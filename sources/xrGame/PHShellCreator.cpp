#include "stdafx.h"

#include "PHShellCreator.h"
#include "PhysicsShell.h"
//#include "GameObject.h"
#include "PHShellHolder.h"
#include "..\ENGINE\skeletoncustom.h"

void CPHShellSimpleCreator::CreatePhysicsShell()
{
	CPHShellHolder* owner = smart_cast<CPHShellHolder*>(this);
	VERIFY(owner);
	if (!owner->Visual( ))
	{
		return;
	}
	
	CKinematics* pKinematics		= smart_cast<CKinematics*>(owner->Visual());
	VERIFY							(pKinematics);

	if (owner->PPhysicsShell( ))
	{
		return;
	}

	owner->PPhysicsShell()			= P_create_Shell();

#ifdef DEBUG
	owner->PPhysicsShell()->dbg_obj=owner;
#endif // def DEBUG

	owner->m_pPhysicsShell->build_FromKinematics	(pKinematics,0);

	owner->PPhysicsShell()->set_PhysicsRefObject	(owner);
	//m_pPhysicsShell->SmoothElementsInertia(0.3f);
	owner->PPhysicsShell()->mXFORM.set				(owner->XFORM());
	owner->PPhysicsShell()->SetAirResistance		(0.001f, 0.02f);
}
