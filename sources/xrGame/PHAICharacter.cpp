#include "stdafx.h"

#include "PHDynamicData.h"
#include "Physics.h"
#include "ExtendedGeom.h"
#include "..\XR_3DA\cl_intersect.h"
#include "tri-colliderKNoOPC\__aabb_tri.h"

#include "phaicharacter.h"

#ifdef DEBUG
#	include "..\XR_3DA\StatGraph.h"
#	include "PHDebug.h"
#	include "level.h"
#	include "DebugRenderer.h"
#endif

CPHAICharacter::CPHAICharacter( )
{
	m_forced_physics_control = false;
}

void CPHAICharacter::Create(dVector3 sizes)
{
	inherited::Create(sizes);
	m_forced_physics_control = false;
}

bool CPHAICharacter::TryPosition(fVector3 pos, bool exact_state)
{
	if (!b_exist)
	{
		return false;
	}

	if (m_forced_physics_control || JumpState( ))
	{
		return false;
	}//b_was_on_object||b_on_object||

	if (DoCollideObj( ))
	{
		return false;
	}

	fVector3	current_pos;
	GetPosition(current_pos);
	fVector3	cur_vel;
	GetVelocity(cur_vel);

	fVector3	displace;
	displace.sub(pos, current_pos);
	f32	disp_mag = displace.magnitude( );

	if (fis_zero(disp_mag) || fis_zero(Device.fTimeDelta))
	{
		return true;
	}

	const	u32		max_steps = 15;
	const	f32	fmax_steps = f32(max_steps);
	f32	fsteps_num = 1.f;
	u32		steps_num = 1;
	f32	disp_pstep = FootRadius( );
	f32	rest = 0.f;

	f32	parts = disp_mag / disp_pstep;
	fsteps_num = floor(parts);
	steps_num = iFloor(parts);
	if (steps_num > max_steps)
	{
		steps_num = max_steps;
		fsteps_num = fmax_steps;
		disp_pstep = disp_mag / fsteps_num;
	}

	rest = disp_mag - fsteps_num * disp_pstep;

	fVector3	vel;
	vel.mul(displace, disp_pstep / fixed_step / disp_mag);
	bool	ret = true;
	s32 save_gm = dBodyGetGravityMode(m_body);
	dBodySetGravityMode(m_body, 0);
	for (u32 i = 0; steps_num > i; ++i)
	{
		SetVelocity(vel);
		Enable( );
		if (!step_single(fixed_step))
		{
			SetVelocity(cur_vel);
			ret = false;
			break;
		}
	}

	vel.mul(displace, rest / fixed_step / disp_mag);
	SetVelocity(vel);
	Enable( );
	ret = step_single(fixed_step);

	dBodySetGravityMode(m_body, save_gm);
	SetVelocity(cur_vel);
	fVector3	pos_new;
	GetPosition(pos_new);

#if 0
	fVector3	dif;
	dif.sub(pos, pos_new);
	f32	dif_m = dif.magnitude( );
	if (ret && dif_m > EPSILON_3)
	{
		Msg("dif vec %f,%f,%f \n", dif.x, dif.y, dif.z);
		Msg("dif mag %f \n", dif_m);
	}
#endif

	SetPosition(pos_new);
	m_last_move.sub(pos_new, current_pos).mul(1.f / Device.fTimeDelta);
	m_body_interpolation.UpdatePositions( );
	m_body_interpolation.UpdatePositions( );
	if (ret)
	{
		Disable( );
	}

	m_collision_damage_info.m_contact_velocity = 0.0f;
	return ret;
}

void CPHAICharacter::SetPosition(fVector3 pos)
{
	m_vDesiredPosition.set(pos);
	inherited::SetPosition(pos);
}

void CPHAICharacter::BringToDesired(f32 time, f32 velocity, f32 /**force/**/)
{
	fVector3 pos;
	fVector3 move;
	GetPosition(pos);

	move.sub(m_vDesiredPosition, pos);
	move.y = 0.0f;
	f32 dist = move.magnitude( );

	f32 vel;
	if (dist > EPSILON_3 * 100.0f)
	{
		vel = dist / time;
		move.mul(1.f / dist);
	}
	else if (dist > EPSILON_3 * 10.0f)
	{
		vel = dist * dist * dist;
		move.mul(1.0f / dist);
	}
	else
	{
		vel = 0.0f;
		move.set(0.0f, 0.0f, 0.0f);
	}

	if (vel > velocity)//&&velocity>EPSILON_3
	{
		vel = velocity;
	}

	if (velocity < EPSILON_3 / fixed_step)
	{
		vel = 0.f;
		move.set(0.0f, 0.0f, 0.0f);
	}

	SetMaximumVelocity(vel);

	SetAcceleration(move);
}

void	CPHAICharacter::Jump(const fVector3& jump_velocity)
{
	b_jump = true;
	m_jump_accel.set(jump_velocity);
}

void	CPHAICharacter::ValidateWalkOn( )
{
//if(b_on_object)
//	ValidateWalkOnObject();
// else 
//	 b_clamb_jump=true;
	inherited::ValidateWalkOn( );
}

void CPHAICharacter::InitContact(dContact* c, bool& do_collide, u16 material_idx_1, u16 material_idx_2)
{
	inherited::InitContact(c, do_collide, material_idx_1, material_idx_2);
	if (is_control || b_lose_control || b_jumping)
	{
		c->surface.mu = 0.0f;
	}
	dxGeomUserData* D1 = retrieveGeomUserData(c->geom.g1);
	dxGeomUserData* D2 = retrieveGeomUserData(c->geom.g2);
	if (D1 && D2 && D1->ph_object && D2->ph_object && D1->ph_object->CastType( ) == tpCharacter && D2->ph_object->CastType( ) == tpCharacter)
	{
		b_on_object = true;
		b_valide_wall_contact = false;
	}

#ifdef DEBUG
	if (ph_dbg_draw_mask.test(phDbgNeverUseAiPhMove))
	{
		do_collide = false;
	}
#endif

}

#ifdef DEBUG
void	CPHAICharacter::OnRender( )
{
	inherited::OnRender( );

	if (!b_exist)
	{
		return;
	}

	fVector3 pos;
	GetDesiredPosition(pos);
	pos.y += m_radius;

	fVector3 scale;
	scale.set(0.35f, 0.35f, 0.35f);
	fMatrix4x4 M;
	M.identity( );
	M.scale(scale);
	M.c.set(pos);

	Level( ).debug_renderer( ).draw_ellipse(M, 0xffffffff);
}
#endif
