////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_manager.h
//	Created 	: 24.03.2004
//  Modified 	: 24.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover manager class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Profiler.h"
#include "QuadTree.h"

class CCoverPoint;

namespace LevelGraph {
	class	CVertex;
};

class CCoverManager {
public:
	typedef CQuadTree<CCoverPoint> CPointQuadTree;

protected:
	CPointQuadTree					*m_covers;
	xr_vector<bool>					m_temp;
	mutable xr_vector<CCoverPoint*>	m_nearest;

protected:
	inline		bool					edge_vertex			(u32 index);
	inline		bool					cover				(LevelGraph::CVertex *v, u32 index0, u32 index1);
	inline		bool					critical_point		(LevelGraph::CVertex *v, u32 index, u32 index0, u32 index1);
	inline		bool					critical_cover		(u32 index);

private:
	template <typename _evaluator_type, typename _restrictor_type>
	inline		bool					inertia				(float radius, _evaluator_type &evaluator, const _restrictor_type &restrictor) const;

public:
									CCoverManager		();
	virtual							~CCoverManager		();
			void					compute_static_cover();
	inline		CPointQuadTree			&covers				() const;
	inline		CPointQuadTree			*get_covers			();
			void					clear				();
	template <typename _evaluator_type, typename _restrictor_type>
	inline		const CCoverPoint		*best_cover			(const Fvector3& position, float radius, _evaluator_type &evaluator, const _restrictor_type &restrictor) const;
	template <typename _evaluator_type>
	inline		const CCoverPoint		*best_cover			(const Fvector3& position, float radius, _evaluator_type &evaluator) const;
	inline		bool					operator()			(const CCoverPoint *) const;
	inline		float					weight				(const CCoverPoint *) const;
	inline		void					finalize			(const CCoverPoint *) const;
};

#include "cover_manager_inline.h"