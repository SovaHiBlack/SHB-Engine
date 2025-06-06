#include "stdafx.h"

#include "EntityAlive.h"
#include "InventoryOwner.h"
#include "Inventory.h"
#include "PhysicsShell.h"
#include "gamemtllib.h"
#include "PHMovementControl.h"
#include "Wound.h"
#include "xrmessages.h"
#include "level.h"
#include "..\XR_3DA\skeletoncustom.h"
#include "RelationRegistry.h"
#include "MonsterCommunity.h"
#include "EntityCondition.h"
#include "script_game_object.h"
#include "Hit.h"
#include "PHDestroyable.h"
#include "CharacterPhysicsSupport.h"
#include "script_callback_ex.h"
#include "GameObject_space.h"
#include "material_manager.h"
#include "game_base_space.h"

#define SMALL_ENTITY_RADIUS		0.6f
#define BLOOD_MARKS_SECT		"bloody_marks"

//������� ����� �� ������ 
SHADER_VECTOR* CEntityAlive::m_pBloodMarksVector = NULL;
f32 CEntityAlive::m_fBloodMarkSizeMin = 0.0f;
f32 CEntityAlive::m_fBloodMarkSizeMax = 0.0f;
f32 CEntityAlive::m_fBloodMarkDistance = 0.0f;
f32 CEntityAlive::m_fNominalHit = 0.0f;

//������� �����
SHADER_VECTOR* CEntityAlive::m_pBloodDropsVector = NULL;
f32 CEntityAlive::m_fStartBloodWoundSize = 0.3f;
f32 CEntityAlive::m_fStopBloodWoundSize = 0.1f;
f32 CEntityAlive::m_fBloodDropSize = 0.03f;

//����������� ������ �����, ����� �������� ����� ��������
//����������� ����� �������
u32 CEntityAlive::m_dwMinBurnTime = 10000;
//������ ����, ���� ��������� ��������
f32 CEntityAlive::m_fStartBurnWoundSize = 0.3f;
//������ ����, ���� ���������� ��������
f32 CEntityAlive::m_fStopBurnWoundSize = 0.1f;

STR_VECTOR* CEntityAlive::m_pFireParticlesVector = NULL;

/////////////////////////////////////////////
// CEntityAlive
/////////////////////////////////////////////
CEntityAlive::CEntityAlive( )
{
	monster_community = xr_new<CMonsterCommunity>( );

	m_ef_weapon_type = u32(-1);
	m_ef_detector_type = u32(-1);

	m_material_manager = 0;
}

CEntityAlive::~CEntityAlive( )
{
	xr_delete(monster_community);
	xr_delete(m_material_manager);
}

void CEntityAlive::Load(pcstr section)
{
	CEntity::Load(section);
	conditions( ).LoadCondition(section);
	conditions( ).LoadImmunities(pSettings->r_string(section, "immunities_sect"), pSettings);

	m_fFood = 100 * pSettings->r_float(section, "ph_mass");

	//bloody wallmarks
	if (0 == m_pBloodMarksVector)
	{
		LoadBloodyWallmarks(BLOOD_MARKS_SECT);
	}

	if (0 == m_pFireParticlesVector)
		LoadFireParticles("entity_fire_particles");

	//������. ��� �������� ����������� ������ ��� ��������
	monster_community->set(pSettings->r_string(section, "species"));
}

void CEntityAlive::LoadBloodyWallmarks(pcstr section)
{
	VERIFY(0 == m_pBloodMarksVector);
	VERIFY(0 == m_pBloodDropsVector);
	m_pBloodMarksVector = xr_new<SHADER_VECTOR>( );
	m_pBloodDropsVector = xr_new<SHADER_VECTOR>( );

	//�������� ������� �� ������
	string256	tmp;
	pcstr wallmarks_name = pSettings->r_string(section, "wallmarks");

	s32 cnt = _GetItemCount(wallmarks_name);

	ref_shader	s;
	for (s32 k = 0; k < cnt; ++k)
	{
		s.create("effects\\wallmark", _GetItem(wallmarks_name, k, tmp));
		m_pBloodMarksVector->push_back(s);
	}

	m_fBloodMarkSizeMin = pSettings->r_float(section, "min_size");
	m_fBloodMarkSizeMax = pSettings->r_float(section, "max_size");
	m_fBloodMarkDistance = pSettings->r_float(section, "dist");
	m_fNominalHit = pSettings->r_float(section, "nominal_hit");

	//����� ����� � �������� ���
	wallmarks_name = pSettings->r_string(section, "blood_drops");
	cnt = _GetItemCount(wallmarks_name);

	for (s32 k = 0; k < cnt; ++k)
	{
		s.create("effects\\wallmark", _GetItem(wallmarks_name, k, tmp));
		m_pBloodDropsVector->push_back(s);
	}

	m_fStartBloodWoundSize = pSettings->r_float(section, "start_blood_size");
	m_fStopBloodWoundSize = pSettings->r_float(section, "stop_blood_size");
	m_fBloodDropSize = pSettings->r_float(section, "blood_drop_size");
}

