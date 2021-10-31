////////////////////////////////////////////////////////////////////////////
//	Module 		: script_movement_action.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script movement action class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ScriptAbstractAction.h"
#include "script_export_space.h"

namespace DetailPathManager {
	enum EDetailPathType;
};

namespace MonsterSpace {
	enum EBodyState;
	enum EMovementType;
	enum EScriptMonsterMoveAction;
	enum EScriptMonsterSpeedParam;
};

namespace PatrolPathManager {
	enum EPatrolStartType;
	enum EPatrolRouteType;
};

class CPatrolPath;
class CScriptGameObject;
class CPatrolPathParams;

class CScriptMovementAction : public CScriptAbstractAction {
public:
	enum EGoalType {
		eGoalTypeObject = u32(0),
		eGoalTypePatrolPath,
		eGoalTypePathPosition,
		eGoalTypeNoPathPosition,
		eGoalTypePathNodePosition,
		eGoalTypeInput,
		eGoalTypeDummy = u32(-1),
	};

	enum EInputKeys {
		eInputKeyNone		= u32(1) << 0,
		eInputKeyForward	= u32(1) << 1,
		eInputKeyBack		= u32(1) << 2,
		eInputKeyLeft		= u32(1) << 3,
		eInputKeyRight		= u32(1) << 4,
		eInputKeyShiftUp	= u32(1) << 5,
		eInputKeyShiftDown	= u32(1) << 6,
		eInputKeyBreaks		= u32(1) << 7,
		eInputKeyEngineOn	= u32(1) << 8,
		eInputKeyEngineOff	= u32(1) << 9,
		eInputKeyDummy		= u32(1) << 10,
	};

public:
	CSharedString									m_path_name;
	MonsterSpace::EBodyState					m_tBodyState;
	MonsterSpace::EMovementType					m_tMovementType;
	DetailPathManager::EDetailPathType			m_tPathType;
	CObject										*m_tpObjectToGo;
	const CPatrolPath							*m_path;
	PatrolPathManager::EPatrolStartType			m_tPatrolPathStart;
	PatrolPathManager::EPatrolRouteType			m_tPatrolPathStop;
	Fvector3										m_tDestinationPosition;
	u32											m_tNodeID;
	EGoalType									m_tGoalType;
	float										m_fSpeed;
	bool										m_bRandom;
	EInputKeys									m_tInputKeys;
	MonsterSpace::EScriptMonsterMoveAction		m_tMoveAction;
	MonsterSpace::EScriptMonsterSpeedParam		m_tSpeedParam;
	u32											m_previous_patrol_point;
	float										m_fDistToEnd;


public:
					CScriptMovementAction	();
	inline				CScriptMovementAction	(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, DetailPathManager::EDetailPathType tPathType, CScriptGameObject *tpObjectToGo, float fSpeed = 0.f);
					CScriptMovementAction	(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, DetailPathManager::EDetailPathType tPathType, const CPatrolPathParams &tPatrolPathParams, float fSpeed = 0.f);
	inline				CScriptMovementAction	(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, DetailPathManager::EDetailPathType tPathType, const Fvector3& tPosition, float fSpeed = 0.f);
					CScriptMovementAction	(const Fvector3& tPosition, float fSpeed);
	inline				CScriptMovementAction	(const EInputKeys tInputKeys, float fSpeed = 0.f);
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------						
	// Monsters
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
					CScriptMovementAction	(MonsterSpace::EScriptMonsterMoveAction tAct, Fvector3& tPosition, float	dist_to_end						= -1.f);
					CScriptMovementAction	(MonsterSpace::EScriptMonsterMoveAction tAct, CPatrolPathParams &tPatrolPathParams, float dist_to_end	= -1.f);
					CScriptMovementAction	(MonsterSpace::EScriptMonsterMoveAction tAct, CScriptGameObject *tpObjectToGo, float dist_to_end		= -1.f);
					CScriptMovementAction	(MonsterSpace::EScriptMonsterMoveAction tAct, u32 node_id, Fvector3& tPosition, float dist_to_end		= -1.f);
	inline				CScriptMovementAction	(MonsterSpace::EScriptMonsterMoveAction tAct, Fvector3& tPosition, float	dist_to_end,							MonsterSpace::EScriptMonsterSpeedParam speed_param);
					CScriptMovementAction	(MonsterSpace::EScriptMonsterMoveAction tAct, CPatrolPathParams &tPatrolPathParams, float dist_to_end,			MonsterSpace::EScriptMonsterSpeedParam speed_param);
	inline				CScriptMovementAction	(MonsterSpace::EScriptMonsterMoveAction tAct, CScriptGameObject *tpObjectToGo, float dist_to_end,				MonsterSpace::EScriptMonsterSpeedParam speed_param);
	virtual			~CScriptMovementAction	();
	inline		void	SetBodyState			(const MonsterSpace::EBodyState tBodyState);
	inline		void	SetMovementType			(const MonsterSpace::EMovementType tMovementType);
	inline		void	SetPathType				(const DetailPathManager::EDetailPathType tPathType);
			void	SetObjectToGo			(CScriptGameObject *tpObjectToGo);
	inline		void	SetPatrolPath			(const CPatrolPath *path, CSharedString path_name);
	inline		void	SetPosition				(const Fvector3& tPosition);
	inline		void	SetSpeed				(float fSpeed);
	inline		void	SetPatrolStart			(PatrolPathManager::EPatrolStartType tPatrolPathStart);
	inline		void	SetPatrolStop			(PatrolPathManager::EPatrolRouteType tPatrolPathStop);
	inline		void	SetPatrolRandom			(bool bRandom);
	inline		void	SetInputKeys			(const EInputKeys tInputKeys);
	inline		void	initialize				();

public:
	static void script_register(lua_State*);
};

add_to_type_list(CScriptMovementAction)
#undef script_type_list
#define script_type_list save_type_list(CScriptMovementAction)

#include "script_movement_action_inline.h"