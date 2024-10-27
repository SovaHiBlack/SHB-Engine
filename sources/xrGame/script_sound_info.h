#pragma once

#include "script_export_space.h"

class CScriptGameObject;

class CScriptSoundInfo {
public:
	CScriptGameObject			*who;
	fVector3					position;
	f32					power;
	int						time;		
	int						dangerous;

	CScriptSoundInfo				()
	{
		who				= 0;
		time			= 0;
		dangerous		= 0;
		power			= 0.0f;
		position		= fVector3().set(0.0f,0.0f,0.0f);
	}

	void set(CScriptGameObject *p_who, bool p_danger, fVector3 p_position, f32 p_power, int p_time) {
		who			= p_who;
		position	= p_position;
		power		= p_power;
		time		= p_time;
		dangerous	= int(p_danger);
	}
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CScriptSoundInfo)
#undef script_type_list
#define script_type_list save_type_list(CScriptSoundInfo)
