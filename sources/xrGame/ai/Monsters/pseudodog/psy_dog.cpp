#include "stdafx.h"
#include "psy_dog.h"
#include "../../../level_graph.h"
#include "../../../AISpace.h"
#include "../../../alife_simulator.h"
#include "../../../xrServer_Object_Base.h"
#include "../../../xrserver.h"
#include "../../../ai_object_location.h"
#include "../../../level.h"
#include "../control_movement_base.h"
#include "../monster_velocity_space.h"
#include "../../../restricted_object.h"
#include "../../../Actor.h"
#include "../ai_monster_effector.h"
#include "../../../ActorEffector.h"
#include "psy_dog_aura.h"
#include "psy_dog_state_manager.h"
#include "../../../alife_object_registry.h"
#include "../../../xrServer_Objects_Alife_Monsters.h"

CPsyDog::CPsyDog()
{
	m_aura = xr_new<CPsyDogAura>(this);
}

CPsyDog::~CPsyDog()
{
	xr_delete(m_aura);
}

void CPsyDog::Load(pcstr section)
{
	inherited::Load(section);
	
	m_aura->load(pSettings->r_string(section,"aura_effector"));

	m_phantoms_max			= pSettings->r_u8(section,"Max_Phantoms_Count");
	m_phantoms_min			= pSettings->r_u8(section,"Min_Phantoms_Count");
	m_time_phantom_appear	= pSettings->r_u32(section,"Time_Phantom_Appear");
}

BOOL CPsyDog::net_Spawn(CSE_Abstract *dc)
{
	if (!inherited::net_Spawn(dc)) return FALSE;

	return TRUE;
}

void CPsyDog::reinit()
{
	inherited::reinit	();
	m_aura->reinit		();
	m_time_last_phantom_appear = 0;
}

void CPsyDog::reload(pcstr section)
{
	inherited::reload(section);
}

//////////////////////////////////////////////////////////////////////////
// Register/Unregister
//////////////////////////////////////////////////////////////////////////

void CPsyDog::register_phantom(CPsyDogPhantom *phantom)
{
 	m_storage.push_back(phantom);
}

void CPsyDog::unregister_phantom(CPsyDogPhantom *phantom)
{
	xr_vector<CPsyDogPhantom*>::iterator it = std::find(m_storage.begin(),m_storage.end(), phantom);
	VERIFY(it != m_storage.end());
	m_storage.erase(it);
}

//////////////////////////////////////////////////////////////////////////
// Spawn phantom
//////////////////////////////////////////////////////////////////////////

