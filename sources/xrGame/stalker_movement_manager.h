////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker movement manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "movement_manager.h"
#include "ai_monster_space.h"
#include "stalker_movement_params.h"

using namespace MonsterSpace;

class CStalker;
class CStalkerVelocityCollection;
class CGameObject;

class CStalkerMovementManager : public CMovementManager {
protected:
	typedef CMovementManager					inherited;

public:
	using inherited::speed;

public:
	typedef DetailPathManager::EDetailPathType	EDetailPathType;

private:
	const CStalkerVelocityCollection	*m_velocities;

protected:
	CStalkerMovementParams				m_current;
	CStalkerMovementParams				m_target;
	CStalker							*m_object;

public:
	MonsterSpace::SBoneRotation			m_head;
	u32									m_last_turn_index;
	float								m_danger_head_speed;

private:
	inline		void	setup_head_speed				();
	inline		void	add_velocity					(int mask, float linear, float compute_angular, float angular);
	inline		void	add_velocity					(int mask, float linear, float compute_angular);
	inline		void	setup_body_orientation			();
			void	init_velocity_masks				();
			void	setup_movement_params			();
			bool	script_control					();
			void	setup_velocities				();
			void	parse_velocity_mask				();
			void	check_for_bad_path				();

public:
					CStalkerMovementManager			(CStalker *object);
	virtual			~CStalkerMovementManager		();
	virtual	void	Load							(const char* section);
	virtual	void	reinit							();
	virtual	void	reload							(const char* section);
	virtual	void	update							(u32 time_delta);
	virtual void	on_travel_point_change			(const u32 &previous_travel_point_index);
	virtual	void	on_restrictions_change			();
			void	initialize						();
	inline		float	path_direction_angle			();
	inline		bool	turn_in_place					() const;

	inline		void	set_head_orientation			(const MonsterSpace::SBoneRotation &orientation);
			void	set_desired_position			(const Fvector *desired_position);
	inline		void	set_desired_direction			(const Fvector *desired_direction);
	inline		void	set_body_state					(EBodyState body_state);
	inline		void	set_movement_type				(EMovementType movement_type);
	inline		void	set_mental_state				(EMentalState mental_state);
	inline		void	set_path_type					(EPathType path_type);
	inline		void	set_detail_path_type			(EDetailPathType detail_path_type);
			void	set_nearest_accessible_position	();
			void	set_nearest_accessible_position	(Fvector desired_position, u32 level_vertex_id);
			float	speed							(const EMovementDirection &movement_direction);
			void	setup_speed_from_animation		(const float &speed);

public:
	inline		const MonsterSpace::SBoneRotation		&head_orientation		() const;
	inline		const Fvector							&desired_position		() const;
	inline		const Fvector							&desired_direction		() const;
	inline		const MonsterSpace::EBodyState			body_state				() const;
	inline		const MonsterSpace::EBodyState			target_body_state		() const;
	inline		const MonsterSpace::EMovementType		movement_type			() const;
	inline		const MonsterSpace::EMentalState		mental_state			() const;
	inline		const MonsterSpace::EMentalState		target_mental_state		() const;
	inline		const EPathType							path_type				() const;
	inline		const EDetailPathType					detail_path_type		() const;
	inline		bool									use_desired_position	() const;
	inline		bool									use_desired_direction	() const;

public:
	inline		const MonsterSpace::EMovementType		target_movement_type	() const;

public:
	inline		CStalker								&object					() const;

private:
	const CGameObject	*m_last_query_object;
	Fvector				m_last_query_position;
	Fvector				m_last_query_object_position;
	float				m_last_query_distance;
	bool				m_last_query_result;
	bool				m_force_update;

public:
	virtual void									on_build_path			();
			void									update_object_on_the_way(const CGameObject *object, const float &distance);
			bool									is_object_on_the_way	(const CGameObject *object, const float &distance);
			void									force_update			(const bool &force_update);
	inline		void									danger_head_speed		(const float &speed);
};

#include "stalker_movement_manager_inline.h"
