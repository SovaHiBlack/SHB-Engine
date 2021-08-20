////////////////////////////////////////////////////////////////////////////
//	Module 		: script_effector.h
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script effector class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\ENGINE\EffectorPostProcess.h"
#include "script_export_space.h"
#include "..\ENGINE\CameraManager.h"

class CScriptEffector : public CEffectorPostProcess
{
public:
	typedef CEffectorPostProcess inherited;
	EEffectorPostProcessType		m_tEffectorType;

	inline					CScriptEffector		(int iType, float time);
	virtual				~CScriptEffector	();
	virtual	BOOL		Process				(SPostProcessInfo& pp);
	virtual	bool		process				(SPostProcessInfo* pp);
	virtual	void		Add					();
	virtual	void		Remove				();
public:
	static void script_register(lua_State*);
};

add_to_type_list(CScriptEffector)
#undef script_type_list
#define script_type_list save_type_list(CScriptEffector)

#include "script_effector_inline.h"
