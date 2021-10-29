#pragma once

#include "../../cover_evaluators.h"

class CMonsterCorpseCoverEvaluator : public CCoverEvaluatorBase
{
protected:
	typedef CCoverEvaluatorBase inherited;

	float				m_min_distance;
	float				m_max_distance;

public:

	inline	CMonsterCorpseCoverEvaluator(CRestrictedObject* object) : inherited(object)
	{ }

	inline void		setup(float min_enemy_distance, float max_enemy_distance)
	{
		inherited::setup( );
		m_min_distance = min_enemy_distance;
		m_max_distance = max_enemy_distance;
	}

	void		evaluate(const CCoverPoint* cover_point, float weight);
};
