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
		f32			linear_velocity;
		f32			angular_velocity;
		f32			real_angular_velocity;

		STravelParams(){}
		STravelParams(f32 l, f32 a) : linear_velocity(l), angular_velocity(a), real_angular_velocity(a) {}
		STravelParams(f32 l, f32 a, f32 ra) : linear_velocity(l), angular_velocity(a), real_angular_velocity(ra) {}
	};

	struct STravelParamsIndex : public STravelParams {
		u32				index;
		
		STravelParamsIndex(){}
		STravelParamsIndex(f32 l, f32 a, u32 i) : STravelParams(l,a), index(i) {}
	};

	struct STravelPoint {
		fVector2		position;
		u32				vertex_id;
	};

	struct SPathPoint : public STravelParams, public STravelPoint {
		fVector2		direction;
	};

	struct SCirclePoint {
		fVector2		center;
		f32			radius;
		fVector2		point;
		f32			angle;
	};

	struct STrajectoryPoint :
		public SPathPoint,
		public SCirclePoint
	{
	};

	struct SDist {
		u32		index;
		f32	time;

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
	fVector3										m_start_position;
	fVector3										m_start_direction;
	fVector3										m_dest_position;
	fVector3										m_corrected_dest_position;
	fVector3										m_dest_direction;

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
	f32										m_extrapolate_length;

private:
	f32										m_distance_to_target;
	bool										m_distance_to_target_actual;

private:
	IC	STravelPoint compute_better_key_point	(const STravelPoint		&point0,	const STravelPoint					&point1,		const STravelPoint					&point2,				bool								reverse_order);
	IC		bool	better_key_point			(const STravelPoint		&point0,	const STravelPoint					&point2,		const STravelPoint					&point10,			const STravelPoint					&point11);
	IC		bool	check_mask					(u32					mask,			  u32							test) const;
	IC		void	adjust_point				(const fVector2&		source, f32							yaw, f32							magnitude, fVector2&	dest) const;
	IC		void	assign_angle				(f32&					angle,		const f32							start_yaw,		const f32							dest_yaw,			const bool							positive,			const EDirectionType				direction_type,				const bool				start = true) const;
	IC		void	compute_circles				(STrajectoryPoint		&point,			  SCirclePoint					*circles);
			bool	compute_tangent				(const STrajectoryPoint	&start,		const SCirclePoint					&start_circle,	const STrajectoryPoint				&dest,				const SCirclePoint					&dest_circle,		      SCirclePoint					*tangents,					const EDirectionType	direction_type);
			bool	build_circle_trajectory		(const STrajectoryPoint &position,		  xr_vector<STravelPathPoint>	*path,				  u32							*vertex_id,			const u32							velocity);
			bool	build_line_trajectory		(const STrajectoryPoint &start,		const STrajectoryPoint				&dest,				  u32							vertex_id,				  xr_vector<STravelPathPoint>	*path,				const u32							velocity);
			bool	build_trajectory			(const STrajectoryPoint &start,		const STrajectoryPoint				&dest,				  xr_vector<STravelPathPoint>	*path,				const u32							velocity1,			const u32							velocity2,					const u32				velocity3);
			bool	build_trajectory			(	   STrajectoryPoint &start,			  STrajectoryPoint				&dest,			const SCirclePoint					tangents[4][2],		const u32							tangent_count,		      xr_vector<STravelPathPoint>	*path, f32&time,							const u32 velocity1, const u32				velocity2,		const u32 velocity3);
			bool	compute_trajectory			(      STrajectoryPoint &start,			  STrajectoryPoint				&dest,				  xr_vector<STravelPathPoint>	*path, f32& time,				const u32							velocity1,					const u32				velocity2,						const u32 velocity3, const EDirectionType	direction_type);
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
			fVector3 direction					() const;
			bool	actual						() const;
	IC		bool	failed						() const;
	IC		bool	completed(const fVector3& position, bool bRealCompleted = true) const;
	bool	valid(const fVector3& position) const;
	IC		u32		curr_travel_point_index		() const;

public:
	IC		const xr_vector<STravelPathPoint>	&path					() const;
	IC		const STravelPathPoint				&curr_travel_point		() const;
	IC		const fVector3& start_position( ) const;
	IC		const fVector3& start_direction( ) const;
	IC		const fVector3& dest_position( ) const;
	IC		const fVector3& dest_direction( ) const;
	IC		const u32							velocity_mask			() const;
	IC		const u32							desirable_mask			() const;
	IC		const bool							try_min_time			() const;
	IC		const bool							use_dest_orientation	() const;
	IC		const u32							time_path_built			() const;
	IC		const STravelParams					&velocity				(const u32 &velocity_id) const;
	IC		const VELOCITIES					&velocities				() const;
	IC		void								add_velocity			(const u32 &velocity_id, const STravelParams &params);
	IC		void								set_start_position(const fVector3& start_position);
	IC		void								set_start_direction(const fVector3& start_direction);
	IC		void								set_dest_position(const fVector3& dest_position);
	IC		void								set_dest_direction(const fVector3& dest_direction);
	IC		void								set_path_type			(const EDetailPathType path_type);
	IC		void								set_velocity_mask		(const u32 mask);
	IC		void								set_desirable_mask		(const u32 mask);
	IC		void								set_try_min_time		(const bool try_min_time);
	IC		void								set_use_dest_orientation(const bool use_dest_orientation);
	IC		void								set_state_patrol_path	(const bool state_patrol_path);
	IC		bool								state_patrol_path		() const;
	IC		void								extrapolate_length		(f32 extrapolate_length);
	IC		f32								extrapolate_length		() const;

public:
			void								on_travel_point_change	(const u32 &previous_travel_point_index);
	IC		const f32&							distance_to_target		();
	u32									location_on_path(const CGameObject* object, f32 distance, fVector3& result) const;
};

#include "detail_path_manager_inline.h"