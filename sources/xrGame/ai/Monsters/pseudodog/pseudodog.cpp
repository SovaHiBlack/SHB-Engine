#include "stdafx.h"
#include "PseudoDog.h"
#include "pseudodog_state_manager.h"
#include "../../../..\XR_3DA\skeletonanimated.h"
#include "../../../sound_player.h"
#include "../../../level.h"
#include "../../../ai_monster_space.h"
#include "../monster_velocity_space.h"
#include "../control_animation_base.h"
#include "../control_movement_base.h"

#ifdef _DEBUG
#	include <dinput.h>
#	include "../../../ai_object_location.h"
#	include "../../../LevelDebug.h"
#	include "../../../level_graph.h"
#	include "../../../AISpace.h"
#	include "../../../alife_simulator.h"
#	include "../../../xrServer_Object_Base.h"
#	include "../../../xrserver.h"
#endif

CPseudoDog::CPseudoDog()
{
	com_man().add_ability(ControlCom::eControlJump);
	com_man().add_ability(ControlCom::eControlRotationJump);
}

DLL_Pure* CPseudoDog::_construct( )
{
	inherited::_construct			();
	StateMan = create_state_manager	();
	return							(this);
}

CPseudoDog::~CPseudoDog()
{
	xr_delete(StateMan);
}

void CPseudoDog::reinit()
{
	inherited::reinit();

	m_time_became_angry				= 0;
	time_growling					= 0;

	if(CCustomMonster::use_simplified_visual())	return;
	com_man().add_rotation_jump_data	("1","2","3","4", deg(90));
}

