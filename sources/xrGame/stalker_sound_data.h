////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_sound_data.h
//	Created 	: 02.02.2005
//  Modified 	: 02.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker sound data
////////////////////////////////////////////////////////////////////////////

#pragma once

class CStalker;

class CStalkerSoundData : public CSound_UserData
{
private:
	CStalker* m_object;

public:
	IC					CStalkerSoundData(CStalker* object);
	virtual				~CStalkerSoundData( );
	virtual void		invalidate( );
	virtual void		accept(CSound_UserDataVisitor* visitor);
	IC		CStalker& object( ) const;
};

#include "stalker_sound_data_inline.h"
