//	Module 		: ScriptActionCondition.h
//	Description : Script action condition class

#pragma once

#include "script_export_space.h"
#include "ALife_space.h"

class CScriptActionCondition
{
public:
	enum EActionFlags
	{
		MOVEMENT_FLAG								= unsigned int(1 << 0),
		WATCH_FLAG									= unsigned int(1 << 1),
		ANIMATION_FLAG								= unsigned int(1 << 2),
		SOUND_FLAG									= unsigned int(1 << 3),
		PARTICLE_FLAG								= unsigned int(1 << 4),
		OBJECT_FLAG									= unsigned int(1 << 5),
		TIME_FLAG									= unsigned int(1 << 6),
		ACT_FLAG									= unsigned int(1 << 7)
	};

	unsigned int									m_dwFlags;
	ALife::_TIME_ID									m_tLifeTime;
	ALife::_TIME_ID									m_tStartTime;

	inline				CScriptActionCondition		( );
	inline				CScriptActionCondition		(unsigned int dwFlags, double dTime = -1);
	virtual				~CScriptActionCondition		( );
	inline void			initialize					( );

	static void			script_register				(lua_State* L);
};

add_to_type_list(CScriptActionCondition)
#undef script_type_list
#define script_type_list save_type_list(CScriptActionCondition)

#include "ScriptActionCondition_inline.h"
