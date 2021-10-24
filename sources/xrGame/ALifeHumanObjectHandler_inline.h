//	Module 		: ALifeHumanObjectHandler_inline.h
//	Description : ALife human object handler class inline functions

#pragma once

inline CALifeHumanObjectHandler::CALifeHumanObjectHandler(object_type* object)
{
	VERIFY(object);
	m_object = object;
}

inline CALifeHumanObjectHandler::object_type& CALifeHumanObjectHandler::object( ) const
{
	VERIFY(m_object);
	return *m_object;
}
