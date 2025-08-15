#include "stdafx.h"

#include "..\XR_3DA\LightAnimLibrary.h"
#include "..\XR_3DA\skeletonanimated.h"
#include "ai/stalker/Stalker.h"
#include "clsid_game.h"
#include "CustomZone.h"
#include "extendedgeom.h"
#include "GameObject_space.h"
#include "GroupHierarchyHolder.h"
#include "Helicopter.h"
#include "Level.h"
#include "MathUtils.h"
#include "PhysicsShell.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
#include "SeniorityHierarchyHolder.h"
#include "SquadHierarchyHolder.h"
#include "TeamHierarchyHolder.h"

bool CHelicopter::isObjectVisible(CObject* O)
{
	fVector3					dir_to_object;
	fVector3					to_point;
	O->Center(to_point);
	fVector3 from_point = XFORM( ).c;
	dir_to_object.sub(to_point, from_point).normalize_safe( );
	f32 ray_length = from_point.distance_to(to_point);

	BOOL res = Level( ).ObjectSpace.RayTest(from_point, dir_to_object, ray_length, collide::rqtStatic, NULL, NULL);

	return !res;
}

bool CHelicopter::isVisible(CScriptGameObject* O)
{
	return isObjectVisible(&O->object( ));
}

void CHelicopter::TurnLighting(bool bOn)
{
	m_light_render->set_active(bOn);
	m_light_started = bOn;
}
void CHelicopter::TurnEngineSound(bool bOn)
{
	if (bOn)
	{
		m_engineSound.set_volume(1.0f);
	}
	else
	{
		m_engineSound.set_volume(0.0f);
	}
}

void CHelicopter::StartFlame( )
{
	if (m_pParticle)
	{
		return;
	}

	m_pParticle = CParticlesObject::Create(*m_smoke_particle, FALSE);

	fVector3 zero_vector;
	zero_vector.set(0.0f, 0.0f, 0.0f);
	m_pParticle->UpdateParent(m_particleXFORM, zero_vector);
	m_pParticle->Play( );
	m_flame_started = true;
}

void CHelicopter::UpdateHeliParticles( )
{
	CKinematics* K = smart_cast<CKinematics*>(Visual( ));
	m_particleXFORM = K->LL_GetTransform(m_smoke_bone);
	m_particleXFORM.mulA_43(XFORM( ));

	if (m_pParticle)
	{
		fVector3 vel;
		fVector3 last_pos = PositionStack.back( ).vPosition;
		vel.sub(Position( ), last_pos);
		vel.mul(5.0f);

		m_pParticle->UpdateParent(m_particleXFORM, vel);
	}
	//lighting
	if (m_light_render->get_active( ))
	{
		fMatrix4x4 xf;
		fMatrix4x4& M = K->LL_GetTransform(u16(m_light_bone));
		xf.mul(XFORM( ), M);
		VERIFY(!fis_zero(DET(xf)));

		m_light_render->set_rotation(xf.k, xf.i);
		m_light_render->set_position(xf.c);

		if (m_lanim)
		{
			s32 frame;
			u32 clr = m_lanim->CalculateBGR(Device.fTimeGlobal, frame); // тючтЁр•рхЄ т ЇюЁьрЄх BGR
			fColor					fclr;
			fclr.set((f32) color_get_B(clr), (f32) color_get_G(clr), (f32) color_get_R(clr), 1.f);
			fclr.mul_rgb(m_light_brightness / 255.f);
			m_light_render->set_color(fclr);
		}
	}
}

void CHelicopter::ExplodeHelicopter( )
{
	m_ready_explode = false;
	m_exploded = true;
	if (m_pParticle)
	{
		m_pParticle->Stop( );
		CParticlesObject::Destroy(m_pParticle);
	}

	if (CPHDestroyable::CanDestroy( ))
	{
		CPHDestroyable::Destroy(ID( ), "physic_destroyable_object");
	}

	CExplosive::SetInitiator(ID( ));
	CExplosive::GenExplodeEvent(Position( ), fVector3( ).set(0.0f, 1.0f, 0.0f));
	m_brokenSound.stop( );
}

