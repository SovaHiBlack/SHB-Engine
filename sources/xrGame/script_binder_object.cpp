////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder_object.cpp
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object binder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_binder_object.h"
#include "script_game_object.h"

CScriptBinderObject::CScriptBinderObject(CScriptGameObject* object)
{
	m_object = object;
}

CScriptBinderObject::~CScriptBinderObject( )
{
#ifdef DESTROYING_BINDED_OBJECT_LOG
	if (m_object)
		Msg("Destroying binded object %s", m_object->Name( ));
#endif // DESTROYING_BINDED_OBJECT_LOG
}

void CScriptBinderObject::reinit( )
{ }

void CScriptBinderObject::reload(pcstr section)
{ }

bool CScriptBinderObject::net_Spawn(SpawnType DC)
{
	return			(true);
}

void CScriptBinderObject::net_Destroy( )
{ }

void CScriptBinderObject::net_Import(CNetPacket* net_packet)
{ }

void CScriptBinderObject::net_Export(CNetPacket* net_packet)
{ }

void CScriptBinderObject::shedule_Update(u32 time_delta)
{ }

void CScriptBinderObject::save(CNetPacket* output_packet)
{ }

void CScriptBinderObject::load(IReader* input_packet)
{ }

bool CScriptBinderObject::net_SaveRelevant( )
{
	return		(false);
}

void CScriptBinderObject::net_Relcase(CScriptGameObject* object)
{ }
