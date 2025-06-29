// Actor_Movement.cpp:	 ������������ ������
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Actor.h"
#include "Inventory.h"
#include "Weapon.h"
#include "..\XR_3DA\CameraBase.h"
#include "xrMessages.h"

#include "level.h"
#include "HUDManager.h"
#include "ui/UI.h"
#include "StringTable.h"
#include "actorcondition.h"
#include "game_cl_base.h"
#include "WeaponMagazined.h"
#include "CharacterPhysicsSupport.h"

#ifdef DEBUG
#include "phdebug.h"
#endif

static const f32	s_fLandingTime1 = 0.1f;// ����� ������� ����� ���� Landing1 (�.�. �������� ��������� ��������)
static const f32	s_fLandingTime2 = 0.3f;// ����� ������� ����� ���� Landing2 (�.�. �������� ��������� ��������)
static const f32	s_fJumpTime = 0.3f;
static const f32	s_fJumpGroundTime = 0.1f;	// ��� ������ ������ Jump ���� �� �����
const f32	s_fFallTime = 0.2f;

IC static void generate_orthonormal_basis1(const fVector3& dir, fVector3& updir, fVector3& right)
{
	right.crossproduct(dir, updir); //. <->
	right.normalize( );
	updir.crossproduct(right, dir);
}

void CActor::g_cl_ValidateMState(f32 dt, u32 mstate_wf)
{
	// Lookout
	if (mstate_wf & mcLookout)	mstate_real |= mstate_wf & mcLookout;
	else						mstate_real &= ~mcLookout;

	if (mstate_real & (mcJump | mcFall | mcLanding | mcLanding2))
		mstate_real &= ~mcLookout;

	// ��������� �����������
	if (mstate_real & (mcLanding | mcLanding2))
	{
		m_fLandingTime -= dt;
		if (m_fLandingTime <= 0.f)
		{
			mstate_real &= ~(mcLanding | mcLanding2);
			mstate_real &= ~(mcFall | mcJump);
		}
	}

	// ��������� �������
	if (character_physics_support( )->movement( )->gcontact_Was)
	{
		if (mstate_real & mcFall)
		{
			if (character_physics_support( )->movement( )->GetContactSpeed( ) > 4.f)
			{
				if (fis_zero(character_physics_support( )->movement( )->gcontact_HealthLost))
				{
					m_fLandingTime = s_fLandingTime1;
					mstate_real |= mcLanding;
				}
				else
				{
					m_fLandingTime = s_fLandingTime2;
					mstate_real |= mcLanding2;
				}
			}
		}

		m_bJumpKeyPressed = TRUE;
		m_fJumpTime = s_fJumpTime;
		mstate_real &= ~(mcFall | mcJump);
	}

	if ((mstate_wf & mcJump) == 0)
		m_bJumpKeyPressed = FALSE;

	// ������-�� ����/������ - �� ��������
	if (((character_physics_support( )->movement( )->GetVelocityActual( ) < 0.2f) && (!(mstate_real & (mcFall | mcJump)))) || character_physics_support( )->movement( )->bSleep)
	{
		mstate_real &= ~mcAnyMove;
	}

	if (character_physics_support( )->movement( )->Environment( ) == CPHMovementControl::peOnGround || character_physics_support( )->movement( )->Environment( ) == CPHMovementControl::peAtWall)
	{
		// ���� �� ����� �������������� ������� ������ Jump
		if (((s_fJumpTime - m_fJumpTime) > s_fJumpGroundTime) && (mstate_real & mcJump))
		{
			mstate_real &= ~mcJump;
			m_fJumpTime = s_fJumpTime;
		}
	}

	if (character_physics_support( )->movement( )->Environment( ) == CPHMovementControl::peAtWall)
	{
		if (!(mstate_real & mcClimb))
		{
			mstate_real |= mcClimb;
			mstate_real &= ~mcSprint;
			cam_SetLadder( );
		}
	}
	else
	{
		if (mstate_real & mcClimb)
		{
			cam_UnsetLadder( );
		}
		mstate_real &= ~mcClimb;
	}

	if (mstate_wf != mstate_real)
	{
		if ((mstate_real & mcCrouch) && ((0 == (mstate_wf & mcCrouch)) || mstate_real & mcClimb))
		{
			if (character_physics_support( )->movement( )->ActivateBoxDynamic(0))
			{
				mstate_real &= ~mcCrouch;
			}
		}
	}

	if (!CanAccelerate( ) && isActorAccelerated(mstate_real, IsZoomAimingMode( )))
	{
		mstate_real ^= mcAccel;
	}

	if (this == Level( ).CurrentControlEntity( ))
	{
		bool bOnClimbNow = !!(mstate_real & mcClimb);
		bool bOnClimbOld = !!(mstate_old & mcClimb);

		if (bOnClimbNow != bOnClimbOld)
		{
			SetWeaponHideState(INV_STATE_LADDER, bOnClimbNow);
		}
	/*
	if ((mstate_real&mcSprint) != (mstate_old&mcSprint))
	{
		CHudItem* pHudItem = smart_cast<CHudItem*>(inventory().ActiveItem());
		if (pHudItem) pHudItem->onMovementChanged(mcSprint);
	};
	*/
	}
}

