//	Module 		: StalkerAnimationData.cpp
//	Description : Stalker animation data

#include "stdafx.h"

#include "StalkerAnimationData.h"

CStalkerAnimationData::CStalkerAnimationData(CKinematicsAnimated* skeleton_animated)
{
	m_part_animations.Load		(skeleton_animated, "");
	m_head_animations.Load		(skeleton_animated, "");
	m_global_animations.Load	(skeleton_animated, "item_");
}
