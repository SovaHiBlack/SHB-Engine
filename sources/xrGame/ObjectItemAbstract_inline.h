////////////////////////////////////////////////////////////////////////////
//	Module 		: ObjectItemAbstract_inline.h
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item abstract class inline functions
////////////////////////////////////////////////////////////////////////////
#pragma once

inline	CObjectItemAbstract::CObjectItemAbstract	(const CLASS_ID &clsid, const char* script_clsid) :
	m_clsid				(clsid),
	m_script_clsid		(script_clsid)
{
}

inline	const CLASS_ID &CObjectItemAbstract::clsid	() const
{
	return				(m_clsid);
}

inline	shared_str	CObjectItemAbstract::script_clsid	() const
{
	return				(m_script_clsid);
}