void CEntityAlive::UnloadBloodyWallmarks( )
{
	if (m_pBloodMarksVector)
	{
		m_pBloodMarksVector->clear( );
		xr_delete(m_pBloodMarksVector);
	}

	if (m_pBloodDropsVector)
	{
		m_pBloodDropsVector->clear( );
		xr_delete(m_pBloodDropsVector);
	}
}

void CEntityAlive::LoadFireParticles(pcstr section)
{
	m_pFireParticlesVector = xr_new<STR_VECTOR>( );

	string256	tmp;
	pcstr particles_name = pSettings->r_string(section, "fire_particles");

	s32 cnt = _GetItemCount(particles_name);

	shared_str	s;
	for (s32 k = 0; k < cnt; ++k)
	{
		s = _GetItem(particles_name, k, tmp);
		m_pFireParticlesVector->push_back(s);
	}

	m_fStartBurnWoundSize = pSettings->r_float(section, "start_burn_size");
	m_fStopBurnWoundSize = pSettings->r_float(section, "stop_burn_size");

	m_dwMinBurnTime = pSettings->r_u32(section, "min_burn_time");
}

void CEntityAlive::UnloadFireParticles( )
{
	if (m_pFireParticlesVector)
	{
		m_pFireParticlesVector->clear( );
		xr_delete(m_pFireParticlesVector);
	}
}

void CEntityAlive::reinit( )
{
	CEntity::reinit( );

	m_fAccuracy = 25.0f;
	m_fIntelligence = 25.0f;
}

void CEntityAlive::reload(pcstr section)
{
	CEntity::reload(section);
//	CEntityCondition::reload(section);

	m_ef_creature_type = pSettings->r_u32(section, "ef_creature_type");
	m_ef_weapon_type = READ_IF_EXISTS(pSettings, r_u32, section, "ef_weapon_type", u32(-1));
	m_ef_detector_type = READ_IF_EXISTS(pSettings, r_u32, section, "ef_detector_type", u32(-1));

	m_fFood = 100 * pSettings->r_float(section, "ph_mass");
}

void CEntityAlive::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);

	//condition update with the game time pass
	conditions( ).UpdateConditionTime( );
	conditions( ).UpdateCondition( );
	//���������� ��������� ����
	UpdateFireParticles( );
	//����� �����
	UpdateBloodDrops( );
	//�������� ����
	conditions( ).UpdateWounds( );

	//����� ��������
	if (Local( ) && !g_Alive( ) && !AlreadyDie( ))
	{
		if (conditions( ).GetWhoHitLastTime( ))
		{
//			Msg			("%6d : KillEntity from CEntityAlive (using who hit last time) for object %s",Device.dwTimeGlobal,*cName());
			KillEntity(conditions( ).GetWhoHitLastTimeID( ));
		}
		else
		{
//			Msg			("%6d : KillEntity from CEntityAlive for object %s",Device.dwTimeGlobal,*cName());
			KillEntity(ID( ));
		}
	}
}

BOOL CEntityAlive::net_Spawn(CSE_Abstract* DC)
{
	//���������� ������� � ������������ � community
/*	if(monster_community->team() != 255)
		id_Team = monster_community->team();*/

	conditions( ).reinit( );
	inherited::net_Spawn(DC);

	m_BloodWounds.clear( );
	m_ParticleWounds.clear( );

	//�������� ����� � ����� �� ��������, ���� �����
	for (WOUND_VECTOR::const_iterator it = conditions( ).wounds( ).begin( ); conditions( ).wounds( ).end( ) != it; ++it)
	{
		CWound* pWound = *it;
		StartFireParticles(pWound);
		StartBloodDrops(pWound);
	}

	return TRUE;
}

