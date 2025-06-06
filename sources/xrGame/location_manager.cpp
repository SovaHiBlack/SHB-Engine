////////////////////////////////////////////////////////////////////////////
//	Module 		: location_manager.cpp
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Location manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "location_manager.h"
#include "GameObject.h"

extern void	setup_location_types(GameGraph::TERRAIN_VECTOR& m_vertex_types, CIniFile* ini, pcstr string);

CLocationManager::~CLocationManager( )
{ }

void CLocationManager::Load(pcstr section)
{
	if (pSettings->line_exist(section, "terrain"))
	{
		setup_location_types(m_vertex_types, pSettings, pSettings->r_string(section, "terrain"));
	}
	else
	{
		setup_location_types(m_vertex_types, pSettings, section);
	}
}

void CLocationManager::reload(pcstr section)
{
	if (!m_object->spawn_ini( ) || !m_object->spawn_ini( )->section_exist("alife") || !m_object->spawn_ini( )->line_exist("alife", "terrain"))
		return;
	setup_location_types(m_vertex_types, m_object->spawn_ini( ), m_object->spawn_ini( )->r_string("alife", "terrain"));
}
