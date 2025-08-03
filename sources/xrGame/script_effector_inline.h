////////////////////////////////////////////////////////////////////////////
//	Module 		: script_effector_inline.h
//	Description : XRay Script effector class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CScriptEffector::CScriptEffector(s32 iType, f32 time) : CEffectorPP(EEffectorPPType(iType), time, false)
{
	m_tEffectorType = EEffectorPPType(iType);
}
