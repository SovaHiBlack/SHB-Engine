#pragma once

#include "../../cover_evaluators.h"

class CMonsterCorpseCoverEvaluator: public CCoverEvaluatorBase {
protected:
	typedef CCoverEvaluatorBase inherited;

protected:
	F32				m_min_distance;
	F32				m_max_distance;

public:

	IC	CMonsterCorpseCoverEvaluator		(CRestrictedObject *object) : inherited(object)
	{
	}

	IC		void		setup				(F32 min_enemy_distance, F32 max_enemy_distance){
		inherited::setup		();
		m_min_distance			= min_enemy_distance;
		m_max_distance			= max_enemy_distance;
	}

			void		evaluate			(const CCoverPoint *cover_point, F32 weight);
};




