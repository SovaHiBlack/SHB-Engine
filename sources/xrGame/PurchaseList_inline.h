////////////////////////////////////////////////////////////////////////////
//	Module 		: PurchaseList_inline.h
//	Description : purchase list class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC f32 CPurchaseList::deficit(const shared_str& section) const
{
	DEFICITS::const_iterator I = m_deficits.find(section);
	if (I != m_deficits.end( ))
	{
		return (*I).second;
	}

	return 1.0f;
}

IC const CPurchaseList::DEFICITS& CPurchaseList::deficits( ) const
{
	return m_deficits;
}

IC void CPurchaseList::deficit(const shared_str& section, const f32& deficit)
{
	DEFICITS::iterator I = m_deficits.find(section);
	if (I != m_deficits.end( ))
	{
		(*I).second = deficit;
		return;
	}

	m_deficits.insert(std::make_pair(section, deficit));
}