void CActor::g_cl_CheckControls(u32 mstate_wf, fVector3& vControlAccel, f32& Jump, f32 dt)
{
	mstate_old = mstate_real;
	vControlAccel.set(0, 0, 0);

	if (!(mstate_real & mcFall) && (character_physics_support( )->movement( )->Environment( ) == CPHMovementControl::peInAir))
	{
		m_fFallTime -= dt;
		if (m_fFallTime <= 0.f)
		{
			m_fFallTime = s_fFallTime;
			mstate_real |= mcFall;
			mstate_real &= ~mcJump;
		}
	}

	if (!CanMove( ))
	{
		if (mstate_wf & mcAnyMove)
		{
			StopAnyMove( );
			mstate_wf &= ~mcAnyMove;
			mstate_wf &= ~mcJump;
		}
			//character_physics_support()->movement()->EnableCharacter();
		//return;
	}

	// update player accel
	if (mstate_wf & mcFwd)		vControlAccel.z += 1;
	if (mstate_wf & mcBack)		vControlAccel.z += -1;
	if (mstate_wf & mcLStrafe)	vControlAccel.x += -1;
	if (mstate_wf & mcRStrafe)	vControlAccel.x += 1;

	if (character_physics_support( )->movement( )->Environment( ) == CPHMovementControl::peOnGround || character_physics_support( )->movement( )->Environment( ) == CPHMovementControl::peAtWall)
	{
		// crouch
		if ((0 == (mstate_real & mcCrouch)) && (mstate_wf & mcCrouch))
		{
			if (mstate_real & mcClimb)
			{
				mstate_wf &= ~mcCrouch;
			}
			else
			{
				character_physics_support( )->movement( )->EnableCharacter( );
				bool Crouched = false;
				if (isActorAccelerated(mstate_wf, IsZoomAimingMode( )))
					Crouched = character_physics_support( )->movement( )->ActivateBoxDynamic(1);
				else
					Crouched = character_physics_support( )->movement( )->ActivateBoxDynamic(2);
				if (Crouched) mstate_real |= mcCrouch;
			}
		}
		// jump
		m_fJumpTime -= dt;

		if (CanJump( ) && (mstate_wf & mcJump))
		{
			mstate_real |= mcJump;
			m_bJumpKeyPressed = TRUE;
			Jump = m_fJumpSpeed;
			m_fJumpTime = s_fJumpTime;


			//��������� ���� ������ ��-�� ����������� ������
			if (!GodMode( ))
				conditions( ).ConditionJump(inventory( ).TotalWeight( ) / MaxCarryWeight( ));
		}

		/*
		if(m_bJumpKeyPressed)
		Jump				= m_fJumpSpeed;
		*/


		// mask input into "real" state
		u32 move = mcAnyMove | mcAccel;

		if (((mstate_real & mcCrouch)))
		{
			if (!isActorAccelerated(mstate_real, IsZoomAimingMode( )) && isActorAccelerated(mstate_wf, IsZoomAimingMode( )))
			{
				character_physics_support( )->movement( )->EnableCharacter( );
				if (!character_physics_support( )->movement( )->ActivateBoxDynamic(1))move &= ~mcAccel;
			}

			if (isActorAccelerated(mstate_real, IsZoomAimingMode( )) && !isActorAccelerated(mstate_wf, IsZoomAimingMode( )))
			{
				character_physics_support( )->movement( )->EnableCharacter( );
				if (character_physics_support( )->movement( )->ActivateBoxDynamic(2))mstate_real &= ~mcAccel;
			}
		}

		if ((mstate_wf & mcSprint) && !CanSprint( ))
		{
			mstate_wf &= ~mcSprint;
		}

		mstate_real &= (~move);
		mstate_real |= (mstate_wf & move);

		if (mstate_wf & mcSprint)
			mstate_real |= mcSprint;
		else
			mstate_real &= ~mcSprint;
		if (!(mstate_real & (mcFwd | mcLStrafe | mcRStrafe)) || mstate_real & (mcCrouch | mcClimb) || !isActorAccelerated(mstate_wf, IsZoomAimingMode( )))
		{
			mstate_real &= ~mcSprint;
			mstate_wishful &= ~mcSprint;
		}

		// check player move state
		if (mstate_real & mcAnyMove)
		{
			BOOL	bAccelerated = isActorAccelerated(mstate_real, IsZoomAimingMode( )) && CanAccelerate( );

			// correct "mstate_real" if opposite keys pressed
			if (_abs(vControlAccel.z) < EPSILON_5)	mstate_real &= ~(mcFwd + mcBack);
			if (_abs(vControlAccel.x) < EPSILON_5)	mstate_real &= ~(mcLStrafe + mcRStrafe);

			// normalize and analyze crouch and run
			f32	scale = vControlAccel.magnitude( );
			if (scale > EPSILON_5)
			{
				scale = m_fWalkAccel / scale;
				if (bAccelerated)
					if (mstate_real & mcBack)
						scale *= m_fRunBackFactor;
					else
						scale *= m_fRunFactor;
				else
					if (mstate_real & mcBack)
						scale *= m_fWalkBackFactor;

				if (mstate_real & mcCrouch)	scale *= m_fCrouchFactor;
				if (mstate_real & mcClimb)	scale *= m_fClimbFactor;
				if (mstate_real & mcSprint)	scale *= m_fSprintFactor;

				if (mstate_real & (mcLStrafe | mcRStrafe) && !(mstate_real & mcCrouch))
				{
					if (bAccelerated)
						scale *= m_fRun_StrafeFactor;
					else
						scale *= m_fWalk_StrafeFactor;
				}

				vControlAccel.mul(scale);
			}
			else
			{
						//				mstate_real	&= ~mcAnyMove;
			}
		}
	}
	else
	{
		//		mstate_real			&=~ mcAnyMove;
	}

	//-------------------------------------------------------------------------------	

	//transform local dir to world dir
	fMatrix4x4				mOrient;
	mOrient.rotateY(-r_model_yaw);
	mOrient.transform_dir(vControlAccel);
	//XFORM().transform_dir(vControlAccel);

	/*
	if(mstate_real&mcClimb&&mstate_real&mcAnyMove&&
	inventory().ActiveItem()&&inventory().ActiveItem()->HandDependence()==eHD_2_HAND)
	{
		//inventory().ActiveItem()->Deactivate();
		inventory().Activate(NO_ACTIVE_SLOT);
	}
*/
}

