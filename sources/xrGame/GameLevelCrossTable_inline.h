//	Module 		: GameLevelCrossTable_inline.h
//	Description : Cross table between game and level graphs inline functions

#pragma once

inline CGameLevelCrossTable::CGameLevelCrossTable	()
{
	string_path			fName;
	FS.update_path		(fName,"$level$",CROSS_TABLE_NAME);

	m_tpCrossTableVFS	= FS.r_open(fName);
	R_ASSERT2			(m_tpCrossTableVFS,"Can't open cross table!");
	
	IReader				*chunk = m_tpCrossTableVFS->open_chunk(CROSS_TABLE_CHUNK_VERSION);
	R_ASSERT2			(chunk,"Cross table is corrupted!");
	chunk->r			(&m_tCrossTableHeader,sizeof(m_tCrossTableHeader));
	chunk->close		();
	
	R_ASSERT2			(m_tCrossTableHeader.version() == XRAI_CURRENT_VERSION,"Cross table version mismatch!");

	m_chunk				= m_tpCrossTableVFS->open_chunk(CROSS_TABLE_CHUNK_DATA);
	R_ASSERT2			(m_chunk,"Cross table is corrupted!");
	m_tpaCrossTable		= (CCell*)m_chunk->pointer();
};

inline CGameLevelCrossTable::~CGameLevelCrossTable	()
{
	VERIFY				(m_chunk);
	m_chunk->close		();
	
	VERIFY				(m_tpCrossTableVFS);
	FS.r_close			(m_tpCrossTableVFS);
};

inline const CGameLevelCrossTable::CCell &CGameLevelCrossTable::vertex(u32 level_vertex_id) const
{
	VERIFY				(level_vertex_id < header().level_vertex_count());
	return				(m_tpaCrossTable[level_vertex_id]);
}

inline	u32	CGameLevelCrossTable::CHeader::version() const
{
	return				(dwVersion);
}

inline	u32	CGameLevelCrossTable::CHeader::level_vertex_count() const
{
	return				(dwNodeCount);
}

inline	u32	CGameLevelCrossTable::CHeader::game_vertex_count() const
{
	return				(dwGraphPointCount);
}

inline	const xrGUID &CGameLevelCrossTable::CHeader::level_guid	() const
{
	return				(m_level_guid);
}

inline	const xrGUID &CGameLevelCrossTable::CHeader::game_guid	() const
{
	return				(m_game_guid);
}

inline	GameGraph::_GRAPH_ID CGameLevelCrossTable::CCell::game_vertex_id() const
{
	return				(tGraphIndex);
}

inline	float CGameLevelCrossTable::CCell::distance() const
{
	return				(fDistance);
}

inline	const CGameLevelCrossTable::CHeader &CGameLevelCrossTable::header() const
{
	return				(m_tCrossTableHeader);
}
