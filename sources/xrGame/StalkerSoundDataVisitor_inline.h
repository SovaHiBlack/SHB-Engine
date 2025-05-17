////////////////////////////////////////////////////////////////////////////
//	Module 		: StalkerSoundDataVisitor_inline.h
//	Description : Stalker sound data visitor inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CStalkerSoundDataVisitor::CStalkerSoundDataVisitor(CStalker* object)
{
	VERIFY(object);
	m_object = object;
}

IC CStalker& CStalkerSoundDataVisitor::object( ) const
{
	VERIFY(m_object);
	return *m_object;
}