void CHelicopter::SetDestPosition(fVector3* pos)
{
	m_movement.SetDestPosition(pos);
	if (bDebug)
	{
		Msg("---SetDestPosition %f %f %f", pos->x, pos->y, pos->z);
	}
}

f32 CHelicopter::GetDistanceToDestPosition( )
{
	return m_movement.GetDistanceToDestPosition( );
}

void CHelicopter::UnSetEnemy( )
{
	m_enemy.type = eEnemyNone;
}

void CHelicopter::SetEnemy(CScriptGameObject* e)
{
	m_enemy.type = eEnemyEntity;
	m_enemy.destEnemyID = e->ID( );
}

void CHelicopter::SetEnemy(fVector3* pos)
{
	m_enemy.type = eEnemyPoint;
	m_enemy.destEnemyPos = *pos;
}

f32 CHelicopter::GetCurrVelocity( )
{
	return m_movement.curLinearSpeed;
}

void CHelicopter::SetMaxVelocity(f32 v)
{
	m_movement.maxLinearSpeed = v;
}

f32 CHelicopter::GetMaxVelocity( )
{
	return m_movement.maxLinearSpeed;
}

void CHelicopter::SetLinearAcc(f32 LAcc_fw, f32 LAcc_bw)
{
	m_movement.LinearAcc_fw = LAcc_fw;	//ускорение разгона
	m_movement.LinearAcc_bk = LAcc_bw;	//ускорение торможения
}

void CHelicopter::SetSpeedInDestPoint(f32 sp)
{
	m_movement.SetSpeedInDestPoint(sp);
	if (bDebug)
	{
		Msg("---SetSpeedInDestPoint %f", sp);
	}
}

f32 CHelicopter::GetSpeedInDestPoint(f32 sp)
{
	return m_movement.GetSpeedInDestPoint( );
}

void CHelicopter::SetOnPointRangeDist(f32 d)
{
	m_movement.onPointRangeDist = d;
	if (bDebug)
	{
		Msg("---SetOnPointRangeDist %f", d);
	}
}

f32 CHelicopter::GetOnPointRangeDist( )
{
	return m_movement.onPointRangeDist;
}

f32 CHelicopter::GetRealAltitude( )
{
	collide::rq_result		cR;
	fVector3 down_dir;

	down_dir.set(0.0f, -1.0f, 0.0f);

	Level( ).ObjectSpace.RayPick(XFORM( ).c, down_dir, 1000.0f, collide::rqtStatic, cR, NULL);

	return cR.range;
}

void	CHelicopter::Hit(SHit* pHDS)
{
	if (GetfHealth( ) < 0.005f)
	{
		return;
	}

	if (state( ) == CHelicopter::eDead)
	{
		return;
	}

	if (pHDS->who == this)
	{
		return;
	}

	bonesIt It = m_hitBones.find(pHDS->bone( ));
	if (It != m_hitBones.end( ) && pHDS->hit_type == ALife::eHitTypeFireWound)
	{
		f32 curHealth = GetfHealth( );
		curHealth -= pHDS->damage( ) * It->second * 1000.0f;
		SetfHealth(curHealth);

#ifdef DEBUG
		if (bDebug)
		{
			Log("----Helicopter::PilotHit(). health=", curHealth);
		}
#endif

	}
	else
	{
		f32 hit_power = pHDS->damage( );
		hit_power *= m_HitTypeK[pHDS->hit_type];

		SetfHealth(GetfHealth( ) - hit_power);

#ifdef DEBUG
		if (bDebug)
		{
			Log("----Helicopter::Hit(). health=", GetfHealth( ));
		}
#endif

	}

	if (pHDS->who &&
		(pHDS->who->CLS_ID == CLSID_OBJECT_ACTOR ||
		 smart_cast<CStalker*>(pHDS->who) ||
		 smart_cast<CCustomZone*>(pHDS->who))
		)
	{
		callback(GameObject::eHelicopterOnHit)(pHDS->damage( ), pHDS->impulse, pHDS->hit_type, pHDS->who->ID( ));
	}

	CPHDestroyable::SetFatalHit(*pHDS);
}

