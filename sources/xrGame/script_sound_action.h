////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_action.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script sound action class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_abstract_action.h"
#include "script_export_space.h"
#include "ai_sounds.h"
#include "ai_monster_space.h"
#include "script_sound.h"
#include "AI/monsters/monster_sound_defs.h"

class CScriptSoundAction : public CScriptAbstractAction {
public:
	enum EGoalType {
		eGoalTypeSoundAttached = u32(0),
		eGoalTypeSoundPosition,
		eGoalTypeDummy = u32(-1),
	};

public:
	shared_str								m_caSoundToPlay;
	shared_str								m_caBoneName;
	EGoalType							m_tGoalType;
	bool								m_bLooped;
	bool								m_bStartedToPlay;
	Fvector								m_tSoundPosition;
	Fvector								m_tSoundAngles;
	ESoundTypes							m_sound_type;
	MonsterSound::EType					m_monster_sound;
	int									m_monster_sound_delay;
	MonsterSpace::EMonsterHeadAnimType	m_tHeadAnimType;

public:
	inline				CScriptSoundAction	();
	inline				CScriptSoundAction	(const char* caSoundToPlay, const char* caBoneName, const Fvector &tPositionOffset = Fvector().set(0,0,0), const Fvector &tAngleOffset = Fvector().set(0,0,0), bool bLooped = false, ESoundTypes sound_type = SOUND_TYPE_NO_SOUND);
	inline				CScriptSoundAction	(const char* caSoundToPlay, const Fvector &tPosition, const Fvector &tAngleOffset = Fvector().set(0,0,0), bool bLooped = false, ESoundTypes sound_type = SOUND_TYPE_NO_SOUND);
	inline				CScriptSoundAction	(CScriptSound &sound, const char* caBoneName, const Fvector &tPositionOffset = Fvector().set(0,0,0), const Fvector &tAngleOffset = Fvector().set(0,0,0), bool bLooped = false, ESoundTypes sound_type = SOUND_TYPE_NO_SOUND);
	inline				CScriptSoundAction	(CScriptSound &sound, const Fvector &tPosition, const Fvector &tAngleOffset = Fvector().set(0,0,0), bool bLooped = false, ESoundTypes sound_type = SOUND_TYPE_NO_SOUND);
	////////////////////////////////////////////////////////////////////////////////////
	// Monster Specific
	///////////////////////////////////////////////////////////////////////////////////
	inline				CScriptSoundAction	(MonsterSound::EType sound_type);
	inline				CScriptSoundAction	(MonsterSound::EType sound_type, int delay);
	////////////////////////////////////////////////////////////////////////////////////
	// Trader Specific
	///////////////////////////////////////////////////////////////////////////////////
	inline				CScriptSoundAction	(const char* caSoundToPlay, const char* caBoneName, MonsterSpace::EMonsterHeadAnimType head_anim_type);
	virtual			~CScriptSoundAction	();
			void	SetSound			(const char* caSoundToPlay);
	inline		void	SetSound			(const CScriptSound &sound);
	inline		void	SetPosition			(const Fvector &tPosition);
	inline		void	SetBone				(const char* caBoneName);
	inline		void	SetAngles			(const Fvector &tAngles);
	inline		void	SetSoundType		(const ESoundTypes sound_type);
	inline		void	initialize			();

public:
	static void script_register(lua_State*);
};

add_to_type_list(CScriptSoundAction)
#undef script_type_list
#define script_type_list save_type_list(CScriptSoundAction)

#include "script_sound_action_inline.h"