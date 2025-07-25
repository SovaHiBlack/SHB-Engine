////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_callbacks.cpp
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation manager : bone callbacks
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_animation_manager.h"
#include "ai/stalker/Stalker.h"
#include "sight_manager.h"
#include "stalker_movement_manager.h"
#include "GameObject_space.h"
#include "effectorshot.h"

#define TEMPLATE_SPECIALIZATION\
	template <\
		int yaw_factor_non_fire,\
		int pitch_factor_non_fire,\
		int yaw_factor_fire,\
		int pitch_factor_fire\
	>

#define _detail \
	detail<\
		yaw_factor_non_fire,\
		pitch_factor_non_fire,\
		yaw_factor_fire,\
		pitch_factor_fire\
	>

TEMPLATE_SPECIALIZATION
struct detail {
	static void callback	(CBoneInstance *B);
};

typedef detail	<  25,   0,  50,  50>	spine;
typedef detail	<  25,   0,  50,  50>	shoulder;
typedef detail	<  50, 100,   0,   0>	head;

TEMPLATE_SPECIALIZATION
void _detail::callback		(CBoneInstance *B)
{
	CStalker*			A = static_cast<CStalker*>(B->Callback_Param);
	VERIFY					(_valid(B->mTransform));
	fVector3 c				= B->mTransform.c;
	fMatrix4x4					spin;
	f32						yaw_factor = 0.0f;
	f32						pitch_factor = 0.0f;
	if (A->sight().use_torso_look())
	{
		yaw_factor			= yaw_factor_fire/100.f;
		pitch_factor		= pitch_factor_fire/100.f;
	}
	else
	{
		yaw_factor			= yaw_factor_non_fire/100.f;
		pitch_factor		= pitch_factor_non_fire/100.f;
	}

	f32						effector_yaw = 0.0f;
	f32						effector_pitch = 0.0f;
	if (A->weapon_shot_effector().IsActive())
	{
		fVector3				temp;
		A->weapon_shot_effector().GetDeltaAngle(temp);
		effector_yaw		= temp.y;
		VERIFY				(_valid(effector_yaw));
		effector_pitch		= temp.x;
		VERIFY				(_valid(effector_pitch));
	}

	VERIFY					(_valid(A->movement().head_orientation().current.yaw));
	VERIFY					(_valid(A->movement().body_orientation().current.yaw));
	VERIFY					(_valid(A->NET_Last.o_torso.pitch));

	f32					yaw		= angle_normalize_signed(-yaw_factor * angle_normalize_signed(A->movement().head_orientation().current.yaw + effector_yaw - (A->movement().body_orientation().current.yaw)));
	f32					pitch	= angle_normalize_signed(-pitch_factor * angle_normalize_signed(A->NET_Last.o_torso.pitch + effector_pitch));
	VERIFY					(_valid(yaw));
	VERIFY					(_valid(pitch));

	spin.setXYZ				(pitch, yaw, 0);
	VERIFY					(_valid(spin));
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

#undef TEMPLATE_SPECIALIZATION
#undef _detail

void CStalkerAnimationManager::assign_bone_callbacks	()
{
	CKinematics						*kinematics = smart_cast<CKinematicsAnimated*>(m_visual);
	VERIFY							(kinematics);

	pcstr							section = *object().cNameSect();
	
	int								head_bone = kinematics->LL_BoneID(pSettings->r_string(section,"bone_head"));
	kinematics->LL_GetBoneInstance	(u16(head_bone)).set_callback(bctCustom,&head::callback,&object());

	int								shoulder_bone = kinematics->LL_BoneID(pSettings->r_string(section,"bone_shoulder"));
	kinematics->LL_GetBoneInstance	(u16(shoulder_bone)).set_callback(bctCustom,&shoulder::callback,&object());

	int								spin_bone = kinematics->LL_BoneID(pSettings->r_string(section,"bone_spin"));
	kinematics->LL_GetBoneInstance	(u16(spin_bone)).set_callback(bctCustom,&spine::callback,&object());
}
