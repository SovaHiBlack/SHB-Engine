//	Module 		: ALifeMonsterPatrolPathManager.h
//	Description : ALife monster patrol path manager class

#pragma once

#include "game_graph_space.h"
#include "script_export_space.h"

class CSE_ALifeMonsterAbstract;
class CPatrolPath;

namespace PatrolPathManager
{
	enum EPatrolStartType;
	enum EPatrolRouteType;
};

class CALifeMonsterPatrolPathManager
{
public:
	using object_type = CSE_ALifeMonsterAbstract;
	using EPatrolStartType = PatrolPathManager::EPatrolStartType;
	using EPatrolRouteType = PatrolPathManager::EPatrolRouteType;
	using _GRAPH_ID = GameGraph::_GRAPH_ID;

private:
	object_type* m_object;

	const CPatrolPath* m_path;
	EPatrolStartType				m_start_type;
	EPatrolRouteType				m_route_type;
	bool							m_use_randomness;
	unsigned int					m_start_vertex_index;

	bool							m_actual;
	bool							m_completed;
	unsigned int					m_current_vertex_index;
	unsigned int					m_previous_vertex_index;

	void					select_nearest( );
	void					actualize( );
	bool					location_reached( ) const;
	void					navigate( );

public:
	CALifeMonsterPatrolPathManager(object_type* object);
	void					update( );
	void					path(const CSharedString& path_name);

	inline object_type& object( ) const;
	inline void					path(const CPatrolPath* path);
	inline void					path(const char* path_name);
	inline void					start_type(const EPatrolStartType& start_type);
	inline void					route_type(const EPatrolRouteType& route_type);
	inline const EPatrolStartType& start_type( ) const;
	inline const EPatrolRouteType& route_type( ) const;
	inline bool					actual( ) const;
	inline bool					completed( ) const;
	inline const CPatrolPath& path( ) const;
	inline void					start_vertex_index(const unsigned int& start_vertex_index);
	inline bool					use_randomness( ) const;
	inline void					use_randomness(const bool& use_randomness);
	const _GRAPH_ID& target_game_vertex_id( ) const;
	const unsigned int& target_level_vertex_id( ) const;
	const Fvector3& target_position( ) const;

	static void script_register(lua_State*);
};

add_to_type_list(CALifeMonsterPatrolPathManager)
#undef script_type_list
#define script_type_list save_type_list(CALifeMonsterPatrolPathManager)

#include "ALifeMonsterPatrolPathManager_inline.h"
