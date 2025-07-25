#include "stdafx.h"
#include "bloodsucker.h"
#include "bloodsucker_state_manager.h"
#include "../../../..\XR_3DA\skeletoncustom.h"
#include "../../../Actor.h"
#include "../../../ActorEffector.h"
#include "../../../..\XR_3DA\skeletonanimated.h"
#include "../../../level.h"
#include "../../../LevelDebug.h"
#include "../../../material_manager.h"
#include "bloodsucker_vampire_effector.h"
#include "../../../detail_path_manager.h"
#include "../monster_velocity_space.h"
#include "../../../gamepersistent.h"
#include "../../../GameObject_space.h"

#include "../control_animation_base.h"
#include "../control_movement_base.h"
#include "../control_rotation_jump.h"

#include "../../../sound_player.h"
#include "../../../..\XR_3DA\camerabase.h"
#include "../../../xr_level_controller.h"
#include "../../../ActorCondition.h"

#include "../../../PHDestroyable.h"
#include "../../../CharacterPhysicsSupport.h"

#ifdef DEBUG
#include <dinput.h>
#endif



CAI_Bloodsucker::CAI_Bloodsucker()
{
	StateMan						= xr_new<CStateManagerBloodsucker>	(this);
	m_alien_control.init_external	(this);
	
	com_man().add_ability			(ControlCom::eControlRunAttack);
	com_man().add_ability			(ControlCom::eControlRotationJump);

	invisible_vel.set				(0.1f, 0.1f);

	EnemyMemory.init_external		(this, 40000);
}

CAI_Bloodsucker::~CAI_Bloodsucker()
{
	xr_delete	(StateMan);
}

