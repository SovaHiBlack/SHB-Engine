///////////////////////////////////////////////////////////////
// BlackGraviArtefact.cpp
// BlackGraviArtefact - гравитационный артефакт, 
// такой же как и обычный, но при получении хита
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BlackGraviArtefact.h"
#include "PhysicsShell.h"
#include "EntityAlive.h"
#include "ParticlesObject.h"
#include "PHMovementControl.h"
#include "xrmessages.h"
#include "physicsshellholder.h"
#include "Explosive.h"
#include "../../xrNetServer/net_utils.h"
#include "PHWorld.h"
#include "CharacterPhysicsSupport.h"

extern CPHWorld* ph_world;
CBlackGraviArtefact::CBlackGraviArtefact( )
{
	m_fImpulseThreshold = 10.0f;
	m_fRadius = 10.0f;
	m_fStrikeImpulse = 50.0f;

	m_bStrike = false;
}

CBlackGraviArtefact::~CBlackGraviArtefact( )
{
	m_GameObjectList.clear( );
}

void CBlackGraviArtefact::Load(pcstr section)
{
	inherited::Load(section);

	m_fImpulseThreshold = pSettings->r_float(section, "impulse_threshold");
	m_fRadius = pSettings->r_float(section, "radius");
	m_fStrikeImpulse = pSettings->r_float(section, "strike_impulse");
	m_sParticleName = pSettings->r_string(section, "particle");
}

BOOL CBlackGraviArtefact::net_Spawn(CSE_Abstract* DC)
{
	if (!inherited::net_Spawn(DC))
	{
		return FALSE;
	}

	CParticlesObject* pStaticPG;
	pStaticPG = CParticlesObject::Create("anomaly\\galantine", FALSE);
	fMatrix4x4 pos;
	pos.scale(0.7f, 0.7f, 0.7f);
	pos.translate_over(XFORM( ).c);

	fVector3 vel;
	vel.set(0.0f, 0.0f, 0.0f);
	pStaticPG->UpdateParent(pos, vel);
	pStaticPG->Play( );

	return TRUE;
}
struct SRP
{
	const CPhysicsShellHolder* obj;
	SRP(const CPhysicsShellHolder* O)
	{
		obj = O;
	}
	bool operator	() (CPhysicsShellHolder* O) const
	{
		return obj == O;
	}
};
void CBlackGraviArtefact::net_Relcase(CObject* O)
{
	inherited::net_Relcase(O);
	//for vector
	GAME_OBJECT_LIST_it I = std::remove_if(m_GameObjectList.begin( ), m_GameObjectList.end( ), SRP(smart_cast<CPhysicsShellHolder*>(O)));
	m_GameObjectList.erase(I, m_GameObjectList.end( ));
}

void CBlackGraviArtefact::UpdateCLChild( )
{
	VERIFY(!ph_world->Processing( ));
	inherited::UpdateCLChild( );

	if (getVisible( ) && m_pPhysicsShell)
	{
		if (m_bStrike)
		{
			fVector3 P;
			P.set(Position( ));
			feel_touch_update(P, m_fRadius);

			GraviStrike( );

			CParticlesObject* pStaticPG;
			pStaticPG = CParticlesObject::Create(*m_sParticleName, TRUE);
			fMatrix4x4 pos;
			pos.set(XFORM( ));
			fVector3 vel;
			vel.set(0.0f, 0.0f, 0.0f);
			pStaticPG->UpdateParent(pos, vel);
			pStaticPG->Play( );

			m_bStrike = false;
		}
	}
	else if (H_Parent( ))
	{
		XFORM( ).set(H_Parent( )->XFORM( ));
	}
}

void	CBlackGraviArtefact::Hit(SHit* pHDS)
{
	SHit HDS = *pHDS;
	if (HDS.impulse > m_fImpulseThreshold)
	{
		m_bStrike = true;
		//чтоб выстрел не повлиял на траекторию полета артефакта
		HDS.impulse = 0;
	}

	inherited::Hit(&HDS);
}

void CBlackGraviArtefact::feel_touch_new(CObject* O)
{
	CPhysicsShellHolder* pGameObject = smart_cast<CPhysicsShellHolder*>(O);
	CArtefact* pArtefact = smart_cast<CArtefact*>(O);

	if (pGameObject && !pArtefact)
	{
		m_GameObjectList.push_back(pGameObject);
	}
}

void CBlackGraviArtefact::feel_touch_delete(CObject* O)
{
	CGameObject* pGameObject = static_cast<CGameObject*>(O);
	CArtefact* pArtefact = smart_cast<CArtefact*>(O);

	if (pGameObject && !pArtefact)
	{
		m_GameObjectList.erase(std::find(m_GameObjectList.begin( ), m_GameObjectList.end( ), pGameObject));
	}
}

BOOL CBlackGraviArtefact::feel_touch_contact(CObject* O)
{
	CGameObject* pGameObject = static_cast<CGameObject*>(O);
	if (pGameObject)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CBlackGraviArtefact::GraviStrike( )
{
	xr_list<s16> elements_list;
	xr_list<fVector3> bone_position_list;

	fVector3 object_pos;
	fVector3 strike_dir;

	rq_storage.r_clear( );

	for (GAME_OBJECT_LIST_it it = m_GameObjectList.begin( ); m_GameObjectList.end( ) != it; ++it)
	{
		CPhysicsShellHolder* pGameObject = *it;

		if (pGameObject->Visual( ))
		{
			pGameObject->Center(object_pos);
		}
		else
		{
			object_pos.set(pGameObject->Position( ));
		}

		strike_dir.sub(object_pos, Position( ));
		f32 distance = strike_dir.magnitude( );

		f32 impulse = 100.0f * m_fStrikeImpulse * (1.0f - (distance / m_fRadius) * (distance / m_fRadius));
		if (impulse > EPSILON_3)
		{
			impulse *= CExplosive::ExplosionEffect(rq_storage, NULL, pGameObject, Position( ), m_fRadius);
		}

		f32 hit_power;
		CEntityAlive* pEntityAlive = smart_cast<CEntityAlive*>(pGameObject);
		if (pGameObject->m_pPhysicsShell)
		{
			hit_power = 0.0f;
		}
		else if (pEntityAlive && pEntityAlive->g_Alive( ) && pEntityAlive->character_physics_support( )->movement( )->CharacterExist( ))
		{
			hit_power = 0.0f;
		}
		else
		{
			hit_power = impulse;
		}

		if (impulse > EPSILON_3)
		{
			while (!elements_list.empty( ))
			{
				s16 element = elements_list.front( );
				fVector3 bone_pos = bone_position_list.front( );

				CNetPacket P;
				SHit HS;
				HS.GenHeader(GE_HIT, pGameObject->ID( ));
				HS.whoID = ID( );
				HS.weaponID = ID( );
				HS.dir = strike_dir;
				HS.power = hit_power;
				HS.boneID = element;
				HS.p_in_bone_space = bone_pos;
				HS.impulse = impulse;
				HS.hit_type = (ALife::eHitTypeWound);
				HS.Write_Packet(P);

				u_EventSend(P);
				elements_list.pop_front( );
				bone_position_list.pop_front( );
			}
		}
	}
}
