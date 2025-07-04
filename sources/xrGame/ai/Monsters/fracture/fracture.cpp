#include "stdafx.h"
#include "fracture.h"
#include "fracture_state_manager.h"
#include "../monster_velocity_space.h"
#include "../control_animation_base.h"
#include "../control_movement_base.h"

CFracture::CFracture()
{
	StateMan = xr_new<CStateManagerFracture>(this);
}

CFracture::~CFracture()
{
	xr_delete(StateMan);
}

void CFracture::Load(pcstr section)
{
	inherited::Load	(section);

	anim().AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	anim().AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	anim().accel_load			(section);
	anim().accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	anim().accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	SVelocityParam &velocity_none		= move().get_velocity(MonsterMovement::eMP_IDLE);
	SVelocityParam &velocity_turn		= move().get_velocity(MonsterMovement::eMP_STAND);
	SVelocityParam &velocity_walk		= move().get_velocity(MonsterMovement::eVelocityParameterWalkNormal);
	SVelocityParam &velocity_run		= move().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
	SVelocityParam &velocity_walk_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterWalkDamaged);
	SVelocityParam &velocity_run_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterRunDamaged);
	SVelocityParam &velocity_steal		= move().get_velocity(MonsterMovement::eVelocityParameterSteal);
	//SVelocityParam &velocity_drag		= move().get_velocity(MonsterMovement::eVelocityParameterDrag);


	anim().AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &velocity_none,			PS_STAND);
	anim().AddAnim(eAnimStandDamaged,	"stand_idle_dmg_",		-1, &velocity_none,			PS_STAND);
	anim().AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &velocity_turn,			PS_STAND);
	anim().AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &velocity_turn,			PS_STAND);
	anim().AddAnim(eAnimWalkFwd,		"stand_walk_fwd_",		-1, &velocity_walk,			PS_STAND);
	anim().AddAnim(eAnimWalkDamaged,	"stand_walk_fwd_dmg_",	-1, &velocity_walk_dmg,		PS_STAND);
	anim().AddAnim(eAnimRun,			"stand_run_",			-1,	&velocity_run,			PS_STAND);
	anim().AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&velocity_run_dmg,		PS_STAND);
	anim().AddAnim(eAnimAttack,			"stand_attack_",		-1, &velocity_turn,			PS_STAND);
	anim().AddAnim(eAnimDie,			"stand_die_",			0,  &velocity_none,			PS_STAND);
	anim().AddAnim(eAnimCheckCorpse,	"stand_check_corpse_",	-1,	&velocity_none,			PS_STAND);
	anim().AddAnim(eAnimSteal,			"stand_crawl_",			-1, &velocity_steal,		PS_STAND);
	anim().AddAnim(eAnimSitIdle,		"sit_idle_",			-1, &velocity_none,			PS_SIT);
	anim().AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &velocity_none,			PS_SIT);
	anim().AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &velocity_none,			PS_STAND);
	anim().AddAnim(eAnimLookAround,		"stand_look_around_",	-1, &velocity_none,			PS_STAND);
	anim().AddAnim(eAnimEat,			"sit_eat_",				-1, &velocity_none,			PS_SIT);

	anim().AddTransition(PS_STAND,		PS_SIT,		eAnimStandSitDown,	false);
	anim().AddTransition(PS_SIT,		PS_STAND,	eAnimSitStandUp,	false);

	anim().LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	anim().LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
	anim().LinkAction(ACT_LIE_IDLE,		eAnimSitIdle);
	anim().LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	anim().LinkAction(ACT_WALK_BKWD,	eAnimWalkFwd);
	anim().LinkAction(ACT_RUN,			eAnimRun);
	anim().LinkAction(ACT_EAT,			eAnimEat);
	anim().LinkAction(ACT_SLEEP,		eAnimSitIdle);
	anim().LinkAction(ACT_REST,			eAnimSitIdle);
	anim().LinkAction(ACT_DRAG,			eAnimStandIdle);
	anim().LinkAction(ACT_ATTACK,		eAnimAttack);
	anim().LinkAction(ACT_STEAL,		eAnimSteal);
	anim().LinkAction(ACT_LOOK_AROUND,	eAnimLookAround);

#ifdef DEBUG	
	anim().accel_chain_test		();
#endif

}

void CFracture::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		com_man().seq_run(anim().get_motion_id(eAnimCheckCorpse));
	}

	if ((spec_params & ASP_STAND_SCARED) == ASP_STAND_SCARED) {
		anim().SetCurAnim(eAnimLookAround);
		return;
	}
}
