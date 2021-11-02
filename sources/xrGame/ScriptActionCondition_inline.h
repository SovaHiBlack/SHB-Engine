//	Module 		: ScriptActionCondition_inline.h
//	Description : Script action condition class inline functions

#pragma once

inline CScriptActionCondition::CScriptActionCondition( )
{
	m_dwFlags = 0;
	m_tLifeTime = ALife::_TIME_ID(-1);
	m_tStartTime = ALife::_TIME_ID(-1);
}

inline CScriptActionCondition::CScriptActionCondition(unsigned int dwFlags, double dTime)
{
	m_dwFlags = dwFlags;
	m_tLifeTime = ALife::_TIME_ID(dTime);
	m_tStartTime = ALife::_TIME_ID(-1);
}

inline void CScriptActionCondition::initialize( )
{
	m_tStartTime = Device.dwTimeGlobal;
}
