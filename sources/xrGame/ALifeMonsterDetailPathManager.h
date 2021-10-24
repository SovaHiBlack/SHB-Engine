//	Module 		: ALifeMonsterDetailPathManager.h
//	Description : ALife monster detail path manager class

#pragma once

#include "game_graph_space.h"
#include "ALife_space.h"
#include "script_export_space.h"

class CSE_ALifeMonsterAbstract;
class CALifeSmartTerrainTask;

class CALifeMonsterDetailPathManager
{
public:
	using object_type = CSE_ALifeMonsterAbstract;
	using PATH = xr_vector<u32>;

private:
	struct parameters
	{
		GameGraph::_GRAPH_ID			m_game_vertex_id;
		u32								m_level_vertex_id;
		Fvector3							m_position;
	};

	object_type* m_object;
	ALife::_TIME_ID						m_last_update_time;
	parameters							m_destination;
	float								m_walked_distance;
	float								m_speed;

	PATH								m_path;
	// this is INVERTED path, i.e. 
	// start vertex is the last one
	// destination vertex is the first one.
	// this is useful, since iterating back
	// on this vector during path following
	// we just repeatedly remove the last 
	// vertex, and this operation is 
	// efficiently implemented in std::vector

	void		actualize( );
	void		setup_current_speed( );
	void		follow_path(const ALife::_TIME_ID& time_delta);
	void		update(const ALife::_TIME_ID& time_delta);

public:
	CALifeMonsterDetailPathManager(object_type* object);
	inline		object_type& object( ) const;

	void		target(const GameGraph::_GRAPH_ID& game_vertex_id, const u32& level_vertex_id, const Fvector3& position);
	void		target(const GameGraph::_GRAPH_ID& game_vertex_id);
	void		target(const CALifeSmartTerrainTask& task);
	void		target(const CALifeSmartTerrainTask* task);

	void		update( );
	void		on_switch_online( );
	void		on_switch_offline( );
	inline void		speed(const float& speed);

	inline const float& speed( ) const;
	bool		completed( ) const;
	bool		actual( ) const;
	bool		failed( ) const;
	inline const PATH& path( ) const;
	inline const float& walked_distance( ) const;
	Fvector3		draw_level_position( ) const;

	static void script_register(lua_State*);
};

add_to_type_list(CALifeMonsterDetailPathManager)
#undef script_type_list
#define script_type_list save_type_list(CALifeMonsterDetailPathManager)

#include "ALifeMonsterDetailPathManager_inline.h"
