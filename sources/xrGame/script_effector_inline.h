////////////////////////////////////////////////////////////////////////////
//	Module 		: script_effector_inline.h
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script effector class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CScriptEffector::CScriptEffector	(int iType, float time) : 
	CEffectorPostProcess(EEffectorPostProcessType(iType),time,false)
{
	m_tEffectorType		= EEffectorPostProcessType(iType);
}
