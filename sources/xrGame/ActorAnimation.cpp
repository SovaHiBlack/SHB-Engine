#include "stdafx.h"
#include "Actor.h"
//#include "ActorAnimation.h"
#include "actor_anim_defs.h"

#include "hudmanager.h"
#include "ui/UI.h"
#include "Weapon.h"
#include "Inventory.h"
#include "Missile.h"
#include "Level.h"
#ifdef DEBUG
#include "PHDebug.h"
#endif
#include "Hit.h"
#include "PHDestroyable.h"
#include "Car.h"
#include "..\XR_3DA\skeletonanimated.h"
#include "clsid_game.h"
#include "ai_object_location.h"
#include "game_cl_base.h"
#include "..\XR_3DA\motion.h"
#include "Artefact.h"
#include "IKLimbsController.h"

static const f32 y_spin0_factor = 0.0f;
static const f32 y_spin1_factor = 0.4f;
static const f32 y_shoulder_factor = 0.4f;
static const f32 y_head_factor = 0.2f;
static const f32 p_spin0_factor = 0.0f;
static const f32 p_spin1_factor = 0.2f;
static const f32 p_shoulder_factor = 0.7f;
static const f32 p_head_factor = 0.1f;
static const f32 r_spin0_factor = 0.3f;
static const f32 r_spin1_factor = 0.3f;
static const f32 r_shoulder_factor = 0.2f;
static const f32 r_head_factor = 0.2f;

void  CActor::Spin0Callback(CBoneInstance* B)
{
	CActor* A = static_cast<CActor*>(B->Callback_Param);	VERIFY(A);

	fMatrix4x4				spin;
	f32				bone_yaw = angle_normalize_signed(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta) * y_spin0_factor;
	f32				bone_pitch = angle_normalize_signed(A->r_torso.pitch) * p_spin0_factor;
	f32				bone_roll = angle_normalize_signed(A->r_torso.roll) * r_spin0_factor;
	fVector3 c = B->mTransform.c;
	spin.setXYZ(-bone_pitch, bone_yaw, bone_roll);
	B->mTransform.mulA_43(spin);
	B->mTransform.c = c;
}

void  CActor::Spin1Callback(CBoneInstance* B)
{
	CActor* A = static_cast<CActor*>(B->Callback_Param);	VERIFY(A);

	fMatrix4x4				spin;
	f32				bone_yaw = angle_normalize_signed(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta) * y_spin1_factor;
	f32				bone_pitch = angle_normalize_signed(A->r_torso.pitch) * p_spin1_factor;
	f32				bone_roll = angle_normalize_signed(A->r_torso.roll) * r_spin1_factor;
	fVector3 c = B->mTransform.c;
	spin.setXYZ(-bone_pitch, bone_yaw, bone_roll);
	B->mTransform.mulA_43(spin);
	B->mTransform.c = c;
}

void  CActor::ShoulderCallback(CBoneInstance* B)
{
	CActor* A = static_cast<CActor*>(B->Callback_Param);	VERIFY(A);
	fMatrix4x4				spin;
	f32				bone_yaw = angle_normalize_signed(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta) * y_shoulder_factor;
	f32				bone_pitch = angle_normalize_signed(A->r_torso.pitch) * p_shoulder_factor;
	f32				bone_roll = angle_normalize_signed(A->r_torso.roll) * r_shoulder_factor;
	fVector3 c = B->mTransform.c;
	spin.setXYZ(-bone_pitch, bone_yaw, bone_roll);
	B->mTransform.mulA_43(spin);
	B->mTransform.c = c;
}

void  CActor::HeadCallback(CBoneInstance* B)
{
	CActor* A = static_cast<CActor*>(B->Callback_Param);	VERIFY(A);
	fMatrix4x4				spin;
	f32				bone_yaw = angle_normalize_signed(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta) * y_head_factor;
	f32				bone_pitch = angle_normalize_signed(A->r_torso.pitch) * p_head_factor;
	f32				bone_roll = angle_normalize_signed(A->r_torso.roll) * r_head_factor;
	fVector3 c = B->mTransform.c;
	spin.setXYZ(-bone_pitch, bone_yaw, bone_roll);
	B->mTransform.mulA_43(spin);
	B->mTransform.c = c;
}

