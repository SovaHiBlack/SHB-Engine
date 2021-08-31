//	Module 		: SpaceRestrictionBase_inline.h
//	Description : Space restriction base inline functions

#pragma once

#ifdef DEBUG
inline bool CSpaceRestrictionBase::correct( ) const
{
	return m_correct;
}
#endif // def DEBUG