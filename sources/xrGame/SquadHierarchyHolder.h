////////////////////////////////////////////////////////////////////////////
//	Module 		: SquadHierarchyHolder.h
//	Description : Squad hierarchy holder
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SeniorityHierarchy_space.h"

class CGroupHierarchyHolder;
class CEntity;
class CTeamHierarchyHolder;

namespace SquadHierarchyHolder
{
	typedef xr_vector<CGroupHierarchyHolder*>		GROUP_REGISTRY;
}

class CSquadHierarchyHolder
{
private:
	enum
	{
		max_group_count = 32
	};

private:
	typedef SquadHierarchyHolder::GROUP_REGISTRY	GROUP_REGISTRY;

private:
	CTeamHierarchyHolder* m_team;
	mutable GROUP_REGISTRY			m_groups;

#ifdef SQUAD_HIERARCHY_HOLDER_USE_LEADER
private:
	CEntity* m_leader;
#endif // SQUAD_HIERARCHY_HOLDER_USE_LEADER

public:
	IC								CSquadHierarchyHolder(CTeamHierarchyHolder* team);
	virtual							~CSquadHierarchyHolder( );
	CGroupHierarchyHolder& group(u32 group_id) const;
	IC		CTeamHierarchyHolder& team( ) const;
	IC		const GROUP_REGISTRY& groups( ) const;

#ifdef SQUAD_HIERARCHY_HOLDER_USE_LEADER
public:
	IC		void					leader(CEntity* leader);
	IC		CEntity* leader( ) const;
	void					update_leader( );
#endif // SQUAD_HIERARCHY_HOLDER_USE_LEADER

};

#include "SquadHierarchyHolder_inline.h"
