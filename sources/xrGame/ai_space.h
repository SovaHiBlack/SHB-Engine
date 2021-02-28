//	Module 		: ai_space.h
//	Description : AI space class

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

class CAI_Space {
private:
	friend class CALifeSimulator;
	friend class CALifeGraphRegistry;
	friend class CALifeSpawnRegistry;
	friend class CALifeSpawnRegistry;
	friend class CLevel;

private:
	CGameGraph							*m_game_graph;
	CGameLevelCrossTable				*m_cross_table;
	CLevelGraph							*m_level_graph;
	CGraphEngine						*m_graph_engine;
	CEF_Storage							*m_ef_storage;
	CALifeSimulator						*m_alife_simulator;
	CCoverManager						*m_cover_manager;
	CScriptEngine						*m_script_engine;
	CPatrolPathStorage					*m_patrol_path_storage;

private:
			void						load					(const char* level_name);
			void						unload					(bool reload = false);
			void						patrol_path_storage_raw	(IReader &stream);
			void						patrol_path_storage		(IReader &stream);
			void						set_alife				(CALifeSimulator *alife_simulator);

public:
										CAI_Space				();
	virtual								~CAI_Space				();
			void						init					();
			inline		CGameGraph					&game_graph				() const;
			inline		CGameGraph					*get_game_graph			() const;
			inline		CLevelGraph					&level_graph			() const;
			inline		const CLevelGraph			*get_level_graph		() const;
			inline		const CGameLevelCrossTable	&cross_table			() const;
			inline		const CGameLevelCrossTable	*get_cross_table		() const;
			inline		const CPatrolPathStorage	&patrol_paths			() const;
			inline		CEF_Storage					&ef_storage				() const;
			inline		CGraphEngine				&graph_engine			() const;
			inline		const CALifeSimulator		&alife					() const;
			inline		const CALifeSimulator		*get_alife				() const;
			inline		const CCoverManager			&cover_manager			() const;
			inline		CScriptEngine				&script_engine			() const;

#ifdef DEBUG
			void						validate				(const u32			level_id) const;
#endif
};

inline	CAI_Space	&ai	();

extern CAI_Space *g_ai_space;

#include "ai_space_inline.h"
