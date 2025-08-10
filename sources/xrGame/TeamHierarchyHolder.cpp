////////////////////////////////////////////////////////////////////////////
//	Module 		: TeamHierarchyHolder.cpp
//	Description : Team hierarchy holder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TeamHierarchyHolder.h"
#include "SquadHierarchyHolder.h"
#include "object_broker.h"
#include "SeniorityHierarchy_space.h"

CTeamHierarchyHolder::~CTeamHierarchyHolder( )
{
	delete_data(m_squads);
}

CSquadHierarchyHolder& CTeamHierarchyHolder::squad(u32 squad_id) const
{
	VERIFY3(squad_id < max_squad_count, "Squad id is invalid : ", *SeniorityHierarchy::to_string(squad_id));
	if (!m_squads[squad_id])
	{
		m_squads[squad_id] = xr_new<CSquadHierarchyHolder>(const_cast<CTeamHierarchyHolder*>(this));
	}

	return *m_squads[squad_id];
}
