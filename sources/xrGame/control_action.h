////////////////////////////////////////////////////////////////////////////
//	Module 		: animation_action.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Control action
////////////////////////////////////////////////////////////////////////////

#pragma once

class CStalker;

class CControlAction
{
protected:
	CStalker* m_object;

public:
	IC					CControlAction	();
	IC		void		set_object(CStalker* object);
	IC		bool		applicable		() const;
	IC		bool		completed		() const;
	IC		void		initialize		();
	IC		void		execute			();
	IC		void		finalize		();
	IC		CStalker& object( ) const;
	IC		void		remove_links	(CObject *object);
};

#include "control_action_inline.h"