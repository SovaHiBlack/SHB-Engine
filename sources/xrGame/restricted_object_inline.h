////////////////////////////////////////////////////////////////////////////
//	Module 		: restricted_object_inline.h
//	Created 	: 18.08.2004
//  Modified 	: 23.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restricted object inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CRestrictedObject::CRestrictedObject		(CCustomMonster *object)
{
	VERIFY				(object);
	m_object			= object;
}

inline	bool CRestrictedObject::applied				() const
{
	return				(m_applied);
}

inline	CCustomMonster &CRestrictedObject::object	() const
{
	VERIFY				(m_object);
	return				(*m_object);
}

inline	bool CRestrictedObject::actual				() const
{
	return				(m_actual);
}

#ifdef DEBUG
inline	void CRestrictedObject::initialize			()
{
	if (m_applied)
		remove_border	();
}
#endif
