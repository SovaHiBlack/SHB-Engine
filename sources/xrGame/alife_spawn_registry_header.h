////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_spawn_registry_header.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife spawn registry header
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\ENGINE\xrLevel.h"

class CALifeSpawnHeader {
protected:
	u32						m_version;
	xrGUID					m_guid;
	xrGUID					m_graph_guid;
	u32						m_count;
	u32						m_level_count;

public:
	virtual					~CALifeSpawnHeader	();
	virtual void			load				(IReader &file_stream);
	inline		u32				version				() const;
	inline		const xrGUID	&guid				() const;
	inline		const xrGUID	&graph_guid			() const;
	inline		u32				count				() const;
	inline		u32				level_count			() const;
};

#include "alife_spawn_registry_header_inline.h"
