//	Module 		: ScriptEntity_space.h
//	Description : Script entity space

#pragma once

namespace ScriptEntity
{
	enum EActionType
	{
		eActionTypeMovement = u32(0),
		eActionTypeWatch,
		eActionTypeAnimation,
		eActionTypeSound,
		eActionTypeParticle,
		eActionTypeObject,
		eActionTypeCount
	};
};
