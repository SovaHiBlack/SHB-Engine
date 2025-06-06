////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_restriction.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker movement restriction
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai/stalker/ai_stalker_impl.h"
#include "agent_manager.h"
#include "agent_location_manager.h"

class CStalker;

class CStalkerMovementRestrictor {
private:
	CStalker*m_object;
	const CAgentManager	*m_agent_manager;
	bool				m_use_enemy_info;
	bool				m_notify_agent_manager;

public:
	IC					CStalkerMovementRestrictor	(CStalker*object, bool use_enemy_info, bool notify_agent_manager = true);
	IC		bool		operator()					(const CCoverPoint *cover) const;
	IC		f32		weight						(const CCoverPoint *cover) const;
	IC		void		finalize					(const CCoverPoint *cover) const;
};

#include "stalker_movement_restriction_inline.h"
