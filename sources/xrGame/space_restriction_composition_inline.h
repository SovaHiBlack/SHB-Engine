////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_composition_inline.h
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction composition inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CSpaceRestrictionComposition::CSpaceRestrictionComposition	(CSpaceRestrictionHolder *space_restriction_holder, CSharedString space_restrictors)
{
	VERIFY						(space_restriction_holder);
	m_space_restriction_holder	= space_restriction_holder;
	m_space_restrictors			= space_restrictors;

#ifdef DEBUG
	check_restrictor_type		();
#endif // DEBUG

	++g_restriction_checker;
}

inline	CSharedString	CSpaceRestrictionComposition::name					() const
{
	return						(m_space_restrictors);
}

inline	bool CSpaceRestrictionComposition::shape					() const
{
	return						(false);
}

inline	bool CSpaceRestrictionComposition::default_restrictor		() const
{
	return						(false);
}
