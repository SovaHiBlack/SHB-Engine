//	Module 		: ActionBase.h
//	Description : Base action

#pragma once

#include "property_storage.h"
#include "script_export_space.h"
#include "operator_abstract.h"
#include "ALife_space.h"

class CScriptGameObject;

template <typename _object_type>
class CActionBase : public GraphEngine::CWorldOperator
{
protected:
	typedef GraphEngine::CWorldOperator			inherited;
	typedef GraphEngine::CWorldProperty			COperatorCondition;
	typedef GraphEngine::_solver_condition_type	_condition_type;
	typedef GraphEngine::_solver_value_type		_value_type;

	enum EActionStates
	{
		eActionStateConstructed = u32(0),
		eActionStateSetup,
		eActionStateInitialized,
		eActionStateExecuted,
		eActionStateFinalized,
		eActionStateDummy = u32(-1)
	};

public:
	_object_type* m_object;
	CPropertyStorage* m_storage;

protected:
	u32							m_start_level_time;
	u32							m_start_game_time;
	u32							m_inertia_time;
	mutable _edge_value_type	m_weight;
	bool						m_first_time;

#ifdef LOG_ACTION
public:
	const char* m_action_name;
	bool						m_use_log;
	bool						m_switched;

	virtual void				debug_log(const EActionStates state_state) const;
	virtual	void				set_use_log(bool value);
	virtual void				show(const char* offset = "");
#endif // def LOG_ACTION

public:
	inline							CActionBase(const xr_vector<COperatorCondition>& conditions, const xr_vector<COperatorCondition>& effects, _object_type* object = 0, const char* action_name = "");
	inline							CActionBase(_object_type* object, const char* action_name = "");
	virtual						~CActionBase( );
	inline		void				init(_object_type* object, const char* action_name);
	virtual void				setup(_object_type* object, CPropertyStorage* storage);
	virtual void				initialize( );
	virtual void				execute( );
	virtual void				finalize( );
	virtual _edge_value_type	weight(const CSConditionState& condition0, const CSConditionState& condition1) const;
	inline void				set_inertia_time(u32 inertia_time);
	inline u32					start_level_time( ) const;
	inline u32					inertia_time( ) const;
	inline bool				completed( ) const;
	inline void				set_property(const _condition_type& condition_id, const _value_type& value);
	inline const _value_type& property(const _condition_type& condition_id) const;
	inline void 				set_weight(const _edge_value_type& weight);
	inline bool				first_time( ) const;

	virtual void				save(CNetPacket& packet)
	{ }
	virtual void				load(IReader& packet)
	{ }

	static void script_register(lua_State*);
};

using CScriptActionBase = CActionBase<CScriptGameObject>;

add_to_type_list(CScriptActionBase)
#undef script_type_list
#define script_type_list save_type_list(CScriptActionBase)

#include "ActionBase_inline.h"