void CEntityAlive::net_Destroy( )
{
	inherited::net_Destroy( );
}

void CEntityAlive::HitImpulse(f32 /**amount/**/, fVector3& /**vWorldDir/**/, fVector3& /**vLocalDir/**/)
{
	//	f32 Q					= 2*f32(amount)/m_PhysicMovementControl->GetMass();
	//	m_PhysicMovementControl->vExternalImpulse.mad	(vWorldDir,Q);
}

void CEntityAlive::Hit(SHit* pHDS)
{
	SHit HDS = *pHDS;
	//-------------------------------------------------------------------
	if (HDS.hit_type == ALife::eHitTypeWound_2)
	{
		HDS.hit_type = ALife::eHitTypeWound;
	}
	//-------------------------------------------------------------------
	CDamageManager::HitScale(HDS.boneID, conditions( ).hit_bone_scale( ), conditions( ).wound_bone_scale( ), pHDS->aim_bullet);

	//�������� ���������, ����� ��� ��� ������������ ����� ���������� ���
	CWound* pWound = conditions( ).ConditionHit(&HDS);

	if (pWound)
	{
		if (ALife::eHitTypeBurn == HDS.hit_type)
		{
			StartFireParticles(pWound);
		}
		else if (ALife::eHitTypeWound == HDS.hit_type || ALife::eHitTypeFireWound == HDS.hit_type)
		{
			StartBloodDrops(pWound);
		}
	}

	if (HDS.hit_type != ALife::eHitTypeTelepatic)
	{
		//�������� ����� �� �����
		if (!use_simplified_visual( ))
		{
			BloodyWallmarks(HDS.damage( ), HDS.dir, HDS.bone( ), HDS.p_in_bone_space);
		}
	}

	//-------------------------------------------
	conditions( ).SetConditionDeltaTime(0);
	//-------------------------------------------
	inherited::Hit(&HDS);

	if (g_Alive( ))
	{
		CEntityAlive* EA = smart_cast<CEntityAlive*>(HDS.who);
		if (EA && EA->g_Alive( ) && EA->ID( ) != ID( ))
		{
			SRelationRegistry( ).FightRegister(EA->ID( ), ID( ), this->tfGetRelationType(EA), HDS.damage( ));
			SRelationRegistry( ).Action(EA, this, SRelationRegistry::ATTACK);
		}
	}
}

void CEntityAlive::Die(CObject* who)
{
	SRelationRegistry( ).Action(smart_cast<CEntityAlive*>(who), this, SRelationRegistry::KILL);
	inherited::Die(who);

	const CGameObject* who_object = smart_cast<const CGameObject*>(who);
	callback(GameObject::eDeath)(lua_game_object( ), who_object ? who_object->lua_game_object( ) : 0);

	if (!getDestroy( ))
	{
		CNetPacket		P;
		u_EventGen(P, GE_ASSIGN_KILLER, ID( ));
		P.w_u16(u16(who->ID( )));
		u_EventSend(P);
	}

	// disable react to sound
	ISpatial* self = smart_cast<ISpatial*> (this);
	if (self)
	{
		self->spatial.type &= ~STYPE_REACTTOSOUND;
	}
}

//��������� ��� �������� ����
f32 CEntityAlive::CalcCondition(f32 /**hit/**/)
{
	conditions( ).UpdateCondition( );

	//dont call inherited::CalcCondition it will be meaningless
	return conditions( ).GetHealthLost( );//*100.f;
}

