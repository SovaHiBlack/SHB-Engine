////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_action_inline.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script sound action class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CScriptSoundAction::CScriptSoundAction	()
{
	m_caSoundToPlay		= "";
	m_caBoneName		= "";
	m_tGoalType			= eGoalTypeDummy;
	m_bStartedToPlay	= false;
	m_bLooped			= false;
	m_tSoundPosition.set(0,0,0);
	m_tSoundAngles.set	(0,0,0);
	m_bCompleted		= true;
	m_monster_sound		= MonsterSound::eMonsterSoundDummy;
	m_tHeadAnimType		= MonsterSpace::eHeadAnimNone;
}

IC	CScriptSoundAction::CScriptSoundAction		(pcstr caSoundToPlay, pcstr caBoneName, const fVector3& tPositionOffset, const fVector3& tAngleOffset, bool bLooped, ESoundTypes sound_type)
{
	m_bLooped			= bLooped;
	SetBone				(caBoneName);
	SetPosition			(tPositionOffset);
	SetAngles			(tAngleOffset);
	SetSound			(caSoundToPlay);
	SetSoundType		(sound_type);
	m_monster_sound		= MonsterSound::eMonsterSoundDummy;
	m_tHeadAnimType		= MonsterSpace::eHeadAnimNone;
}

IC	CScriptSoundAction::CScriptSoundAction		(pcstr caSoundToPlay, const fVector3& tPosition, const fVector3& tAngleOffset, bool bLooped, ESoundTypes sound_type)
{
	m_bLooped			= bLooped;
	SetSound			(caSoundToPlay);
	SetPosition			(tPosition);
	SetAngles			(tAngleOffset);
	SetSoundType		(sound_type);
	m_monster_sound		= MonsterSound::eMonsterSoundDummy;
	m_tHeadAnimType		= MonsterSpace::eHeadAnimNone;
}

IC	CScriptSoundAction::CScriptSoundAction		(CScriptSound &sound, pcstr caBoneName, const fVector3& tPositionOffset, const fVector3& tAngleOffset, bool bLooped, ESoundTypes sound_type)
{
	m_bLooped			= bLooped;
	SetBone				(caBoneName);
	SetPosition			(tPositionOffset);
	SetAngles			(tAngleOffset);
	SetSound			(sound);
	SetSoundType		(sound_type);
	m_monster_sound		= MonsterSound::eMonsterSoundDummy;
	m_tHeadAnimType		= MonsterSpace::eHeadAnimNone;
}

IC	CScriptSoundAction::CScriptSoundAction		(CScriptSound &sound, const fVector3& tPosition, const fVector3& tAngleOffset, bool bLooped, ESoundTypes sound_type)
{
	m_bLooped			= bLooped;
	SetSound			(sound);
	SetPosition			(tPosition);
	SetAngles			(tAngleOffset);
	SetSoundType		(sound_type);
	m_monster_sound		= MonsterSound::eMonsterSoundDummy;
	m_tHeadAnimType		= MonsterSpace::eHeadAnimNone;
}

////////////////////////////////////////////////////////////////////////////////////
// Monster Specific
///////////////////////////////////////////////////////////////////////////////////
						
IC	CScriptSoundAction::CScriptSoundAction		(MonsterSound::EType sound_type)
{
	m_monster_sound			= sound_type;
	m_monster_sound_delay	= s32(-1);
	m_bCompleted			= false;
}

IC	CScriptSoundAction::CScriptSoundAction		(MonsterSound::EType sound_type, s32 delay)
{
	m_monster_sound			= sound_type;
	m_monster_sound_delay	= delay;
	m_bCompleted			= false;
}

////////////////////////////////////////////////////////////////////////////////////
// Trader Specific
///////////////////////////////////////////////////////////////////////////////////

IC	CScriptSoundAction::CScriptSoundAction		(pcstr caSoundToPlay, pcstr caBoneName, MonsterSpace::EMonsterHeadAnimType head_anim_type)
{
	SetBone				(caBoneName);
	SetSound			(caSoundToPlay);		
	m_tSoundPosition.set(0,0,0);
	m_tSoundAngles.set	(0,0,0);
	m_bCompleted		= false;
	m_bLooped			= false;
	m_tHeadAnimType		= head_anim_type;
	m_monster_sound		= MonsterSound::eMonsterSoundDummy;
}

///////////////////////////////////////////////////////////////////////////////////

IC	void CScriptSoundAction::SetSound			(const CScriptSound &sound)
{
	m_caSoundToPlay		= sound.m_caSoundToPlay;
	m_tGoalType			= eGoalTypeSoundAttached;
	m_bStartedToPlay	= false;
	m_bCompleted		= false;
}

IC	void CScriptSoundAction::SetPosition		(const fVector3& tPosition)
{
	m_tSoundPosition	= tPosition;
	m_tGoalType			= eGoalTypeSoundPosition;
	m_bStartedToPlay	= false;
}

IC	void CScriptSoundAction::SetBone			(pcstr caBoneName)
{
	m_caBoneName		= caBoneName;
	m_bStartedToPlay	= false;
}

IC	void CScriptSoundAction::SetAngles			(const fVector3& tAngles)
{
	m_tSoundAngles		= tAngles;
	m_bStartedToPlay	= false;
}

IC	void CScriptSoundAction::SetSoundType		(const ESoundTypes sound_type)
{
	m_sound_type		= sound_type;
	m_bStartedToPlay	= false;
}

IC	void CScriptSoundAction::initialize			()
{
	m_bStartedToPlay	= false;
}
