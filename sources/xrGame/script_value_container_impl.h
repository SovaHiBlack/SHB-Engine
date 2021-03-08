////////////////////////////////////////////////////////////////////////////
//	Module 		: script_value_container_impl.h
//	Created 	: 16.07.2004
//  Modified 	: 16.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Script value container
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_broker.h"

inline	CScriptValueContainer::~CScriptValueContainer	()
{
	clear				();
}

inline	void CScriptValueContainer::add			(CScriptValue *new_value)
{ }

inline	void CScriptValueContainer::assign		()
{ }

inline	void CScriptValueContainer::clear		()
{
	delete_data			(m_values);
}
