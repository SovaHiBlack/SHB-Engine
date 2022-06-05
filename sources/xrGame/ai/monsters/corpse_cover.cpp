#include "stdafx.h"
#include "corpse_cover.h"
#include "../../cover_point.h"
#include "../../ai_space.h"
#include "../../level_graph.h"

void CMonsterCorpseCoverEvaluator::evaluate			(const CCoverPoint *cover_point, F32 weight)
{
	F32					my_distance		= m_start_position.distance_to(cover_point->position());

	if (my_distance <= m_min_distance)
		return;

	if (my_distance >= m_max_distance)
		return;

	Fvector					direction;
	F32						y;
	F32						p;
	direction.sub			(m_start_position,cover_point->position());
	direction.getHP			(y,p);
	
	F32					cover_value = ai().level_graph().cover_in_direction(y,cover_point->level_vertex_id());
	if (cover_value >= 2.f*m_best_value)
		return;

	m_selected				= cover_point;
	m_best_value			= cover_value;
}

