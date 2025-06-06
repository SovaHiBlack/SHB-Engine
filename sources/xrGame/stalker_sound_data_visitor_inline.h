////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_sound_data_visitor_inline.h
//	Created 	: 02.02.2005
//  Modified 	: 02.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker sound data visitor inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CStalkerSoundDataVisitor::CStalkerSoundDataVisitor(CStalker* object)
{
	VERIFY(object);
	m_object = object;
}

IC	CStalker& CStalkerSoundDataVisitor::object( ) const
{
	VERIFY(m_object);
	return		(*m_object);
}
