////////////////////////////////////////////////////////////////////////////
//	Module 		: SeniorityHierarchyHolder_inline.h
//	Description : Seniority hierarchy holder inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CSeniorityHierarchyHolder::CSeniorityHierarchyHolder( )
{
	SeniorityHierarchy::assign_svector(m_teams, max_team_count, 0);
}

IC const CSeniorityHierarchyHolder::TEAM_REGISTRY& CSeniorityHierarchyHolder::teams( ) const
{
	return m_teams;
}