void  CActor::VehicleHeadCallback(CBoneInstance* B)
{
	CActor* A = static_cast<CActor*>(B->Callback_Param);	VERIFY(A);
	fMatrix4x4				spin;
	f32				bone_yaw = angle_normalize_signed(A->r_torso.yaw) * 0.75f;
	f32				bone_pitch = angle_normalize_signed(A->r_torso.pitch) * 0.75f;
	f32				bone_roll = angle_normalize_signed(A->r_torso.roll) * r_head_factor;
	fVector3 c = B->mTransform.c;
	spin.setHPB(bone_yaw, bone_pitch, -bone_roll);
	B->mTransform.mulA_43(spin);
	B->mTransform.c = c;
}

void STorsoWpn::Create(CKinematicsAnimated* K, pcstr base0, pcstr base1)
{
	char			buf[128];
	moving[eIdle] = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_torso", base1, "_aim_1"));
	moving[eWalk] = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_torso", base1, "_aim_2"));
	moving[eRun] = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_torso", base1, "_aim_3"));
	moving[eSprint] = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_torso", base1, "_escape_0"));
	zoom = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_torso", base1, "_aim_0"));
	holster = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_torso", base1, "_holster_0"));
	draw = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_torso", base1, "_draw_0"));
	reload = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_torso", base1, "_reload_0"));
	reload_1 = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_torso", base1, "_reload_1"));
	reload_2 = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_torso", base1, "_reload_2"));
	drop = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_torso", base1, "_drop_0"));
	attack = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_torso", base1, "_attack_1"));
	attack_zoom = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_torso", base1, "_attack_0"));
	fire_idle = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_torso", base1, "_attack_1"));
	fire_end = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_torso", base1, "_attack_2"));
	all_attack_0 = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_all", base1, "_attack_0"));
	all_attack_1 = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_all", base1, "_attack_1"));
	all_attack_2 = K->ID_Cycle_Safe(strconcat(sizeof(buf), buf, base0, "_all", base1, "_attack_2"));
}

void SAnimState::Create(CKinematicsAnimated* K, pcstr base0, pcstr base1)
{
	char			buf[128];
	legs_fwd = K->ID_Cycle(strconcat(sizeof(buf), buf, base0, base1, "_fwd_0"));
	legs_back = K->ID_Cycle(strconcat(sizeof(buf), buf, base0, base1, "_back_0"));
	legs_ls = K->ID_Cycle(strconcat(sizeof(buf), buf, base0, base1, "_ls_0"));
	legs_rs = K->ID_Cycle(strconcat(sizeof(buf), buf, base0, base1, "_rs_0"));
}

void SActorState::CreateClimb(CKinematicsAnimated* K)
{
	string128		buf, buf1;
	string16		base;

	//climb anims
	strcpy(base, "cl");
	legs_idle = K->ID_Cycle(strconcat(sizeof(buf), buf, base, "_idle_1"));
	m_torso_idle = K->ID_Cycle(strconcat(sizeof(buf), buf, base, "_torso_0_aim_0"));
	m_walk.Create(K, base, "_run");
	m_run.Create(K, base, "_run");

	//norm anims
	strcpy(base, "norm");
	legs_turn = K->ID_Cycle(strconcat(sizeof(buf), buf, base, "_turn"));
	death = K->ID_Cycle(strconcat(sizeof(buf), buf, base, "_death_0"));
	m_torso[0].Create(K, base, "_1");
	m_torso[1].Create(K, base, "_2");
	m_torso[2].Create(K, base, "_3");
	m_torso[3].Create(K, base, "_4");
	m_torso[4].Create(K, base, "_5");
	m_torso[5].Create(K, base, "_6");
	m_torso[6].Create(K, base, "_7");
	m_torso[7].Create(K, base, "_8");
	m_torso[8].Create(K, base, "_9");
	m_torso[9].Create(K, base, "_10");
	m_torso[10].Create(K, base, "_11");
	m_torso[11].Create(K, base, "_12");
	m_torso[12].Create(K, base, "_13");

	m_head_idle.invalidate( );///K->ID_Cycle("head_idle_0");
	jump_begin = K->ID_Cycle(strconcat(sizeof(buf), buf, base, "_jump_begin"));
	jump_idle = K->ID_Cycle(strconcat(sizeof(buf), buf, base, "_jump_idle"));
	landing[0] = K->ID_Cycle(strconcat(sizeof(buf), buf, base, "_jump_end"));
	landing[1] = K->ID_Cycle(strconcat(sizeof(buf), buf, base, "_jump_end_1"));

	for (s32 k = 0; k < 12; ++k)
		m_damage[k] = K->ID_FX(strconcat(sizeof(buf), buf, base, "_damage_", itoa(k, buf1, 10)));
}

