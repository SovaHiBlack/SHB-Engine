////////////////////////////////////////////////////////////////////////////
//	Module 		: Crow.cpp
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../../PhysicsShell.h"
#include "Crow.h"
#include "../../hudmanager.h"
#include "../../Level.h"
#include "../../..\XR_3DA\skeletonanimated.h"

void CCrow::SAnim::Load(CKinematicsAnimated* visual, pcstr prefix)
{
	const MotionID& M = visual->ID_Cycle_Safe(prefix);
	if (M)
	{
		m_Animations.push_back(M);
	}

	for (s32 i = 0; (i < MAX_ANIM_COUNT) && (m_Animations.size( ) < MAX_ANIM_COUNT); ++i)
	{
		string128 sh_anim;
		sprintf_s(sh_anim, "%s_%d", prefix, i);
		const MotionID& M = visual->ID_Cycle_Safe(sh_anim);
		if (M)
		{
			m_Animations.push_back(M);
		}
	}

	R_ASSERT(m_Animations.size( ));
}

void CCrow::SSound::Load(pcstr prefix)
{
	string_path	fn;
	if (FS.exist(fn, "$game_sounds$", prefix, ".ogg"))
	{
		m_Sounds.push_back(ref_sound( ));
		::Sound->create(m_Sounds.back( ), prefix, st_Effect, sg_SourceType);
	}

	for (s32 i = 0; (i < MAX_SND_COUNT) && (m_Sounds.size( ) < MAX_SND_COUNT); ++i)
	{
		string64 name;
		sprintf_s(name, "%s_%d", prefix, i);
		if (FS.exist(fn, "$game_sounds$", name, ".ogg"))
		{
			m_Sounds.push_back(ref_sound( ));
			::Sound->create(m_Sounds.back( ), name, st_Effect, sg_SourceType);
		}
	}

	R_ASSERT(m_Sounds.size( ));
}

void CCrow::SSound::SetPosition(const fVector3& pos)
{
	for (s32 i = 0; i < (s32)m_Sounds.size( ); ++i)
	{
		if (m_Sounds[i]._feedback( ))
		{
			m_Sounds[i].set_position(pos);
		}
	}
}

void CCrow::SSound::Unload( )
{
	for (s32 i = 0; i < (s32)m_Sounds.size( ); ++i)
	{
		::Sound->destroy(m_Sounds[i]);
	}
}

void cb_OnHitEndPlaying(CBlend* B)
{
	((CCrow*)B->CallbackParam)->OnHitEndPlaying(B);
}

void CCrow::OnHitEndPlaying(CBlend* /**B/**/)
{
	bPlayDeathIdle = true;
}

CCrow::CCrow( )
{
	init( );
}

CCrow::~CCrow( )
{
	// removing all data no more being neded 
	m_Sounds.m_idle.Unload( );
}

void CCrow::init( )
{
	st_current = eUndef;
	st_target = eFlyIdle;
	vGoalDir.set(10.0f * (Random.randF( ) - Random.randF( )), 10.0f * (Random.randF( ) - Random.randF( )), 10.0f * (Random.randF( ) - Random.randF( )));
	vCurrentDir.set(0.0f, 0.0f, 1.0f);
	vHPB.set(0.0f, 0.0f, 0.0f);
	fDHeading = 0.0f;
	fGoalChangeDelta = 10.0f;
	fGoalChangeTime = 0.0f;
	fSpeed = 5.0f;
	fASpeed = 0.2f;
	fMinHeight = 40.0f;
	vVarGoal.set(10.0f, 10.0f, 100.0f);
	fIdleSoundDelta = 10.0f;
	fIdleSoundTime = fIdleSoundDelta;
	bPlayDeathIdle = false;
}

