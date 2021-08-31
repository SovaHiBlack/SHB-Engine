//	Module 		: SpaceRestrictionHolder_inline.h
//	Description : Space restriction holder inline functions

#pragma once

inline CSpaceRestrictionHolder::CSpaceRestrictionHolder( )
{
	m_default_out_restrictions = "";
	m_default_in_restrictions = "";
}

inline CSharedString	CSpaceRestrictionHolder::default_out_restrictions( ) const
{
	return m_default_out_restrictions;
}

inline CSharedString	CSpaceRestrictionHolder::default_in_restrictions( ) const
{
	return m_default_in_restrictions;
}