void SActorState::Create(CKinematicsAnimated* K, pcstr base)
{
	string128		buf, buf1;
	legs_turn = K->ID_Cycle(strconcat(sizeof(buf), buf, base, "_turn"));
	legs_idle = K->ID_Cycle(strconcat(sizeof(buf), buf, base, "_idle_0"));
	death = K->ID_Cycle(strconcat(sizeof(buf), buf, base, "_death_0"));

	m_walk.Create(K, base, "_walk");
	m_run.Create(K, base, "_run");

	m_torso[0].Create(K, base, "_1");
	m_torso[1].Create(K, base, "_2");
	m_torso[2].Create(K, base, "_3");
	m_torso[3].Create(K, base, "_4");
	m_torso[4].Create(K, base, "_5");
	m_torso[5].Create(K, base, "_6");
	m_torso[6].Create(K, base, "_7");
	m_torso[7].Create(K, base, "_8");
	m_torso[8].Create(K, base, "_9");
	m_torso[9].Create(K, base, "_10");
	m_torso[10].Create(K, base, "_11");
	m_torso[11].Create(K, base, "_12");
	m_torso[12].Create(K, base, "_13");

	m_torso_idle = K->ID_Cycle(strconcat(sizeof(buf), buf, base, "_torso_0_aim_0"));
	m_head_idle = K->ID_Cycle("head_idle_0");
	jump_begin = K->ID_Cycle(strconcat(sizeof(buf), buf, base, "_jump_begin"));
	jump_idle = K->ID_Cycle(strconcat(sizeof(buf), buf, base, "_jump_idle"));
	landing[0] = K->ID_Cycle(strconcat(sizeof(buf), buf, base, "_jump_end"));
	landing[1] = K->ID_Cycle(strconcat(sizeof(buf), buf, base, "_jump_end_1"));

	for (s32 k = 0; k < 12; ++k)
		m_damage[k] = K->ID_FX(strconcat(sizeof(buf), buf, base, "_damage_", itoa(k, buf1, 10)));
}

void SActorSprintState::Create(CKinematicsAnimated* K)
{
	//leg anims
	legs_fwd = K->ID_Cycle("norm_escape_00");
	legs_ls = K->ID_Cycle("norm_escape_ls_00");
	legs_rs = K->ID_Cycle("norm_escape_rs_00");
}

void SActorMotions::Create(CKinematicsAnimated* V)
{
	m_dead_stop = V->ID_Cycle("norm_dead_stop_0");

	m_normal.Create(V, "norm");
	m_crouch.Create(V, "cr");
	//m_climb.Create	(V,"cr");
	m_climb.CreateClimb(V);
	m_sprint.Create(V);
}

SActorVehicleAnims::SActorVehicleAnims( )
{ }

void SActorVehicleAnims::Create(CKinematicsAnimated* V)
{
	for (u16 i = 0; TYPES_NUMBER > i; ++i) m_vehicles_type_collections[i].Create(V, i);
}

SVehicleAnimCollection::SVehicleAnimCollection( )
{
	for (u16 i = 0; MAX_IDLES > i; ++i) idles[i].invalidate( );
	idles_num = 0;
	steer_left.invalidate( );
	steer_right.invalidate( );
}

