#pragma once

#include "EncyclopediaArticle_defs.h"
#include "GameTask_defs.h"
#include "script_export_space.h"

class CGameTaskManager;
class CMapLocation;
class CGameTask;

class SScriptObjectiveHelper : public IPureSerializeObject<IReader, IWriter>
{
public:
	xr_vector<shared_str>	m_s_complete_lua_functions;
	xr_vector<shared_str>	m_s_fail_lua_functions;

	xr_vector<shared_str>	m_s_lua_functions_on_complete;
	xr_vector<shared_str>	m_s_lua_functions_on_fail;
public:
	bool			not_empty()
	{
		return m_s_complete_lua_functions.size() ||
			m_s_fail_lua_functions.size() ||
			m_s_lua_functions_on_complete.size() ||
			m_s_lua_functions_on_fail.size();
	}

	virtual void			save(IWriter& stream);
	virtual void			load(IReader& stream);

	void			init_functors(xr_vector<shared_str>& v_src, xr_vector<luabind::functor<bool>>& v_dest);
};

class SGameTaskObjective : public IPureSerializeObject<IReader, IWriter>
{
	friend struct SGameTaskKey;
	friend class CGameTaskManager;

private:
	ETaskState				task_state;
	CGameTask* parent;
	s32						idx;
	void					SendInfo(xr_vector<shared_str>&);
	void					CallAllFuncs(xr_vector<luabind::functor<bool> >& v);
	bool					CheckInfo(xr_vector<shared_str>&);
	bool					CheckFunctions(xr_vector<luabind::functor<bool> >& v);
	void					SetTaskState(ETaskState new_state);

public:
	SScriptObjectiveHelper	m_pScriptHelper;
	virtual void			save(IWriter& stream);
	virtual void			load(IReader& stream);

	SGameTaskObjective(CGameTask* parent, s32 idx);
	SGameTaskObjective();
	shared_str				description;
	shared_str				article_id;
	shared_str				map_hint;
	shared_str				map_location;
	u16						object_id;
	shared_str				article_key;
	CMapLocation* LinkedMapLocation();
	ETaskState				TaskState()
	{
		return task_state;
	}
	ETaskState				UpdateState();

	shared_str							icon_texture_name;
	fRect								icon_rect;
	bool								def_location_enabled;
	//complete/fail stuff
	xr_vector<shared_str>				m_completeInfos;
	xr_vector<shared_str>				m_failInfos;
	xr_vector<shared_str>				m_infos_on_complete;
	xr_vector<shared_str>				m_infos_on_fail;

	xr_vector<luabind::functor<bool>>	m_complete_lua_functions;
	xr_vector<luabind::functor<bool>>	m_fail_lua_functions;

	xr_vector<luabind::functor<bool>>	m_lua_functions_on_complete;
	xr_vector<luabind::functor<bool>>	m_lua_functions_on_fail;

	// for scripting access
	void					SetDescription_script(pcstr _descr);
	void					SetArticleID_script(pcstr _id);
	s32						GetIDX_script()
	{
		return idx;
	}
	void					SetMapHint_script(pcstr _str);
	void					SetMapLocation_script(pcstr _str);
	void					SetObjectID_script(u16 id);
	void					SetArticleKey_script(pcstr _str);

	void					SetIconName_script(pcstr _str);

	void					AddCompleteInfo_script(pcstr _str);
	void					AddFailInfo_script(pcstr _str);
	void					AddOnCompleteInfo_script(pcstr _str);
	void					AddOnFailInfo_script(pcstr _str);

	void					AddCompleteFunc_script(pcstr _str);
	void					AddFailFunc_script(pcstr _str);
	void					AddOnCompleteFunc_script(pcstr _str);
	void					AddOnFailFunc_script(pcstr _str);
	pcstr					GetDescription_script()
	{
		return *description;
	}
	void					ChangeStateCallback();
};

DEFINE_VECTOR(SGameTaskObjective, OBJECTIVE_VECTOR, OBJECTIVE_VECTOR_IT);

class CGameTask
{
private:
	CGameTask(const CGameTask&)
	{} //disable copy ctor

protected:
	void					Load(const TASK_ID& id);

public:
	CGameTask(const TASK_ID& id);
	CGameTask();

	bool					HasLinkedMapLocations();
	bool					HasInProgressObjective();

	SGameTaskObjective& Objective(s32 objectice_id)
	{
		return m_Objectives[objectice_id];
	}

	TASK_ID					m_ID;
	shared_str				m_Title;
	OBJECTIVE_VECTOR		m_Objectives;
	ALife::_TIME_ID			m_ReceiveTime;
	ALife::_TIME_ID			m_FinishTime;
	ALife::_TIME_ID			m_TimeToComplete;
	u32						m_priority;

	// for scripting access
	void					Load_script(pcstr _id);
	void					SetTitle_script(pcstr _title);
	pcstr					GetTitle_script()
	{
		return *m_Title;
	}
	void					SetPriority_script(s32 _prio);
	s32						GetPriority_script()
	{
		return m_priority;
	}
	void					AddObjective_script(SGameTaskObjective* O);
	SGameTaskObjective* GetObjective_script(s32 objective_id)
	{
		return &(Objective(objective_id));
	}
	pcstr					GetID_script()
	{
		return *m_ID;
	}
	void					SetID_script(pcstr _id)
	{
		m_ID = _id;
	}
	s32						GetObjectiveSize_script()
	{
		return m_Objectives.size();
	}

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CGameTask)
#undef script_type_list
#define script_type_list save_type_list(CGameTask)
