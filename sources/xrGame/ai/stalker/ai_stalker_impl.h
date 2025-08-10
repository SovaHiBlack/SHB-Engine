////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_impl.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker" (inline functions implementation)
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../Level.h"
#include "../../SeniorityHierarchyHolder.h"
#include "../../TeamHierarchyHolder.h"
#include "../../SquadHierarchyHolder.h"
#include "../../GroupHierarchyHolder.h"
#include "../../EffectorShot.h"

IC	CAgentManager & CStalker::agent_manager	() const
{
	return			(Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).agent_manager());
}

IC	fVector3 CStalker::weapon_shot_effector_direction	(const fVector3& current) const
{
	VERIFY			(weapon_shot_effector().IsActive());
	fVector3			result;
	weapon_shot_effector().GetDeltaAngle(result);

	f32			y;
	f32 p;
	current.getHP	(y,p);

	result.setHP	(-result.y + y, -result.x + p);

	return			(result);
}
