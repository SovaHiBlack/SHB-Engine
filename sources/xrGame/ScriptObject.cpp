//	Module 		: ScriptObject.cpp
//	Description : Script object class

#include "stdafx.h"

#include "ScriptObject.h"

CScriptObject::CScriptObject( )
{ }

CScriptObject::~CScriptObject( )
{ }

DLL_Pure* CScriptObject::_construct( )
{
	CGameObject::_construct( );
	CScriptEntity::_construct( );
	return this;
}

void CScriptObject::reinit( )
{
	CScriptEntity::reinit( );
	CGameObject::reinit( );
}

BOOL CScriptObject::net_Spawn(CSE_Abstract* DC)
{
	return (CGameObject::net_Spawn(DC) && CScriptEntity::net_Spawn(DC));
}

void CScriptObject::net_Destroy( )
{
	CGameObject::net_Destroy( );
	CScriptEntity::net_Destroy( );
}

BOOL CScriptObject::UsedAI_Locations( )
{
	return FALSE;
}

void CScriptObject::shedule_Update(u32 DT)
{
	CGameObject::shedule_Update(DT);
	CScriptEntity::shedule_Update(DT);
}

void CScriptObject::UpdateCL( )
{
	CGameObject::UpdateCL( );
	CScriptEntity::UpdateCL( );
}
