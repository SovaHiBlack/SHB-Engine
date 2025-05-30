#include "stdafx.h"

#include "PHCollisionDamageReceiver.h"
#include "PhysicObject.h"
#include "Hit.h"
#include "PHDestroyable.h"
#include "HitImmunity.h"
#include "damage_manager.h"
#include "DestroyablePhysicsObject.h"
#include "..\XR_3DA\SkeletonCustom.h"
#include "xrServer_Objects_ALife.h"
#include "game_object_space.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
#include "PhysicsShell.h"
#ifdef DEBUG
#include "PHWorld.h"
extern CPHWorld			*ph_world;
#endif
CDestroyablePhysicsObject ::CDestroyablePhysicsObject()
{
	m_fHealth=1.f;
}

CDestroyablePhysicsObject::~CDestroyablePhysicsObject()
{

}
void CDestroyablePhysicsObject::OnChangeVisual()
{
	if (m_pPhysicsShell){
		if(m_pPhysicsShell)m_pPhysicsShell->Deactivate();
		xr_delete		(m_pPhysicsShell);
		VERIFY			(0==Visual());
	}
	inherited::OnChangeVisual();
}
CPhysicsShellHolder*	 CDestroyablePhysicsObject ::	PPhysicsShellHolder			()
{
	return cast_physics_shell_holder();
}

void CDestroyablePhysicsObject::net_Destroy()
{
	inherited::net_Destroy();
	CPHDestroyable::RespawnInit();
	CPHCollisionDamageReceiver::Clear();
}

BOOL CDestroyablePhysicsObject::net_Spawn(CSE_Abstract* DC)
{
	BOOL res=inherited::net_Spawn(DC);
	CKinematics		*K=smart_cast<CKinematics*>(Visual());
	CIniFile* ini=K->LL_UserData();
	//R_ASSERT2(ini->section_exist("destroyed"),"destroyable_object must have -destroyed- section in model user data");
	CPHDestroyable::Init();
	if(ini&&ini->section_exist("destroyed"))
		CPHDestroyable::Load(ini,"destroyed");
	
	CDamageManager::reload("damage_section",ini);
	if(ini){	
		if(ini->section_exist("immunities"))		CHitImmunity::LoadImmunities("immunities",ini);
		CPHCollisionDamageReceiver::Init();
		if(ini->section_exist("sound"))				m_destroy_sound.create(ini->r_string("sound","break_sound"),st_Effect,sg_SourceType);
		if(ini->section_exist("particles"))			m_destroy_particles=ini->r_string("particles","destroy_particles");
	}
	CParticlesPlayer::LoadParticles(K);
	RunStartupAnim(DC);
	return res;
}

void	CDestroyablePhysicsObject::Hit					(SHit* pHDS)
{
	SHit	HDS = *pHDS;
	callback(GameObject::eHit)(
		lua_game_object(), 
		HDS.power,
		HDS.dir,
		smart_cast<const CGameObject*>(HDS.who)->lua_game_object(),
		HDS.bone()
		);
	HDS.power=CHitImmunity::AffectHit(HDS.power,HDS.hit_type);
	f32 hit_scale = 1.0f;
	f32 wound_scale = 1.0f;
	CDamageManager::HitScale(HDS.bone(),hit_scale,wound_scale);
	HDS.power*=hit_scale;

	inherited::Hit(&HDS);
	m_fHealth-=HDS.power;
	if(m_fHealth<=0.f)
	{
		CPHDestroyable::SetFatalHit(HDS);
		if(CPHDestroyable::CanDestroy())Destroy();
	}
}
void CDestroyablePhysicsObject::Destroy()
{
	VERIFY(!ph_world->Processing());
	const CGameObject *who_object = smart_cast<const CGameObject*>(FatalHit().initiator());
	callback(GameObject::eDeath)(lua_game_object( ), who_object ? who_object->lua_game_object( ) : 0);
	CPHDestroyable::Destroy(ID(),"physic_destroyable_object");
	if(m_destroy_sound._handle())
	{
		m_destroy_sound.play_at_pos(this,Position());
	}
	if(*m_destroy_particles)
	{		
			//fVector3 dir;dir.set(0,1,0);
		fMatrix4x4 m;
		m.identity();
		/////////////////////////////////////////////////
		m.j.set(0,1.f,0);
		///////////////////////////////////////////////

		fVector3 hdir;
		hdir.set(CPHDestroyable::FatalHit().direction());

		if(fsimilar(_abs(m.j.dotproduct(hdir)),1.f, EPSILON_3))
		{
			do {
				hdir.random_dir();
			} while(fsimilar(_abs(m.j.dotproduct(hdir)),1.f, EPSILON_3));
		}

		m.i.crossproduct(m.j, hdir);
		m.i.normalize( );
		m.k.crossproduct(m.i, m.j);
		StartParticles(m_destroy_particles, m, ID( ));
	}
	SheduleRegister();
}
void CDestroyablePhysicsObject::InitServerObject(CSE_Abstract* D)
{
	CSE_PHSkeleton					*ps = smart_cast<CSE_PHSkeleton*>(D);
	R_ASSERT						(ps);
	if(ps->_flags.test(CSE_PHSkeleton::flSpawnCopy)) 
									inherited::InitServerObject(D);
	else					
									CPHDestroyable::InitServerObject(D);

	CSE_ALifeObjectPhysic			*PO = smart_cast<CSE_ALifeObjectPhysic*>(D);
	if(PO)PO->type=epotSkeleton;
}
void CDestroyablePhysicsObject::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	CPHDestroyable::SheduleUpdate(dt);
}

bool CDestroyablePhysicsObject::CanRemoveObject()
{
	return !CParticlesPlayer::IsPlaying()&& !m_destroy_sound._feedback();//&& sound!
}
DLL_Pure	*CDestroyablePhysicsObject::_construct()
{
	
	CDamageManager::_construct();
	return inherited::_construct();
}