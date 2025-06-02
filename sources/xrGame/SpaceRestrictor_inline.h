////////////////////////////////////////////////////////////////////////////
//	Module 		: SpaceRestrictor_inline.h
//	Description : Space restrictor inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CSpaceRestrictor::CSpaceRestrictor( )
{
	m_space_restrictor_type = RestrictionSpace::eRestrictorTypeNone;
}

IC bool CSpaceRestrictor::actual( ) const
{
	return m_actuality;
}

IC void CSpaceRestrictor::actual(bool value) const
{
	m_actuality = value;
}

IC RestrictionSpace::ERestrictorTypes CSpaceRestrictor::restrictor_type( ) const
{
	return RestrictionSpace::ERestrictorTypes(m_space_restrictor_type);
}
