////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_restriction.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker movement restriction
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai/stalker/ai_stalker_impl.h"
#include "AgentManager.h"
#include "agent_location_manager.h"

class CStalker;

class CStalkerMovementRestrictor {
private:
	CStalker			*m_object;
	const CAgentManager	*m_agent_manager;
	bool				m_use_enemy_info;
	bool				m_notify_agent_manager;

public:
	inline					CStalkerMovementRestrictor	(CStalker *object, bool use_enemy_info, bool notify_agent_manager = true);
	inline		bool		operator()					(const CCoverPoint *cover) const;
	inline		float		weight						(const CCoverPoint *cover) const;
	inline		void		finalize					(const CCoverPoint *cover) const;
};

#include "stalker_movement_restriction_inline.h"
