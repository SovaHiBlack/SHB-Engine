////////////////////////////////////////////////////////////////////////////
//	Module 		: sight_control_action.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Sight control action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "sight_action.h"

class CSightControlAction : public CSightAction {
protected:
	float								m_weight;
	u32									m_inertia_time;
	u32									m_start_time;

public:
	inline									CSightControlAction	(float weight, u32 inertia_time, const CSightAction &sight_action);
	inline	float							weight				() const;
	inline	bool							completed			() const;
	inline	bool							use_torso_look		() const;
	inline	const SightManager::ESightType	&sight_type			() const;
	inline	const Fvector3&					vector3d			() const;
	inline	const CGameObject				&object				() const;
};

#include "sight_control_action_inline.h"