//	Module 		: script_value_container_impl.h
//	Description : Script value container

#pragma once

#include "object_broker.h"

inline CScriptValueContainer::~CScriptValueContainer( )
{
	clear( );
}

inline void CScriptValueContainer::add(CScriptValue* new_value)
{ }

inline void CScriptValueContainer::assign( )
{ }

inline void CScriptValueContainer::clear( )
{
	delete_data(m_values);
}
