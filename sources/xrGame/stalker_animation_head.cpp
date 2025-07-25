////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_head.cpp
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation manager : head animations
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_animation_manager.h"
#include "ai/stalker/Stalker.h"
#include "ai/stalker/ai_stalker_space.h"
#include "sound_player.h"
#include "stalker_animation_data.h"

void CStalkerAnimationManager::head_play_callback		(CBlend *blend)
{
	CStalker* object = (CStalker*)blend->CallbackParam;
	VERIFY					(object);

	CStalkerAnimationPair	&pair = object->animation().head();
	pair.on_animation_end	();
}

MotionID CStalkerAnimationManager::assign_head_animation	()
{
	const ANIM_VECTOR		&animations = m_data_storage->m_head_animations.A;
	CSoundPlayer			&sound = object().sound();
	if (!sound.active_sound_count(true))
		return				(animations[0]);

	if (!sound.active_sound_type((u32)StalkerSpace::eStalkerSoundMaskMovingInDanger))
		return				(animations[1]);

	return					(animations[0]);
}
