//	Module 		: SpaceRestrictionManager_inline.h
//	Description : Space restriction manager inline functions

#pragma once

#ifdef DEBUG
inline const CSpaceRestrictionManager::SPACE_RESTRICTIONS& CSpaceRestrictionManager::restrictions( ) const
{
	return m_space_restrictions;
}
#endif // def DEBUG

template <typename T1, typename T2>
inline void CSpaceRestrictionManager::add_border(ALife::_OBJECT_ID id, T1 p1, T2 p2)
{
	CRestrictionPtr client_restriction = restriction(id);
	if (client_restriction)
	{
		client_restriction->add_border(p1, p2);
	}
}
