////////////////////////////////////////////////////////////////////////////
//	Module 		: TeamHierarchyHolder_inline.h
//	Description : Team hierarchy holder inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CTeamHierarchyHolder::CTeamHierarchyHolder(CSeniorityHierarchyHolder* seniority_manager)
{
	VERIFY(seniority_manager);
	m_seniority_manager = seniority_manager;
	SeniorityHierarchy::assign_svector(m_squads, max_squad_count, 0);
}

IC CSeniorityHierarchyHolder& CTeamHierarchyHolder::team( ) const
{
	VERIFY(m_seniority_manager);
	return *m_seniority_manager;
}

IC const CTeamHierarchyHolder::SQUAD_REGISTRY& CTeamHierarchyHolder::squads( ) const
{
	return m_squads;
}
