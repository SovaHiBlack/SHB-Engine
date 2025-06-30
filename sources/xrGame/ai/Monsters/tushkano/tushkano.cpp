#include "stdafx.h"
#include "Tushkano.h"
#include "StateManagerTushkano.h"
#include "../monster_velocity_space.h"
#include "../control_animation_base.h"
#include "../control_movement_base.h"

CTushkano::CTushkano( )
{
	StateMan = xr_new<CStateManagerTushkano>(this);

	CControlled::init_external(this);
}

CTushkano::~CTushkano( )
{
	xr_delete(StateMan);
}

void CTushkano::Load(pcstr section)
{
	inherited::Load(section);

	anim( ).accel_load(section);
	anim( ).accel_chain_add(eAnimWalkFwd, eAnimRun);

	SVelocityParam& velocity_none = move( ).get_velocity(MonsterMovement::eMP_IDLE);
	SVelocityParam& velocity_turn = move( ).get_velocity(MonsterMovement::eMP_STAND);
	SVelocityParam& velocity_walk = move( ).get_velocity(MonsterMovement::eVelocityParameterWalkNormal);
	SVelocityParam& velocity_run = move( ).get_velocity(MonsterMovement::eVelocityParameterRunNormal);

	anim( ).AddAnim(eAnimStandIdle, "stand_idle_", -1, &velocity_none, PS_STAND);
	anim( ).AddAnim(eAnimStandTurnLeft, "stand_turn_left_", -1, &velocity_turn, PS_STAND);
	anim( ).AddAnim(eAnimStandTurnRight, "stand_turn_right_", -1, &velocity_turn, PS_STAND);
	anim( ).AddAnim(eAnimWalkFwd, "stand_walk_fwd_", -1, &velocity_walk, PS_STAND);
	anim( ).AddAnim(eAnimRun, "stand_run_", -1, &velocity_run, PS_STAND);
	anim( ).AddAnim(eAnimAttack, "stand_attack_", -1, &velocity_turn, PS_STAND);

	anim( ).LinkAction(ACT_STAND_IDLE, eAnimStandIdle);
	anim( ).LinkAction(ACT_SIT_IDLE, eAnimStandIdle);
	anim( ).LinkAction(ACT_LIE_IDLE, eAnimStandIdle);
	anim( ).LinkAction(ACT_WALK_FWD, eAnimWalkFwd);
	anim( ).LinkAction(ACT_WALK_BKWD, eAnimWalkFwd);
	anim( ).LinkAction(ACT_RUN, eAnimRun);
	anim( ).LinkAction(ACT_EAT, eAnimStandIdle);
	anim( ).LinkAction(ACT_SLEEP, eAnimStandIdle);
	anim( ).LinkAction(ACT_REST, eAnimStandIdle);
	anim( ).LinkAction(ACT_DRAG, eAnimStandIdle);
	anim( ).LinkAction(ACT_ATTACK, eAnimAttack);
	anim( ).LinkAction(ACT_STEAL, eAnimStandIdle);
	anim( ).LinkAction(ACT_LOOK_AROUND, eAnimStandIdle);

#ifdef DEBUG
	anim( ).accel_chain_test( );
#endif

}

void CTushkano::CheckSpecParams(u32 spec_params)
{
	//if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
	//	anim().Seq_Add(eAnimCheckCorpse);
	//	anim().Seq_Switch();
	//}

	//if ((spec_params & ASP_STAND_SCARED) == ASP_STAND_SCARED) {
	//	anim().SetCurAnim(eAnimLookAround);
	//	return;
	//}
}

using namespace luabind;

#pragma optimize("s",on)
void CTushkano::script_register(lua_State* L)
{
	module(L)
		[
			class_<CTushkano, CGameObject>("CTushkano")
				.def(constructor<>( ))
		];
}
