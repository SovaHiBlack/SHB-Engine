////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_death_actions.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker death action classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_base_action.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDead
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDead : public CStalkerActionBase {
protected:
	typedef CStalkerActionBase inherited;

private:
			bool		fire						() const;

public:
	CStalkerActionDead(CStalker* object, pcstr action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
};
