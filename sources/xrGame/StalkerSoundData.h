////////////////////////////////////////////////////////////////////////////
//	Module 		: StalkerSoundData.h
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

#include "StalkerSoundData_inline.h"
