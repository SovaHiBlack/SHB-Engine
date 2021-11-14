#include "stdafx.h"

#include "NoGravityZone.h"
#include "physicsshell.h"
#include "EntityAlive.h"
#include "PHMovementControl.h"
#include "PhWorld.h"
#include "CharacterPhysicsSupport.h"

extern CPHWorld* ph_world;

void CNoGravityZone::enter_Zone(SZoneObjectInfo& io)
{
	inherited::enter_Zone(io);
	switchGravity(io,false);
}

void CNoGravityZone::exit_Zone(SZoneObjectInfo& io)
{
	switchGravity(io,true);
	inherited::exit_Zone(io);
}

void CNoGravityZone::UpdateWorkload(u32 dt)
{
	OBJECT_INFO_VEC_IT i=m_ObjectInfoMap.begin(),e=m_ObjectInfoMap.end();
	for(;e!=i;i++)switchGravity(*i,false);
}

void CNoGravityZone::switchGravity(SZoneObjectInfo& io, bool val)
{
	if(io.object->getDestroy()) return;
	CPHShellHolder* sh= smart_cast<CPHShellHolder*>(io.object);
	if(!sh)return;
	CPhysicsShell* shell=sh->PPhysicsShell();
	if(shell&&shell->isActive())
	{
		shell->set_ApplyByGravity(val);
		if(!val&&shell->get_ApplyByGravity())
		{
			CPhysicsElement* e=shell->get_ElementByStoreOrder(unsigned short(Random.randI(0,shell->get_ElementsNumber())));
			if(e->isActive())
			{
				e->applyImpulseTrace(Fvector3().random_point(e->getRadius()), Fvector3().random_dir(),shell->getMass()*ph_world->Gravity()*fixed_step,e->m_SelfID);
			}
		}
		//shell->SetAirResistance(0.0f,0.0f);
		//shell->set_DynamicScales(1.0f);
		return;
	}

	if(!io.nonalive_object)
	{
		CEntityAlive* ea=smart_cast<CEntityAlive*>(io.object);
		CPHMovementControl*mc=ea->character_physics_support()->movement();
		mc->SetApplyGravity(BOOL(val));
		mc->SetForcedPhysicsControl(!val);
		if(!val&&mc->Environment()==CPHMovementControl::peOnGround)
		{
			Fvector3 gn;
			mc->GroundNormal(gn);
			mc->ApplyImpulse(gn,mc->GetMass()*ph_world->Gravity()*fixed_step);
		}
	}
}