void CHelicopter::PHHit(f32 P, fVector3& dir, CObject* who, s16 element, fVector3 p_in_object_space, f32 impulse, ALife::EHitType hit_type)
{
	if (!g_Alive( ))
	{
		inherited::PHHit(P, dir, who, element, p_in_object_space, impulse, hit_type);
	}
}

void CollisionCallbackDead(bool& do_colide, bool bo1, dContact& c, SGameMtl* material_1, SGameMtl* material_2)
{
	do_colide = true;

	CHelicopter* l_this = bo1 ? smart_cast<CHelicopter*>(retrieveGeomUserData(c.geom.g1)->ph_ref_object) : smart_cast<CHelicopter*>(retrieveGeomUserData(c.geom.g2)->ph_ref_object);

	if (l_this && !l_this->m_exploded)
		l_this->m_ready_explode = true;
}

void CHelicopter::DieHelicopter( )
{
	if (state( ) == CHelicopter::eDead)
		return;
	CEntity::Die(NULL);

	m_engineSound.stop( );

	m_brokenSound.create(pSettings->r_string(*cNameSect( ), "broken_snd"), st_Effect, sg_SourceType);
	m_brokenSound.play_at_pos(0, XFORM( ).c, sm_Looped);

	CKinematics* K = smart_cast<CKinematics*>(Visual( ));
	if (true /*!PPhysicsShell()*/)
	{
		string256						I;
		pcstr bone;

		u16 bone_id;
		for (u32 i = 0, n = _GetItemCount(*m_death_bones_to_hide); i < n; ++i)
		{
			bone = _GetItem(*m_death_bones_to_hide, i, I);
			bone_id = K->LL_BoneID(bone);
			K->LL_SetBoneVisible(bone_id, FALSE, TRUE);
		}

		///PPhysicsShell()=P_build_Shell	(this,false);
		PPhysicsShell( )->EnabledCallbacks(TRUE);
		PPhysicsShell( )->set_ObjectContactCallback(CollisionCallbackDead);
		PPhysicsShell( )->set_ContactCallback(ContactShotMark);
	}

	fVector3 lin_vel;

	fVector3 prev_pos = PositionStack.front( ).vPosition;
	lin_vel.sub(XFORM( ).c, prev_pos);

	if (Device.dwTimeGlobal != PositionStack.front( ).dwTime)
		lin_vel.div((Device.dwTimeGlobal - PositionStack.front( ).dwTime) / 1000.0f);

	lin_vel.mul(m_death_lin_vel_k);
	PPhysicsShell( )->set_LinearVel(lin_vel);
	PPhysicsShell( )->set_AngularVel(m_death_ang_vel);
	PPhysicsShell( )->Enable( );
	K->CalculateBones_Invalidate( );
	K->CalculateBones( );
	setState(CHelicopter::eDead);
	m_engineSound.stop( );
	processing_deactivate( );
	m_dead = true;
}

void SHeliEnemy::Load(pcstr section)
{
	fire_trail_length_des = pSettings->r_float(section, "fire_trail_length");
	bUseFireTrail = !!pSettings->r_bool(section, "use_fire_trail");
}

void SHeliEnemy::reinit( )
{
	type = eEnemyNone;
	destEnemyPos.set(0.0f, 0.0f, 0.0f);
	destEnemyID = u32(-1);
	fStartFireTime = -1.0f;
}

void SHeliEnemy::Update( )
{
	switch (type)
	{
		case eEnemyNone:
		case eEnemyPoint:
			break;
		case eEnemyEntity:
		{
			CObject* O = Level( ).Objects.net_Find(destEnemyID);
			if (O)	O->Center(destEnemyPos);
			else	type = eEnemyNone;
		}break;
		default:
			NODEFAULT;
	};
}

void SHeliEnemy::save(CNetPacket& output_packet)
{
	output_packet.w_s16((s16) type);
	output_packet.w_vec3(destEnemyPos);
	output_packet.w_u32(destEnemyID);

	output_packet.w_float(fire_trail_length_des);
	output_packet.w_u8(bUseFireTrail ? 1 : 0);
}

