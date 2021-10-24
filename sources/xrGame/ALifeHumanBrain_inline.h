//	Module 		: ALifeHumanBrain_inline.h
//	Description : ALife human brain class inline functions

#pragma once

inline CALifeHumanBrain::object_type& CALifeHumanBrain::object( ) const
{
	VERIFY(m_object);
	return *m_object;
}

inline	CALifeHumanBrain::object_handler_type& CALifeHumanBrain::objects( ) const
{
	VERIFY(m_object_handler);
	return *m_object_handler;
}
