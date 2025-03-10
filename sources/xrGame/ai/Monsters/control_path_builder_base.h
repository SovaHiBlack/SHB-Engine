#pragma once

#include "ai_monster_defs.h"
#include "control_combase.h"
#include "../../movement_manager_space.h"

class CMotionStats;
class CCoverEvaluatorCloseToEnemy;

class CControlPathBuilderBase : public CControl_ComBase {
	typedef CControl_ComBase inherited;

	// -----------------------------------------------------------
	// external setup
	bool						m_try_min_time;
	bool						m_enable;
	bool						m_use_dest_orient;
	fVector3						m_dest_dir;
	MovementManager::EPathType	m_path_type;
	bool						m_extrapolate;
	u32							m_velocity_mask;
	u32							m_desirable_mask;
	bool						m_reset_actuality;
	u32							m_game_graph_target_vertex;

	// -----------------------------------------------------------
    // build path members
	// -----------------------------------------------------------

	struct STarget {
		fVector3		position;
		u32			node;
		void		init		() {
			position.set	(0.f,0.f,0.f);
			node			= u32(-1);
		}

		void		set(const fVector3& pos, u32 vertex)
		{
			position.set	(pos);
			node			= vertex;
		}

	} m_target_set, m_target_found;

	u32			m_time;					// ����� ������������ ����
	u32			m_last_time_target_set;
	f32		m_distance_to_path_end;
	bool		m_failed;
	u32			m_last_time_dir_set;

	bool		m_target_actual;		// ��������������� ������ ������������� �����������

	struct {
		bool	use_covers;
		f32	min_dist;
		f32	max_dist;
		f32	deviation;
		f32	radius;
	} m_cover_info;

	enum {
		eMoveToTarget,
		eRetreatFromTarget,
	} m_target_type;

	CCoverEvaluatorCloseToEnemy	*m_cover_approach;

	// -----------------------------------------------------------

	enum {
		eStatePathValid			= u32(1) << 0,		
		eStateWaitNewPath		= u32(1) << 1,
		eStatePathEnd			= u32(1) << 2,
		eStateNoPath			= u32(1) << 3,
		eStatePathFailed		= u32(1) << 4
	};
	u32							m_state;

	bool						m_path_end;

	// ���������, � ������� path_builder �������� ����������
	u32							m_time_global_failed_started;
	u32							m_time_path_updated_external;
	
public:
						CControlPathBuilderBase				();
	virtual				~CControlPathBuilderBase			();
	
	// -------------------------------------------------------------------
	// Control Interface
	virtual void		reinit				();
	virtual void		update_frame		();
	virtual void		on_event			(ControlCom::EEventType, ControlCom::IEventData*);	
	virtual void		on_start_control	(ControlCom::EControlType type);
	virtual void		on_stop_control		(ControlCom::EControlType type);

	// -------------------------------------------------------------------

			void		pre_update			();
			

	// -------------------------------------------------------------------
	IC	void	set_try_min_time		(bool new_val) {m_try_min_time		= new_val;}
	IC	void	set_use_dest_orient		(bool new_val) {m_use_dest_orient	= new_val;}
	IC	void	disable_path			() {m_enable = false;}
	IC	void	enable_path				() {m_enable = true;}
	IC	void	extrapolate_path		(bool val) {m_extrapolate = val;}
	IC	void	set_level_path_type		() {m_path_type = MovementManager::ePathTypeLevelPath;}
	IC	void	set_game_path_type		() {m_path_type = MovementManager::ePathTypeGamePath;}
	IC	void	set_patrol_path_type	() {m_path_type = MovementManager::ePathTypePatrolPath;}
	IC	void	set_velocity_mask		(u32 mask) {m_velocity_mask = mask;}
	IC	void	set_desirable_mask		(u32 mask) {m_desirable_mask = mask;}
	void	set_dest_direction(const fVector3& dir);

	IC	bool	enabled					() {return m_enable;}
	// -------------------------------------------------------------------
	// Set methods
	void		set_target_point(const fVector3& position, u32 node = u32(-1));
		void		set_target_point		(u32 node);
		void		set_retreat_from_point(const fVector3& position);

	IC	void		set_rebuild_time		(u32 time);
	IC	void		set_cover_params		(f32 min, f32 max, f32 dev, f32 radius);
	IC	void		set_use_covers			(bool val = true);
	IC	void		set_distance_to_end		(f32 dist);

		void		prepare_builder			();
		void		detour_graph_points		(u32 game_graph_vertex_id = u32(-1));
	IC	void		set_generic_parameters	();

	fVector3		get_target_found		() {return m_target_found.position;}
	fVector3		get_target_set			() {return m_target_set.position;}

		// -------------------------------------------------------------------
		// Services
	void		set_target_accessible(STarget& target, const fVector3& position);

private:
		// functional
		void		update_path_builder_state	();
		void		update_target_point			();
		void		check_failure				();

		bool		target_point_need_update	();
		void		find_target_point_set		();
		void		find_target_point_failed	();

		void		select_target				();		// ������� 

		void		set_path_builder_params		();		// set params to control
		
		void		reset						();

		void		travel_point_changed		();
		void		on_path_built				();
		void		on_path_end					();
		void		on_path_updated				();

		// ����� �������, ����� ����
		void		find_node					();

		bool		global_failed				();
};

#include "control_path_builder_base_inline.h"