#define ACTOR_ANIM_SECT "actor_animation"

void CActor::g_Orientate(u32 mstate_rl, f32 dt)
{
	static f32 fwd_l_strafe_yaw = deg2rad(pSettings->r_float(ACTOR_ANIM_SECT, "fwd_l_strafe_yaw"));
	static f32 back_l_strafe_yaw = deg2rad(pSettings->r_float(ACTOR_ANIM_SECT, "back_l_strafe_yaw"));
	static f32 fwd_r_strafe_yaw = deg2rad(pSettings->r_float(ACTOR_ANIM_SECT, "fwd_r_strafe_yaw"));
	static f32 back_r_strafe_yaw = deg2rad(pSettings->r_float(ACTOR_ANIM_SECT, "back_r_strafe_yaw"));
	static f32 l_strafe_yaw = deg2rad(pSettings->r_float(ACTOR_ANIM_SECT, "l_strafe_yaw"));
	static f32 r_strafe_yaw = deg2rad(pSettings->r_float(ACTOR_ANIM_SECT, "r_strafe_yaw"));

	if (!g_Alive( ))return;
	// visual effect of "fwd+strafe" like motion
	f32 calc_yaw = 0.0f;
	if (mstate_real & mcClimb)
	{
		if (g_LadderOrient( )) return;
	}
	switch (mstate_rl & mcAnyMove)
	{
		case mcFwd + mcLStrafe:
			calc_yaw = +fwd_l_strafe_yaw;//+PI_DIV_4; 
			break;
		case mcBack + mcRStrafe:
			calc_yaw = +back_r_strafe_yaw;//+PI_DIV_4; 
			break;
		case mcFwd + mcRStrafe:
			calc_yaw = -fwd_r_strafe_yaw;//-PI_DIV_4; 
			break;
		case mcBack + mcLStrafe:
			calc_yaw = -back_l_strafe_yaw;//-PI_DIV_4; 
			break;
		case mcLStrafe:
			calc_yaw = +l_strafe_yaw;//+PI_DIV_3-EPSILON_3; 
			break;
		case mcRStrafe:
			calc_yaw = -r_strafe_yaw;//-PI_DIV_4+EPSILON_3; 
			break;
	}

	// lerp angle for "effect" and capture torso data from camera
	angle_lerp(r_model_yaw_delta, calc_yaw, PI_MUL_4, dt);

	// build matrix
	fMatrix4x4 mXFORM;
	mXFORM.rotateY(-(r_model_yaw + r_model_yaw_delta));
	mXFORM.c.set(Position( ));
	XFORM( ).set(mXFORM);

	//-------------------------------------------------

	f32 tgt_roll = 0.0f;
	if (mstate_rl & mcLookout)
	{
		tgt_roll = (mstate_rl & mcLLookout) ? -ACTOR_LOOKOUT_ANGLE : ACTOR_LOOKOUT_ANGLE;

		if ((mstate_rl & mcLLookout) && (mstate_rl & mcRLookout))
		{
			tgt_roll = 0.0f;
		}
	}

	if (!fsimilar(tgt_roll, r_torso_tgt_roll, EPSILON_5))
	{
		angle_lerp(r_torso_tgt_roll, tgt_roll, PI_MUL_2, dt);
		r_torso_tgt_roll = angle_normalize_signed(r_torso_tgt_roll);
	}
}

