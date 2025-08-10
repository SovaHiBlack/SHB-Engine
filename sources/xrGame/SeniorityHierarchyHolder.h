////////////////////////////////////////////////////////////////////////////
//	Module 		: SeniorityHierarchyHolder.h
//	Description : Seniority hierarchy holder
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SeniorityHierarchy_space.h"

class CTeamHierarchyHolder;

class CSeniorityHierarchyHolder
{
private:
	enum
	{
		max_team_count = 64
	};

private:
	typedef svector<CTeamHierarchyHolder*, max_team_count> TEAM_REGISTRY;

private:
	TEAM_REGISTRY					m_teams;

public:
	IC								CSeniorityHierarchyHolder( );
	virtual							~CSeniorityHierarchyHolder( );
	CTeamHierarchyHolder& team(u32 team_id);
	IC		const TEAM_REGISTRY& teams( ) const;
};

#include "SeniorityHierarchyHolder_inline.h"