void SVehicleAnimCollection::Create(CKinematicsAnimated* V, u16 num)
{
	string128 buf, buff1, buff2;
	strconcat(sizeof(buff1), buff1, itoa(num, buf, 10), "_");
	steer_left = V->ID_Cycle(strconcat(sizeof(buf), buf, "steering_idle_", buff1, "ls"));
	steer_right = V->ID_Cycle(strconcat(sizeof(buf), buf, "steering_idle_", buff1, "rs"));

	for (s32 i = 0; MAX_IDLES > i; ++i)
	{
		idles[i] = V->ID_Cycle_Safe(strconcat(sizeof(buf), buf, "steering_idle_", buff1, itoa(i, buff2, 10)));
		if (idles[i]) idles_num++;
		else break;
	}
}

void CActor::steer_Vehicle(f32 angle)
{
	if (!m_holder)		return;
	CCar* car = smart_cast<CCar*>(m_holder);
	u16 anim_type = car->DriverAnimationType( );
	SVehicleAnimCollection& anims = m_vehicle_anims->m_vehicles_type_collections[anim_type];
	if (angle == 0.f) 		smart_cast<CKinematicsAnimated*>	(Visual( ))->PlayCycle(anims.idles[0]);
	else if (angle > 0.f)	smart_cast<CKinematicsAnimated*>	(Visual( ))->PlayCycle(anims.steer_right);
	else				smart_cast<CKinematicsAnimated*>	(Visual( ))->PlayCycle(anims.steer_left);
}

void legs_play_callback(CBlend* blend)
{
	CActor* object = (CActor*)blend->CallbackParam;
	VERIFY(object);
	object->m_current_legs.invalidate( );
}

void CActor::g_SetSprintAnimation(u32 mstate_rl, MotionID& head, MotionID& torso, MotionID& legs)
{
	SActorSprintState& sprint = m_anims->m_sprint;

	if (mstate_rl & mcFwd)		legs = sprint.legs_fwd;
	else if (mstate_rl & mcLStrafe) legs = sprint.legs_ls;
	else if (mstate_rl & mcRStrafe)	legs = sprint.legs_rs;
}

CMotion* FindMotionKeys(MotionID motion_ID, IRenderVisual* V)
{
	CKinematicsAnimated* VA = PKinematicsAnimated(V);
	return (VA && motion_ID.valid( )) ? VA->LL_GetRootMotion(motion_ID) : 0;
}

#ifdef DEBUG
BOOL	g_ShowAnimationInfo = TRUE;
#endif // DEBUG

pstr mov_state[ ] = {
	"idle",
	"walk",
	"run",
	"sprint",
};

