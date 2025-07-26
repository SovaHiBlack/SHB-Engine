#include "stdafx.h"

#include "InteractiveMotion.h"

#include "PhysicsShell.h"
#include "PhysicsShellHolder.h"
#include "MathUtils.h"

CInteractiveMotion::CInteractiveMotion( )
{
	init( );
}

void CInteractiveMotion::init( )
{
	flags.assign(0);
}

void CInteractiveMotion::setup(pcstr m, CPhysicsShell* s)
{
	VERIFY(s);
	motion = smart_cast<CKinematicsAnimated*>(s->PKinematics( ))->LL_MotionID(m);
	if (motion.valid( ))
	{
		flags.set(fl_use_death_motion, TRUE);
	}
}

void CInteractiveMotion::anim_callback(CBlend* B)
{
	VERIFY(B->CallbackParam);
	((CInteractiveMotion*)(B->CallbackParam))->flags.set(fl_switch_dm_toragdoll, TRUE);
}

void CInteractiveMotion::play(CPhysicsShell* s)
{
	VERIFY(s);
	smart_cast<CKinematicsAnimated*>(s->PKinematics( ))->PlayCycle(motion, TRUE, anim_callback, this);
	state_start(s);
}

f32 depth = 0;
void get_depth(bool& do_colide, bool bo1, dContact& c, SGameMtl* /*material_1*/, SGameMtl* /*material_2*/)
{
	save_max(depth, c.geom.depth);
}
void CInteractiveMotion::state_start(CPhysicsShell* s)
{
	s->add_ObjectContactCallback(get_depth);
	collide(s);

	if (flags.test(fl_switch_dm_toragdoll))
	{
		flags.assign(0);
		s->remove_ObjectContactCallback(get_depth);
		return;
	}
}

void	CInteractiveMotion::state_end(CPhysicsShell* s)
{
	flags.set(fl_switch_dm_toragdoll, FALSE);
	flags.set(fl_use_death_motion, FALSE);
	s->Enable( );
	s->remove_ObjectContactCallback(get_depth);
////set and velocities
	s->AnimToVelocityState(Device.fTimeDelta, default_l_limit * 10, default_w_limit * 10);
}

void CInteractiveMotion::update(CPhysicsShell* s)
{
	CKinematics* K = s->PKinematics( );
	VERIFY(K);
	K->CalculateBones( );
	collide(s);
	if (flags.test(fl_switch_dm_toragdoll))
	{
		switch_to_free(s);
	}
	else
	{
		move_update(s);
	}
}

void	CInteractiveMotion::switch_to_free(CPhysicsShell* s)
{
	//set to normal state
	state_end(s);
	//set all matrises valide
	CPhysicsShellHolder* obj = s->get_ElementByStoreOrder(0)->PhysicsRefObject( );
	VERIFY(obj);
	s->InterpolateGlobalTransform(&obj->XFORM( ));
	CKinematics* K = s->PKinematics( );
	VERIFY(K);
	K->CalculateBones_Invalidate( );
	K->CalculateBones(TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////
void CInteractiveMotionPosition::state_start(CPhysicsShell* s)
{
	inherited::state_start(s);
	if (!is_enabled( ))
	{
		return;
	}

	s->Disable( );
	s->EnabledCallbacks(FALSE);
}

void CInteractiveMotionPosition::state_end(CPhysicsShell* s)
{
	inherited::state_end(s);
	s->ToAnimBonesPositions( );
	s->EnabledCallbacks(TRUE);
}

void CInteractiveMotionPosition::move_update(CPhysicsShell* s)
{
	s->Disable( );
	s->ToAnimBonesPositions( );
}

void CInteractiveMotionPosition::collide(CPhysicsShell* s)
{
	depth = 0;
	s->CollideAll( );
	if (depth > 0.05)
	{
		flags.set(fl_switch_dm_toragdoll, TRUE);
	}
}
////////////////////////////////////////////////////////////////////////////////////

void CInteractiveMotionVelocity::state_start(CPhysicsShell* s)
{
	inherited::state_start(s);
	if (!is_enabled( ))
	{
		return;
	}

	s->set_ApplyByGravity(false);
	//s->set_DynamicLimits(default_l_limit,default_w_limit * 5.f);
	//s->set_DynamicScales(1,1);
	//s->SetAirResistance(0,0);
}

void	CInteractiveMotionVelocity::state_end(CPhysicsShell* s)
{
	inherited::state_end(s);
	s->set_ApplyByGravity(true);
}

void CInteractiveMotionVelocity::collide(CPhysicsShell* s)
{ }

void CInteractiveMotionVelocity::move_update(CPhysicsShell* s)
{
	if (!s->AnimToVelocityState(Device.fTimeDelta, 2 * default_l_limit, 10.f * default_w_limit))
	{
		flags.set(fl_switch_dm_toragdoll, TRUE);
	}

	fMatrix4x4 sv;
	sv.set(s->mXFORM);
	s->InterpolateGlobalTransform(&s->mXFORM);
	s->mXFORM.set(sv);
}
