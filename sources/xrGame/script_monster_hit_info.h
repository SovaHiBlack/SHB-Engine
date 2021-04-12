#pragma once

#include "script_export_space.h"

class CScriptGameObject;

class CScriptMonsterHitInfo {
public:
	CScriptGameObject			*who;
	Fvector3					direction;
	int						time;

	CScriptMonsterHitInfo		()
	{
		who				= 0;
		time			= 0;
		direction		= Fvector3().set(0.f,0.f,1.f);
	}

	void set(CScriptGameObject *p_who, Fvector3 p_direction, int p_time) {
		who			= p_who;
		direction	= p_direction;
		time		= p_time;
	}
public:
	static void script_register(lua_State*);
};

add_to_type_list(CScriptMonsterHitInfo)
#undef script_type_list
#define script_type_list save_type_list(CScriptMonsterHitInfo)