///////////////////////////////////////////////////////////////////////
u16	CEntityAlive::PHGetSyncItemsNumber( )
{
	if (character_physics_support( )->movement( )->CharacterExist( )) return 1;
	else										  return inherited::PHGetSyncItemsNumber( );
}
CPHSynchronize* CEntityAlive::PHGetSyncItem(u16 item)
{
	if (character_physics_support( )->movement( )->CharacterExist( )) return character_physics_support( )->movement( )->GetSyncItem( );
	else										 return inherited::PHGetSyncItem(item);
}
void CEntityAlive::PHUnFreeze( )
{
	if (character_physics_support( )->movement( )->CharacterExist( )) character_physics_support( )->movement( )->UnFreeze( );
	else if (m_pPhysicsShell) m_pPhysicsShell->UnFreeze( );
}
void CEntityAlive::PHFreeze( )
{
	if (character_physics_support( )->movement( )->CharacterExist( )) character_physics_support( )->movement( )->Freeze( );
	else if (m_pPhysicsShell) m_pPhysicsShell->Freeze( );
}
//////////////////////////////////////////////////////////////////////

//���������� �������� ������� �� ������, ����� ��������� ����
void CEntityAlive::BloodyWallmarks(f32 P, const fVector3& dir, s16 element, const fVector3& position_in_object_space)
{
	if (BI_NONE == (u16)element)
	{
		return;
	}

	//��������� ���������� ���������
	CKinematics* V = smart_cast<CKinematics*>(Visual( ));

	fVector3 start_pos = position_in_object_space;
	if (V)
	{
		fMatrix4x4& m_bone = (V->LL_GetBoneInstance(u16(element))).mTransform;
		m_bone.transform_tiny(start_pos);
	}

	XFORM( ).transform_tiny(start_pos);

	f32 small_entity = 1.0f;
	if (Radius( ) < SMALL_ENTITY_RADIUS)
	{
		small_entity = 0.5f;
	}

	f32 wallmark_size = m_fBloodMarkSizeMax;
	wallmark_size *= (P / m_fNominalHit);
	wallmark_size *= small_entity;
	clamp(wallmark_size, m_fBloodMarkSizeMin, m_fBloodMarkSizeMax);

	VERIFY(m_pBloodMarksVector);
	PlaceBloodWallmark(dir, start_pos, m_fBloodMarkDistance, wallmark_size, *m_pBloodMarksVector);
}

void CEntityAlive::PlaceBloodWallmark(const fVector3& dir, const fVector3& start_pos, f32 trace_dist, f32 wallmark_size, SHADER_VECTOR& wallmarks_vector)
{
	collide::rq_result	result;
	BOOL reach_wall = Level( ).ObjectSpace.RayPick(start_pos, dir, trace_dist, collide::rqtBoth, result, this) && !result.O;

	//���� ����� �������� �� ������������ �������
	if (reach_wall)
	{
		CDB::TRI* pTri = Level( ).ObjectSpace.GetStaticTris( ) + result.element;
		SGameMtl* pMaterial = GMLib.GetMaterialByIdx(pTri->material);

		if (pMaterial->Flags.is(SGameMtl::flBloodmark))
		{
			//��������� ������� � ���������� �����������
			fVector3* pVerts = Level( ).ObjectSpace.GetStaticVerts( );

			//��������� ����� ���������
			fVector3 end_point;
			end_point.set(0.0f, 0.0f, 0.0f);
			end_point.mad(start_pos, dir, result.range);

			ref_shader wallmarkShader = wallmarks_vector[::Random.randI(wallmarks_vector.size( ))];

			{
				//�������� ������� �� ���������
				::Render->add_StaticWallmark(wallmarkShader, end_point, wallmark_size, pTri, pVerts);
			}
		}
	}
}

void CEntityAlive::StartFireParticles(CWound* pWound)
{
	if (pWound->TypeSize(ALife::eHitTypeBurn) > m_fStartBurnWoundSize)
	{
		if (std::find(m_ParticleWounds.begin( ),
			m_ParticleWounds.end( ),
			pWound) == m_ParticleWounds.end( ))
		{
			m_ParticleWounds.push_back(pWound);
		}

		CKinematics* V = smart_cast<CKinematics*>(Visual( ));

		u16 particle_bone = CParticlesPlayer::GetNearestBone(V, pWound->GetBoneNum( ));
		VERIFY(particle_bone < 64 || BI_NONE == particle_bone);

		pWound->SetParticleBoneNum(particle_bone);
		pWound->SetParticleName((*m_pFireParticlesVector)[::Random.randI(0, m_pFireParticlesVector->size( ))]);

		if (BI_NONE != particle_bone)
		{
			CParticlesPlayer::StartParticles(pWound->GetParticleName( ),
											 pWound->GetParticleBoneNum( ),
											 fVector3( ).set(0.0f, 1.0f, 0.0f),
											 ID( ),
											 u32(f32(m_dwMinBurnTime) * ::Random.randF(0.5f, 1.5f)), false);
		}
		else
		{
			CParticlesPlayer::StartParticles(pWound->GetParticleName( ),
											 fVector3( ).set(0.0f, 1.0f, 0.0f),
											 ID( ),
											 u32(f32(m_dwMinBurnTime) * ::Random.randF(0.5f, 1.5f)), false);
		}
	}
}

