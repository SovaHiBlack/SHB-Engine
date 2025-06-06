////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_action.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script sound action class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_sound_action.h"
#include "AISpace.h"
#include "script_engine.h"

CScriptSoundAction::~CScriptSoundAction	()
{ }

void CScriptSoundAction::SetSound			(pcstr caSoundToPlay)
{
	m_caSoundToPlay		= caSoundToPlay;
	m_tGoalType			= eGoalTypeSoundAttached;
	m_bStartedToPlay	= false;
	string_path			l_caFileName;
	if (FS.exist(l_caFileName,"$game_sounds$",*m_caSoundToPlay,".ogg")) {
		m_bStartedToPlay= false;
		m_bCompleted	= false;
	}
	else {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"File not found \"%s\"!",l_caFileName);
		m_bStartedToPlay= true;
		m_bCompleted	= true;
	}
}
