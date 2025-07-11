#include "stdafx.h"
#include "poltergeist.h"
#include "../../../PhysicsShellHolder.h"
#include "../../../level.h"
#include "../../../Actor.h"

CPolterTele::CPolterTele(CPoltergeist *polter) : inherited (polter)
{ }

CPolterTele::~CPolterTele()
{ }

void CPolterTele::load(pcstr section)
{
	inherited::load(section);

	m_pmt_radius						= READ_IF_EXISTS(pSettings,r_float,section,	"Tele_Find_Radius",					10.f);
	m_pmt_object_min_mass				= READ_IF_EXISTS(pSettings,r_float,section,	"Tele_Object_Min_Mass",				40.f);
	m_pmt_object_max_mass				= READ_IF_EXISTS(pSettings,r_float,section,	"Tele_Object_Max_Mass",				500.f);
	m_pmt_object_count					= READ_IF_EXISTS(pSettings,r_u32,section,	"Tele_Object_Count",				10);
	m_pmt_time_to_hold					= READ_IF_EXISTS(pSettings,r_u32,section,	"Tele_Hold_Time",					3000);
	m_pmt_time_to_wait					= READ_IF_EXISTS(pSettings,r_u32,section,	"Tele_Wait_Time",					3000);
	m_pmt_time_to_wait_in_objects		= READ_IF_EXISTS(pSettings,r_u32,section,	"Tele_Delay_Between_Objects_Time",	500);
	m_pmt_distance						= READ_IF_EXISTS(pSettings,r_float,section,	"Tele_Distance",					50.f);
	m_pmt_object_height					= READ_IF_EXISTS(pSettings,r_float,section,	"Tele_Object_Height",				10.f);
	m_pmt_time_object_keep				= READ_IF_EXISTS(pSettings,r_u32,section,	"Tele_Time_Object_Keep",			10000);
	m_pmt_raise_speed					= READ_IF_EXISTS(pSettings,r_float,section,	"Tele_Raise_Speed",					3.f);
	m_pmt_raise_time_to_wait_in_objects	= READ_IF_EXISTS(pSettings,r_u32,section,	"Tele_Delay_Between_Objects_Raise_Time", 500);
	m_pmt_fly_velocity					= READ_IF_EXISTS(pSettings,r_float,section, "Tele_Fly_Velocity",				30.f);

	::Sound->create						(m_sound_tele_hold,		pSettings->r_string(section,"sound_tele_hold"),	st_Effect, eST_WORLD);
	::Sound->create						(m_sound_tele_throw,	pSettings->r_string(section,"sound_tele_throw"),st_Effect, eST_WORLD);

	m_state								= eWait;
	m_time								= 0;
	m_time_next							= 0;
}

