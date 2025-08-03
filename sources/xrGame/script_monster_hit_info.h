#pragma once

#include "script_export_space.h"

class CScriptGameObject;

class CScriptMonsterHitInfo {
public:
	CScriptGameObject			*who;
	fVector3					direction;
	s32						time;

	CScriptMonsterHitInfo		()
	{
		who				= 0;
		time			= 0;
		direction		= fVector3().set(0.0f,0.0f,1.0f);
	}

	void set(CScriptGameObject *p_who, fVector3 p_direction, s32 p_time) {
		who			= p_who;
		direction	= p_direction;
		time		= p_time;
	}
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CScriptMonsterHitInfo)
#undef script_type_list
#define script_type_list save_type_list(CScriptMonsterHitInfo)
