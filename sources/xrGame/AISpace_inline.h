////////////////////////////////////////////////////////////////////////////
//	Module 		: AISpace_inline.h
//	Created 	: 12.11.2003
//  Modified 	: 25.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI space class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CGameGraph& CAISpace::game_graph( ) const
{
	VERIFY(m_game_graph);
	return *m_game_graph;
}

IC CGameGraph* CAISpace::get_game_graph( ) const
{
	return m_game_graph;
}

IC CLevelGraph& CAISpace::level_graph( ) const
{
	VERIFY(m_level_graph);
	return *m_level_graph;
}

IC const CLevelGraph* CAISpace::get_level_graph( ) const
{
	return m_level_graph;
}

IC const CGameLevelCrossTable& CAISpace::cross_table( ) const
{
	VERIFY(m_cross_table);
	return *m_cross_table;
}

IC const CGameLevelCrossTable* CAISpace::get_cross_table( ) const
{
	return m_cross_table;
}

IC CEF_Storage& CAISpace::ef_storage( ) const
{
	VERIFY(m_ef_storage);
	return *m_ef_storage;
}

IC CGraphEngine& CAISpace::graph_engine( ) const
{
	VERIFY(m_graph_engine);
	return *m_graph_engine;
}

IC const CALifeSimulator& CAISpace::alife( ) const
{
	VERIFY(m_alife_simulator);
	return *m_alife_simulator;
}

IC const CALifeSimulator* CAISpace::get_alife( ) const
{
	return m_alife_simulator;
}

IC const CCoverManager& CAISpace::cover_manager( ) const
{
	VERIFY(m_cover_manager);
	return *m_cover_manager;
}

IC CScriptEngine& CAISpace::script_engine( ) const
{
	VERIFY(m_script_engine);
	return *m_script_engine;
}

IC const CPatrolPathStorage& CAISpace::patrol_paths( ) const
{
	VERIFY(m_patrol_path_storage);
	return *m_patrol_path_storage;
}

IC CAISpace& ai( )
{
	if (!g_ai_space)
	{
		g_ai_space = xr_new<CAISpace>( );
		g_ai_space->init( );
	}

	return *g_ai_space;
}
