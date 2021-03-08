////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_monster_patrol_path_manager.h
//	Created 	: 01.11.2005
//  Modified 	: 22.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife monster patrol path manager class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "game_graph_space.h"
#include "script_export_space.h"

class CSE_ALifeMonsterAbstract;
class CPatrolPath;

namespace PatrolPathManager {
	enum EPatrolStartType;
	enum EPatrolRouteType;
};

class CALifeMonsterPatrolPathManager {
public:
	typedef CSE_ALifeMonsterAbstract							object_type;
	typedef PatrolPathManager::EPatrolStartType					EPatrolStartType;
	typedef PatrolPathManager::EPatrolRouteType					EPatrolRouteType;
	typedef GameGraph::_GRAPH_ID								_GRAPH_ID;

private:
	object_type						*m_object;

private:
	const CPatrolPath				*m_path;
	EPatrolStartType				m_start_type;
	EPatrolRouteType				m_route_type;
	bool							m_use_randomness;
	u32								m_start_vertex_index;

private:
	bool							m_actual;
	bool							m_completed;
	u32								m_current_vertex_index;
	u32								m_previous_vertex_index;

private:
			void					select_nearest				();
			void					actualize					();
			bool					location_reached			() const;
			void					navigate					();

public:
									CALifeMonsterPatrolPathManager(object_type *object);
			void					update						();
			void					path						(const shared_str &path_name);

public:
	inline		object_type				&object						() const;
	inline		void					path						(const CPatrolPath *path);
	inline		void					path						(const char* path_name);
	inline		void					start_type					(const EPatrolStartType	&start_type);
	inline		void					route_type					(const EPatrolRouteType	&route_type);
	inline		const EPatrolStartType	&start_type					() const;
	inline		const EPatrolRouteType	&route_type					() const;
	inline		bool					actual						() const;
	inline		bool					completed					() const;
	inline		const CPatrolPath		&path						() const;
	inline		void					start_vertex_index			(const u32 &start_vertex_index);
	inline		bool					use_randomness				() const;
	inline		void					use_randomness				(const bool &use_randomness);
			const _GRAPH_ID			&target_game_vertex_id		() const;
			const u32				&target_level_vertex_id		() const;
			const Fvector3&			target_position			() const;

public:
	static void script_register(lua_State*);
};

add_to_type_list(CALifeMonsterPatrolPathManager)
#undef script_type_list
#define script_type_list save_type_list(CALifeMonsterPatrolPathManager)

#include "alife_monster_patrol_path_manager_inline.h"
