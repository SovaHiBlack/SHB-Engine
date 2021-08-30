//	Module 		: PurchaseList_inline.h
//	Description : purchase list class inline functions

#pragma once

inline float CPurchaseList::deficit(const CSharedString& section) const
{
	DEFICITS::const_iterator I = m_deficits.find(section);
	if (I != m_deficits.end( ))
	{
		return (*I).second;
	}

	return 1.0f;
}

inline const CPurchaseList::DEFICITS& CPurchaseList::deficits( ) const
{
	return m_deficits;
}

inline void CPurchaseList::deficit(const CSharedString& section, const float& deficit)
{
	DEFICITS::iterator I = m_deficits.find(section);
	if (I != m_deficits.end( ))
	{
		(*I).second = deficit;
		return;
	}

	m_deficits.insert(std::make_pair(section, deficit));
}