void SHeliEnemy::load(IReader& input_packet)
{
	type = (EHeliHuntState) input_packet.r_s16( );
	input_packet.r_fvector3(destEnemyPos);
	destEnemyID = input_packet.r_u32( );

	fire_trail_length_des = input_packet.r_float( );
	bUseFireTrail = !!input_packet.r_u8( );
}

void CHelicopter::SetFireTrailLength(f32 val)
{
	m_enemy.fire_trail_length_des = val;
}

bool CHelicopter::UseFireTrail( )
{
	return m_enemy.bUseFireTrail;
}

void CHelicopter::UseFireTrail(bool val)
{
	m_enemy.bUseFireTrail = val;
	if (val)
	{
		fireDispersionBase = pSettings->r_float(*cNameSect( ), "fire_dispersion_null");
		fireDispersionBase = deg2rad(fireDispersionBase);
	}
	else
	{
		fireDispersionBase = pSettings->r_float(*cNameSect( ), "fire_dispersion_base");
		fireDispersionBase = deg2rad(fireDispersionBase);
	}
}

void SHeliBodyState::Load(pcstr section)
{
	model_angSpeedBank = pSettings->r_float(section, "model_angular_sp_bank");
	model_angSpeedPitch = pSettings->r_float(section, "model_angular_sp_pitch");
	model_pitch_k = pSettings->r_float(section, "model_pitch_koef");
	model_bank_k = pSettings->r_float(section, "model_bank_koef");
}

void SHeliBodyState::reinit( )
{
	type = eBodyByPath;
	b_looking_at_point = false;
	looking_point.set(0.0f, 0.0f, 0.0f);
	parent->XFORM( ).getHPB(currBodyHPB.x, currBodyHPB.y, currBodyHPB.z);
}

void SHeliBodyState::LookAtPoint(fVector3 point, bool do_it)
{
	b_looking_at_point = do_it;
	looking_point = point;
	type = (do_it) ? eBodyToPoint : eBodyByPath;
}

void SHeliBodyState::save(CNetPacket& output_packet)
{
	output_packet.w_s16((s16) type);
	output_packet.w_u8(b_looking_at_point ? 1 : 0);
	output_packet.w_float(currBodyHPB.x);
	output_packet.w_float(currBodyHPB.y);
	output_packet.w_float(currBodyHPB.z);
}

void SHeliBodyState::load(IReader& input_packet)
{
	type = (EHeliBodyState) input_packet.r_s16( );
	b_looking_at_point = !!input_packet.r_u8( );
	currBodyHPB.x = input_packet.r_float( );
	currBodyHPB.y = input_packet.r_float( );
	currBodyHPB.z = input_packet.r_float( );
}



f32 t_xx(f32 V0, f32 V1, f32 a0, f32 a1, f32 d, f32 fSign)
{
	return (V1 + _sqrt(V1 * V1 - (a1 / (a1 - a0)) * (V1 * V1 - V0 * V0 - 2 * a0 * d)) * fSign) / a1;
}

f32 t_1(f32 t10, f32 t11)
{
	if (t10 < 0)
		return t11;
	else
		if (t11 < 0)
			return t10;
		else
			return _min(t10, t11);

}

f32 t_0(f32 V0, f32 V1, f32 a0, f32 a1, f32 t1)
{
	return (V1 - V0 - a1 * t1) / a0;
}

f32 getA(f32 t0, f32 a1, f32 a0)
{
	f32 eps = 0.001f;
	return (t0 < eps) ? a1 : a0;
}

f32 GetCurrAcc(f32 V0, f32 V1, f32 dist, f32 a0, f32 a1)
{
	f32 t10, t11, t0, t1;

	t10 = t_xx(V0, V1, a0, a1, dist, 1.0f);
	t11 = t_xx(V0, V1, a0, a1, dist, -1.0f);
	t1 = t_1(t10, t11);
	t0 = t_0(V0, V1, a0, a1, t1);
	return getA(t0, a1, a0);
}
