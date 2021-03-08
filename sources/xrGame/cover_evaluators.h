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
	float				m_best_value;
	bool				m_initialized;
	Fvector				m_start_position;
	CRestrictedObject	*m_object;
	bool				m_actuality;
	float				m_last_radius;

public:
	inline							CCoverEvaluatorBase	(CRestrictedObject *object);
	inline		const CCoverPoint	*selected			() const;
	inline		bool				inertia				(float radius);
	inline		bool				initialized			() const;
	inline		void				setup				();
	inline		void				set_inertia			(u32 inertia_time);
	inline		void				initialize			(const Fvector &start_position, bool fake_call = false);
	virtual void				finalize			();
	inline		bool				accessible			(const Fvector &position);
	inline		bool				actual				() const;
	inline		CRestrictedObject	&object				() const;
	inline		void				invalidate			();
	inline		float				best_value			() const;
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorCloseToEnemy
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorCloseToEnemy : public CCoverEvaluatorBase {
protected:
	typedef CCoverEvaluatorBase inherited;

protected:
	Fvector				m_enemy_position;
	float				m_min_distance;
	float				m_max_distance;
	float				m_current_distance;
	float				m_deviation;
	float				m_best_distance;

public:
	inline					CCoverEvaluatorCloseToEnemy	(CRestrictedObject *object);
	inline		void		initialize			(const Fvector &start_position, bool fake_call = false);
	inline		void		setup				(const Fvector &enemy_position, float min_enemy_distance, float	max_enemy_distance, float deviation = 0.f);
			void		evaluate			(const CCoverPoint *cover_point, float weight);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorFarFromEnemy
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorFarFromEnemy : public CCoverEvaluatorCloseToEnemy {
protected:
	typedef CCoverEvaluatorCloseToEnemy inherited;

public:
	inline					CCoverEvaluatorFarFromEnemy	(CRestrictedObject *object);
			void		evaluate			(const CCoverPoint *cover_point, float weight);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorBest
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorBest : public CCoverEvaluatorCloseToEnemy {
protected:
	typedef CCoverEvaluatorCloseToEnemy inherited;

public:
	inline					CCoverEvaluatorBest	(CRestrictedObject *object);
			void		evaluate			(const CCoverPoint *cover_point, float weight);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorBestByTime
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorBestByTime : public CCoverEvaluatorBest {
protected:
	typedef CCoverEvaluatorBest inherited;

public:
	inline					CCoverEvaluatorBestByTime	(CRestrictedObject *object);
			void		evaluate					(const CCoverPoint *cover_point, float weight);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorAngle
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorAngle : public CCoverEvaluatorCloseToEnemy {
protected:
	typedef CCoverEvaluatorCloseToEnemy inherited;

protected:
	Fvector				m_direction;
	Fvector				m_best_direction;
	float				m_best_alpha;
	u32					m_level_vertex_id;

public:
	inline					CCoverEvaluatorAngle(CRestrictedObject *object);
	inline		void		setup				(const Fvector &enemy_position, float min_enemy_distance, float	max_enemy_distance, u32 level_vertex_id);
			void		initialize			(const Fvector &start_position, bool fake_call = false);
			void		evaluate			(const CCoverPoint *cover_point, float weight);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorSafe
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorSafe : public CCoverEvaluatorBase {
protected:
	typedef CCoverEvaluatorBase inherited;

protected:
	float				m_min_distance;

public:
	inline					CCoverEvaluatorSafe	(CRestrictedObject *object);
	inline		void		setup				(float min_distance);
			void		evaluate			(const CCoverPoint *cover_point, float weight);
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorRandomGame
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorRandomGame : public CCoverEvaluatorBase {
protected:
	typedef CCoverEvaluatorBase inherited;

protected:
	GameGraph::_GRAPH_ID			m_game_vertex_id;
	Fvector							m_start_position;
	float							m_max_distance_sqr;
	xr_vector<const CCoverPoint*>	m_covers;

public:
	inline					CCoverEvaluatorRandomGame	(CRestrictedObject *object);
			void		setup						(GameGraph::_GRAPH_ID game_vertex_id, float max_distance);
			void		evaluate					(const CCoverPoint *cover_point, float weight);
	virtual	void		finalize					();
};

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorAmbush
//////////////////////////////////////////////////////////////////////////

class CCoverEvaluatorAmbush : public CCoverEvaluatorBase {
protected:
	typedef CCoverEvaluatorBase inherited;

private:
	Fvector				m_my_position;
	Fvector				m_enemy_position;
	float				m_min_enemy_distance;

public:
	inline					CCoverEvaluatorAmbush	(CRestrictedObject *object);
			void		setup					(const Fvector &my_position, const Fvector &enemy_position, float min_enemy_distance);
			void		evaluate				(const CCoverPoint *cover_point, float weight);
};

#include "cover_evaluators_inline.h"
