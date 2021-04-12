////////////////////////////////////////////////////////////////////////////
//	Module 		: script_watch_action.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script watch action class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_abstract_action.h"
#include "script_export_space.h"

namespace SightManager {
	enum ESightType;
};

class CScriptGameObject;

class CScriptWatchAction : public CScriptAbstractAction {
public:
	enum EGoalType {
		eGoalTypeObject = u32(0),
		eGoalTypeWatchType,
		eGoalTypeDirection,
		eGoalTypeCurrent,
		eGoalTypeDummy = u32(-1),
	};

public:
	CObject						*m_tpObjectToWatch;
	SightManager::ESightType	m_tWatchType;
	EGoalType					m_tGoalType;
	Fvector3						m_tWatchVector;
	shared_str						m_bone_to_watch;

	// Searchlight 
	Fvector3						m_tTargetPoint;
	float						vel_bone_x;
	float						vel_bone_y;

public:
					CScriptWatchAction	();
	inline				CScriptWatchAction	(SightManager::ESightType tWatchType);
	inline				CScriptWatchAction	(SightManager::ESightType tWatchType, const Fvector3& tDirection);
	inline				CScriptWatchAction	(SightManager::ESightType tWatchType, CScriptGameObject *tpObjectToWatch, const char* bone_to_watch = "");
	// Searchlight look ///////////////////////////////////////////////
					CScriptWatchAction	(const Fvector3& tTarget, float vel1, float vel2);
	inline				CScriptWatchAction	(CScriptGameObject *tpObjectToWatch, float vel1, float vel2);
	///////////////////////////////////////////////////////////////////
	virtual			~CScriptWatchAction	();
			void	SetWatchObject		(CScriptGameObject *tpObjectToWatch);
	inline		void	SetWatchType		(SightManager::ESightType tWatchType);
	inline		void	SetWatchDirection	(const Fvector3& tDirection);
	inline		void	SetWatchBone		(const char* bone_to_watch);
	inline		void	initialize			();

public:
	static void script_register(lua_State*);
};

add_to_type_list(CScriptWatchAction)
#undef script_type_list
#define script_type_list save_type_list(CScriptWatchAction)

#include "script_watch_action_inline.h"
