#include "stdafx.h"
#include "alife_space.h"
#include "Hit.h"
#include "PHDestroyable.h"
#include "mincer.h"
#include "hudmanager.h"
#include "xrmessages.h"
#include "level.h"
#include "CustomZone.h"
#include "clsid_game.h"
#include "EntityAlive.h"
#include "PHDestroyableNotificate.h"

CMincer::CMincer( )
{
	m_fActorBlowoutRadiusPercent = 0.5f;
}

CMincer::~CMincer( )
{ }
void CMincer::OnStateSwitch(EZoneState new_state)
{
	if (m_eZoneState != eZoneStateBlowout && new_state == eZoneStateBlowout)
	{
		OBJECT_INFO_VEC_IT it;
		for (it = m_ObjectInfoMap.begin( ); m_ObjectInfoMap.end( ) != it; ++it)
		{
			CPhysicsShellHolder* GO = smart_cast<CPhysicsShellHolder*>((*it).object);
			if (GO)
			{
				Telekinesis( ).activate(GO, m_fThrowInImpulse, m_fTeleHeight, 100000);
			}
		}
	}

	if (m_eZoneState == eZoneStateBlowout && new_state != eZoneStateBlowout)
	{
		Telekinesis( ).clear_deactivate( );
	}

	inherited::OnStateSwitch(new_state);
}

void CMincer::Load(pcstr section)
{
	inherited::Load(section);

	m_telekinetics.set_destroing_particles(shared_str(pSettings->r_string(section, "tearing_particles")));
	m_telekinetics.set_throw_power(pSettings->r_float(section, "throw_out_impulse"));
	m_torn_particles = pSettings->r_string(section, "torn_particles");
	m_tearing_sound.create(pSettings->r_string(section, "body_tearing_sound"), st_Effect, sg_SourceType);
	m_fActorBlowoutRadiusPercent = pSettings->r_float(section, "actor_blowout_radius_percent");
}

BOOL CMincer::net_Spawn(CSE_Abstract* DC)
{
	BOOL result = inherited::net_Spawn(DC);
	fVector3 C;
	Center(C);
	C.y += m_fTeleHeight;
	m_telekinetics.SetCenter(C);
	m_telekinetics.SetOwnerObject(smart_cast<CGameObject*>(this));
	return result;
}
void CMincer::net_Destroy( )
{
	inherited::net_Destroy( );
	m_telekinetics.clear_impacts( );
}
void CMincer::feel_touch_new(CObject* O)
{
	inherited::feel_touch_new(O);
	if (m_eZoneState == eZoneStateBlowout && (m_dwBlowoutExplosionTime > (u32)m_iStateTime))
	{
		CPhysicsShellHolder* GO = smart_cast<CPhysicsShellHolder*>(O);
		Telekinesis( ).activate(GO, m_fThrowInImpulse, m_fTeleHeight, 100000);
	}
}
BOOL CMincer::feel_touch_contact(CObject* O)
{
	return (inherited::feel_touch_contact(O) && smart_cast<CPhysicsShellHolder*>(O));
}
void CMincer::AffectThrow(SZoneObjectInfo* O, CPhysicsShellHolder* GO, const fVector3& throw_in_dir, f32 dist)
{
	inherited::AffectThrow(O, GO, throw_in_dir, dist);
}

bool CMincer::BlowoutState( )
{
	bool ret = inherited::BlowoutState( );

	//xr_set<CObject*>::iterator it=m_inZone.begin(),e=m_inZone.end();
	//for(;e!=it;++it)
	//{
	//	CEntityAlive * EA = smart_cast<CEntityAlive *>(*it);
	//	if(!EA)continue;
	//	CPhysicsShellHolder * GO = smart_cast<CPhysicsShellHolder *>(*it);
	//	Telekinesis().activate(GO,m_fThrowInImpulse, m_fTeleHeight, 100000);

	//}

	if (m_dwBlowoutExplosionTime < (u32)m_iPreviousStateTime || m_dwBlowoutExplosionTime >= (u32)m_iStateTime)
	{
		return ret;
	}

	Telekinesis( ).deactivate( );
	return ret;
}
void CMincer::ThrowInCenter(fVector3& C)
{
	C.set(m_telekinetics.Center( ));
	C.y = Position( ).y;
}

void CMincer::Center(fVector3& C) const
{
	C.set(Position( ));
}

void CMincer::NotificateDestroy(CPHDestroyableNotificate* dn)
{
	fVector3 dir;
	f32 impulse;
	//if(!m_telekinetics.has_impacts()) return;

	//CObject* obj=Level().Objects.net_Find(id);
	CPhysicsShellHolder* obj = dn->PPhysicsShellHolder( );
	m_telekinetics.draw_out_impact(dir, impulse);
	CParticlesPlayer* PP = smart_cast<CParticlesPlayer*>(obj);
	if (PP && *m_torn_particles)
	{
		PP->StartParticles(m_torn_particles, fVector3( ).set(0.0f, 1.0f, 0.0f), ID( ));
	}

	m_tearing_sound.play_at_pos(0, m_telekinetics.Center( ));

	fVector3 position_in_bone_space;
	fVector3 throw_in_dir;
	position_in_bone_space.set(0.0f, 0.0f, 0.0f);
	throw_in_dir.set(1.0f, 0.0f, 1.0f);
	CreateHit(obj->ID( ), ID( ), throw_in_dir, 0.0f, 0, position_in_bone_space, impulse, ALife::eHitTypeExplosion);
}

void CMincer::AffectPullAlife(CEntityAlive* EA, const fVector3& throw_in_dir, f32 dist)
{
	f32 power = Power(dist);
	//fVector3 dir;
	//dir.random_dir(throw_in_dir,2.f*M_PI);
	if (EA->CLS_ID != CLSID_OBJECT_ACTOR)
	{
		fVector3 pos_in_bone_space;
		pos_in_bone_space.set(0.0f, 0.0f, 0.0f);
		CreateHit(EA->ID( ), ID( ), throw_in_dir, power, 0, pos_in_bone_space, 0.0f, m_eHitTypeBlowout);
	}

	inherited::AffectPullAlife(EA, throw_in_dir, dist);
}

f32 CMincer::BlowoutRadiusPercent(CPhysicsShellHolder* GO)
{
	return	(GO->CLS_ID != CLSID_OBJECT_ACTOR ? m_fBlowoutRadiusPercent : m_fActorBlowoutRadiusPercent);
}
