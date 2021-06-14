#pragma once

#include "GameTask_defs.h"
#include "object_interfaces.h"

class CGameTaskWrapper;
class CGameTask;
class CMapLocation;
class SGameTaskObjective;

class CGameTaskManager
{
	CGameTaskWrapper* m_gametasks;
	enum
	{
		eChanged = (1 << 0)
	};
	Flags8					m_flags;

protected:
	void					UpdateActiveTask( );

public:
	CGameTaskManager( );
	~CGameTaskManager( );

	void					initialize(U16 id);
	GameTasks& GameTasks( );
	CGameTask* HasGameTask(const TASK_ID& id);
	CGameTask* GiveGameTaskToActor(const TASK_ID& id, u32 timeToComplete, bool bCheckExisting = true);
	CGameTask* GiveGameTaskToActor(CGameTask* t, u32 timeToComplete, bool bCheckExisting = true);
	void					SetTaskState(const TASK_ID& id, U16 objective_num, ETaskState state);
	void					SetTaskState(CGameTask* t, U16 objective_num, ETaskState state);

	void					UpdateTasks( );
//.	void					RemoveUserTask					(CMapLocation* ml);

	CGameTask* ActiveTask( );
	SGameTaskObjective* ActiveObjective( );
	void					SetActiveTask(const TASK_ID& id, U16 idx);
};
