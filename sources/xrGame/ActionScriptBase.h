//	Module		: ActionScriptBase.h
//	Description	: Base action with script support

#pragma once

#include "ActionBase.h"

class CScriptGameObject;

template <typename _object_type>
class CActionScriptBase : public CScriptActionBase
{
protected:
	using inherited = CScriptActionBase;

public:
	_object_type* m_object;

	inline					CActionScriptBase(const xr_vector<COperatorCondition>& conditions, const xr_vector<COperatorCondition>& effects, _object_type* object = 0, const char* action_name = "");
	inline					CActionScriptBase(_object_type* object = 0, const char* action_name = "");
	virtual				~CActionScriptBase( );
	virtual void		setup(_object_type* object, CPropertyStorage* storage);
	virtual void		setup(CScriptGameObject* object, CPropertyStorage* storage);
};

#include "ActionScriptBase_inline.h"
