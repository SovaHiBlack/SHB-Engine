#include "stdafx.h"
#include "control_path_builder_base.h"
#include "../../cover_point.h"
#include "../../cover_manager.h"
#include "../../cover_evaluators.h"
#include "BaseMonster/base_monster.h"
#include "../../detail_path_manager.h"
#include "../../level_location_selector.h"
#include "../../level_path_manager.h"
#include "../../ai_object_location.h"

const f32		pmt_find_point_dist				= 30.f;
const u32		pmt_find_random_pos_attempts	= 5;

//////////////////////////////////////////////////////////////////////////
bool CControlPathBuilderBase::target_point_need_update()
{
	if ((m_state & eStatePathFailed) == eStatePathFailed)
		return true;
	else if (m_state == eStatePathValid) {
		
		// ���� ���� ��� �� ��������
		if (!m_man->path_builder().is_path_end(m_distance_to_path_end)) {

			if (m_target_actual && !global_failed()) return false;  // ���� global_failed - ������������ ������������

			// ���� ������ ��� ������
			if (m_last_time_target_set == 0) return true;

			// ���� ����� �������� �� ���� �� �����, �� �������������
			return (m_last_time_target_set + m_time < time());
		}
	
		//return (!m_target_actual); // ���������� ����� ����
		return (true);
	//} else if ((m_state & eStateWaitParamsApplied) == eStateWaitParamsApplied) {
	//	return false;
	} else if ((m_state & eStateWaitNewPath) == eStateWaitNewPath) {
		return false;
	} else if ((m_state & eStateNoPath) == eStateNoPath) {
		return true;
	} else if ((m_state & eStatePathEnd) == eStatePathEnd) {
		if (m_target_set.node != m_object->ai_location().level_vertex_id())
			return true; // ���������� ����� ����
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
// ���������� m_target_found
// �� ����� ���� ������������� ���� � ������� m_target_set
void CControlPathBuilderBase::find_target_point_set()
{
	m_target_found.set(m_target_set.position,m_target_set.node);
	
	//---------------------------------------------------
	// ������� �����

	if (m_target_type == eMoveToTarget) {
		// 1. ������� ���� �� ������������ ����
		if (m_man->path_builder().valid_and_accessible(m_target_found.position,m_target_found.node)) 
			return;

		// 2. ������� ���� �� �������������� ���� (������� ��������� �������)
		if (!m_man->path_builder().accessible(m_target_found.position)) {
			m_target_found.node = m_man->path_builder().restrictions().accessible_nearest(fVector3().set(m_target_found.position),m_target_found.position);
			
			fVector3	pos_random;
			fVector3 dir;
			dir.random_dir			();

			pos_random.mad			(m_object->Position(), dir, pmt_find_point_dist);
			set_target_accessible	(m_target_found, pos_random);

			if (m_target_found.node != u32(-1)) return;
		}
	}

	m_target_found.node = u32(-1);
	
	//---------------------------------------------------
	// I. ������� �������

	if (m_target_type == eRetreatFromTarget) {
		fVector3	dir;
		dir.sub						(m_object->Position(), m_target_found.position);
		dir.normalize_safe			();
		m_target_found.position.mad	(m_object->Position(), dir, pmt_find_point_dist);
	}

	// ��������� ������� �� accessible
	if (!m_man->path_builder().accessible(m_target_found.position)) {
		m_target_found.node = m_man->path_builder().restrictions().accessible_nearest(fVector3().set(m_target_found.position),m_target_found.position);
	}
	
	// ���� ����� ������� = ������� ������� - ������� ��������� �������� �������
	for (u32 i = 0; i < pmt_find_random_pos_attempts; i++ ) {
		if (m_target_found.position.similar(m_object->Position(), 0.5f)) {
			
			fVector3	pos_random;
			fVector3 dir;
			dir.random_dir			();

			pos_random.mad			(m_object->Position(), dir, pmt_find_point_dist);
			set_target_accessible	(m_target_found, pos_random);
		} else break;
	}

	if (m_target_found.node != u32(-1)) return;

	//---------------------------------------------------
	// II. ������� �������, ���� ����
	
	find_node();
}

//////////////////////////////////////////////////////////////////////////
// if path FAILED
void CControlPathBuilderBase::find_target_point_failed() 
{
	// ���� ����� ������� = ������� ������� - ������� ��������� �������� �������
	for (u32 i = 0; i < pmt_find_random_pos_attempts; i++ ) {
		fVector3	pos_random;
		fVector3 dir;
		dir.random_dir			();

		pos_random.mad			(m_object->Position(), dir, pmt_find_point_dist);
		set_target_accessible	(m_target_found, pos_random);

		if (!m_target_found.position.similar(m_object->Position(), 0.5f)) break;
	}

	if (m_target_found.node != u32(-1)) return;

	//---------------------------------------------------
	// II. ������� �������, ���� ����
	find_node();
}



void CControlPathBuilderBase::find_node()
{
	// ���� � ������ ���������?
	m_man->path_builder().restrictions().add_border		(m_object->Position(), m_target_found.position);
	m_target_found.node	= ai().level_graph().check_position_in_direction(m_object->ai_location().level_vertex_id(),m_object->Position(),m_target_found.position);
	m_man->path_builder().restrictions().remove_border	();

	if (ai().level_graph().valid_vertex_id(m_target_found.node) && m_man->path_builder().accessible(m_target_found.node)) {
		// ������������� �������
		m_man->path_builder().fix_position(fVector3().set(m_target_found.position), m_target_found.node, m_target_found.position);
		return;
	}

	// ������ ���� �� ������� �������
	if (ai().level_graph().valid_vertex_position(m_target_found.position)) {
		m_target_found.node = ai().level_graph().vertex_id(m_target_found.position);
		if (ai().level_graph().valid_vertex_id(m_target_found.node) && m_man->path_builder().accessible(m_target_found.node)) {
			// ������������� �������
			m_man->path_builder().fix_position(fVector3().set(m_target_found.position), m_target_found.node, m_target_found.position);
			return;
		}
	}

	// ������� � ������� �������
	if (m_cover_info.use_covers) {
		m_cover_approach->setup	(m_target_found.position, m_cover_info.min_dist, m_cover_info.max_dist, m_cover_info.deviation);
		const CCoverPoint	*point = ai().cover_manager().best_cover(m_object->Position(),m_cover_info.radius,*m_cover_approach);
		// ����� �����?	
		if (point) {
			m_target_found.node				= point->m_level_vertex_id;
			m_target_found.position			= point->m_position;	
			return;
		}
	}

	// ���� �� �������. �� ��������� ����� ����� ����������� ��������
	m_target_found.node		= m_man->path_builder().find_nearest_vertex(m_object->ai_location().level_vertex_id(),m_target_found.position,30.f);
	m_target_found.position = ai().level_graph().vertex_position(m_target_found.node);
}