bool CPsyDog::spawn_phantom()
{
	u32 node;
	if (!control().path_builder().get_node_in_radius(ai_location().level_vertex_id(), 4,8,5,node)) return false;
	
 	// set id to created server object
	CSE_Abstract			*phantom = Level().spawn_item("psy_dog_phantom", ai().level_graph().vertex_position(node), node, 0xffff, true);
	CSE_ALifeMonsterBase	*pSE_Monster = smart_cast<CSE_ALifeMonsterBase*>(phantom);
	VERIFY(pSE_Monster);

	pSE_Monster->m_spec_object_id = ID();
	
	// spawn here
	CNetPacket					P;
	phantom->Spawn_Write		(P,TRUE);
	Level().Send				(P,net_flags(TRUE));
	F_entity_Destroy			(phantom);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// Destroy all phantoms
//////////////////////////////////////////////////////////////////////////
void CPsyDog::delete_all_phantoms()
{
	for (xr_vector<CPsyDogPhantom*>::iterator it = m_storage.begin(); it != m_storage.end(); it++) 
		(*it)->destroy_from_parent();

	m_storage.clear();
}

void CPsyDog::Think()
{
	inherited::Think();
	if (!g_Alive()) return;
	
	m_aura->update_schedule();

	// check spawn / destroy phantoms
	if (EnemyMan.get_enemy() && (get_phantoms_count() < m_phantoms_max) && (m_time_last_phantom_appear + m_time_phantom_appear < time())){
		if (spawn_phantom()) m_time_last_phantom_appear = time();
	}else {
		if (!EnemyMan.get_enemy() && !m_storage.empty()) {
			delete_all_phantoms();
		}
	}
}

void CPsyDog::net_Destroy()
{
	delete_all_phantoms	();
	inherited::net_Destroy();
}

void CPsyDog::Die(CObject* who)
{
	inherited::Die		(who);
	m_aura->on_death	();
	delete_all_phantoms	();
}

IStateManagerBase *CPsyDog::create_state_manager()
{
	return xr_new<CStateManagerPsyDog>(this);
}

u8 CPsyDog::get_phantoms_count()
{
	return u8(m_storage.size());
}

//////////////////////////////////////////////////////////////////////////
// Phantom Psy Dog
//////////////////////////////////////////////////////////////////////////
CPsyDogPhantom::CPsyDogPhantom()
{ }

CPsyDogPhantom::~CPsyDogPhantom()
{ }

BOOL CPsyDogPhantom::net_Spawn(CSE_Abstract *dc)
{
	if (!inherited::net_Spawn(dc)) return FALSE;

	CSE_ALifeMonsterBase *se_monster	= smart_cast<CSE_ALifeMonsterBase*>(dc);
	m_parent_id = se_monster->m_spec_object_id;
	m_parent	= 0;
	VERIFY		(m_parent_id != 0xffff);
	
	try_to_register_to_parent();

	setVisible	(FALSE);
	setEnabled	(FALSE);

	// load effector
	// Load psi postprocess --------------------------------------------------------
	
	load_effector(*cNameSect(), "appear_effector",m_appear_effector);
	
	// --------------------------------------------------------------------------------
	m_particles_appear		= pSettings->r_string(*cNameSect(), "particles_appear");
	m_particles_disappear	= pSettings->r_string(*cNameSect(), "particles_disappear");

	m_time_spawned			= time();

	return (TRUE);
}

const u32 pmt_time_wait_parent = 10000;

void CPsyDogPhantom::Think()
{
	if (is_wait_to_destroy_object()) return;
	inherited::Think();

	try_to_register_to_parent();

	// still have no parent ?
	if (!m_parent) {
		// if there is no parent long period of time - destroy me
		if (m_time_spawned + pmt_time_wait_parent > time()) destroy_me();		
		return;
	}

	if (m_state != eWaitToAppear) return;

	EnemyMan.transfer_enemy(m_parent);
	
	SVelocityParam &velocity_run = move().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
	if (control().movement().real_velocity() < 2*velocity_run.velocity.linear/3) return;
	if (!EnemyMan.get_enemy()) return;
	if (!control().direction().is_face_target(EnemyMan.get_enemy(), PI_DIV_6)) return;

	fVector3 target;
	target.mad(Position(),Direction(), 10.0f);

	// ���� � ������ ���������?
	control().path_builder().restrictions().add_border(Position(), target);
	u32 node = ai().level_graph().check_position_in_direction(ai_location().level_vertex_id(),Position(),target);
	control().path_builder().restrictions().remove_border();

	if (!ai().level_graph().valid_vertex_id(node) || !control().path_builder().accessible(node)) return;

	target.y += 1.f;
	com_man().jump	(target);
	m_state			= eAttack;
	
	setVisible		(TRUE);
	setEnabled		(TRUE);

	CParticlesPlayer::StartParticles(m_particles_appear, fVector3().set(0.0f,0.1f,0.0f),ID());

	if (EnemyMan.get_enemy() != Actor()) return;

	Actor()->Cameras().AddCamEffector(xr_new<CMonsterEffectorHit>(m_appear_effector.ce_time,m_appear_effector.ce_amplitude,m_appear_effector.ce_period_number,m_appear_effector.ce_power));
	Actor()->Cameras().AddPPEffector(xr_new<CMonsterEffector>(m_appear_effector.ppi, m_appear_effector.time, m_appear_effector.time_attack, m_appear_effector.time_release));
}

void	CPsyDogPhantom::Hit					(SHit* pHDS)
{
	if (is_wait_to_destroy_object()) return;
	if ((pHDS->who == EnemyMan.get_enemy())  && (pHDS->who != 0)) destroy_me();
}

void CPsyDogPhantom::net_Destroy()
{
	fVector3 center;
	Center(center);
	PlayParticles(m_particles_disappear,center, fVector3().set(0.0f,1.0f,0.0f));
	
	if (m_parent && !is_wait_to_destroy_object()) {
		m_parent->unregister_phantom	(this);
		m_parent						= 0;
		m_parent_id						= 0xffff;
	}

	inherited::net_Destroy();
}

void CPsyDogPhantom::Die(CObject* who)
{
	inherited::Die	(who);
	destroy_me		();
}

void CPsyDogPhantom::try_to_register_to_parent()
{
	// parent not ready yet
	if(m_parent) return;
	
	CObject	*obj = Level().Objects.net_Find(m_parent_id);
	if (obj) {
		CPsyDog *dog = smart_cast<CPsyDog *>(obj);
		VERIFY(dog);
		
		m_parent = dog;
		m_parent->register_phantom	(this);

		m_state						= eWaitToAppear;
	}
}

void CPsyDogPhantom::destroy_me() 
{
	VERIFY(!is_wait_to_destroy_object());

	if (m_parent) {
		m_parent->unregister_phantom	(this);
		m_parent						= 0;
		m_parent_id						= 0xffff;
	}

	CNetPacket		P;
	u_EventGen		(P,GE_DESTROY,ID());
	u_EventSend		(P);
}

void CPsyDogPhantom::destroy_from_parent()
{
	m_parent_id		= 0xffff;

	CNetPacket		P;
	u_EventGen		(P,GE_DESTROY,ID());
	u_EventSend		(P);
}
