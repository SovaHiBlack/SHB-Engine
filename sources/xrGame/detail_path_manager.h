////////////////////////////////////////////////////////////////////////////
//	Module 		: detailed_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Detail path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "restricted_object.h"
#include "detail_path_manager_space.h"
#include "associative_vector.h"

using namespace DetailPathManager;

class CDetailPathManager {
public:
	struct STravelParams {
		float			linear_velocity;
		float			angular_velocity; 
		float			real_angular_velocity; 

		STravelParams(){}
		STravelParams(float l, float a) : linear_velocity(l), angular_velocity(a), real_angular_velocity(a) {}
		STravelParams(float l, float a, float ra) : linear_velocity(l), angular_velocity(a), real_angular_velocity(ra) {}
	};

	struct STravelParamsIndex : public STravelParams {
		u32				index;
		
		STravelParamsIndex(){}
		STravelParamsIndex(float l, float a, u32 i) : STravelParams(l,a), index(i) {}
	};

	struct STravelPoint {
		Fvector2		position;
		u32				vertex_id;
	};

	struct SPathPoint : public STravelParams, public STravelPoint {
		Fvector2		direction;
	};

	struct SCirclePoint {
		Fvector2		center;
		float			radius;
		Fvector2		point;
		float			angle;
	};

	struct STrajectoryPoint :
		public SPathPoint,
		public SCirclePoint
	{
	};

	struct SDist {
		u32		index;
		float	time;

		bool operator<(const SDist &d1) const
		{
			return		(time < d1.time);
		}
	};

private:
	CRestrictedObject							*m_restricted_object;

public:
	typedef
		associative_vector<
			u32,
			STravelParams
		>										VELOCITIES;

protected:
	VELOCITIES									m_movement_params;
	u32											m_current_travel_point;

private:
	enum EDirectionType {
		eDirectionTypeFP = u32(0),
		eDirectionTypeFN = u32(1),
		eDirectionTypeSP = u32(0),
		eDirectionTypeSN = u32(2),
		eDirectionTypePP = eDirectionTypeFP | eDirectionTypeSP, // both linear velocities are positive
		eDirectionTypeNN = eDirectionTypeFN | eDirectionTypeSN, // both linear velocities are negative
		eDirectionTypePN = eDirectionTypeFP | eDirectionTypeSN, // the first linear velocity is positive, the second one - negative
		eDirectionTypeNP = eDirectionTypeFN | eDirectionTypeSP, // the first linear velocity is negative, the second one - positive
	};

private:
	bool										m_actuality;
	bool										m_failed;
	EDetailPathType								m_path_type;

private:
	Fvector3									m_start_position;
	Fvector3									m_start_direction;
	Fvector3									m_dest_position;
	Fvector3									m_corrected_dest_position;
	Fvector3									m_dest_direction;

private:
	xr_vector<STravelPathPoint>					m_path;
	xr_vector<STravelPoint>						m_key_points;
	xr_vector<STravelParamsIndex>				m_start_params;
	xr_vector<STravelParamsIndex>				m_dest_params;
	xr_vector<STravelPathPoint>					m_temp_path;
	u32											m_desirable_mask;
	u32											m_velocity_mask;

private:
	bool										m_try_min_time;
	bool										m_use_dest_orientation;
	bool										m_state_patrol_path;
	u32											m_last_patrol_point;
	u32											m_time_path_built;
	float										m_extrapolate_length;

private:
	float										m_distance_to_target;
	bool										m_distance_to_target_actual;

private:
	inline	STravelPoint compute_better_key_point	(const STravelPoint		&point0,	const STravelPoint					&point1,		const STravelPoint					&point2,				bool								reverse_order);
	inline		bool	better_key_point			(const STravelPoint		&point0,	const STravelPoint					&point2,		const STravelPoint					&point10,			const STravelPoint					&point11);
	inline		bool	check_mask					(u32					mask,			  u32							test) const;
	inline		void	adjust_point				(const Fvector2			&source,		  float							yaw,				  float							magnitude,				  Fvector2						&dest) const;
	inline		void	assign_angle				(float					&angle,		const float							start_yaw,		const float							dest_yaw,			const bool							positive,			const EDirectionType				direction_type,				const bool				start = true) const;
	inline		void	compute_circles				(STrajectoryPoint		&point,			  SCirclePoint					*circles);
			bool	compute_tangent				(const STrajectoryPoint	&start,		const SCirclePoint					&start_circle,	const STrajectoryPoint				&dest,				const SCirclePoint					&dest_circle,		      SCirclePoint					*tangents,					const EDirectionType	direction_type);
			bool	build_circle_trajectory		(const STrajectoryPoint &position,		  xr_vector<STravelPathPoint>	*path,				  u32							*vertex_id,			const u32							velocity);
			bool	build_line_trajectory		(const STrajectoryPoint &start,		const STrajectoryPoint				&dest,				  u32							vertex_id,				  xr_vector<STravelPathPoint>	*path,				const u32							velocity);
			bool	build_trajectory			(const STrajectoryPoint &start,		const STrajectoryPoint				&dest,				  xr_vector<STravelPathPoint>	*path,				const u32							velocity1,			const u32							velocity2,					const u32				velocity3);
			bool	build_trajectory			(	   STrajectoryPoint &start,			  STrajectoryPoint				&dest,			const SCirclePoint					tangents[4][2],		const u32							tangent_count,		      xr_vector<STravelPathPoint>	*path,							  float				&time,							const u32 velocity1, const u32				velocity2,		const u32 velocity3);
			bool	compute_trajectory			(      STrajectoryPoint &start,			  STrajectoryPoint				&dest,				  xr_vector<STravelPathPoint>	*path,					  float							&time,				const u32							velocity1,					const u32				velocity2,						const u32 velocity3, const EDirectionType	direction_type);
			bool	compute_path				(      STrajectoryPoint &start,			  STrajectoryPoint				&dest,				  xr_vector<STravelPathPoint>	*m_tpTravelLine, 	const xr_vector<STravelParamsIndex> &m_start_params,	const xr_vector<STravelParamsIndex> &m_dest_params,				const u32				straight_line_index,			const u32 straight_line_index_negative);
			void	validate_vertex_position	(	   STrajectoryPoint &point) const;
			bool	init_build					(const xr_vector<u32>	&level_path,	  u32							intermediate_index,	  STrajectoryPoint				&start,					  STrajectoryPoint				&dest,					  u32							&straight_line_index,			  u32				&straight_line_index_negative);
			bool	fill_key_points				(const xr_vector<u32>	&level_path,	  u32							intermediate_index,   STrajectoryPoint				&start,					  STrajectoryPoint				&dest);
			void	add_patrol_point			();
			void	postprocess_key_points		(const xr_vector<u32>	&level_path,	  u32							intermediate_index,   STrajectoryPoint				&start,					  STrajectoryPoint				&dest,xr_vector<STravelParamsIndex> &finish_params,		const u32							straight_line_index,const u32							straight_line_index_negative);
			void	build_path_via_key_points	(STrajectoryPoint		&start,			  STrajectoryPoint				&dest,				  xr_vector<STravelParamsIndex> &finish_params,		const u32							straight_line_index,const u32							straight_line_index_negative);
			void	build_smooth_path			(const xr_vector<u32>	&level_path,	  u32							intermediate_index);

private:
			void	update_distance_to_target	();

protected:
			void	build_path					(const xr_vector<u32> &level_path, u32 intermediate_index);

