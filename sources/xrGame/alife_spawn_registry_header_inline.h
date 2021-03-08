////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_spawn_registry_header_inline.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife spawn registry header inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	u32	CALifeSpawnHeader::version				() const
{
	return			(m_version);
}

inline	const xrGUID &CALifeSpawnHeader::guid		() const
{
	return			(m_guid);
}

inline	const xrGUID &CALifeSpawnHeader::graph_guid	() const
{
	return			(m_graph_guid);
}

inline	u32	CALifeSpawnHeader::count				() const
{
	return			(m_count);
}

inline	u32	CALifeSpawnHeader::level_count			() const
{
	return			(m_level_count);
}