void CCrow::Load(pcstr section)
{
	inherited::Load(section);
	//////////////////////////////////////////////////////////////////////////
	ISpatial* self = smart_cast<ISpatial*> (this);
	if (self)
	{
		self->spatial.type &= ~STYPE_VISIBLEFORAI;
		self->spatial.type &= ~STYPE_REACTTOSOUND;
	}
	//////////////////////////////////////////////////////////////////////////

	// sounds
	m_Sounds.m_idle.Load("monsters\\crow\\idle");
	// play defaut

	fSpeed = pSettings->r_float(section, "speed");
	fASpeed = pSettings->r_float(section, "angular_speed");
	fGoalChangeDelta = pSettings->r_float(section, "goal_change_delta");
	fMinHeight = pSettings->r_float(section, "min_height");
	vVarGoal = pSettings->r_fvector3(section, "goal_variability");
	fIdleSoundDelta = pSettings->r_float(section, "idle_sound_delta");
	fIdleSoundTime = fIdleSoundDelta + fIdleSoundDelta * Random.randF(-.5f, .5f);
}

BOOL CCrow::net_Spawn(CSE_Abstract* DC)
{
	BOOL R = inherited::net_Spawn(DC);
	setVisible(TRUE);
	setEnabled(TRUE);

	// animations
	CKinematicsAnimated* M = smart_cast<CKinematicsAnimated*>(Visual( )); R_ASSERT(M);
	m_Anims.m_death.Load(M, "norm_death");
	m_Anims.m_death_dead.Load(M, "norm_death_dead");
	m_Anims.m_death_idle.Load(M, "norm_death_idle");
	m_Anims.m_fly.Load(M, "norm_fly_fwd");
	m_Anims.m_idle.Load(M, "norm_idle");

	// disable UpdateCL, enable only on HIT
	processing_deactivate( );

	return R;
}

void CCrow::net_Destroy( )
{
	inherited::net_Destroy( );

	m_Anims.m_death.m_Animations.clear( );
	m_Anims.m_death_dead.m_Animations.clear( );
	m_Anims.m_death_idle.m_Animations.clear( );
	m_Anims.m_fly.m_Animations.clear( );
	m_Anims.m_idle.m_Animations.clear( );
}

// crow update
void CCrow::switch2_FlyUp( )
{
	smart_cast<CKinematicsAnimated*>(Visual( ))->PlayCycle(m_Anims.m_fly.GetRandom( ));
}

void CCrow::switch2_FlyIdle( )
{
	smart_cast<CKinematicsAnimated*>(Visual( ))->PlayCycle(m_Anims.m_idle.GetRandom( ));
}

void CCrow::switch2_DeathDead( )
{
	// AI need to pickup this
	ISpatial* self = smart_cast<ISpatial*> (this);
	if (self)
	{
		self->spatial.type |= STYPE_VISIBLEFORAI;
	}
	//
	smart_cast<CKinematicsAnimated*>(Visual( ))->PlayCycle(m_Anims.m_death_dead.GetRandom( ));
}

void CCrow::switch2_DeathFall( )
{
	fVector3 V;
	V.mul(XFORM( ).k, fSpeed);
//	m_PhysicMovementControl->SetVelocity(V);
	smart_cast<CKinematicsAnimated*>(Visual( ))->PlayCycle(m_Anims.m_death.GetRandom( ), TRUE, cb_OnHitEndPlaying, this);
}

void CCrow::state_Flying(f32 fdt)
{
	// Update position and orientation of the planes
	f32 fAT = fASpeed * fdt;
	fVector3& vDirection = XFORM( ).k;

	// Tweak orientation based on last position and goal
	fVector3 vOffset;
	vOffset.sub(vGoalDir, Position( ));

	// First, tweak the pitch
	if (vOffset.y > 1.0)
	{			// We're too low
		vHPB.y += fAT;
		if (vHPB.y > 0.8f)	vHPB.y = 0.8f;
	}
	else if (vOffset.y < -1.0)
	{	// We're too high
		vHPB.y -= fAT;
		if (vHPB.y < -0.8f)vHPB.y = -0.8f;
	}
	else							// Add damping
	{
		vHPB.y *= 0.95f;
	}

	// Now figure out yaw changes
	vOffset.y = 0.0f;
	vDirection.y = 0.0f;

	vDirection.normalize( );
	vOffset.normalize( );

	f32 fDot = vDirection.dotproduct(vOffset);
	fDot = (1.0f - fDot) / 2.0f * fAT * 10.0f;

	vOffset.crossproduct(vOffset, vDirection);

	if (vOffset.y > 0.01f)
	{
		fDHeading = (fDHeading * 9.0f + fDot) * 0.1f;
	}
	else if (vOffset.y < 0.01f)
	{
		fDHeading = (fDHeading * 9.0f - fDot) * 0.1f;
	}

	vHPB.x += fDHeading;
	vHPB.z = -fDHeading * 9.0f;

	// Update position
	vOldPosition.set(Position( ));
	XFORM( ).setHPB(vHPB.x, vHPB.y, vHPB.z);
	Position( ).mad(vOldPosition, vDirection, fSpeed * fdt);
}

