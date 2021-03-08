////////////////////////////////////////////////////////////////////////////
//	Module 		: base_monster_anim.cpp
//	Created 	: 22.05.2003
//  Modified 	: 23.09.2003
//	Author		: Serge Zhem
//	Description : Animations for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "BaseMonster.h"
#include "../../../..\ENGINE\skeletonanimated.h"
#include "../../../sound_player.h"
#include "../../../ai_monster_space.h"
#include "../control_animation_base.h"

// Установка анимации
void CBaseMonster::SelectAnimation(const Fvector3&/**_view/**/, const Fvector3&/**_move/**/, float /**speed/**/)
{
	control().animation().update_frame();
}