bool CActor::g_LadderOrient( )
{
	fVector3 leader_norm;
	character_physics_support( )->movement( )->GroundNormal(leader_norm);
	if (_abs(leader_norm.y) > M_SQRT1_2)
	{
		return false;
	}

	//leader_norm.y=0.f;
	f32 mag = leader_norm.magnitude( );
	if (mag < EPSILON_3) return false;
	leader_norm.div(mag);
	leader_norm.invert( );
	fMatrix4x4 M;
	M.set(Fidentity);
	M.k.set(leader_norm);
	M.j.set(0.f, 1.f, 0.f);
	generate_orthonormal_basis1(M.k, M.j, M.i);
	M.i.invert( );
	//M.j.invert();


	//fQuaternion q1,q2,q3;
	//q1.set(XFORM());
	//q2.set(M);
	//q3.slerp(q1,q2,dt);
	//fVector3 angles1,angles2,angles3;
	//XFORM().getHPB(angles1.x,angles1.y,angles1.z);
	//M.getHPB(angles2.x,angles2.y,angles2.z);
	////angle_lerp(angles3.x,angles1.x,angles2.x,dt);
	////angle_lerp(angles3.y,angles1.y,angles2.y,dt);
	////angle_lerp(angles3.z,angles1.z,angles2.z,dt);

	//angles3.lerp(angles1,angles2,dt);
	////angle_lerp(angles3.y,angles1.y,angles2.y,dt);
	////angle_lerp(angles3.z,angles1.z,angles2.z,dt);
	//angle_lerp(angles3.x,angles1.x,angles2.x,dt);
	//XFORM().setHPB(angles3.x,angles3.y,angles3.z);
	fVector3 position;
	position.set(Position( ));
	//XFORM().rotation(q3);
	VERIFY2(_valid(M), "Invalide matrix in g_LadderOrient");
	XFORM( ).set(M);
	VERIFY2(_valid(position), "Invalide position in g_LadderOrient");
	Position( ).set(position);
	return true;
}
// ****************************** Update actor orientation according to camera orientation
void CActor::g_cl_Orientate(u32 mstate_rl, f32 dt)
{
	// capture camera into torso (only for FirstEye & LookAt cameras)
	if (eacFreeLook != cam_active)
	{
		r_torso.yaw = cam_Active( )->GetWorldYaw( );
		r_torso.pitch = cam_Active( )->GetWorldPitch( );
	}
	else
	{
		r_torso.yaw = cam_FirstEye( )->GetWorldYaw( );
		r_torso.pitch = cam_FirstEye( )->GetWorldPitch( );
	}

	unaffected_r_torso.yaw = r_torso.yaw;
	unaffected_r_torso.pitch = r_torso.pitch;
	unaffected_r_torso.roll = r_torso.roll;

	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*>(inventory( ).GetActiveSlot( ) != NO_ACTIVE_SLOT ? inventory( ).ItemFromSlot(inventory( ).GetActiveSlot( )) : NULL);
	if (pWM && pWM->GetCurrentFireMode( ) == 1 && eacFirstEye != cam_active)
	{
		fVector3 dangle = weapon_recoil_last_delta( );
		r_torso.yaw = unaffected_r_torso.yaw + dangle.y;
		r_torso.pitch = unaffected_r_torso.pitch + dangle.x;
	}

	// ���� ���� �������� - ��������� ������ �� ������
	if (mstate_rl & mcAnyMove)
	{
		r_model_yaw = angle_normalize(r_torso.yaw);
		mstate_real &= ~mcTurn;
	}
	else
	{
	 // if camera rotated more than 45 degrees - align model with it
		f32 ty = angle_normalize(r_torso.yaw);
		if (_abs(r_model_yaw - ty) > PI_DIV_4)
		{
			r_model_yaw_dest = ty;
			// 
			mstate_real |= mcTurn;
		}

		if (_abs(r_model_yaw - r_model_yaw_dest) < EPSILON_3)
		{
			mstate_real &= ~mcTurn;
		}

		if (mstate_rl & mcTurn)
		{
			angle_lerp(r_model_yaw, r_model_yaw_dest, PI_MUL_2, dt);
		}
	}
}