void CActor::g_SetAnimation(u32 mstate_rl)
{
	if (!g_Alive( ))
	{
		if (m_current_legs || m_current_torso)
		{
			SActorState* ST = 0;
			if (mstate_rl & mcCrouch)		ST = &m_anims->m_crouch;
			else						ST = &m_anims->m_normal;
			mstate_real = 0;
			m_current_legs.invalidate( );
			m_current_torso.invalidate( );

			smart_cast<CKinematicsAnimated*>(Visual( ))->PlayCycle(m_anims->m_dead_stop);
		}

		return;
	}

	STorsoWpn::eMovingState	moving_idx = STorsoWpn::eIdle;
	SActorState* ST = 0;
	SAnimState* AS = 0;

	if (mstate_rl & mcCrouch)	ST = &m_anims->m_crouch;
	else if (mstate_rl & mcClimb)		ST = &m_anims->m_climb;
	else							ST = &m_anims->m_normal;

	bool bAccelerated = isActorAccelerated(mstate_rl, IsZoomAimingMode( ));
	if (bAccelerated)
	{
		AS = &ST->m_run;
	}
	else
	{
		AS = &ST->m_walk;
	}
	if (mstate_rl & mcAnyMove)
	{
		if (bAccelerated)
			moving_idx = STorsoWpn::eRun;
		else
			moving_idx = STorsoWpn::eWalk;
	}
	// ��������
	MotionID 						M_legs;
	MotionID 						M_torso;
	MotionID 						M_head;

	//���� �� ������ ����� �� �����
	bool is_standing = false;

	// Legs
	if (mstate_rl & mcLanding)	M_legs = ST->landing[0];
	else if (mstate_rl & mcLanding2)	M_legs = ST->landing[1];
	else if ((mstate_rl & mcTurn) &&
			 !(mstate_rl & mcClimb))	M_legs = ST->legs_turn;
	else if (mstate_rl & mcFall)		M_legs = ST->jump_idle;
	else if (mstate_rl & mcJump)		M_legs = ST->jump_begin;
	else if (mstate_rl & mcFwd)		M_legs = AS->legs_fwd;
	else if (mstate_rl & mcBack)		M_legs = AS->legs_back;
	else if (mstate_rl & mcLStrafe)	M_legs = AS->legs_ls;
	else if (mstate_rl & mcRStrafe)	M_legs = AS->legs_rs;
	else is_standing = true;

	if (mstate_rl & mcSprint)
	{
		g_SetSprintAnimation(mstate_rl, M_head, M_torso, M_legs);
		moving_idx = STorsoWpn::eSprint;
	}
	//---------------------------------------------------------------
	if (this == Level( ).CurrentViewEntity( ))
	{
		if ((mstate_rl & mcSprint) != (mstate_old & mcSprint))
		{
			CHudItem* pHudItem = smart_cast<CHudItem*>(inventory( ).ActiveItem( ));
			if (pHudItem) pHudItem->onMovementChanged(mcSprint);
		}
	}
	//-----------------------------------------------------------------------
	// Torso
	if (mstate_rl & mcClimb)
	{
		if (mstate_rl & mcFwd)		M_torso = AS->legs_fwd;
		else if (mstate_rl & mcBack)		M_torso = AS->legs_back;
		else if (mstate_rl & mcLStrafe)	M_torso = AS->legs_ls;
		else if (mstate_rl & mcRStrafe)	M_torso = AS->legs_rs;
	}

	if (!M_torso)
	{
		CInventoryItem* _i = inventory( ).ActiveItem( );
		CHudItem* H = smart_cast<CHudItem*>(_i);
		CWeapon* W = smart_cast<CWeapon*>(_i);
		CMissile* M = smart_cast<CMissile*>(_i);
		CArtefact* A = smart_cast<CArtefact*>(_i);

		if (H)
		{
			VERIFY(H->animation_slot( ) <= _total_anim_slots_);
			STorsoWpn* TW = &ST->m_torso[H->animation_slot( ) - 1];
			if (!b_DropActivated && !fis_zero(f_DropPower))
			{
				M_torso = TW->drop;
				if (!M_torso)
				{
					Msg("! drop animation for %s", *(H->object( ).cName( )));
					M_torso = ST->m_torso_idle;
				};
				m_bAnimTorsoPlayed = TRUE;
			}
			else
			{
				if (!m_bAnimTorsoPlayed)
				{
					if (W)
					{
						bool K = inventory( ).GetActiveSlot( ) == KNIFE_SLOT;
						bool R3 = W->IsTriStateReload( );

						if (K)
						{
							switch (W->GetState( ))
							{
								case CWeapon::eIdle:		M_torso = TW->moving[moving_idx];		break;

								case CWeapon::eFire:
									if (is_standing)
										M_torso = M_legs = M_head = TW->all_attack_0;
									else
										M_torso = TW->attack_zoom;
									break;

								case CWeapon::eFire2:
									if (is_standing)
										M_torso = M_legs = M_head = TW->all_attack_1;
									else
										M_torso = TW->fire_idle;
									break;

								case CWeapon::eReload:		M_torso = TW->reload;					break;
								case CWeapon::eShowing:		M_torso = TW->draw;						break;
								case CWeapon::eHiding:		M_torso = TW->holster;					break;
								default:  	M_torso = TW->moving[moving_idx];		break;
							}
						}
						else
						{
							switch (W->GetState( ))
							{
								case CWeapon::eIdle:		M_torso = W->IsZoomed( ) ? TW->zoom : TW->moving[moving_idx];	break;
								case CWeapon::eFire:		M_torso = W->IsZoomed( ) ? TW->attack_zoom : TW->attack;				break;
								case CWeapon::eFire2:		M_torso = W->IsZoomed( ) ? TW->attack_zoom : TW->attack;				break;
								case CWeapon::eReload:
									if (!R3)
										M_torso = TW->reload;
									else
									{
										CWeapon::EWeaponSubStates sub_st = W->GetReloadState( );
										switch (sub_st)
										{
											case CWeapon::eSubstateReloadBegin:			M_torso = TW->reload;	break;
											case CWeapon::eSubstateReloadInProcess:		M_torso = TW->reload_1; break;
											case CWeapon::eSubstateReloadEnd:			M_torso = TW->reload_2; break;
											default:									M_torso = TW->reload;	break;
										}
									}break;

								case CWeapon::eShowing:	M_torso = TW->draw;					break;
								case CWeapon::eHiding:	M_torso = TW->holster;				break;
								default:  M_torso = TW->moving[moving_idx];	break;
							}
						}
					}
					else if (M)
					{
						if (is_standing)
						{
							switch (M->GetState( ))
							{
								case MS_SHOWING:		M_torso = TW->draw;			break;
								case MS_HIDING:		M_torso = TW->holster;		break;
								case MS_IDLE:		M_torso = TW->moving[moving_idx];		break;
								case MS_EMPTY:		M_torso = TW->zoom;		break;
								case MS_THREATEN:		M_torso = M_legs = M_head = TW->all_attack_0;	break;
								case MS_READY:		M_torso = M_legs = M_head = TW->all_attack_1;	break;
								case MS_THROW:		M_torso = M_legs = M_head = TW->all_attack_2;	break;
								case MS_END:		M_torso = M_legs = M_head = TW->all_attack_2;	break;
								default:		M_torso = TW->draw;			break;
							}
						}
						else
						{
							switch (M->GetState( ))
							{
								case MS_SHOWING:		M_torso = TW->draw;						break;
								case MS_HIDING:		M_torso = TW->holster;					break;
								case MS_IDLE:		M_torso = TW->moving[moving_idx];		break;
								case MS_EMPTY:		M_torso = TW->moving[moving_idx];		break;
								case MS_THREATEN:		M_torso = TW->attack_zoom;				break;
								case MS_READY:		M_torso = TW->fire_idle;				break;
								case MS_THROW:		M_torso = TW->fire_end;					break;
								case MS_END:		M_torso = TW->fire_end;					break;
								default:		M_torso = TW->draw;						break;
							}
						}
					}
					else if (A)
					{
						switch (A->GetState( ))
						{
							case CArtefact::eIdle: M_torso = TW->moving[moving_idx];	break;
							case CArtefact::eShowing: M_torso = TW->draw;					break;
							case CArtefact::eHiding: M_torso = TW->holster;				break;
							case CArtefact::eActivating: M_torso = TW->zoom;					break;
							default: M_torso = TW->moving[moving_idx];
						}

					}
				}
			}
		}
	}

	if (!M_legs)
	{
		if ((mstate_rl & mcCrouch) && !isActorAccelerated(mstate_rl, IsZoomAimingMode( )))//!(mstate_rl&mcAccel))
		{
			M_legs = smart_cast<CKinematicsAnimated*>(Visual( ))->ID_Cycle("cr_idle_1");
		}
		else
			M_legs = ST->legs_idle;
	}
	if (!M_head)					M_head = ST->m_head_idle;
	if (!M_torso)
	{
		if (m_bAnimTorsoPlayed)		M_torso = m_current_torso;
		else						M_torso = ST->m_torso_idle;
	}

	// ���� �������� ��� ����� - �������� / ����� �������� �������� �� ������
	if (m_current_torso != M_torso)
	{
		if (m_bAnimTorsoPlayed)		m_current_torso_blend = smart_cast<CKinematicsAnimated*>	(Visual( ))->PlayCycle(M_torso, TRUE, AnimTorsoPlayCallBack, this);
		else						/**/m_current_torso_blend = /**/smart_cast<CKinematicsAnimated*>	(Visual( ))->PlayCycle(M_torso);

		m_current_torso = M_torso;
	}
	if (m_current_head != M_head)
	{
		if (M_head)smart_cast<CKinematicsAnimated*>(Visual( ))->PlayCycle(M_head);
		m_current_head = M_head;
	}
	if (m_current_legs != M_legs)
	{
		f32 pos = 0.f;
		VERIFY(!m_current_legs_blend || !fis_zero(m_current_legs_blend->timeTotal));
		if ((mstate_real & mcAnyMove) && (mstate_old & mcAnyMove) && m_current_legs_blend)
			pos = fmod(m_current_legs_blend->timeCurrent, m_current_legs_blend->timeTotal) / m_current_legs_blend->timeTotal;
		m_current_legs_blend = smart_cast<CKinematicsAnimated*>(Visual( ))->PlayCycle(M_legs, TRUE, legs_play_callback, this);
		if ((!(mstate_old & mcAnyMove)) && (mstate_real & mcAnyMove))
			pos = 0.5f * Random.randI(2);
		if (m_current_legs_blend)
			m_current_legs_blend->timeCurrent = m_current_legs_blend->timeTotal * pos;
		m_current_legs = M_legs;

		CStepManager::on_animation_start(M_legs, m_current_legs_blend);
	}

#ifdef _DEBUG
	if (bDebug)
	{
		HUD( ).Font( ).pFontSmall->OutSetI(0, 0);
		HUD( ).Font( ).pFontSmall->OutNext("[%s]", mov_state[moving_idx]);
	}
#endif

#ifdef _DEBUG
	if ((Level( ).CurrentControlEntity( ) == this) && g_ShowAnimationInfo)
	{
		string128 buf;
		strcpy(buf, "");
		if (isActorAccelerated(mstate_rl, IsZoomAimingMode( )))		strcat(buf, "Accel ");
		if (mstate_rl & mcCrouch)		strcat(buf, "Crouch ");
		if (mstate_rl & mcFwd)		strcat(buf, "Fwd ");
		if (mstate_rl & mcBack)		strcat(buf, "Back ");
		if (mstate_rl & mcLStrafe)	strcat(buf, "LStrafe ");
		if (mstate_rl & mcRStrafe)	strcat(buf, "RStrafe ");
		if (mstate_rl & mcJump)		strcat(buf, "Jump ");
		if (mstate_rl & mcFall)		strcat(buf, "Fall ");
		if (mstate_rl & mcTurn)		strcat(buf, "Turn ");
		if (mstate_rl & mcLanding)	strcat(buf, "Landing ");
		if (mstate_rl & mcLLookout)	strcat(buf, "LLookout ");
		if (mstate_rl & mcRLookout)	strcat(buf, "RLookout ");
		if (m_bJumpKeyPressed)		strcat(buf, "+Jumping ");
		HUD( ).Font( ).pFontSmall->OutNext("MSTATE:     [%s]", buf);
/*
		switch (m_PhysicMovementControl->Environment())
		{
		case CPHMovementControl::peOnGround:	strcpy(buf,"ground");			break;
		case CPHMovementControl::peInAir:		strcpy(buf,"air");				break;
		case CPHMovementControl::peAtWall:		strcpy(buf,"wall");				break;
		}
		HUD().Font().pFontSmall->OutNext	(buf);
		HUD().Font().pFontSmall->OutNext	("Accel     [%3.2f, %3.2f, %3.2f]",VPUSH(NET_SavedAccel));
		HUD().Font().pFontSmall->OutNext	("V         [%3.2f, %3.2f, %3.2f]",VPUSH(m_PhysicMovementControl->GetVelocity()));
		HUD().Font().pFontSmall->OutNext	("vertex ID   %d",ai_location().level_vertex_id());

		Game().m_WeaponUsageStatistic->Draw();
		*/
	};
#endif

	if (!m_current_torso_blend)
		return;

	CKinematicsAnimated* skeleton_animated = PKinematicsAnimated(Visual( ));

	CMotionDef* motion0 = skeleton_animated->LL_GetMotionDef(m_current_torso);
	VERIFY(motion0);
	if (!(motion0->flags & esmSyncPart))
		return;

	if (!m_current_legs_blend)
		return;

	CMotionDef* motion1 = skeleton_animated->LL_GetMotionDef(m_current_legs);
	VERIFY(motion1);
	if (!(motion1->flags & esmSyncPart))
		return;

	m_current_torso_blend->timeCurrent = m_current_legs_blend->timeCurrent / m_current_legs_blend->timeTotal * m_current_torso_blend->timeTotal;
}
