#include "stdafx.h"
#include "Zombie.h"
#include "StateManagerZombie.h"
#include "../../../Profiler.h"
#include "../../../..\XR_3DA\skeletonanimated.h"
#include "../../../EntityCondition.h"
#include "../monster_velocity_space.h"

#include "../control_animation_base.h"
#include "../control_movement_base.h"

#ifdef _DEBUG
#include <dinput.h>
#endif

CZombie::CZombie( )
{
	StateMan = xr_new<CStateManagerZombie>(this);
	CControlled::init_external(this);
}

CZombie::~CZombie( )
{
	xr_delete(StateMan);
}

void CZombie::Load(pcstr section)
{
	inherited::Load(section);

	anim( ).accel_load(section);
	anim( ).accel_chain_add(eAnimWalkFwd, eAnimRun);

	fake_death_count = 1 + u8(Random.randI(pSettings->r_u8(section, "FakeDeathCount")));
	health_death_threshold = pSettings->r_float(section, "StartFakeDeathHealthThreshold");

	SVelocityParam& velocity_none = move( ).get_velocity(MonsterMovement::eMP_IDLE);
	SVelocityParam& velocity_turn = move( ).get_velocity(MonsterMovement::eMP_STAND);
	SVelocityParam& velocity_walk = move( ).get_velocity(MonsterMovement::eVelocityParameterWalkNormal);
	SVelocityParam& velocity_run = move( ).get_velocity(MonsterMovement::eVelocityParameterRunNormal);

	anim( ).AddAnim(eAnimStandIdle, "stand_idle_", -1, &velocity_none, PS_STAND, "fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim( ).AddAnim(eAnimStandTurnLeft, "stand_turn_ls_", -1, &velocity_turn, PS_STAND, "fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim( ).AddAnim(eAnimStandTurnRight, "stand_turn_rs_", -1, &velocity_turn, PS_STAND, "fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim( ).AddAnim(eAnimWalkFwd, "stand_walk_fwd_", -1, &velocity_walk, PS_STAND, "fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim( ).AddAnim(eAnimRun, "stand_run_", -1, &velocity_run, PS_STAND, "fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim( ).AddAnim(eAnimAttack, "stand_attack_", -1, &velocity_turn, PS_STAND, "fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim( ).AddAnim(eAnimDie, "stand_die_", 0, &velocity_none, PS_STAND, "fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

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
	anim( ).LinkAction(ACT_STEAL, eAnimWalkFwd);
	anim( ).LinkAction(ACT_LOOK_AROUND, eAnimStandIdle);

#ifdef DEBUG
	anim( ).accel_chain_test( );
#endif

}

void CZombie::reinit( )
{
	inherited::reinit( );

	Bones.Reset( );

	time_dead_start = 0;
	last_hit_frame = 0;
	time_resurrect = 0;
	fake_death_left = fake_death_count;
	active_triple_idx = u8(-1);
}

void CZombie::reload(pcstr section)
{
	inherited::reload(section);

	com_man( ).ta_fill_data(anim_triple_death[0], "fake_death_0_0", "fake_death_0_1", "fake_death_0_2", true, false);
	com_man( ).ta_fill_data(anim_triple_death[1], "fake_death_1_0", "fake_death_1_1", "fake_death_1_2", true, false);
	com_man( ).ta_fill_data(anim_triple_death[2], "fake_death_2_0", "fake_death_2_1", "fake_death_2_2", true, false);
	com_man( ).ta_fill_data(anim_triple_death[3], "fake_death_3_0", "fake_death_3_1", "fake_death_3_2", true, false);
}

void CZombie::BoneCallback(CBoneInstance* B)
{
	CZombie* this_class = static_cast<CZombie*>(B->Callback_Param);

	START_PROFILE("Zombie/Bones Update");
	this_class->Bones.Update(B, Device.dwTimeGlobal);
	STOP_PROFILE("AI/Zombie/Bones Update");
}

void CZombie::vfAssignBones( )
{
	// ��������� callback �� �����
	bone_spine = &smart_cast<CKinematics*>(Visual( ))->LL_GetBoneInstance(smart_cast<CKinematics*>(Visual( ))->LL_BoneID("bip01_spine"));
	bone_head = &smart_cast<CKinematics*>(Visual( ))->LL_GetBoneInstance(smart_cast<CKinematics*>(Visual( ))->LL_BoneID("bip01_head"));

	// Bones settings
	Bones.Reset( );
	Bones.AddBone(bone_spine, AXIS_Z);
	Bones.AddBone(bone_spine, AXIS_Y);
	Bones.AddBone(bone_spine, AXIS_X);

	Bones.AddBone(bone_head, AXIS_Z);
	Bones.AddBone(bone_head, AXIS_Y);
}

BOOL CZombie::net_Spawn(CSE_Abstract* DC)
{
	if (!inherited::net_Spawn(DC))
	{
		return FALSE;
	}

	vfAssignBones( );

	return TRUE;
}

#define TIME_FAKE_DEATH			5000
#define TIME_RESURRECT_RESTORE	2000

void	CZombie::Hit(SHit* pHDS)
{
	inherited::Hit(pHDS);

	if (!g_Alive( ))
	{
		return;
	}

	if ((pHDS->hit_type == ALife::eHitTypeFireWound) && (Device.dwFrame != last_hit_frame))
	{
		if (!com_man( ).ta_is_active( ) && (time_resurrect + TIME_RESURRECT_RESTORE < Device.dwTimeGlobal) && (conditions( ).GetHealth( ) < health_death_threshold))
		{
			if (conditions( ).GetHealth( ) < (health_death_threshold - f32(fake_death_count - fake_death_left) * health_death_threshold / fake_death_count))
			{
				active_triple_idx = u8(Random.randI(FAKE_DEATH_TYPES_COUNT));
				com_man( ).ta_activate(anim_triple_death[active_triple_idx]);
				move( ).stop( );
				time_dead_start = Device.dwTimeGlobal;

				if (fake_death_left == 0)
				{
					fake_death_left = 1;
				}

				fake_death_left--;
			}
		}
	}

	last_hit_frame = Device.dwFrame;
}

void CZombie::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);

	if (time_dead_start != 0)
	{
		if (time_dead_start + TIME_FAKE_DEATH < Device.dwTimeGlobal)
		{
			time_dead_start = 0;

			com_man( ).ta_pointbreak( );

			time_resurrect = Device.dwTimeGlobal;
		}
	}
}

bool CZombie::fake_death_fall_down( )
{
	if (com_man( ).ta_is_active( ))
	{
		return false;
	}

	com_man( ).ta_activate(anim_triple_death[u8(Random.randI(FAKE_DEATH_TYPES_COUNT))]);
	move( ).stop( );

	return true;
}

void CZombie::fake_death_stand_up( )
{
	// check if state active
	bool active = false;
	for (u32 i = 0; i < FAKE_DEATH_TYPES_COUNT; i++)
	{
		if (com_man( ).ta_is_active(anim_triple_death[i]))
		{
			active = true;
			break;
		}
	}

	if (!active)
	{
		return;
	}

	com_man( ).ta_pointbreak( );
}

#ifdef _DEBUG
void CZombie::debug_on_key(s32 key)
{
	switch (key)
	{
		case DIK_MINUS:
		{
			fake_death_fall_down( );
		}
		break;
		case DIK_EQUALS:
		{
			fake_death_stand_up( );
		}
		break;
	}
}
#endif

using namespace luabind;

#pragma optimize("s",on)
void CZombie::script_register(lua_State* L)
{
	module(L)
		[
			class_<CZombie, CGameObject>("CZombie")
				.def(constructor<>( ))
		];
}
