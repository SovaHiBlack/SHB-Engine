////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_base_action.h
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker base action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_script_base.h"

class CStalker;

//////////////////////////////////////////////////////////////////////////
// CStalkerActionBase
//////////////////////////////////////////////////////////////////////////

class CStalkerActionBase : public CActionScriptBase<CStalker> {
protected:
	typedef CActionScriptBase<CStalker>				inherited;
	typedef GraphEngineSpace::_solver_condition_type	_condition_type;
	typedef GraphEngineSpace::_solver_value_type		_value_type;

public:
	CStalkerActionBase(CStalker* object, pcstr action_name = "");
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
	IC		CStalker& object( ) const
	{
		VERIFY			(m_object);
		return			(*m_object);
	}
};
