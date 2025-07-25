#include "stdafx.h"
#include "Snork.h"
#include "SnorkJump.h"
//#include "../jump_ability.h"
#include "../../../..\XR_3DA\skeletonanimated.h"
#include "../../../level.h"

//CSnorkJump::CSnorkJump(CSnork *monster)
//{
//	m_object				= monster;
//	
//	//m_jumper				= xr_new<CJumpingAbility>();
//	//m_jumper->init_external	(m_object);
//	//m_jumper->reinit		(MotionID(),MotionID(),MotionID());
//}
//
//CSnorkJump::~CSnorkJump()
//{
//	xr_delete(m_jumper);
//}
//
//void CSnorkJump::load(pcstr section)
//{
//	//m_jumper->load(section);
//}
//
//void CSnorkJump::update_frame()
//{
//	//if (!m_jumper->active()) return;
//
//	//if (m_specific_jump) {
//	//	f32 dist = trace_current(10.f);
//	//	if (dist > 10.f) {
//	//		m_jumper->stop();
//	//		return;
//	//	} else if (dist < 2.f) {
//	//		m_jumper->stop();
//	//		init_jump_specific();
//	//		
//	//		fVector3 dir;
//	//		f32 h,p;
//	//		f32 h2,p2;
//	//		fVector3().sub(m_target_object->Position(), m_object->Position()).getHP(h,p);
//	//		m_target_object->Direction().getHP(h2,p2);
//	//		dir.set		(1,0,0);
//	//		dir.setHP	(h,p2);
//	//		dir.normalize();
//	//		
//	//		fVector3 pos;
//	//		pos.mad		(m_target_object->Position(), dir, 4.f);
//	//		pos.y+=2.f;
//
//	//		m_jumper->jump(pos, m_velocity_mask);
//	//		m_specific_jump				= false;
//	//		m_jumper->disable_bounce	();
//	//	}
//	//}
//	//
//	//m_jumper->update_frame();
//}
//
//void CSnorkJump::try_to_jump(u32 velocity_mask)
//{
//	//CObject *target = const_cast<CEntityAlive *>(m_object->EnemyMan.get_enemy());
//	CObject *target = Level().CurrentEntity();
//	if (!target) return;
//	 
//	m_specific_jump		= false;
//	m_target_object		= target;
//	m_velocity_mask		= velocity_mask;
//
//	// 1. Get enemy position, yaw
//	fVector3 source_position		= m_object->Position();
//	fVector3 target_position;
//	target->Center				(target_position);
//
//	// �������� ������ ����������� � ��� ��� ����
//	fVector3		dir;
//	f32		dir_yaw, dir_pitch;
//
//	dir.sub		(target_position, source_position);
//	dir.getHP	(dir_yaw, dir_pitch);
//
//	// �������� �� angle � �� dist
//	f32 yaw_current, yaw_target;
//	m_object->control().direction().get_heading(yaw_current, yaw_target);
//	if (angle_difference(yaw_current, -dir_yaw) < PI_DIV_6) {
//		try_jump_normal();
//		return;
//	}
//
//	//try_jump_specific();
//}
//
//void CSnorkJump::try_jump_normal()
//{
//	if (!m_object->EnemyMan.see_enemy_now()) return;
//
//	if (m_jumper->can_jump(m_target_object)) {
//		init_jump_normal	();
//		m_jumper->jump		(m_target_object, m_velocity_mask);
//	}
//}
//
//void CSnorkJump::try_jump_specific()
//{
//	f32			yaw, pitch;
//	fVector3().sub	(m_target_object->Position(), m_object->Position()).getHP(yaw, pitch);
//
//	// �������� ������ ����������� � ��� ��� ����
//	//// �������� �� angle � �� dist
//	//if (angle_difference(m_object->movement().m_body.current.yaw, -yaw) < PI_DIV_2) {
//	//	return;
//	//}
//	
//	// 2. Trace geometry
//	m_cur_dist = trace_current(10.f);
//	if (m_cur_dist > 10.f) return;
//
//	// 3. Setup jump
//	init_jump_normal();
//
//	// 4. Perform jump
//	fVector3 pos;
//	pos.mad			(m_object->Position(), m_object->Direction(), 10.f);
//	pos.y += 1.5;
//	m_jumper->jump	(pos, m_velocity_mask);
//
//	m_jumper->disable_bounce();
//
//	m_specific_jump = true;
//}
//
//void CSnorkJump::init_jump_normal()
//{
//	MotionID			def1, def2, def3;
//	CKinematicsAnimated	*pSkel = smart_cast<CKinematicsAnimated*>(m_object->Visual());
//
//	def1				= pSkel->ID_Cycle_Safe("stand_attack_2_0");		VERIFY(def1);
//	def2				= pSkel->ID_Cycle_Safe("stand_attack_2_1");		VERIFY(def2);
//	def3				= pSkel->ID_Cycle_Safe("stand_somersault_0");	VERIFY(def3);
//
//	m_jumper->reinit	(def1, def2, def3);
//}
//
//void CSnorkJump::init_jump_specific()
//{
//	MotionID			def1, def2, def3;
//	CKinematicsAnimated	*pSkel = smart_cast<CKinematicsAnimated*>(m_object->Visual());
//
//	def1				= pSkel->ID_Cycle_Safe("stand_attack_2_0");		VERIFY(def1);
//	def2				= pSkel->ID_Cycle_Safe("jump_rs_0");			VERIFY(def2);
//	def3				= pSkel->ID_Cycle_Safe("stand_somersault_0");	VERIFY(def3);
//
//	m_jumper->reinit	(def1, def2, def3);
//}
//
//f32 CSnorkJump::trace_current(f32 dist)
//{
//	f32 ret_val = flt_max;
//	
//	BOOL				enabled = m_object->getEnabled();
//	m_object->setEnabled(FALSE);
//	collide::rq_result	l_rq;
//
//	fVector3			trace_from;
//	m_object->Center(trace_from);
//
//	f32			trace_dist = m_object->Radius() + dist;
//
//	if (Level().ObjectSpace.RayPick(trace_from, m_object->Direction(), trace_dist, collide::rqtStatic, l_rq)) {
//		if ((l_rq.range < trace_dist)) ret_val = l_rq.range;
//	}
//
//	m_object->setEnabled(enabled);
//
//	return ret_val;
//}
