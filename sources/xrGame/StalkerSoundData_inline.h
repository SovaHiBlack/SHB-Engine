////////////////////////////////////////////////////////////////////////////
//	Module 		: StalkerSoundData_inline.h
//	Description : Stalker sound data inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CStalkerSoundData::CStalkerSoundData(CStalker* object)
{
	VERIFY(object);
	m_object = object;
}

IC CStalker& CStalkerSoundData::object( ) const
{
	VERIFY(m_object);
	return *m_object;
}
