////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_sound_data.cpp
//	Description : Stalker sound data
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StalkerSoundData.h"
#include "sound_user_data_visitor.h"
#include "ai/stalker/Stalker.h"

CStalkerSoundData::~CStalkerSoundData( )
{ }

void CStalkerSoundData::accept(CSound_UserDataVisitor* visitor)
{
	if (!m_object || m_object->getDestroy( ))
	{
		return;
	}

	visitor->visit(this);
}

void CStalkerSoundData::invalidate( )
{
	m_object = 0;
}
