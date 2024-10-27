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
	fVector3								m_tSoundPosition;
	fVector3								m_tSoundAngles;
	ESoundTypes							m_sound_type;
	MonsterSound::EType					m_monster_sound;
	int									m_monster_sound_delay;
	MonsterSpace::EMonsterHeadAnimType	m_tHeadAnimType;

public:
	IC				CScriptSoundAction	();
	IC				CScriptSoundAction	(pcstr caSoundToPlay, pcstr caBoneName, const fVector3& tPositionOffset = fVector3().set(0.0f, 0.0f, 0.0f), const fVector3& tAngleOffset = fVector3().set(0.0f, 0.0f, 0.0f), bool bLooped = false, ESoundTypes sound_type = SOUND_TYPE_NO_SOUND);
	IC				CScriptSoundAction	(pcstr caSoundToPlay, const fVector3& tPosition, const fVector3& tAngleOffset = fVector3().set(0.0f, 0.0f, 0.0f), bool bLooped = false, ESoundTypes sound_type = SOUND_TYPE_NO_SOUND);
	IC				CScriptSoundAction	(CScriptSound &sound, pcstr caBoneName, const fVector3& tPositionOffset = fVector3().set(0.0f, 0.0f, 0.0f), const fVector3& tAngleOffset = fVector3().set(0.0f, 0.0f, 0.0f), bool bLooped = false, ESoundTypes sound_type = SOUND_TYPE_NO_SOUND);
	IC				CScriptSoundAction	(CScriptSound &sound, const fVector3& tPosition, const fVector3& tAngleOffset = fVector3().set(0.0f, 0.0f, 0.0f), bool bLooped = false, ESoundTypes sound_type = SOUND_TYPE_NO_SOUND);
	////////////////////////////////////////////////////////////////////////////////////
	// Monster Specific
	///////////////////////////////////////////////////////////////////////////////////
	IC				CScriptSoundAction	(MonsterSound::EType sound_type);
	IC				CScriptSoundAction	(MonsterSound::EType sound_type, int delay);
	////////////////////////////////////////////////////////////////////////////////////
	// Trader Specific
	///////////////////////////////////////////////////////////////////////////////////
	IC				CScriptSoundAction	(pcstr caSoundToPlay, pcstr caBoneName, MonsterSpace::EMonsterHeadAnimType head_anim_type);
	virtual			~CScriptSoundAction	();
			void	SetSound			(pcstr caSoundToPlay);
	IC		void	SetSound			(const CScriptSound &sound);
	IC		void	SetPosition			(const fVector3& tPosition);
	IC		void	SetBone				(pcstr caBoneName);
	IC		void	SetAngles			(const fVector3& tAngles);
	IC		void	SetSoundType		(const ESoundTypes sound_type);
	IC		void	initialize			();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CScriptSoundAction)
#undef script_type_list
#define script_type_list save_type_list(CScriptSoundAction)

#include "script_sound_action_inline.h"