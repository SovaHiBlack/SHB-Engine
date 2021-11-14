#pragma once

enum ETaskState
{
	eTaskStateFail = 0,
	eTaskStateInProgress,
	eTaskStateCompleted,
	eTaskStateDummy = u32(-1)
};

using TASK_ID = CSharedString;

//DEFINE_VECTOR(TASK_ID, TASK_ID_VECTOR, TASK_ID_IT);
using TASK_ID_VECTOR = xr_vector<TASK_ID>;
using TASK_ID_IT = TASK_ID_VECTOR::iterator;

extern CSharedString		g_active_task_id;
extern unsigned short				g_active_task_objective_id;

#include "alife_abstract_registry.h"

class CGameTask;

struct SGameTaskKey : public IPureSerializeObject<IReader, IWriter>, public IPureDestroyableObject
{
	TASK_ID task_id;
	CGameTask* game_task;
	SGameTaskKey(TASK_ID t_id) : task_id(t_id), game_task(NULL)
	{ }
	SGameTaskKey( ) : task_id(NULL), game_task(NULL)
	{ }

	virtual void save(IWriter& stream);
	virtual void load(IReader& stream);
	virtual void destroy( );
};

//DEFINE_VECTOR(SGameTaskKey, GameTasks, GameTasks_it);
using GameTasks = xr_vector<SGameTaskKey>;
using GameTasks_it = GameTasks::iterator;

struct CGameTaskRegistry : public CALifeAbstractRegistry<unsigned short, GameTasks>
{
	virtual void save(IWriter& stream)
	{
		CALifeAbstractRegistry<unsigned short, GameTasks>::save(stream);
		save_data(g_active_task_id, stream);
		save_data(g_active_task_objective_id, stream);
	}
	virtual void load(IReader& stream)
	{
		CALifeAbstractRegistry<unsigned short, GameTasks>::load(stream);
		load_data(g_active_task_id, stream);
		load_data(g_active_task_objective_id, stream);
	}
};