	friend class CScriptEntity;
	friend class CMovementManager;
	friend class CPoltergeisMovementManager;
	friend class CDetailPathBuilder;
#ifdef DEBUG
	friend class CLevelGraph;
	friend class CCustomMonster;
#endif

public:
					CDetailPathManager			(CRestrictedObject *object);
	virtual			~CDetailPathManager			();
	virtual	void	reinit						();
			bool	valid						() const;
			Fvector3 direction					() const;
			bool	actual						() const;
	inline		bool	failed						() const;
	inline		bool	completed					(const Fvector3& position, bool bRealCompleted = true) const;
			bool	valid						(const Fvector3& position) const;
	inline		u32		curr_travel_point_index		() const;

public:
	inline		const xr_vector<STravelPathPoint>	&path					() const;
	inline		const STravelPathPoint				&curr_travel_point		() const;
	inline		const Fvector3&						start_position			() const;
	inline		const Fvector3&						start_direction		() const;
	inline		const Fvector3&						dest_position			() const;
	inline		const Fvector3&						dest_direction			() const;
	inline		const u32							velocity_mask			() const;
	inline		const u32							desirable_mask			() const;
	inline		const bool							try_min_time			() const;
	inline		const bool							use_dest_orientation	() const;
	inline		const u32							time_path_built			() const;
	inline		const STravelParams					&velocity				(const u32 &velocity_id) const;
	inline		const VELOCITIES					&velocities				() const;
	inline		void								add_velocity			(const u32 &velocity_id, const STravelParams &params);
	inline		void								set_start_position		(const Fvector3& start_position);
	inline		void								set_start_direction		(const Fvector3& start_direction);
	inline		void								set_dest_position		(const Fvector3& dest_position);
	inline		void								set_dest_direction		(const Fvector3& dest_direction);
	inline		void								set_path_type			(const EDetailPathType path_type);
	inline		void								set_velocity_mask		(const u32 mask);
	inline		void								set_desirable_mask		(const u32 mask);
	inline		void								set_try_min_time		(const bool try_min_time);
	inline		void								set_use_dest_orientation(const bool use_dest_orientation);
	inline		void								set_state_patrol_path	(const bool state_patrol_path);
	inline		bool								state_patrol_path		() const;
	inline		void								extrapolate_length		(float extrapolate_length);
	inline		float								extrapolate_length		() const;

public:
			void								on_travel_point_change	(const u32 &previous_travel_point_index);
	inline		const float							&distance_to_target		();
			u32									location_on_path		(const CGameObject *object, float distance, Fvector3& result) const;
};

#include "detail_path_manager_inline.h"