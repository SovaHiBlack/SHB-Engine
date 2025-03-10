////////////////////////////////////////////////////////////////////////////
//	Module 		: script_watch_action_inline.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script watch action class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CScriptWatchAction::CScriptWatchAction		(SightManager::ESightType tWatchType)
{
	SetWatchType		(tWatchType);
	m_tGoalType			= eGoalTypeWatchType;
}

IC	CScriptWatchAction::CScriptWatchAction		(SightManager::ESightType tWatchType, const fVector3& tDirection)
{
	SetWatchDirection	(tDirection);
	SetWatchType		(tWatchType);
}

IC	CScriptWatchAction::CScriptWatchAction		(SightManager::ESightType tWatchType, CScriptGameObject *tpObjectToWatch, pcstr bone_to_watch)
{
	SetWatchType		(tWatchType);
	SetWatchObject		(tpObjectToWatch);
	SetWatchBone		(bone_to_watch);
}

// Searchlight look ///////////////////////////////////////////////
IC	CScriptWatchAction::CScriptWatchAction		(const fVector3& tTarget, f32 vel1, f32 vel2)
{
	m_tpObjectToWatch	= 0;
	m_tTargetPoint		= tTarget;
	vel_bone_x			= vel1;
	vel_bone_y			= vel2;
	m_bCompleted		= false;
}

IC	CScriptWatchAction::CScriptWatchAction		(CScriptGameObject *tpObjectToWatch, f32 vel1, f32 vel2)
{
	SetWatchObject		(tpObjectToWatch);
	vel_bone_x			= vel1;
	vel_bone_y			= vel2;
	m_bCompleted		= false;
}

IC	void CScriptWatchAction::SetWatchType		(SightManager::ESightType tWatchType)
{
	m_tWatchType		= tWatchType;
	m_bCompleted		= false;
}

IC	void CScriptWatchAction::SetWatchDirection	(const fVector3& tDirection)
{
	m_tWatchVector		= tDirection;
	m_tGoalType			= eGoalTypeDirection;
	m_bCompleted		= false;
}

IC	void CScriptWatchAction::SetWatchBone		(pcstr bone_to_watch)
{
	m_bone_to_watch		= bone_to_watch;
	m_bCompleted		= false;
}

IC	void CScriptWatchAction::initialize			()
{ }
