//	Module		: ScriptActionWrapper.h
//	Description	: Script action wrapper

#pragma once

#include "ActionBase.h"

class CScriptActionWrapper : public CScriptActionBase, public luabind::wrap_base
{
public:
	inline							CScriptActionWrapper(CScriptGameObject* object = 0, const char* action_name = "");
	virtual void				setup(CScriptGameObject* object, CPropertyStorage* storage);
	static void					setup_static(CScriptActionBase* action, CScriptGameObject* object, CPropertyStorage* storage);
	virtual void				initialize( );
	static void					initialize_static(CScriptActionBase* action);
	virtual void				execute( );
	static void					execute_static(CScriptActionBase* action);
	virtual void				finalize( );
	static void					finalize_static(CScriptActionBase* action);
	virtual _edge_value_type	weight(const CSConditionState& condition0, const CSConditionState& condition1) const;
	static _edge_value_type		weight_static(CScriptActionBase* action, const CSConditionState& condition0, const CSConditionState& condition1);
};

#include "ScriptActionWrapper_inline.h"
