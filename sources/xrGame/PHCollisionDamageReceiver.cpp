#include "stdafx.h"

#include "PHCollisionDamageReceiver.h"
#include "PHShellHolder.h"
#include "..\ENGINE\skeletoncustom.h"
#include "Geometry.h"
#include "PhysicsShell.h"
#include "GameMtlLib.h"
#include "Physics.h"
#include "..\ENGINE\NetPacket.h"
#include "Messages.h"
#include "CharacterPhysicsSupport.h"

void CPHCollisionDamageReceiver::BoneInsert(unsigned short id, float k)
{
	R_ASSERT2(FindBone(id) == m_controled_bones.end( ), "duplicate bone!");
	m_controled_bones.push_back(SControledBone(id, k));
}

void CPHCollisionDamageReceiver::Init( )
{
	CPHShellHolder* sh = PPhysicsShellHolder( );
	CKinematics* K = smart_cast<CKinematics*>(sh->Visual( ));
	CConfigurationFile* ini = K->LL_UserData( );
	if (ini->section_exist("collision_damage"))
	{
		CConfigurationFile::Sect& data = ini->r_section("collision_damage");
		for (CConfigurationFile::SectCIt I = data.Data.begin( ); I != data.Data.end( ); I++)
		{
			const CConfigurationFile::SItem& item = *I;
			unsigned short index = K->LL_BoneID(*item.first);
			R_ASSERT3(index != BI_NONE, "Wrong bone name", *item.first);
			BoneInsert(index, float(atof(*item.second)));
			CCodeGeom* og = sh->PPhysicsShell( )->get_GeomByID(index);
			//R_ASSERT3(og, "collision damage bone has no physics collision", *item.first);
			if (og)
			{
				og->add_obj_contact_cb(CollisionCallback);
			}
		}
	}
}

void CPHCollisionDamageReceiver::CollisionCallback(bool& do_colide, bool bo1, dContact& c, SGameMtl* material_1, SGameMtl* material_2)
{
	if (material_1->Flags.test(SGameMtl::flPassable) || material_2->Flags.test(SGameMtl::flPassable))
	{
		return;
	}

	dBodyID b1 = dGeomGetBody(c.geom.g1);
	dBodyID b2 = dGeomGetBody(c.geom.g2);
	dxGeomUserData* ud_self = bo1 ? retrieveGeomUserData(c.geom.g1) : retrieveGeomUserData(c.geom.g2);
	dxGeomUserData* ud_damager = bo1 ? retrieveGeomUserData(c.geom.g2) : retrieveGeomUserData(c.geom.g1);

	SGameMtl* material_self = bo1 ? material_1 : material_2;
	SGameMtl* material_damager = bo1 ? material_2 : material_1;
	VERIFY(ud_self);
	CPHShellHolder* o_self = ud_self->ph_ref_object;
	CPHShellHolder* o_damager = nullptr;
	if (ud_damager)
	{
		o_damager = ud_damager->ph_ref_object;
	}

	unsigned short source_id = o_damager ? o_damager->ID( ) : unsigned short(-1);
	CPHCollisionDamageReceiver* dr = o_self->PHCollisionDamageReceiver( );
	VERIFY2(dr, "wrong callback");

	float damager_material_factor = material_damager->fBounceDamageFactor;

	if (ud_damager && ud_damager->ph_object && ud_damager->ph_object->CastType( ) == CPHObject::tpCharacter)
	{
		CCharacterPhysicsSupport* phs = o_damager->character_physics_support( );
		if (phs->IsSpecificDamager( ))
		{
			damager_material_factor = phs->BonceDamageFactor( );
		}
	}

	float dfs = (material_self->fBounceDamageFactor + damager_material_factor);
	if (fis_zero(dfs))
	{
		return;
	}

	Fvector3 dir;
	dir.set(*(Fvector3*) c.geom.normal);
	Fvector3 pos;
	pos.sub(*(Fvector3*) c.geom.pos, *(Fvector3*) dGeomGetPosition(bo1 ? c.geom.g1 : c.geom.g2));//it is not true pos in bone space
	dr->Hit(source_id, ud_self->bone_id, E_NL(b1, b2, c.geom.normal) * damager_material_factor / dfs, dir, pos);
}

const static float hit_threthhold = 5.f;
void CPHCollisionDamageReceiver::Hit(unsigned short source_id, unsigned short bone_id, float power, const Fvector3& dir, Fvector3& pos)
{
	DAMAGE_BONES_I i = FindBone(bone_id);
	if (i == m_controled_bones.end( ))
	{
		return;
	}

	power *= i->second;
	if (power < hit_threthhold)
	{
		return;
	}

	CNetPacket P;
	CPHShellHolder* ph = PPhysicsShellHolder( );
	SHit HS;

	HS.GenHeader(GE_HIT, ph->ID( ));
	HS.whoID = ph->ID( );
	HS.weaponID = source_id;
	HS.dir = dir;
	HS.power = power;
	HS.boneID = signed short(bone_id);
	HS.p_in_bone_space = pos;
	HS.impulse = 0.0f;
	HS.hit_type = (ALife::eHitTypeStrike);
	HS.Write_Packet(P);

	ph->u_EventSend(P);
}

void CPHCollisionDamageReceiver::Clear( )
{
	//CPHShellHolder *sh	=PPhysicsShellHolder	();
	//xr_map<unsigned short,float>::iterator i=m_controled_bones.begin(),e=m_controled_bones.end();
	//for(;e!=i;++i)
	//{
	//	CCodeGeom* og= sh->PPhysicsShell()->get_GeomByID(i->first);
	//	if(og)og->set_obj_contact_cb(NULL);
	//}
	m_controled_bones.clear( );
}
