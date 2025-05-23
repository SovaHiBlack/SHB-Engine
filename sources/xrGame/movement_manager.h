////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_monster_space.h"
#include "graph_engine_space.h"
#include "game_graph_space.h"

namespace MovementManager {
	enum EPathType;
};

namespace DetailPathManager {
	enum EDetailPathType;
};

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
class CBaseLocationSelector;

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type,
	typename _index_type
>
class CBasePathManager;

template <
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>
struct SVertexType;

template <
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>
struct SBaseParameters;

template <
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>
struct SGameVertex;

class CEnemyLocationPredictor;
class CPatrolPathManager;
class CDetailPathManager;
class CPHMovementControl;
class CGameGraph;
class CLevelGraph;
class CRestrictedObject;
class CLocationManager;
class CCustomMonster;

namespace DetailPathManager {
	struct STravelPathPoint;
};

class CLevelPathBuilder;
class CDetailPathBuilder;

class CMovementManager {
private:
	friend class CLevelPathBuilder;
	friend class CDetailPathBuilder;

protected:
	typedef MonsterSpace::SBoneRotation			CBoneRotation;
	typedef MovementManager::EPathType			EPathType;
	typedef DetailPathManager::STravelPathPoint	CTravelPathPoint;
	typedef GraphEngineSpace::CBaseParameters 	CBaseParameters;
	typedef GraphEngineSpace::CGameVertexParams	CGameVertexParams;

	typedef CBaseLocationSelector<
				CGameGraph,
				SGameVertex<
		f32,
					u32,
					u32
				>,
				u32
			>		CGameLocationSelector;

	typedef CBasePathManager<
				CGameGraph,
				SGameVertex<
		f32,
					u32,
					u32
				>,
				u32,
				u32
			>		CGamePathManager;
	typedef CBasePathManager<
				CLevelGraph,
				SBaseParameters<
		f32,
					u32,
					u32
				>,
				u32,
				u32
			>		CLevelPathManager;

private:
	enum EPathState {
		ePathStateSelectGameVertex = u32(0),
		ePathStateBuildGamePath,
		ePathStateContinueGamePath,
		
		ePathStateSelectPatrolPoint,

		ePathStateBuildLevelPath,
		ePathStateContinueLevelPath,

		ePathStateBuildDetailPath,
		
		ePathStatePathVerification,
		
		ePathStatePathCompleted,

		ePathStateTeleport,
		
		ePathStateDummy = u32(-1),
	};

protected:
	typedef xr_vector<CObject*>	NEAREST_OBJECTS;

protected:
	NEAREST_OBJECTS			m_nearest_objects;

protected:
	f32					m_speed;

public:
	CBoneRotation			m_body;

protected:
	bool					m_path_actuality;

private:
	EPathState				m_path_state;
	EPathType				m_path_type;
	bool					m_enabled;
	fVector3					m_on_disable_object_position;
	f32					m_old_desirable_speed;
	bool					m_extrapolate_path;
	bool					m_build_at_once;
	bool					m_wait_for_distributed_computation;

public:
	CGameVertexParams		*m_base_game_selector;
	CBaseParameters			*m_base_level_selector;
	CGameLocationSelector	*m_game_location_selector;
	CGamePathManager		*m_game_path_manager;
	CLevelPathManager		*m_level_path_manager;
	CDetailPathManager		*m_detail_path_manager;
	CPatrolPathManager		*m_patrol_path_manager;
	CRestrictedObject		*m_restricted_object;
	CLocationManager		*m_location_manager;
	CLevelPathBuilder		*m_level_path_builder;
	CDetailPathBuilder		*m_detail_path_builder;
	CCustomMonster			*m_object;

private:
			void	process_game_path		();
			void	process_level_path		();
			void	process_patrol_path		();
#ifdef USE_FREE_IN_RESTRICTIONS
			void	verify_detail_path		();
#endif // USE_FREE_IN_RESTRICTIONS
			void	apply_collision_hit		(CPHMovementControl *movement_control);

protected:
	virtual void	teleport				(u32 game_vertex_id);

public:

					CMovementManager		(CCustomMonster *object);
	virtual			~CMovementManager		();
	virtual void	Load					(pcstr caSection);
	virtual void	reinit					();
	virtual void	reload					(pcstr caSection);
	virtual BOOL	net_Spawn				(CSE_Abstract* data);
	virtual void	net_Destroy				();
	virtual	void	on_frame				(CPHMovementControl *movement_control, fVector3& dest_position);
	IC		bool	actual					() const;
			bool	actual_all				() const;
	IC		void	set_path_type			(EPathType path_type);
			void	set_game_dest_vertex	(const GameGraph::_GRAPH_ID &game_vertex_id);
			void	set_level_dest_vertex	(const u32 level_vertex_id);
	IC		void	set_build_path_at_once	();
			void	enable_movement			(bool enabled);
			EPathType path_type				() const;
			GameGraph::_GRAPH_ID game_dest_vertex_id	() const;
			u32		level_dest_vertex_id	() const;
	IC		bool	enabled					() const;
	IC		bool	path_completed			() const;
	IC		f32	old_desirable_speed		() const;
	IC		void	set_desirable_speed		(f32 speed);
			const xr_vector<CTravelPathPoint>	&path	() const;
	IC		void	set_body_orientation	(const MonsterSpace::SBoneRotation &orientation);
	IC		const CBoneRotation &body_orientation() const;
			void	update_path				();
	virtual	void	move_along_path			(CPHMovementControl *movement_control, fVector3& dest_position, f32 time_delta);

	IC		f32	speed					() const;
	f32	speed					(CPHMovementControl *movement_control) const;

	virtual void	on_travel_point_change	(const u32 &previous_travel_point_index);
	virtual void	on_build_path			() {}

	template <typename T>
	IC		bool	accessible				(T position_or_vertex_id, f32 radius = EPSILON_3) const;

	IC		void	extrapolate_path		(bool value);
	IC		bool	extrapolate_path		() const;

			bool	distance_to_destination_greater	(const f32& distance_to_check) const;

	IC		bool	wait_for_distributed_computation			() const;
	virtual	bool	can_use_distributed_compuations				(u32 option) const;
			
			void	clear_path				();

public:
	IC		CGameVertexParams		*base_game_params			() const;
	IC		CBaseParameters			*base_level_params			() const;
	IC		CGameLocationSelector	&game_selector				() const;
	IC		CGamePathManager		&game_path					() const;
	IC		CLevelPathManager		&level_path					() const;
	IC		CDetailPathManager		&detail						() const;
	IC		CPatrolPathManager		&patrol						() const;
	IC		CRestrictedObject		&restrictions				() const;
	IC		CLocationManager		&locations					() const;
	IC		CCustomMonster			&object						() const;
	IC		CLevelPathBuilder		&level_path_builder			() const;
	IC		CDetailPathBuilder		&detail_path_builder		() const;

public:
	virtual	void					on_restrictions_change		();
};

#include "movement_manager_inline.h"