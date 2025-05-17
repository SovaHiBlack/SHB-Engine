////////////////////////////////////////////////////////////////////////////
//	Module 		: StalkerSoundDataVisitor.h
//	Description : Stalker sound data visitor
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "sound_user_data_visitor.h"

class CStalker;
class CStalkerSoundData;

class CStalkerSoundDataVisitor : public CSound_UserDataVisitor
{
private:
	CStalker* m_object;

public:
	IC					CStalkerSoundDataVisitor(CStalker* object);
	virtual				~CStalkerSoundDataVisitor( );
	virtual void		visit(CStalkerSoundData* data);
	IC		CStalker& object( ) const;
};

#include "StalkerSoundDataVisitor_inline.h"