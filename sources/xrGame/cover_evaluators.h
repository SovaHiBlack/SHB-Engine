////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_evaluators.h
//	Created 	: 24.04.2004
//  Modified 	: 24.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover evaluators
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "restricted_object.h"
#include "game_graph_space.h"

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorBase
//////////////////////////////////////////////////////////////////////////

class CCoverPoint;

class CCoverEvaluatorBase {
protected:
	const CCoverPoint	*m_selected;
	u32					m_last_update;
	u32					m_inertia_time;
	f32				m_best_value;
	bool				m_initialized;
	fVector3				m_start_position;
	CRestrictedObject	*m_object;
	bool				m_actuality;
	f32				m_last_radius;

public:
	IC							CCoverEvaluatorBase	(CRestrictedObject *object);
	IC		const CCoverPoint	*selected			() const;
	IC		bool				inertia				(f32 radius);
	IC		bool				initialized			() const;
	IC		void				setup				();
	IC		void				set_inertia			(u32 inertia_time);
	IC		void				initialize(const fVector3& start_position, bool fake_call = false);
	virtual void				finalize			();
	IC		bool				accessible(const fVector3& position);
	IC		bool				actual				() const;
	IC		CRestrictedObject	&object				() const;
	IC		void				invalidate			();
	IC		f32				best_value			() const;
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorCloseToEnemy
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorCloseToEnemy : public CCoverEvaluatorBase {
protected:
	typedef CCoverEvaluatorBase inherited;

protected:
	fVector3				m_enemy_position;
	f32				m_min_distance;
	f32				m_max_distance;
	f32				m_current_distance;
	f32				m_deviation;
	f32				m_best_distance;

public:
	IC					CCoverEvaluatorCloseToEnemy	(CRestrictedObject *object);
	IC		void		initialize(const fVector3& start_position, bool fake_call = false);
	IC		void		setup(const fVector3& enemy_position, f32 min_enemy_distance, f32 max_enemy_distance, f32 deviation = 0.0f);
			void		evaluate			(const CCoverPoint *cover_point, f32 weight);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorFarFromEnemy
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorFarFromEnemy : public CCoverEvaluatorCloseToEnemy {
protected:
	typedef CCoverEvaluatorCloseToEnemy inherited;

public:
	IC					CCoverEvaluatorFarFromEnemy	(CRestrictedObject *object);
			void		evaluate			(const CCoverPoint *cover_point, f32 weight);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorBest
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorBest : public CCoverEvaluatorCloseToEnemy {
protected:
	typedef CCoverEvaluatorCloseToEnemy inherited;

public:
	IC					CCoverEvaluatorBest	(CRestrictedObject *object);
			void		evaluate			(const CCoverPoint *cover_point, f32 weight);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorBestByTime
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorBestByTime : public CCoverEvaluatorBest {
protected:
	typedef CCoverEvaluatorBest inherited;

public:
	IC					CCoverEvaluatorBestByTime	(CRestrictedObject *object);
			void		evaluate					(const CCoverPoint *cover_point, f32 weight);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorAngle
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorAngle : public CCoverEvaluatorCloseToEnemy {
protected:
	typedef CCoverEvaluatorCloseToEnemy inherited;

protected:
	fVector3				m_direction;
	fVector3				m_best_direction;
	f32				m_best_alpha;
	u32					m_level_vertex_id;

public:
	IC					CCoverEvaluatorAngle(CRestrictedObject *object);
	IC		void		setup(const fVector3& enemy_position, f32 min_enemy_distance, f32 max_enemy_distance, u32 level_vertex_id);
	void		initialize(const fVector3& start_position, bool fake_call = false);
			void		evaluate			(const CCoverPoint *cover_point, f32 weight);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorSafe
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorSafe : public CCoverEvaluatorBase {
protected:
	typedef CCoverEvaluatorBase inherited;

protected:
	f32				m_min_distance;

public:
	IC					CCoverEvaluatorSafe	(CRestrictedObject *object);
	IC		void		setup				(f32 min_distance);
			void		evaluate			(const CCoverPoint *cover_point, f32 weight);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorRandomGame
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorRandomGame : public CCoverEvaluatorBase {
protected:
	typedef CCoverEvaluatorBase inherited;

protected:
	GameGraph::_GRAPH_ID			m_game_vertex_id;
	fVector3							m_start_position;
	f32							m_max_distance_sqr;
	xr_vector<const CCoverPoint*>	m_covers;

public:
	IC					CCoverEvaluatorRandomGame	(CRestrictedObject *object);
			void		setup						(GameGraph::_GRAPH_ID game_vertex_id, f32 max_distance);
			void		evaluate					(const CCoverPoint *cover_point, f32 weight);
	virtual	void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorAmbush
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorAmbush : public CCoverEvaluatorBase {
protected:
	typedef CCoverEvaluatorBase inherited;

private:
	fVector3				m_my_position;
	fVector3				m_enemy_position;
	f32				m_min_enemy_distance;

public:
	IC					CCoverEvaluatorAmbush	(CRestrictedObject *object);
	void		setup(const fVector3& my_position, const fVector3& enemy_position, f32 min_enemy_distance);
			void		evaluate				(const CCoverPoint *cover_point, f32 weight);
};

#include "cover_evaluators_inline.h"
