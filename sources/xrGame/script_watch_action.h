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
	fVector3						m_tWatchVector;
	shared_str						m_bone_to_watch;

	// Searchlight 
	fVector3						m_tTargetPoint;
	f32						vel_bone_x;
	f32						vel_bone_y;

public:
					CScriptWatchAction	();
	IC				CScriptWatchAction	(SightManager::ESightType tWatchType);
	IC				CScriptWatchAction	(SightManager::ESightType tWatchType, const fVector3& tDirection);
	IC				CScriptWatchAction	(SightManager::ESightType tWatchType, CScriptGameObject *tpObjectToWatch, pcstr bone_to_watch = "");
	// Searchlight look ///////////////////////////////////////////////
					CScriptWatchAction	(const fVector3& tTarget, f32 vel1, f32 vel2);
	IC				CScriptWatchAction	(CScriptGameObject *tpObjectToWatch, f32 vel1, f32 vel2);
	///////////////////////////////////////////////////////////////////
	virtual			~CScriptWatchAction	();
			void	SetWatchObject		(CScriptGameObject *tpObjectToWatch);
	IC		void	SetWatchType		(SightManager::ESightType tWatchType);
	IC		void	SetWatchDirection	(const fVector3& tDirection);
	IC		void	SetWatchBone		(pcstr bone_to_watch);
	IC		void	initialize			();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CScriptWatchAction)
#undef script_type_list
#define script_type_list save_type_list(CScriptWatchAction)

#include "script_watch_action_inline.h"