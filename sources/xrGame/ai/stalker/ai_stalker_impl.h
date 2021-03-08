////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_impl.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker" (inline functions implementation)
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\..\Level.h"
#include "../../seniority_hierarchy_holder.h"
#include "../../team_hierarchy_holder.h"
#include "../../squad_hierarchy_holder.h"
#include "../../group_hierarchy_holder.h"
#include "../../effectorshot.h"

inline	CAgentManager &CStalker::agent_manager	() const
{
	return			(Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).agent_manager());
}

inline	Fvector3 CStalker::weapon_shot_effector_direction	(const Fvector3& current) const
{
	VERIFY			(weapon_shot_effector().IsActive());
	Fvector3			result;
	weapon_shot_effector().GetDeltaAngle(result);

	float			y,p;
	current.getHP	(y,p);

	result.setHP	(-result.y + y, -result.x + p);

	return			(result);
}
