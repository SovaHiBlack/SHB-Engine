////////////////////////////////////////////////////////////////////////////
//	Module 		: AISpace.h
//	Created 	: 12.11.2003
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI space class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CGameGraph;
class CGameLevelCrossTable;
class CLevelGraph;
class CGraphEngine;
class CEF_Storage;
class CALifeSimulator;
class CCoverManager;
class CScriptEngine;
class CPatrolPathStorage;

class CAISpace
{
private:
	friend class CALifeSimulator;
	friend class CALifeGraphRegistry;
	friend class CALifeSpawnRegistry;
	friend class CALifeSpawnRegistry;
	friend class CLevel;

private:
	CGameGraph* m_game_graph;
	CGameLevelCrossTable* m_cross_table;
	CLevelGraph* m_level_graph;
	CGraphEngine* m_graph_engine;
	CEF_Storage* m_ef_storage;
	CALifeSimulator* m_alife_simulator;
	CCoverManager* m_cover_manager;
	CScriptEngine* m_script_engine;
	CPatrolPathStorage* m_patrol_path_storage;

private:
	void						load(pcstr level_name);
	void						unload(bool reload = false);
	void						patrol_path_storage_raw(IReader& stream);
	void						patrol_path_storage(IReader& stream);
	void						set_alife(CALifeSimulator* alife_simulator);

public:
	CAISpace( );
	virtual								~CAISpace( );
	void						init( );
	IC		CGameGraph& game_graph( ) const;
	IC		CGameGraph* get_game_graph( ) const;
	IC		CLevelGraph& level_graph( ) const;
	IC		const CLevelGraph* get_level_graph( ) const;
	IC		const CGameLevelCrossTable& cross_table( ) const;
	IC		const CGameLevelCrossTable* get_cross_table( ) const;
	IC		const CPatrolPathStorage& patrol_paths( ) const;
	IC		CEF_Storage& ef_storage( ) const;
	IC		CGraphEngine& graph_engine( ) const;
	IC		const CALifeSimulator& alife( ) const;
	IC		const CALifeSimulator* get_alife( ) const;
	IC		const CCoverManager& cover_manager( ) const;
	IC		CScriptEngine& script_engine( ) const;

#ifdef DEBUG
	void						validate(const u32 level_id) const;
#endif // def DEBUG

};

IC CAISpace& ai( );

extern CAISpace* g_ai_space;

#include "AISpace_inline.h"