static fVector3 vV = {0.0f,0.0f,0.0f};
void CCrow::state_DeathFall( )
{
	fVector3 tAcceleration;
	tAcceleration.set(0.0f, -10.0f, 0.0f);
	if (m_pPhysicsShell)
	{
		fVector3 velocity;
		m_pPhysicsShell->get_LinearVel(velocity);
		if (velocity.y > -0.001f)
		{
			st_target = eDeathDead;
		}
	}

	if (bPlayDeathIdle)
	{
		smart_cast<CKinematicsAnimated*>(Visual( ))->PlayCycle(m_Anims.m_death_idle.GetRandom( ));
		bPlayDeathIdle = false;
	}
}

void CCrow::Die(CObject* who)
{
	inherited::Die(who);
	processing_activate( );	// enable UpdateCL for dead crows - especially for physics support
								// and do it especially before Creating physics shell or it definitely throws processing enable/disable calls: underflow  
	CreateSkeleton( );
}

void CCrow::UpdateWorkload(f32 fdt)
{
	if (o_workload_frame == Device.dwFrame)
	{
		return;
	}

	o_workload_frame = Device.dwFrame;
	switch (st_current)
	{
		case eFlyIdle:
		case eFlyUp:
		{
			state_Flying(fdt);
		}
		break;
		case eDeathFall:
		{
			state_DeathFall( );
		}
		break;
	}
}

void CCrow::UpdateCL( )
{
	inherited::UpdateCL( );
	if (m_pPhysicsShell)
	{
		m_pPhysicsShell->Update( );
		XFORM( ).set(m_pPhysicsShell->mXFORM);
	}
}

void CCrow::renderable_Render( )
{
	UpdateWorkload(Device.fTimeDelta);
	inherited::renderable_Render( );
	o_workload_rframe = Device.dwFrame;
}

void CCrow::shedule_Update(u32 DT)
{
	f32 fDT = f32(DT) / 1000.0f;
	spatial.type &= ~STYPE_VISIBLEFORAI;

	inherited::shedule_Update(DT);

	if (st_target != st_current)
	{
		switch (st_target)
		{
			case eFlyUp:
			{
				switch2_FlyUp( );
			}
			break;
			case eFlyIdle:
			{
				switch2_FlyIdle( );
			}
			break;
			case eDeathFall:
			{
				switch2_DeathFall( );
			}
			break;
			case eDeathDead:
			{
				switch2_DeathDead( );
			}
			break;
		}

		st_current = st_target;
	}

	switch (st_current)
	{
		case eFlyIdle:
		{
			if (Position( ).y > vOldPosition.y)
			{
				st_target = eFlyUp;
			}
		}
		break;
		case eFlyUp:
		{
			if (Position( ).y <= vOldPosition.y)
			{
				st_target = eFlyIdle;
			}
		}
		break;
		case eDeathFall:
		{
			state_DeathFall( );
		}
		break;
	}

	if ((eDeathFall != st_current) && (eDeathDead != st_current))
	{
		// At random times, change the direction (goal) of the plane
		if (fGoalChangeTime <= 0)
		{
			fGoalChangeTime += fGoalChangeDelta + fGoalChangeDelta * Random.randF(-0.5f, 0.5f);
			fVector3 vP;
			vP.set(Device.vCameraPosition.x, Device.vCameraPosition.y + fMinHeight, Device.vCameraPosition.z);
			vGoalDir.x = vP.x + vVarGoal.x * Random.randF(-0.5f, 0.5f);
			vGoalDir.y = vP.y + vVarGoal.y * Random.randF(-0.5f, 0.5f);
			vGoalDir.z = vP.z + vVarGoal.z * Random.randF(-0.5f, 0.5f);
		}

		fGoalChangeTime -= fDT;
		// sounds
		if (fIdleSoundTime <= 0)
		{
			fIdleSoundTime = fIdleSoundDelta + fIdleSoundDelta * Random.randF(-0.5f, 0.5f);
			//if (st_current==eFlyIdle)
			::Sound->play_at_pos(m_Sounds.m_idle.GetRandom( ), H_Root( ), Position( ));
		}

		fIdleSoundTime -= fDT;
	}

	m_Sounds.m_idle.SetPosition(Position( ));

	// work
	if (o_workload_rframe == (Device.dwFrame - 1))
	{
		;
	}
	else
	{
		UpdateWorkload(fDT);
	}
}