void CEntityAlive::UpdateFireParticles( )
{
	if (m_ParticleWounds.empty( )) return;

//	WOUND_VECTOR_IT last_it;

	for (WOUND_VECTOR_IT it = m_ParticleWounds.begin( );
		 it != m_ParticleWounds.end( );)
	{
		CWound* pWound = *it;
		f32 burn_size = pWound->TypeSize(ALife::eHitTypeBurn);

		if (pWound->GetDestroy( ) ||
			(burn_size > 0 && (burn_size < m_fStopBurnWoundSize || !g_Alive( ))))
		{
			CParticlesPlayer::AutoStopParticles(pWound->GetParticleName( ),
												pWound->GetParticleBoneNum( ),
												u32(f32(m_dwMinBurnTime) * ::Random.randF(0.5f, 1.5f))
			);
			it = m_ParticleWounds.erase(it);
			continue;
		}
		it++;
	}
}

ALife::ERelationType CEntityAlive::tfGetRelationType(const CEntityAlive* tpEntityAlive) const
{
	s32 relation = CMonsterCommunity::relation(this->monster_community->index( ), tpEntityAlive->monster_community->index( ));

	switch (relation)
	{
		case 1:		return(ALife::eRelationTypeFriend);		break;
		case 0:		return(ALife::eRelationTypeNeutral);	break;
		case -1:	return(ALife::eRelationTypeEnemy);		break;
		case -2:	return(ALife::eRelationTypeWorstEnemy);	break;

		default:	return(ALife::eRelationTypeDummy);		break;
	}
};

bool CEntityAlive::is_relation_enemy(const CEntityAlive* tpEntityAlive) const
{
	return ((tfGetRelationType(tpEntityAlive) == ALife::eRelationTypeEnemy) ||
			(tfGetRelationType(tpEntityAlive) == ALife::eRelationTypeWorstEnemy));
}

void CEntityAlive::StartBloodDrops(CWound* pWound)
{
	if (pWound->BloodSize( ) > m_fStartBloodWoundSize)
	{
		if (std::find(m_BloodWounds.begin( ), m_BloodWounds.end( ),
			pWound) == m_BloodWounds.end( ))
		{
			m_BloodWounds.push_back(pWound);
			pWound->m_fDropTime = 0.f;
		}
	}
}

void CEntityAlive::UpdateBloodDrops( )
{
	static f32 m_fBloodDropTimeMax = pSettings->r_float(BLOOD_MARKS_SECT, "blood_drop_time_max");
	static f32 m_fBloodDropTimeMin = pSettings->r_float(BLOOD_MARKS_SECT, "blood_drop_time_min");

	if (m_BloodWounds.empty( )) return;

	if (!g_Alive( ))
	{
		m_BloodWounds.clear( );
		return;
	}

//	WOUND_VECTOR_IT last_it;

	for (WOUND_VECTOR_IT it = m_BloodWounds.begin( );
		 it != m_BloodWounds.end( );)
	{
		CWound* pWound = *it;
		f32 blood_size = pWound->BloodSize( );

		if (pWound->GetDestroy( ) || blood_size < m_fStopBloodWoundSize)
		{
			it = m_BloodWounds.erase(it);
			continue;
		}

		if (pWound->m_fDropTime < Device.fTimeGlobal)
		{
			f32 size_k = blood_size - m_fStopBloodWoundSize;
			size_k = size_k < 1.f ? size_k : 1.f;
			pWound->m_fDropTime = Device.fTimeGlobal + (m_fBloodDropTimeMax - (m_fBloodDropTimeMax - m_fBloodDropTimeMin) * size_k) * Random.randF(0.8f, 1.2f);
			VERIFY(m_pBloodDropsVector);
			if (pWound->GetBoneNum( ) != BI_NONE)
			{
				fVector3 pos;
				fVector3 pos_distort;
				pos_distort.random_dir( );
				pos_distort.mul(0.15f);
				CParticlesPlayer::GetBonePos(this, pWound->GetBoneNum( ), fVector3( ).set(0.0f, 0.0f, 0.0f), pos);
				pos.add(pos_distort);
				PlaceBloodWallmark(fVector3( ).set(0.0f, -1.0f, 0.0f),
								   pos, m_fBloodMarkDistance,
								   m_fBloodDropSize, *m_pBloodDropsVector);
			}
		}
		it++;
	}
}