void CAI_Bloodsucker::Load(pcstr section)
{
	inherited::Load(section);

	anim().AddReplacedAnim(&m_bDamaged,			eAnimRun,		eAnimRunDamaged);
	anim().AddReplacedAnim(&m_bDamaged,			eAnimWalkFwd,	eAnimWalkDamaged);
	anim().AddReplacedAnim(&m_bDamaged,			eAnimStandIdle,	eAnimStandDamaged);
	anim().AddReplacedAnim(&m_bRunTurnLeft,		eAnimRun,		eAnimRunTurnLeft);
	anim().AddReplacedAnim(&m_bRunTurnRight,	eAnimRun,		eAnimRunTurnRight);


	anim().accel_load			(section);
	anim().accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	anim().accel_chain_add		(eAnimWalkFwd,		eAnimRunTurnLeft);
	anim().accel_chain_add		(eAnimWalkFwd,		eAnimRunTurnRight);
	anim().accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);


	SVelocityParam &velocity_none		= move().get_velocity(MonsterMovement::eMP_IDLE);
	SVelocityParam &velocity_turn		= move().get_velocity(MonsterMovement::eMP_STAND);
	SVelocityParam &velocity_walk		= move().get_velocity(MonsterMovement::eVelocityParameterWalkNormal);
	SVelocityParam &velocity_run		= move().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
	SVelocityParam &velocity_walk_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterWalkDamaged);
	SVelocityParam &velocity_run_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterRunDamaged);
	SVelocityParam &velocity_steal		= move().get_velocity(MonsterMovement::eVelocityParameterSteal);
	
	anim().AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &velocity_none,		PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimStandDamaged,	"stand_damaged_",		-1, &velocity_none,		PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &velocity_turn,		PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &velocity_turn,		PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimSleep,			"lie_sleep_",			-1, &velocity_none,		PS_LIE,	  	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimWalkFwd,		"stand_walk_fwd_",		-1, &velocity_walk,		PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimWalkDamaged,	"stand_walk_fwd_dmg_",	-1, &velocity_walk_dmg,	PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimRun,			"stand_run_",			-1,	&velocity_run,		PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&velocity_run_dmg,	PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	
	anim().AddAnim(eAnimRunTurnLeft,	"stand_run_turn_left_",	-1, &velocity_run,		PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimRunTurnRight,	"stand_run_turn_right_",-1, &velocity_run,		PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimScared,			"stand_scared_",		-1, &velocity_none,		PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");	

	anim().AddAnim(eAnimCheckCorpse,	"stand_check_corpse_",	-1,	&velocity_none,		PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimEat,			"sit_eat_",				-1, &velocity_none,		PS_SIT,		"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	
	anim().AddAnim(eAnimDie,			"stand_idle_",			-1, &velocity_none,		PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	
	anim().AddAnim(eAnimAttack,			"stand_attack_",		-1, &velocity_turn,		PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimAttackRun,		"stand_attack_run_",	-1, &velocity_run,		PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

	anim().AddAnim(eAnimLookAround,		"stand_look_around_",	-1, &velocity_none,		PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimSitIdle,		"sit_idle_",			-1, &velocity_none,		PS_SIT,		"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &velocity_none,		PS_SIT,		"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimSitToSleep,		"sit_sleep_down_",		-1, &velocity_none,		PS_SIT,		"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &velocity_none,		PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	
	anim().AddAnim(eAnimSteal,			"stand_steal_",			-1, &velocity_steal,	PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	
	anim().AddAnim(eAnimThreaten,		"stand_threaten_",		-1, &velocity_none,		PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimMiscAction_00,	"stand_to_aggressive_",	-1, &velocity_none,		PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");	

	// define transitions
	//	anim().AddTransition(PS_STAND,			eAnimThreaten,	eAnimMiscAction_00,	false);
	anim().AddTransition(eAnimStandSitDown,	eAnimSleep,		eAnimSitToSleep,	false);
	anim().AddTransition(PS_STAND,			eAnimSleep,		eAnimStandSitDown,	true);
	anim().AddTransition(PS_STAND,			PS_SIT,			eAnimStandSitDown,	false);
	anim().AddTransition(PS_STAND,			PS_LIE,			eAnimStandSitDown,	false);
	anim().AddTransition(PS_SIT,			PS_STAND,		eAnimSitStandUp,	false);
	anim().AddTransition(PS_LIE,			PS_STAND,		eAnimSitStandUp,	false);

	// define links from Action to animations
	anim().LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	anim().LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
	anim().LinkAction(ACT_LIE_IDLE,		eAnimSitIdle);
	anim().LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	anim().LinkAction(ACT_WALK_BKWD,	eAnimWalkBkwd);
	anim().LinkAction(ACT_RUN,			eAnimRun);
	anim().LinkAction(ACT_EAT,			eAnimEat);
	anim().LinkAction(ACT_SLEEP,		eAnimSleep);
	anim().LinkAction(ACT_REST,			eAnimSitIdle);
	anim().LinkAction(ACT_DRAG,			eAnimWalkBkwd);
	anim().LinkAction(ACT_ATTACK,		eAnimAttack);
	anim().LinkAction(ACT_STEAL,		eAnimSteal);
	anim().LinkAction(ACT_LOOK_AROUND,	eAnimLookAround); 

	#ifdef DEBUG	
		anim().accel_chain_test		();
	#endif
	
	// load other misc stuff
	invisible_vel.set				(pSettings->r_float(section,"Velocity_Invisible_Linear"),pSettings->r_float(section,"Velocity_Invisible_Angular"));
	movement().detail().add_velocity(MonsterMovement::eVelocityParameterInvisible,CDetailPathManager::STravelParams(invisible_vel.linear, invisible_vel.angular));

	LoadVampirePPEffector			(pSettings->r_string(section,"vampire_effector"));
	m_vampire_min_delay				= pSettings->r_u32(section,"Vampire_Delay");

	m_visual_predator				= pSettings->r_string(section,"Predator_Visual");

	m_vampire_want_speed			= pSettings->r_float(section,"Vampire_Want_Speed");
	m_vampire_wound					= pSettings->r_float(section,"Vampire_Wound");


	invisible_particle_name			= pSettings->r_string(section,"Particle_Invisible");
}


void CAI_Bloodsucker::reinit()
{
	inherited::reinit			();
	CControlledActor::reinit	();

	if(CCustomMonster::use_simplified_visual())	return;

	Bones.Reset					();

	com_man().ta_fill_data(anim_triple_vampire, "vampire_0", "vampire_1", "vampire_2", TA_EXECUTE_LOOPED, TA_DONT_SKIP_PREPARE, ControlCom::eCapturePath | ControlCom::eCaptureMovement);
	
	m_alien_control.reinit();
	
	state_invisible				= false;

	//com_man().add_rotation_jump_data("run_turn_r_0","run_turn_r_1","run_turn_r_0","run_turn_r_1", PI - 0.01f, SControlRotationJumpData::eStopAtOnce | SControlRotationJumpData::eRotateOnce);
	com_man().add_rotation_jump_data("run_turn_l_0","run_turn_l_1","run_turn_r_0","run_turn_r_1", PI_DIV_2);

	// save visual	
	m_visual_default			= cNameVisual();

	m_vampire_want_value		= 0.0f;
	m_predator					= false;
}

void CAI_Bloodsucker::reload(pcstr section)
{
	inherited::reload(section);

	sound().add(pSettings->r_string(section,"Sound_Vampire_Grasp"),				DEFAULT_SAMPLE_COUNT, eST_MONSTER_ATTACKING, MonsterSound::eHighPriority + 4,	MonsterSound::eBaseChannel,	eVampireGrasp,					"bip01_head");
	sound().add(pSettings->r_string(section,"Sound_Vampire_Sucking"),			DEFAULT_SAMPLE_COUNT, eST_MONSTER_ATTACKING, MonsterSound::eHighPriority + 3,	MonsterSound::eBaseChannel,	eVampireSucking,				"bip01_head");
	sound().add(pSettings->r_string(section,"Sound_Vampire_Hit"),				DEFAULT_SAMPLE_COUNT, eST_MONSTER_ATTACKING, MonsterSound::eHighPriority + 2,	MonsterSound::eBaseChannel,	eVampireHit,					"bip01_head");
	sound().add(pSettings->r_string(section,"Sound_Vampire_StartHunt"),			DEFAULT_SAMPLE_COUNT, eST_MONSTER_ATTACKING, MonsterSound::eHighPriority + 5,	MonsterSound::eBaseChannel,	eVampireStartHunt,				"bip01_head");
	sound().add(pSettings->r_string(section,"Sound_Invisibility_Change_State"),	DEFAULT_SAMPLE_COUNT, eST_MONSTER_ATTACKING, MonsterSound::eNormalPriority,	MonsterSound::eChannelIndependent << 1,	eChangeVisibility,	"bip01_head");
	sound().add(pSettings->r_string(section,"Sound_Growl"),						DEFAULT_SAMPLE_COUNT, eST_MONSTER_ATTACKING, MonsterSound::eHighPriority + 6,	MonsterSound::eBaseChannel,	eGrowl,							"bip01_head");
	sound().add(pSettings->r_string(section,"Sound_Alien"),						DEFAULT_SAMPLE_COUNT, eST_MONSTER_ATTACKING, MonsterSound::eCriticalPriority,	u32(MonsterSound::eCaptureAllChannels),	eAlien,				"bip01_head");
}

void CAI_Bloodsucker::LoadVampirePPEffector(pcstr section)
{
	pp_vampire_effector.duality.h			= pSettings->r_float(section,"duality_h");
	pp_vampire_effector.duality.v			= pSettings->r_float(section,"duality_v");
	pp_vampire_effector.gray				= pSettings->r_float(section,"gray");
	pp_vampire_effector.blur				= pSettings->r_float(section,"blur");
	pp_vampire_effector.noise.intensity		= pSettings->r_float(section,"noise_intensity");
	pp_vampire_effector.noise.grain			= pSettings->r_float(section,"noise_grain");
	pp_vampire_effector.noise.fps			= pSettings->r_float(section,"noise_fps");
	VERIFY(!fis_zero(pp_vampire_effector.noise.fps));

	sscanf(pSettings->r_string(section,"color_base"),	"%f,%f,%f", &pp_vampire_effector.color_base.r, &pp_vampire_effector.color_base.g, &pp_vampire_effector.color_base.b);
	sscanf(pSettings->r_string(section,"color_gray"),	"%f,%f,%f", &pp_vampire_effector.color_gray.r, &pp_vampire_effector.color_gray.g, &pp_vampire_effector.color_gray.b);
	sscanf(pSettings->r_string(section,"color_add"),	"%f,%f,%f", &pp_vampire_effector.color_add.r,  &pp_vampire_effector.color_add.g,  &pp_vampire_effector.color_add.b);
}


void  CAI_Bloodsucker::BoneCallback(CBoneInstance *B)
{
	CAI_Bloodsucker*	this_class = static_cast<CAI_Bloodsucker*> (B->Callback_Param);

	this_class->Bones.Update(B, Device.dwTimeGlobal);
}


void CAI_Bloodsucker::vfAssignBones()
{
	// ��������� callback �� �����

	bone_spine =	&smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(smart_cast<CKinematics*>(Visual())->LL_BoneID("bip01_spine"));
	bone_head =		&smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(smart_cast<CKinematics*>(Visual())->LL_BoneID("bip01_head"));
	if(!PPhysicsShell())//������ ������� �������, ���� ������ ��� ��� - � ���� ����� ���� �������!!!
	{
		bone_spine->set_callback(bctCustom,BoneCallback,this);
		bone_head->set_callback(bctCustom,BoneCallback,this);
	}

	// Bones settings
	Bones.Reset();
	Bones.AddBone(bone_spine, AXIS_X);	Bones.AddBone(bone_spine, AXIS_Y);
	Bones.AddBone(bone_head, AXIS_X);	Bones.AddBone(bone_head, AXIS_Y);
}


//#define MAX_BONE_ANGLE PI_DIV_4

void CAI_Bloodsucker::LookDirection(fVector3 to_dir, f32 bone_turn_speed)
{
	//// �������� ������ ����������� � ��������� ����� � ��� ������� ����
	//f32		yaw,pitch;
	//to_dir.getHP(yaw,pitch);

	//// ���������� ��������� �������� �� yaw
	//f32 cur_yaw = -movement().m_body.current.yaw;						// ������� ������� ���� �������
	//f32 bone_angle;											// ���� ��� ����	

	//f32 dy = _abs(angle_normalize_signed(yaw - cur_yaw));		// ������, �� ������� ����� ��������������

	//if (angle_difference(cur_yaw,yaw) <= MAX_BONE_ANGLE) {		// bone turn only
	//	bone_angle = dy;
	//} else {													// torso & bone turn 
	//	if (movement().IsMoveAlongPathFinished() || !movement().enabled()) movement().m_body.target.yaw = angle_normalize(-yaw);
	//	if (dy / 2 < MAX_BONE_ANGLE) bone_angle = dy / 2;
	//	else bone_angle = MAX_BONE_ANGLE;
	//}

	//bone_angle /= 2;
	//if (from_right(yaw,cur_yaw)) bone_angle *= -1.f;

	//Bones.SetMotion(bone_spine, AXIS_X, bone_angle, bone_turn_speed, 100);
	//Bones.SetMotion(bone_head,	AXIS_X, bone_angle, bone_turn_speed, 100);

	//// ���������� ��������� �������� �� pitch
	//clamp(pitch, -MAX_BONE_ANGLE, MAX_BONE_ANGLE);
	//pitch /= 2; 

	//Bones.SetMotion(bone_spine, AXIS_Y, pitch, bone_turn_speed, 100);
	//Bones.SetMotion(bone_head,	AXIS_Y, pitch, bone_turn_speed, 100);	
}

void CAI_Bloodsucker::ActivateVampireEffector()
{
	Actor()->Cameras().AddCamEffector(xr_new<CVampireCameraEffector>(6.0f, get_head_position(this), get_head_position(Actor())));
	Actor()->Cameras().AddPPEffector(xr_new<CVampirePPEffector>(pp_vampire_effector, 6.0f));
}


void CAI_Bloodsucker::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		com_man().seq_run(anim().get_motion_id(eAnimCheckCorpse));
	}

	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) {
		anim().SetCurAnim(eAnimThreaten);
		return;
	}

	if ((spec_params & ASP_STAND_SCARED) == ASP_STAND_SCARED) {
		anim().SetCurAnim(eAnimLookAround);
		return;
	}

}

BOOL CAI_Bloodsucker::net_Spawn (CSE_Abstract* DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	vfAssignBones	();

	return(TRUE);
}

void CAI_Bloodsucker::UpdateCL()
{
	inherited::UpdateCL				();
	CControlledActor::frame_update	();
	
	// update vampire need
	m_vampire_want_value += m_vampire_want_speed * client_update_fdelta();
	clamp(m_vampire_want_value,0.f,1.f);
}


void CAI_Bloodsucker::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	
	if (!g_Alive())	setVisible(TRUE);

	if (m_alien_control.active())	sound().play(eAlien);
}

void CAI_Bloodsucker::Die(CObject* who)
{
	predator_stop				();
	inherited::Die				(who);
}

void CAI_Bloodsucker::post_fsm_update()
{
	inherited::post_fsm_update();
	
	//EMonsterState state = StateMan->get_state_type();
	//
	// ���������� �������������
	//bool aggressive =	(is_state(state, eStateAttack)) || 
	//					(is_state(state, eStatePanic))	|| 
	//					(is_state(state, eStateHitted));

}

bool CAI_Bloodsucker::check_start_conditions(ControlCom::EControlType type)
{
	if (!inherited::check_start_conditions(type))	return false;

	if (type == ControlCom::eControlRunAttack)
		return (!state_invisible);

	return true;
}

void CAI_Bloodsucker::set_alien_control(bool val)
{
	val ? m_alien_control.activate() : m_alien_control.deactivate();
}

void CAI_Bloodsucker::predator_start()
{
	if (m_predator)					return;
	cNameVisual_set					(m_visual_predator);
	CDamageManager::reload(*cNameSect(),"damage",pSettings);

	control().animation().restart	();
	
	CParticlesPlayer::StartParticles(invisible_particle_name, fVector3().set(0.0f,0.1f,0.0f),ID());
	sound().play					(CAI_Bloodsucker::eChangeVisibility);

	m_predator						= true;
	state_invisible					= false;
}

void CAI_Bloodsucker::predator_stop()
{
	if (!m_predator)				return;
	
	cNameVisual_set					(*m_visual_default);
	character_physics_support()->in_ChangeVisual();

	CDamageManager::reload(*cNameSect(),"damage",pSettings);

	control().animation().restart	();
	
	CParticlesPlayer::StartParticles(invisible_particle_name, fVector3().set(0.0f,0.1f,0.0f),ID());
	sound().play					(CAI_Bloodsucker::eChangeVisibility);
	m_predator						= false;
}

void CAI_Bloodsucker::predator_freeze()
{
	control().animation().freeze	();
}

void CAI_Bloodsucker::predator_unfreeze()
{
	control().animation().unfreeze();
}

void CAI_Bloodsucker::move_actor_cam()
{
	f32 turn_angle = PI_DIV_3;
	if (Actor()->cam_Active()) {
		Actor()->cam_Active()->Move(Random.randI(2) ? kRIGHT : kLEFT, turn_angle);	//Random.randF(turn_angle)); 
		Actor()->cam_Active()->Move(Random.randI(2) ? kUP	 : kDOWN, turn_angle);	//Random.randF(turn_angle)); 
	}
}

void CAI_Bloodsucker::HitEntity(const CEntity *pEntity, f32 fDamage, f32 impulse, fVector3& dir)
{
	inherited::HitEntity(pEntity,fDamage,impulse,dir);

	EMonsterState state = StateMan->get_state_type();
	if (is_state(state, eStateVampire_Execute)) {
		VERIFY(Actor());
		
		move_actor_cam();
		
		u16 bone_id = smart_cast<CKinematics*>(Actor()->Visual())->LL_BoneID("bip01_head");
		Actor()->conditions().AddWound(m_vampire_wound, ALife::eHitTypeWound, bone_id);
	}
}

void CAI_Bloodsucker::start_invisible_predator()
{
	state_invisible				= true;
	predator_start				();
}
void CAI_Bloodsucker::stop_invisible_predator()
{
	state_invisible				= false;
	predator_stop				();
}

void CAI_Bloodsucker::manual_activate()
{
	state_invisible = true;
	setVisible		(FALSE);
}

void CAI_Bloodsucker::manual_deactivate()
{
	state_invisible = false;
	setVisible		(TRUE);
}



#ifdef DEBUG
CBaseMonster::SDebugInfo CAI_Bloodsucker::show_debug_info()
{
	CBaseMonster::SDebugInfo info = inherited::show_debug_info();
	if (!info.active) return CBaseMonster::SDebugInfo();

	string128 text;
	sprintf_s(text, "Vampire Want Value = [%f] Speed = [%f]", m_vampire_want_value, m_vampire_want_speed);
	DBG().text(this).add_item(text, info.x, info.y+=info.delta_y, info.color);
	DBG().text(this).add_item("---------------------------------------", info.x, info.y+=info.delta_y, info.delimiter_color);

	return CBaseMonster::SDebugInfo();
}

#ifdef _DEBUG
void CAI_Bloodsucker::debug_on_key(s32 key)
{
	switch (key){
	case DIK_MINUS:
		Actor()->cam_Active()->Move(Random.randI(2) ? kRIGHT : kLEFT, PI_DIV_2);
		//set_alien_control(true);
		break;
	case DIK_EQUALS:
		Actor()->cam_Active()->Move(Random.randI(2) ? kUP	 : kDOWN, PI_DIV_2);
		//set_alien_control(false);
		break;
	}
}
#endif


#endif