// Core events
void CCrow::net_Export(CNetPacket& P)					// export to server
{
	// export 
	R_ASSERT(Local( ));

	u8					flags = 0;
	P.w_float(GetfHealth( ));

	P.w_float(0);
	P.w_u32(0);
	P.w_u32(0);

	P.w_u32(Level( ).timeServer( ));
	P.w_u8(flags);

	f32				yaw;
	f32 pitch;
	f32 bank;
	XFORM( ).getHPB(yaw, pitch, bank);
	P.w_float /*w_angle8*/(yaw);
	P.w_float /*w_angle8*/(yaw);
	P.w_float /*w_angle8*/(pitch);
	P.w_float /*w_angle8*/(0);
	P.w_u8(u8(g_Team( )));
	P.w_u8(u8(g_Squad( )));
	P.w_u8(u8(g_Group( )));
}
//---------------------------------------------------------------------
void CCrow::net_Import(CNetPacket& P)
{
	// import
	R_ASSERT(Remote( ));

	u8					flags;

	f32 health;
	P.r_float(health);
	SetfHealth(health);

	f32 fDummy;
	u32 dwDummy;
	P.r_float(fDummy);
	P.r_u32(dwDummy);
	P.r_u32(dwDummy);

	P.r_u32(dwDummy);
	P.r_u8(flags);

	f32				yaw;
	f32				pitch;
	f32				bank = 0;
	f32				roll = 0;

	P.r_float /*r_angle8*/(yaw);
	P.r_float /*r_angle8*/(yaw);
	P.r_float /*r_angle8*/(pitch);
	P.r_float /*r_angle8*/(roll);

	id_Team = P.r_u8( );
	id_Squad = P.r_u8( );
	id_Group = P.r_u8( );

	XFORM( ).setHPB(yaw, pitch, bank);
}
//---------------------------------------------------------------------
void CCrow::HitSignal(f32 /**HitAmount/**/, fVector3& /**local_dir/**/, CObject* who, s16 /**element/**/)
{
	//bool				first_time = !!g_Alive(); 
//	bool				first_time = !PPhysicsShell(); 
	SetfHealth(0);
	//set_death_time		()	;
	if (eDeathDead != st_current)
	{
//		if (first_time)	Die			(who);
		st_target = eDeathFall;
	}
	else
	{
		smart_cast<CKinematicsAnimated*>(Visual( ))->PlayCycle(m_Anims.m_death_dead.GetRandom( ));
	}
}
//---------------------------------------------------------------------
void CCrow::HitImpulse(f32	/**amount/**/, fVector3& /**vWorldDir/**/, fVector3& /**vLocalDir/**/)
{ }
//---------------------------------------------------------------------
void CCrow::CreateSkeleton( )
{
	m_pPhysicsShell = P_build_SimpleShell(this, 0.3f, false);
	m_pPhysicsShell->SetMaterial(smart_cast<CKinematics*>(Visual( ))->LL_GetData(smart_cast<CKinematics*>(Visual( ))->LL_GetBoneRoot( )).game_mtl_idx);
}

void	CCrow::Hit(SHit* pHDS)
{
	SHit	HDS = *pHDS;
	HDS.impulse /= 100.f;
	inherited::Hit(&HDS);
}

BOOL CCrow::UsedAI_Locations( )
{
	return		(FALSE);
}

void CCrow::create_physic_shell( )
{
	// do not delete!!!
}
