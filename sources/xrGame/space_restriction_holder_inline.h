////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_holder_inline.h
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction holder inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CSpaceRestrictionHolder::CSpaceRestrictionHolder		()
{
	m_default_out_restrictions	= "";
	m_default_in_restrictions	= "";
}

inline	shared_str	CSpaceRestrictionHolder::default_out_restrictions	() const
{
	return					(m_default_out_restrictions);
}

inline	shared_str	CSpaceRestrictionHolder::default_in_restrictions	() const
{
	return					(m_default_in_restrictions);
}
