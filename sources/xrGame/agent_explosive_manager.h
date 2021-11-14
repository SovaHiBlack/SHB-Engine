////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_explosive_manager.h
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Agent explosive manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "danger_explosive.h"

class CObject;
class CAgentManager;
class CMemberOrder;

class CAgentExplosiveManager {
public:
	typedef xr_vector<CDangerExplosive>				EXPLOSIVES;
	typedef xr_vector<unsigned short>							TO_BE_DESTROYED;

private:
	CAgentManager			*m_object;
	EXPLOSIVES				m_explosives;
	TO_BE_DESTROYED			m_explosives_to_remove;

protected:
	inline		CAgentManager	&object					() const;
	inline		EXPLOSIVES		&explosives				();
			bool			process_explosive		(CMemberOrder &member);

public:
	inline						CAgentExplosiveManager	(CAgentManager *object);
			void			remove_links			(CObject *object);
			void			register_explosive		(const CExplosive *explosive, const CGameObject *game_object);
			void			react_on_explosives		();
			void			update					();
};

#include "agent_explosive_manager_inline.h"
