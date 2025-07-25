#include "stdafx.h"
#include "flesh.h"
#include "../../../AISpace.h"
#include "flesh_state_manager.h"
#include "../monster_velocity_space.h"
#include "../control_animation_base.h"
#include "../control_movement_base.h"

CAI_Flesh::CAI_Flesh()
{
	StateMan = xr_new<CStateManagerFlesh>(this);
	
	m_fEyeShiftYaw		= PI_DIV_6;

	CControlled::init_external(this);
}

CAI_Flesh::~CAI_Flesh()
{
	xr_delete(StateMan);
}

BOOL CAI_Flesh::net_Spawn (CSE_Abstract* DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	return TRUE;
}

void CAI_Flesh::Load(pcstr section)
{
	inherited::Load(section);

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
	SVelocityParam &velocity_drag		= move().get_velocity(MonsterMovement::eVelocityParameterDrag);

	// define animation set
	anim().AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &velocity_turn,		PS_STAND);

	anim().AddAnim(eAnimLieIdle,		"lie_idle_",			-1, &velocity_none,		PS_LIE);
	anim().AddAnim(eAnimSleep,			"lie_idle_",			-1, &velocity_none,		PS_LIE);

	anim().AddAnim(eAnimWalkFwd,		"stand_walk_fwd_",		-1, &velocity_walk,		PS_STAND);
	anim().AddAnim(eAnimWalkDamaged,	"stand_walk_fwd_dmg_",	-1, &velocity_walk_dmg,	PS_STAND);

	anim().AddAnim(eAnimRun,			"stand_run_",			-1,	&velocity_run,		PS_STAND);
	anim().AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&velocity_run_dmg,	PS_STAND);

	anim().AddAnim(eAnimAttack,			"stand_attack_",		-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimAttackFromBack,	"stand_attack_back_",	-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimCheckCorpse,	"stand_eat_",			 1,	&velocity_none,		PS_STAND);

	anim().AddAnim(eAnimEat,			"stand_eat_",			-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimDie,			"stand_die_",			-1, &velocity_none,		PS_STAND);

	anim().AddAnim(eAnimStandLieDown,	"stand_lie_down_",		-1, &velocity_none,		PS_STAND);
	anim().AddAnim(eAnimLieStandUp,		"lie_stand_up_",		-1, &velocity_none,		PS_LIE);

	anim().AddAnim(eAnimSteal,			"stand_crawl_",			-1, &velocity_steal,	PS_STAND);
	anim().AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &velocity_drag,		PS_STAND);

	anim().AddAnim(eAnimScared,			"stand_scared_",		-1,	&velocity_none,		PS_STAND);
	anim().AddAnim(eAnimThreaten,		"stand_threaten_",		-1,	&velocity_none,		PS_STAND);

	// define transitions
	anim().AddTransition(PS_STAND,	PS_LIE,		eAnimStandLieDown,		false);
	anim().AddTransition(PS_LIE,		PS_STAND,	eAnimLieStandUp,		false, SKIP_IF_AGGRESSIVE);

	// define links from Action to animations
	anim().LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	anim().LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
	anim().LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	anim().LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	anim().LinkAction(ACT_WALK_BKWD,	eAnimWalkBkwd);
	anim().LinkAction(ACT_RUN,			eAnimRun);
	anim().LinkAction(ACT_EAT,			eAnimEat);
	anim().LinkAction(ACT_SLEEP,		eAnimSleep);
	anim().LinkAction(ACT_REST,			eAnimLieIdle);
	anim().LinkAction(ACT_DRAG,			eAnimDragCorpse);
	anim().LinkAction(ACT_ATTACK,		eAnimAttack);
	anim().LinkAction(ACT_STEAL,		eAnimSteal);
	anim().LinkAction(ACT_LOOK_AROUND,	eAnimScared);

#ifdef DEBUG	
	anim().accel_chain_test		();
#endif

}

// ���������� true, ���� ����� ���������� ���� ������� ���������� �������� ���������
// �.�. ���� ������������ ������������������
void CAI_Flesh::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_DRAG_CORPSE) == 	ASP_DRAG_CORPSE) anim().SetCurAnim(eAnimDragCorpse);

	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		com_man().seq_run(anim().get_motion_id(eAnimCheckCorpse));	}

	if ((spec_params & ASP_BACK_ATTACK) == ASP_BACK_ATTACK) {
		com_man().seq_run(anim().get_motion_id(eAnimAttackFromBack));
	}

	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) anim().SetCurAnim(eAnimThreaten);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// ������� ConeSphereIntersection
// ����������� ������ (�� �������������) �� ������
// ���������� ��� ����������� ����������� ������ ����� � �����-������ �����
// ���������: ConeVertex - ������� ������, ConeAngle - ���� ������ (����� ������������ � �������)
// ConeDir - ����������� ������, SphereCenter - ����� �����, SphereRadius - ������ �����
bool CAI_Flesh::ConeSphereIntersection(fVector3 ConeVertex, f32 ConeAngle, fVector3 ConeDir, fVector3 SphereCenter, f32 SphereRadius)
{
	f32 fInvSin = 1.0f/_sin(ConeAngle);
	f32 fCosSqr = _cos(ConeAngle)*_cos(ConeAngle);

	fVector3 kCmV;
	kCmV.sub(SphereCenter,ConeVertex);
	fVector3 kD		= kCmV;
	fVector3 tempV	= ConeDir;
	tempV.mul		(SphereRadius* fInvSin);
	kD.add			(tempV);

	f32 fDSqrLen = kD.square_magnitude();
	f32 fE = kD.dotproduct(ConeDir);
	if ( fE > 0.0f && fE*fE >= fDSqrLen*fCosSqr ) {
		
		f32 fSinSqr = _sin(ConeAngle)*_sin(ConeAngle);

		fDSqrLen = kCmV.square_magnitude();
		fE = -kCmV.dotproduct(ConeDir);
		if ( fE > 0.0f && fE*fE >= fDSqrLen*fSinSqr ) {
			f32 fRSqr = SphereRadius*SphereRadius;
			return fDSqrLen <= fRSqr;
		} else return true;
	}

	return false;
}