void CEntityAlive::save(CNetPacket& output_packet)
{
	inherited::save(output_packet);
	conditions( ).save(output_packet);
}

void CEntityAlive::load(IReader& input_packet)
{
	inherited::load(input_packet);
	conditions( ).load(input_packet);
}

BOOL	CEntityAlive::net_SaveRelevant( )
{
	return		(TRUE);
}

CEntityConditionSimple* CEntityAlive::create_entity_condition(CEntityConditionSimple* ec)
{
	if (!ec)
		m_entity_condition = xr_new<CEntityCondition>(this);
	else
		m_entity_condition = smart_cast<CEntityCondition*>(ec);

	return		(inherited::create_entity_condition(m_entity_condition));
}

f32 CEntityAlive::SetfRadiation(f32 value)
{
	conditions( ).radiation( ) = value / 100.0f;
	return value;
}

f32 CEntityAlive::g_Radiation( )	const
{
	return conditions( ).GetRadiation( ) * 100.0f;
}

DLL_Pure* CEntityAlive::_construct( )
{
	inherited::_construct( );
	if (character_physics_support( ))m_material_manager = xr_new<CMaterialManager>(this, character_physics_support( )->movement( ));
	return					(this);
}

u32	CEntityAlive::ef_creature_type( ) const
{
	return	(m_ef_creature_type);
}

u32	CEntityAlive::ef_weapon_type( ) const
{
	VERIFY(m_ef_weapon_type != u32(-1));
	return	(m_ef_weapon_type);
}

u32	 CEntityAlive::ef_detector_type( ) const
{
	VERIFY(m_ef_detector_type != u32(-1));
	return	(m_ef_detector_type);
}
void CEntityAlive::PHGetLinearVell(fVector3& velocity)
{
	if (character_physics_support( ))
	{
		character_physics_support( )->PHGetLinearVell(velocity);
	}
	else
		inherited::PHGetLinearVell(velocity);

}
CIKLimbsController* CEntityAlive::character_ik_controller( )
{
	if (character_physics_support( ))
	{
		return character_physics_support( )->ik_controller( );
	}
	else
	{
		return NULL;
	}
}
CPHSoundPlayer* CEntityAlive::ph_sound_player( )
{
	if (character_physics_support( ))
	{
		return character_physics_support( )->ph_sound_player( );
	}
	else
	{
		return NULL;
	}
}

SCollisionHitCallback* CEntityAlive::get_collision_hit_callback( )
{
	CCharacterPhysicsSupport* cs = character_physics_support( );
	if (cs)return cs->get_collision_hit_callback( );
	else return false;
}

bool					CEntityAlive::set_collision_hit_callback(SCollisionHitCallback* cc)
{
	CCharacterPhysicsSupport* cs = character_physics_support( );
	if (cs)return cs->set_collision_hit_callback(cc);
	else return false;
}

void CEntityAlive::net_Relcase(CObject* object)
{
	inherited::net_Relcase(object);
	conditions( ).remove_links(object);
}

void	CEntityAlive::create_anim_mov_ctrl(CBlend* b)
{
	inherited::create_anim_mov_ctrl(b);
	CCharacterPhysicsSupport* cs = character_physics_support( );
	if (cs)
		cs->on_create_anim_mov_ctrl( );
}
void	CEntityAlive::destroy_anim_mov_ctrl( )
{
	inherited::destroy_anim_mov_ctrl( );
	CCharacterPhysicsSupport* cs = character_physics_support( );
	if (cs)
		cs->on_destroy_anim_mov_ctrl( );
}