void CActor::g_sv_Orientate(u32 /**mstate_rl/**/, f32 /**dt/**/)
{
	// rotation
	r_model_yaw = NET_Last.o_model;

	r_torso.yaw = unaffected_r_torso.yaw;
	r_torso.pitch = unaffected_r_torso.pitch;
	r_torso.roll = unaffected_r_torso.roll;

	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*>(inventory( ).GetActiveSlot( ) != NO_ACTIVE_SLOT ? inventory( ).ItemFromSlot(inventory( ).GetActiveSlot( )) : NULL);
	if (pWM && pWM->GetCurrentFireMode( ) == 1/* && eacFirstEye != cam_active*/)
	{
		fVector3 dangle = weapon_recoil_last_delta( );
		r_torso.yaw += dangle.y;
		r_torso.pitch += dangle.x;
		r_torso.roll += dangle.z;
	}
}

bool isActorAccelerated(u32 mstate, bool ZoomMode)
{
	bool res = false;
	if (mstate & mcAccel)
	{
		res = psActorFlags.test(AF_ALWAYSRUN) ? false : true;
	}
	else
	{
		res = psActorFlags.test(AF_ALWAYSRUN) ? true : false;
	}

	if (mstate & (mcCrouch | mcClimb | mcJump | mcLanding | mcLanding2))
	{
		return res;
	}

	if (mstate & mcLookout || ZoomMode)
	{
		return false;
	}

	return res;
}

bool CActor::CanAccelerate( )
{
	bool can_accel = !conditions( ).IsLimping( ) &&
		!character_physics_support( )->movement( )->PHCapture( ) &&
//		&& !m_bZoomAimingMode
//		&& !(mstate_real&mcLookout)
(m_time_lock_accel < Device.dwTimeGlobal)
	;

	return can_accel;
}

bool CActor::CanRun( )
{
	bool can_run = !m_bZoomAimingMode && !(mstate_real & mcLookout);
	return can_run;
}

bool CActor::CanSprint( )
{
	bool can_Sprint = CanAccelerate( ) && !conditions( ).IsCantSprint( ) &&
		Game( ).PlayerCanSprint(this)
		&& CanRun( )
		&& !(mstate_real & mcLStrafe || mstate_real & mcRStrafe)
		&& InventoryAllowSprint( )
		;

	return can_Sprint;
}

bool CActor::CanJump( )
{
	bool can_Jump = /*!IsLimping() &&*/
		!character_physics_support( )->movement( )->PHCapture( ) && ((mstate_real & mcJump) == 0) && (m_fJumpTime <= 0.0f)
		&& !m_bJumpKeyPressed && !m_bZoomAimingMode;// && ((mstate_real&mcCrouch)==0);

	return can_Jump;
}

bool CActor::CanMove( )
{
	if (conditions( ).IsCantWalk( ))
	{
		if (mstate_wishful & mcAnyMove)
		{
			HUD( ).GetUI( )->AddInfoMessage("cant_walk");
		}

		return false;
	}
	else if (conditions( ).IsCantWalkWeight( ))
	{
		if (mstate_wishful & mcAnyMove)
		{
			HUD( ).GetUI( )->AddInfoMessage("cant_walk_weight");
		}

		return false;
	}

	if (IsTalking( ))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void CActor::StopAnyMove( )
{
	mstate_wishful &= ~mcAnyMove;
	mstate_real &= ~mcAnyMove;
}

bool CActor::is_jump( )
{
	return ((mstate_real & (mcJump | mcFall | mcLanding | mcLanding2)) != 0);
}
