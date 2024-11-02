////////////////////////////////////////////////////////////////////////////
//	Module 		: base_monster_anim.cpp
//	Created 	: 22.05.2003
//  Modified 	: 23.09.2003
//	Author		: Serge Zhem
//	Description : Animations for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "base_monster.h"
#include "../../../..\XR_3DA\skeletonanimated.h"
#include "../../../sound_player.h"
#include "../../../ai_monster_space.h"
#include "../control_animation_base.h"

// Установка анимации
void CBaseMonster::SelectAnimation(const fVector3&/**_view/**/, const fVector3&/**_move/**/, f32 /**speed/**/)
{
	control().animation().update_frame();
}