void CPolterTele::update_schedule()
{
	inherited::update_schedule();

	if (!m_object->g_Alive() || !Actor() || !Actor()->g_Alive()) return;
	if (Actor()->Position().distance_to(m_object->Position()) > m_pmt_distance) return;

	switch (m_state) {
	case eStartRaiseObjects:	
		if (m_time + m_time_next < time()) {
			if (!tele_raise_objects())
				m_state	= eRaisingObjects;
			
			m_time			= time();
			m_time_next		= m_pmt_raise_time_to_wait_in_objects / 2 + Random.randI(m_pmt_raise_time_to_wait_in_objects / 2);
		}

		if (m_state == eStartRaiseObjects) {
			if (m_object->CTelekinesis::get_objects_count() >= m_pmt_object_count) {
				m_state		= eRaisingObjects;
				m_time		= time();
			}
		}

		break;
	case eRaisingObjects:
		if (m_time + m_pmt_time_to_hold > time()) break;
		
		m_time				= time();
		m_time_next		= 0;
		m_state			= eFireObjects;
	case eFireObjects:			
		if (m_time + m_time_next < time()) {
			tele_fire_objects	();
			
			m_time			= time();
			m_time_next	= m_pmt_time_to_wait_in_objects / 2 + Random.randI(m_pmt_time_to_wait_in_objects / 2);
		}
		
		if (m_object->CTelekinesis::get_objects_count() == 0) {
			m_state		= eWait;
			m_time			= time();
		}
		break;
	case eWait:				
		if (m_time + m_pmt_time_to_wait < time()) {
			m_time_next	= 0;
			m_state		= eStartRaiseObjects;
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// ����� ���������� �������� ��� ����������
//////////////////////////////////////////////////////////////////////////
class best_object_predicate
{
	fVector3 enemy_pos;
	fVector3 monster_pos;

public:
	best_object_predicate(const fVector3& m_pos, const fVector3& pos) {
		monster_pos = m_pos;
		enemy_pos	= pos;
	}

	bool operator()	 (const CGameObject *tpObject1, const CGameObject *tpObject2) const
	{
		f32 dist1 = monster_pos.distance_to(tpObject1->Position());
		f32 dist2 = enemy_pos.distance_to(tpObject2->Position());
		f32 dist3 = enemy_pos.distance_to(monster_pos);

		return ((dist1 < dist3) && (dist2 > dist3));
	};
};

class best_object_predicate2
{
	fVector3 enemy_pos;
	fVector3 monster_pos;

public:
	typedef CObject*	CObject_ptr;

	best_object_predicate2(const fVector3& m_pos, const fVector3& pos) {
		monster_pos = m_pos;
		enemy_pos	= pos;
	}

	bool operator()	 (const CObject_ptr &tpObject1, const CObject_ptr &tpObject2) const
	{
		f32 dist1 = enemy_pos.distance_to(tpObject1->Position());
		f32 dist2 = enemy_pos.distance_to(tpObject2->Position());

		return (dist1 < dist2);		
	};
};

//////////////////////////////////////////////////////////////////////////

bool CPolterTele::trace_object(CObject *obj, const fVector3& target)
{
	fVector3			trace_from;
	obj->Center		(trace_from);
	
	fVector3			dir;
	f32			range;
	dir.sub			(target, trace_from);
	
	range			= dir.magnitude();
	dir.normalize	();

	collide::rq_result	l_rq;
	if (Level().ObjectSpace.RayPick(trace_from, dir, range, collide::rqtBoth, l_rq, obj)) {
		if (l_rq.O == Actor()) return true;
	}

	return false;
}

void CPolterTele::tele_find_objects(xr_vector<CObject*> &objects, const fVector3& pos)
{
	m_nearest.clear_not_free		();
	Level().ObjectSpace.GetNearest	(m_nearest, pos, m_pmt_radius, NULL);

	for (u32 i=0;i<m_nearest.size();i++) {
		CPhysicsShellHolder *obj			= smart_cast<CPhysicsShellHolder *>(m_nearest[i]);
		CCustomMonster		*custom_monster	= smart_cast<CCustomMonster *>(m_nearest[i]);
		if (!obj || 
			!obj->PPhysicsShell() || 
			!obj->PPhysicsShell()->isActive()|| 
			custom_monster ||
			(obj->spawn_ini() && obj->spawn_ini()->section_exist("ph_heavy")) || 
			(obj->m_pPhysicsShell->getMass() < m_pmt_object_min_mass) || 
			(obj->m_pPhysicsShell->getMass() > m_pmt_object_max_mass) || 
			(obj == m_object) || 
			m_object->CTelekinesis::is_active_object(obj) || 
			!obj->m_pPhysicsShell->get_ApplyByGravity()) continue;
		
		fVector3 center;
		Actor()->Center(center);
		
		if (trace_object(obj, center) || trace_object(obj, get_head_position(Actor())))
			objects.push_back(obj);
	}
}

bool CPolterTele::tele_raise_objects()
{
	// find objects near actor
	xr_vector<CObject*>		tele_objects;
	tele_objects.reserve	(20);

	// �������� ������ �������� ������ �����
	tele_find_objects	(tele_objects, Actor()->Position());

	// �������� ������ �������� ������ �������
	tele_find_objects	(tele_objects, m_object->Position());

	// �������� ������ �������� ����� �������� � ������
	f32 dist			= Actor()->Position().distance_to(m_object->Position());
	fVector3 dir;
	dir.sub				(Actor()->Position(), m_object->Position());
	dir.normalize		();

	fVector3 pos;
	pos.mad				(m_object->Position(), dir, dist / 2.f);
	tele_find_objects	(tele_objects, pos);	

	// ����������� � �������� ������ ����������� ���������� ��������
	std::sort(tele_objects.begin(),tele_objects.end(),best_object_predicate2(m_object->Position(), Actor()->Position()));
	
	// �������� ���������� �������
	tele_objects.erase	(
		std::unique(
			tele_objects.begin(),
			tele_objects.end()
		),
		tele_objects.end()
	);

	// �������� ����������� ���������� ��������
	//if (tele_objects.size() > m_pmt_tele_object_count) tele_objects.resize	(m_pmt_tele_object_count);

	//// ������������
	//for (u32 i=0; i<tele_objects.size(); i++) {
	//	CPhysicsShellHolder *obj = smart_cast<CPhysicsShellHolder *>(tele_objects[i]);

	//	// ��������� ��������� �� ������
	//	bool	rotate = false;

	//	CTelekinesis::activate		(obj, m_pmt_tele_raise_speed, m_pmt_tele_object_height, m_pmt_tele_time_object_keep, rotate);
	//}

	if (!tele_objects.empty()) {
		CPhysicsShellHolder *obj = smart_cast<CPhysicsShellHolder *>(tele_objects[0]);

		// ��������� ��������� �� ������
		bool	rotate = false;

		CTelekineticObject		*tele_obj = m_object->CTelekinesis::activate		(obj, m_pmt_raise_speed, m_pmt_object_height, m_pmt_time_object_keep, rotate);
		tele_obj->set_sound		(m_sound_tele_hold,m_sound_tele_throw);

		return true;
	}

	return false;
}

void CPolterTele::tele_fire_objects()
{
	for (u32 i=0; i < m_object->CTelekinesis::get_objects_total_count();i++) {
		CTelekineticObject tele_object = m_object->CTelekinesis::get_object_by_index(i);
		//if (tele_object.get_state() != TS_Fire) {
		if ((tele_object.get_state() == TS_Raise) || (tele_object.get_state() == TS_Keep))  {
			fVector3					enemy_pos;
			enemy_pos				= get_head_position(Actor());
			m_object->CTelekinesis::fire_t	(tele_object.get_object(),enemy_pos, tele_object.get_object()->Position().distance_to(enemy_pos) / m_pmt_fly_velocity);
			return;
		}
	}
}