void CPseudoDog::Load(pcstr section)
{
	inherited::Load	(section);

	anim().AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	anim().AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);
	anim().AddReplacedAnim(&m_bRunTurnLeft,		eAnimRun,		eAnimRunTurnLeft);
	anim().AddReplacedAnim(&m_bRunTurnRight,	eAnimRun,		eAnimRunTurnRight);

	anim().accel_load			(section);
	anim().accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	anim().accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	m_anger_hunger_threshold	= pSettings->r_float(section, "anger_hunger_threshold");
	m_anger_loud_threshold		= pSettings->r_float(section, "anger_loud_threshold");

	SVelocityParam &velocity_none		= move().get_velocity(MonsterMovement::eMP_IDLE);
	SVelocityParam &velocity_turn		= move().get_velocity(MonsterMovement::eMP_STAND);
	SVelocityParam &velocity_walk		= move().get_velocity(MonsterMovement::eVelocityParameterWalkNormal);
	SVelocityParam &velocity_run		= move().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
	SVelocityParam &velocity_walk_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterWalkDamaged);
	SVelocityParam &velocity_run_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterRunDamaged);
	SVelocityParam &velocity_steal		= move().get_velocity(MonsterMovement::eVelocityParameterSteal);
	SVelocityParam &velocity_drag		= move().get_velocity(MonsterMovement::eVelocityParameterDrag);

	// define animation set
	anim().AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimEat,			"stand_eat_",			-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimSleep,			"lie_sleep_",			-1, &velocity_none,		PS_LIE);
	anim().AddAnim(eAnimLieIdle,		"lie_idle_",			-1, &velocity_none,		PS_LIE);
	anim().AddAnim(eAnimSitIdle,		"sit_idle_",			-1, &velocity_none,		PS_SIT);
	anim().AddAnim(eAnimAttack,			"stand_attack_",		-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimWalkFwd,		"stand_walk_fwd_",		-1, &velocity_walk,		PS_STAND);
	anim().AddAnim(eAnimWalkDamaged,	"stand_walk_dmg_",		-1, &velocity_walk_dmg,	PS_STAND);
	anim().AddAnim(eAnimRun,			"stand_run_",			-1,	&velocity_run,		PS_STAND);
	anim().AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1, &velocity_run_dmg,	PS_STAND);
	anim().AddAnim(eAnimCheckCorpse,	"stand_check_corpse_",	-1,	&velocity_none,		PS_STAND);
	anim().AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &velocity_drag,		PS_STAND);
	anim().AddAnim(eAnimSniff,			"stand_sniff_",			-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimHowling,		"stand_howling_",		-1,	&velocity_none,		PS_STAND);
	anim().AddAnim(eAnimJumpGlide,		"jump_glide_",			-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimSteal,			"stand_steal_",			-1, &velocity_steal,	PS_STAND);
	anim().AddAnim(eAnimDie,			"stand_die_",			-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimSitLieDown,		"sit_lie_down_",		-1, &velocity_none,		PS_SIT);
	anim().AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &velocity_none,		PS_STAND);	
	anim().AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &velocity_none,		PS_SIT);
	anim().AddAnim(eAnimLieToSleep,		"lie_to_sleep_",		-1,	&velocity_none,		PS_LIE);
	anim().AddAnim(eAnimSleepStandUp,	"lie_to_stand_up_",		-1, &velocity_none,		PS_LIE);
	anim().AddAnim(eAnimAttackPsi,		"stand_psi_attack_",	-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimThreaten,		"stand_howling_",		-1,	&velocity_none,		PS_STAND);
	
	anim().AddAnim(eAnimRunTurnLeft,	"stand_run_turn_left_",	-1, &velocity_run,		PS_STAND);
	anim().AddAnim(eAnimRunTurnRight,	"stand_run_turn_right_",-1, &velocity_run,		PS_STAND);

	// define transitions
	// order : 1. [anim -> anim]	2. [anim->state]	3. [state -> anim]		4. [state -> state]
	anim().AddTransition(eAnimLieIdle,	eAnimSleep,	eAnimLieToSleep,	false);
	anim().AddTransition(eAnimSleep,	PS_STAND,	eAnimSleepStandUp,	false);
	anim().AddTransition(PS_SIT,		PS_LIE,		eAnimSitLieDown,	false);
	anim().AddTransition(PS_STAND,		PS_SIT,		eAnimStandSitDown,	false);
	anim().AddTransition(PS_SIT,		PS_STAND,	eAnimSitStandUp,	false);

	// define links from Action to animations
	anim().LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	anim().LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
	anim().LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	anim().LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	anim().LinkAction(ACT_WALK_BKWD,	eAnimWalkBkwd);
	anim().LinkAction(ACT_RUN,			eAnimRun);
	anim().LinkAction(ACT_EAT,			eAnimEat);
	anim().LinkAction(ACT_SLEEP,		eAnimSleep);
	anim().LinkAction(ACT_REST,			eAnimSitIdle);
	anim().LinkAction(ACT_DRAG,			eAnimDragCorpse);
	anim().LinkAction(ACT_ATTACK,		eAnimAttack);
	anim().LinkAction(ACT_STEAL,		eAnimWalkFwd);	
	anim().LinkAction(ACT_LOOK_AROUND,	eAnimSniff);

#ifdef DEBUG	
	anim().accel_chain_test		();
#endif

}

void CPseudoDog::reload(pcstr section)
{
	inherited::reload			(section);

	if(CCustomMonster::use_simplified_visual())	return;
	
	// load additional sounds
	sound().add					(pSettings->r_string(section,"sound_psy_attack"), DEFAULT_SAMPLE_COUNT, eST_MONSTER_ATTACKING,	MonsterSound::eHighPriority+3,	MonsterSound::eBaseChannel,	ePsyAttack, "bip01_head");
	
	// load jump params
	com_man().load_jump_data			(0,"run_jamp_0", "run_jamp_1", "run_jamp_2", MonsterMovement::eVelocityParameterRunNormal,MonsterMovement::eVelocityParameterRunNormal,0);
}

void CPseudoDog::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_PSI_ATTACK) == ASP_PSI_ATTACK) {
		com_man().seq_run(anim().get_motion_id(eAnimAttackPsi));
	}

	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) {
		anim().SetCurAnim(eAnimThreaten);
	}
}

void CPseudoDog::HitEntityInJump		(const CEntity *pEntity)
{
	SAAParam &params	= anim().AA_GetParams("run_jamp_1");
	HitEntity			(pEntity, params.hit_power, params.impulse, params.impulse_dir);
}

#ifdef _DEBUG
void CPseudoDog::debug_on_key(s32 key)
{
}
#endif

IStateManagerBase * CPseudoDog::create_state_manager()
{
	return xr_new<CStateManagerPseudodog>(this);
}
