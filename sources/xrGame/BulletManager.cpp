// BulletManager.cpp:	для обеспечения полета пули по траектории
//								все пули и осколки передаются сюда
//								(для просчета столкновений и их визуализации)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Level.h"
#include "BulletManager.h"
#include "game_cl_base.h"
#include "Actor.h"
#include "gamepersistent.h"
#include "mt_config.h"
#include "game_cl_base_weapon_usage_statistic.h"

#include "entity.h"
#include "gamemtllib.h"
#include "xrmessages.h"
#include "clsid_game.h"
#include "..\XR_3DA\skeletoncustom.h"
#include "AI/Stalker/ai_stalker.h"
#include "character_info.h"
#include "..\XR_3DA\xr_collide_defs.h"
#include "Weapon.h"

#ifdef DEBUG
#	include "DebugRenderer.h"
#endif

#define HIT_POWER_EPSILON 0.05f
#define WALLMARK_SIZE 0.04f

f32 CBulletManager::m_fMinBulletSpeed = 2.0f;

SBullet::SBullet( )
{ }

SBullet::~SBullet( )
{ }

void SBullet::Init(const fVector3& position,
				   const fVector3& direction,
				   f32 starting_speed,
				   f32 power,
				   f32 impulse,
				   u16	sender_id,
				   u16 sendersweapon_id,
				   ALife::EHitType e_hit_type,
				   f32 maximum_distance,
				   const CCartridge& cartridge,
				   bool SendHit)
{
	flags._storage = 0;
	pos = position;
	speed = max_speed = starting_speed;
	VERIFY(speed > 0);

	VERIFY(direction.magnitude( ) > 0);
	dir.normalize(direction);

	hit_power = power * cartridge.m_kHit;
	hit_impulse = impulse * cartridge.m_kImpulse;

	max_dist = maximum_distance * cartridge.m_kDist;
	fly_dist = 0;

	parent_id = sender_id;
	flags.allow_sendhit = SendHit;
	weapon_id = sendersweapon_id;
	hit_type = e_hit_type;

	pierce = cartridge.m_kPierce;
	ap = cartridge.m_kAP;
	air_resistance = cartridge.m_kAirRes;
	wallmark_size = cartridge.fWallmarkSize;
	m_u8ColorID = cartridge.m_u8ColorID;

	bullet_material_idx = cartridge.bullet_material_idx;
	VERIFY(u16(-1) != bullet_material_idx);

	flags.allow_tracer = !!cartridge.m_flags.test(CCartridge::cfTracer);
	flags.allow_ricochet = !!cartridge.m_flags.test(CCartridge::cfRicochet);
	flags.explosive = !!cartridge.m_flags.test(CCartridge::cfExplosive);
	flags.skipped_frame = 0;

	targetID = 0;
}

//////////////////////////////////////////////////////////
//

CBulletManager::CBulletManager( )
{
	m_Bullets.clear( );
	m_Bullets.reserve(100);
}

CBulletManager::~CBulletManager( )
{
	m_Bullets.clear( );
	m_WhineSounds.clear( );
	m_Events.clear( );
}

#define BULLET_MANAGER_SECTION "bullet_manager"

void CBulletManager::Load( )
{
	m_fTracerWidth = pSettings->r_float(BULLET_MANAGER_SECTION, "tracer_width");
	m_fTracerLengthMax = pSettings->r_float(BULLET_MANAGER_SECTION, "tracer_length_max");
	m_fTracerLengthMin = pSettings->r_float(BULLET_MANAGER_SECTION, "tracer_length_min");

	m_fGravityConst = pSettings->r_float(BULLET_MANAGER_SECTION, "gravity_const");
	m_fAirResistanceK = pSettings->r_float(BULLET_MANAGER_SECTION, "air_resistance_k");

	m_dwStepTime = pSettings->r_u32(BULLET_MANAGER_SECTION, "time_step");
	m_fMinBulletSpeed = pSettings->r_float(BULLET_MANAGER_SECTION, "min_bullet_speed");
	m_fCollisionEnergyMin = pSettings->r_float(BULLET_MANAGER_SECTION, "collision_energy_min");
	m_fCollisionEnergyMax = pSettings->r_float(BULLET_MANAGER_SECTION, "collision_energy_max");

	m_fHPMaxDist = pSettings->r_float(BULLET_MANAGER_SECTION, "hit_probability_max_dist");

	pcstr whine_sounds = pSettings->r_string(BULLET_MANAGER_SECTION, "whine_sounds");
	s32 cnt = _GetItemCount(whine_sounds);
	xr_string tmp;
	for (s32 k = 0; k < cnt; ++k)
	{
		m_WhineSounds.push_back(ref_sound( ));
		m_WhineSounds.back( ).create(_GetItem(whine_sounds, k, tmp), st_Effect, sg_SourceType);
	}

	pcstr explode_particles = pSettings->r_string(BULLET_MANAGER_SECTION, "explode_particles");
	cnt = _GetItemCount(explode_particles);
	for (s32 k = 0; k < cnt; ++k)
	{
		m_ExplodeParticles.push_back(_GetItem(explode_particles, k, tmp));
	}
}

void CBulletManager::PlayExplodePS(const fMatrix4x4& xf)
{
	if (!m_ExplodeParticles.empty( ))
	{
		const shared_str& ps_name = m_ExplodeParticles[Random.randI(0, m_ExplodeParticles.size( ))];

		CParticlesObject* ps = CParticlesObject::Create(*ps_name, TRUE);
		ps->UpdateParent(xf, zero_vel);
		GamePersistent( ).ps_needtoplay.push_back(ps);
	}
}

void CBulletManager::PlayWhineSound(SBullet* bullet, CObject* object, const fVector3& pos)
{
	if (m_WhineSounds.empty( ))
	{
		return;
	}

	if (bullet->m_whine_snd._feedback( ) != NULL)
	{
		return;
	}

	if (bullet->hit_type != ALife::eHitTypeFireWound)
	{
		return;
	}

	bullet->m_whine_snd = m_WhineSounds[Random.randI(0, m_WhineSounds.size( ))];
	bullet->m_whine_snd.play_at_pos(object, pos);
}

void CBulletManager::Clear( )
{
	m_Bullets.clear( );
	m_Events.clear( );
}

void CBulletManager::AddBullet(const fVector3& position,
							   const fVector3& direction,
							   f32 starting_speed,
							   f32 power,
							   f32 impulse,
							   u16	sender_id,
							   u16 sendersweapon_id,
							   ALife::EHitType e_hit_type,
							   f32 maximum_distance,
							   const CCartridge& cartridge,
							   bool SendHit,
							   bool AimBullet)
{
	m_Lock.Enter( );
	VERIFY(u16(-1) != cartridge.bullet_material_idx);
	m_Bullets.push_back(SBullet( ));
	SBullet& bullet = m_Bullets.back( );
	bullet.Init(position, direction, starting_speed, power, impulse, sender_id, sendersweapon_id, e_hit_type, maximum_distance, cartridge, SendHit);
	bullet.frame_num = Device.dwFrame;
	bullet.flags.aim_bullet = AimBullet;

	m_Lock.Leave( );
}

void CBulletManager::UpdateWorkload( )
{
	m_Lock.Enter( );
	u32 delta_time = Device.dwTimeDelta + m_dwTimeRemainder;
	u32 step_num = delta_time / m_dwStepTime;
	m_dwTimeRemainder = delta_time % m_dwStepTime;

	rq_storage.r_clear( );
	rq_spatial.clear_not_free( );

	for (s32 k = m_Bullets.size( ) - 1; k >= 0; k--)
	{
		SBullet& bullet = m_Bullets[k];
		//для пули, пущенной на этом же кадре считаем только 1 шаг
		//(хотя по теории вообще ничего считать на надо)
		//который пропустим на следующем кадре, 
		//это делается для того чтоб при скачках FPS не промазать
		//с 2х метров
		u32 cur_step_num = step_num;

		u32 frames_pass = Device.dwFrame - bullet.frame_num;
		if (frames_pass == 0)
		{
			cur_step_num = 1;
		}
		else if (frames_pass == 1 && step_num > 0)
		{
			cur_step_num -= 1;
		}

		// calculate bullet
		for (u32 i = 0; i < cur_step_num; i++)
		{
			if (!CalcBullet(rq_storage, rq_spatial, &bullet, m_dwStepTime))
			{
				collide::rq_result res;
				RegisterEvent(EVENT_REMOVE, FALSE, &bullet, fVector3( ).set(0.0f, 0.0f, 0.0f), res, (u16) k);
				break;
			}
		}
	}

	m_Lock.Leave( );
}

bool CBulletManager::CalcBullet(collide::rq_results& rq_storage, xr_vector<ISpatial*>& rq_spatial, SBullet* bullet, u32 delta_time)
{
	VERIFY(bullet);

	f32 delta_time_sec = f32(delta_time) / 1000.f;
	f32 range = bullet->speed * delta_time_sec;

	f32 max_range = bullet->max_dist - bullet->fly_dist;
	if (range > max_range)
	{
		range = max_range;
	}

	//запомнить текущую скорость пули, т.к. в
	//RayQuery() она может поменяться из-за рикошетов
	//и столкновений с объектами
	fVector3 cur_dir = bullet->dir;
	bullet_test_callback_data bullet_data;
	bullet_data.pBullet = bullet;
	bullet_data.bStopTracing = true;

	bullet->flags.ricochet_was = 0;

	collide::ray_defs RD(bullet->pos, bullet->dir, range, CDB::OPT_CULL, collide::rqtBoth);
	BOOL result = FALSE;
	VERIFY(!fis_zero(RD.dir.square_magnitude( )));
	result = Level( ).ObjectSpace.RayQuery(rq_storage, RD, firetrace_callback, &bullet_data, test_callback, NULL);

	if (result && bullet_data.bStopTracing)
	{
		range = (rq_storage.r_begin( ) + rq_storage.r_count( ) - 1)->range;
	}
	range = _max(EPSILON_3, range);

	bullet->flags.skipped_frame = (Device.dwFrame >= bullet->frame_num);

	if (!bullet->flags.ricochet_was)
	{
		//изменить положение пули
		bullet->pos.mad(bullet->pos, cur_dir, range);
		bullet->fly_dist += range;

		if (bullet->fly_dist >= bullet->max_dist)
		{
			return false;
		}

		fBox3 level_box = Level( ).ObjectSpace.GetBoundingVolume( );
		if (!((bullet->pos.x >= level_box.x1) &&
			  (bullet->pos.x <= level_box.x2) &&
			  (bullet->pos.y >= level_box.y1) &&
			  (bullet->pos.z >= level_box.z1) &&
			  (bullet->pos.z <= level_box.z2)))
		{
			return false;
		}

		//изменить скорость и направление ее полета с учетом гравитации
		bullet->dir.mul(bullet->speed);

		fVector3 air_resistance = bullet->dir;
		air_resistance.mul(-m_fAirResistanceK * delta_time_sec);

		//		Msg("Speed - %f; ar - %f, %f", bullet->dir.magnitude(), air_resistance.magnitude(), air_resistance.magnitude() / bullet->dir.magnitude( ) * 100.0f);

		bullet->dir.add(air_resistance);
		bullet->dir.y -= m_fGravityConst * delta_time_sec;

		bullet->speed = bullet->dir.magnitude( );
		VERIFY(_valid(bullet->speed));
		VERIFY(!fis_zero(bullet->speed));
		//вместо normalize(),	 чтоб не считать 2 раза magnitude()
#pragma todo("а как насчет bullet->speed==0")
		bullet->dir.x /= bullet->speed;
		bullet->dir.y /= bullet->speed;
		bullet->dir.z /= bullet->speed;
	}

	if (bullet->speed < m_fMinBulletSpeed)
	{
		return false;
	}

	return true;
}

#ifdef DEBUG
BOOL g_bDrawBulletHit = FALSE;
#endif

f32 SqrDistancePointToSegment(const fVector3& pt, const fVector3& orig, const fVector3& dir)
{
	fVector3 diff;
	diff.sub(pt, orig);
	f32 fT = diff.dotproduct(dir);

	if (fT <= 0.0f)
	{
		fT = 0.0f;
	}
	else
	{
		f32 fSqrLen = dir.square_magnitude( );
		if (fT >= fSqrLen)
		{
			fT = 1.0f;
			diff.sub(dir);
		}
		else
		{
			fT /= fSqrLen;
			diff.sub(fVector3( ).mul(dir, fT));
		}
	}

	return diff.square_magnitude( );
}

void CBulletManager::Render( )
{

#ifdef DEBUG
	//0-рикошет
	//1-застрявание пули в материале
	//2-пробивание материала
	if (g_bDrawBulletHit)
	{
		extern FvectorVec g_hit[ ];
		FvectorIt it;
		u32 C[3] = { 0xffff0000,0xff00ff00,0xff0000ff };
		RCache.set_xform_world(Fidentity);
		for (s32 i = 0; i < 3; ++i)
		{
			for (it = g_hit[i].begin( ); it != g_hit[i].end( ); ++it)
			{
				Level( ).debug_renderer( ).draw_aabb(*it, 0.01f, 0.01f, 0.01f, C[i]);
			}
		}
	}
#endif // def DEBUG

	if (m_BulletsRendered.empty( ))
	{
		return;
	}

	u32 vOffset = 0;
	u32 bullet_num = m_BulletsRendered.size( );

	FVF::LIT* verts = (FVF::LIT*) RCache.Vertex.Lock((u32) bullet_num * 8, tracers.sh_Geom->vb_stride, vOffset);
	FVF::LIT* start = verts;

	for (BulletVecIt it = m_BulletsRendered.begin( ); it != m_BulletsRendered.end( ); it++)
	{
		SBullet* bullet = &(*it);
		if (!bullet->flags.allow_tracer)
		{
			continue;
		}

		if (!bullet->flags.skipped_frame)
		{
			continue;
		}

		f32 length = bullet->speed * f32(m_dwStepTime) / 1000.0f;

		if (length < m_fTracerLengthMin)
		{
			continue;
		}

		if (length > m_fTracerLengthMax)
		{
			length = m_fTracerLengthMax;
		}

		f32 width = m_fTracerWidth;
		f32 dist2segSqr = SqrDistancePointToSegment(Device.vCameraPosition, bullet->pos, fVector3( ).mul(bullet->dir, length));
		//---------------------------------------------
		f32 MaxDistSqr = 1.0f;
		f32 MinDistSqr = 0.09f;
		if (dist2segSqr < MaxDistSqr)
		{
			if (dist2segSqr < MinDistSqr)
			{
				dist2segSqr = MinDistSqr;
			}

			width *= _sqrt(dist2segSqr / MaxDistSqr);
		}

		if (Device.vCameraPosition.distance_to_sqr(bullet->pos) < (length * length))
		{
			length = Device.vCameraPosition.distance_to(bullet->pos) - 0.3f;
		}

		/*
		//---------------------------------------------
		fVector3 vT, v0, v1;
		vT.mad(Device.vCameraPosition, Device.vCameraDirection, _sqrt(dist2segSqr));
		v0.mad(vT, Device.vCameraTop, width*.5f);
		v1.mad(vT, Device.vCameraTop, -width*.5f);
		fVector3 v0r, v1r;
		Device.mFullTransform.transform(v0r, v0);
		Device.mFullTransform.transform(v1r, v1);
		f32 ViewWidth = v1r.distance_to(v0r);
*/
//		f32 dist = _sqrt(dist2segSqr);
//		Msg("dist - [%f]; ViewWidth - %f, [%f]", dist, ViewWidth, ViewWidth*f32(Device.dwHeight));
//		Msg("dist - [%f]", dist);
		//---------------------------------------------

		fVector3 center;
		center.mad(bullet->pos, bullet->dir, -length * 0.5f);
		tracers.Render(verts, bullet->pos, center, bullet->dir, length, width, bullet->m_u8ColorID);
	}

	u32 vCount = (u32) (verts - start);
	RCache.Vertex.Unlock(vCount, tracers.sh_Geom->vb_stride);

	if (vCount)
	{
		RCache.set_CullMode(CULL_NONE);
		RCache.set_xform_world(Fidentity);
		RCache.set_Shader(tracers.sh_Tracer);
		RCache.set_Geometry(tracers.sh_Geom);
		RCache.Render(D3DPT_TRIANGLELIST, vOffset, 0, vCount, 0, vCount / 2);
		RCache.set_CullMode(CULL_CCW);
	}
}

void CBulletManager::CommitRenderSet( )	// @ the end of frame
{
	m_BulletsRendered = m_Bullets;
	if (g_mt_config.test(mtBullets))
	{
		Device.seqParallel.push_back(fastdelegate::FastDelegate0<>(this, &CBulletManager::UpdateWorkload));
	}
	else
	{
		UpdateWorkload( );
	}
}
void CBulletManager::CommitEvents( )	// @ the start of frame
{
	for (u32 _it = 0; _it < m_Events.size( ); _it++)
	{
		_event& E = m_Events[_it];
		switch (E.Type)
		{
			case EVENT_HIT:
			{
				if (E.dynamic)
				{
					DynamicObjectHit(E);
				}
				else
				{
					StaticObjectHit(E);
				}
			}
			break;
			case EVENT_REMOVE:
			{
				m_Bullets[E.tgt_material] = m_Bullets.back( );
				m_Bullets.pop_back( );
			}
			break;
		}
	}

	m_Events.clear_and_reserve( );
}

void CBulletManager::RegisterEvent(EEventType Type, BOOL _dynamic, SBullet* bullet, const fVector3& end_point, collide::rq_result& R, u16 tgt_material)
{
	m_Events.push_back(_event( ));
	_event& E = m_Events.back( );
	E.Type = Type;
	E.bullet = *bullet;

	switch (Type)
	{
		case EVENT_HIT:
		{
			E.dynamic = _dynamic;
			E.result = ObjectHit(bullet, end_point, R, tgt_material, E.normal);
			E.point = end_point;
			E.R = R;
			E.tgt_material = tgt_material;
			if (_dynamic)
			{
				E.Repeated = (R.O->ID( ) == E.bullet.targetID);
				bullet->targetID = R.O->ID( );
			}
		}
		break;
		case EVENT_REMOVE:
		{
			E.tgt_material = tgt_material;
		}
		break;
	}
}

//--------------------------------------------------------------------------------------------------
//константы shoot_factor, определяющие поведение пули при столкновении с объектом
#define STUCK_THRESHOLD			0.4f

//расстояния не пролетев которого пуля не трогает того кто ее пустил
#define PARENT_IGNORE_DIST		3.0f
extern f32 gCheckHitK;

//test callback функция 
//  object - object for testing
//return TRUE-тестировать объект / FALSE-пропустить объект
BOOL CBulletManager::test_callback(const collide::ray_defs& rd, CObject* object, LPVOID params)
{
	bullet_test_callback_data* pData = (bullet_test_callback_data*) params;
	SBullet* bullet = pData->pBullet;

	if ((object->ID( ) == bullet->parent_id) &&
		(bullet->fly_dist < PARENT_IGNORE_DIST) &&
		(!bullet->flags.ricochet_was))
	{
		return FALSE;
	}

	BOOL bRes = TRUE;
	if (object)
	{
		CEntity* entity = smart_cast<CEntity*>(object);
		if (entity && entity->g_Alive( ) && (entity->ID( ) != bullet->parent_id))
		{
			ICollisionForm* cform = entity->collidable.model;
			if ((NULL != cform) && (cftObject == cform->Type( )))
			{
				CActor* actor = smart_cast<CActor*>(entity);
				CStalker* stalker = smart_cast<CStalker*>(entity);
				// в кого попали?
				if (actor || stalker)
				{
					// попали в актера или сталкера
					fSphere S = cform->getSphere( );
					entity->XFORM( ).transform_tiny(S.P);
					f32 dist = rd.range;
					// проверим попали ли мы в описывающую сферу 
					if (fSphere::rpNone != S.intersect_full(bullet->pos, bullet->dir, dist))
					{
						// да попали, найдем кто стрелял
						bool play_whine = true;
						CObject* initiator = Level( ).Objects.net_Find(bullet->parent_id);
						if (actor)
						{
							// попали в актера
							f32 hpf = 1.0f;
							f32 ahp = actor->HitProbability( );
#if 1
#	if 0
							CObject* weapon_object = Level( ).Objects.net_Find(bullet->weapon_id);
							if (weapon_object)
							{
								CWeapon* weapon = smart_cast<CWeapon*>(weapon_object);
								if (weapon)
								{
									f32 fly_dist = bullet->fly_dist + dist;
									f32 dist_factor = _min(1.0f, fly_dist / Level( ).BulletManager( ).m_fHPMaxDist);
									ahp = dist_factor * weapon->hit_probability( ) + (1.0f - dist_factor) * 1.0f;
								}
							}
#	else
							f32 game_difficulty_hit_probability = actor->HitProbability( );
							CStalker* stalker = smart_cast<CStalker*>(initiator);
							if (stalker)
							{
								hpf = stalker->SpecificCharacter( ).hit_probability_factor( );
							}

							f32 dist_factor = 1.0f;
							CObject* weapon_object = Level( ).Objects.net_Find(bullet->weapon_id);
							if (weapon_object)
							{
								CWeapon* weapon = smart_cast<CWeapon*>(weapon_object);
								if (weapon)
								{
									game_difficulty_hit_probability = weapon->hit_probability( );
									f32 fly_dist = bullet->fly_dist + dist;
									dist_factor = _min(1.f, fly_dist / Level( ).BulletManager( ).m_fHPMaxDist);
								}
							}

							ahp = dist_factor * game_difficulty_hit_probability + (1.f - dist_factor) * 1.f;
#	endif
#else
							CStalker* i_stalker = smart_cast<CStalker*>(initiator);
							// если стрелял сталкер, учитываем - hit_probability_factor сталкерa иначе - 1.0
							if (i_stalker)
							{
								hpf = i_stalker->SpecificCharacter( ).hit_probability_factor( );
								f32 fly_dist = bullet->fly_dist + dist;
								f32 dist_factor = _min(1.0f, fly_dist / Level( ).BulletManager( ).m_fHPMaxDist);
								ahp = dist_factor * actor->HitProbability( ) + (1.0f - dist_factor) * 1.0f;
							}
#endif
							if (Random.randF(0.0f, 1.0f) > (ahp * hpf))
							{
								bRes = FALSE;		// don't hit actor
								play_whine = true;		// play whine sound
							}
							else
							{
								// real test actor CFORM
								Level( ).BulletManager( ).m_rq_results.r_clear( );

								if (cform->_RayQuery(rd, Level( ).BulletManager( ).m_rq_results))
								{
									bRes = TRUE;		// hit actor
									play_whine = false;	// don't play whine sound
								}
								else
								{
									bRes = FALSE;	// don't hit actor
									play_whine = true;		// play whine sound
								}
							}
						}
						// play whine sound
						if (play_whine)
						{
							fVector3 pt;
							pt.mad(bullet->pos, bullet->dir, dist);
							Level( ).BulletManager( ).PlayWhineSound(bullet, initiator, pt);
						}
					}
					else
					{
						// don't test this object again (return FALSE)
						bRes = FALSE;
					}
				}
			}
		}
	}

	return bRes;
}

//callback функция 
//	result.O;		// 0-static else CObject*
//	result.range;	// range from start to element 
//	result.element;	// if (O) "num tri" else "num bone"
//	params;			// user defined abstract data
//	Device.Statistic.TEST0.End();
//return TRUE-продолжить трассировку / FALSE-закончить трассировку
BOOL CBulletManager::firetrace_callback(collide::rq_result& result, LPVOID params)
{
	bullet_test_callback_data* pData = (bullet_test_callback_data*) params;
	pData->bStopTracing = true;
	SBullet* bullet = pData->pBullet;

	//вычислить точку попадания
	fVector3 end_point;
	end_point.mad(bullet->pos, bullet->dir, result.range);

	u16 hit_material_idx = GAMEMTL_NONE_IDX;

	//динамический объект
	if (result.O)
	{
		//получить косточку и ее материал
		CKinematics* V = 0;
		//если мы попали по родителю на первых же
		//кадре, то игнорировать это, так как это он
		//и стрелял
		VERIFY(!(result.O->ID( ) == bullet->parent_id && bullet->fly_dist < PARENT_IGNORE_DIST));
		if (0 != (V = smart_cast<CKinematics*>(result.O->Visual( ))))
		{
			CBoneData& B = V->LL_GetData((u16) result.element);
			hit_material_idx = B.game_mtl_idx;
			Level( ).BulletManager( ).RegisterEvent(EVENT_HIT, TRUE, bullet, end_point, result, hit_material_idx);
		}
	}
	else
	{
		//статический объект
		//получить треугольник и узнать его материал
		CDB::TRI* T = Level( ).ObjectSpace.GetStaticTris( ) + result.element;
		hit_material_idx = T->material;

		SGameMtl* mtl = GMLib.GetMaterialByIdx(hit_material_idx);
		if (fsimilar(mtl->fShootFactor, 1.0f, EPSILON_5))//Если материал полностью простреливаемый
		{
			pData->bStopTracing = false;
		}
		else
		{
			Level( ).BulletManager( ).RegisterEvent(EVENT_HIT, FALSE, bullet, end_point, result, hit_material_idx);
		}
	}

	//проверить достаточно ли силы хита, чтобы двигаться дальше
	if (bullet->speed < m_fMinBulletSpeed || bullet->flags.ricochet_was)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void CBulletManager::FireShotmark(SBullet* bullet, const fVector3& vDir, const fVector3& vEnd, collide::rq_result& R, u16 target_material, const fVector3& vNormal, bool ShowMark)
{
	SGameMtlPair* mtl_pair = GMLib.GetMaterialPair(bullet->bullet_material_idx, target_material);
	fVector3 particle_dir;

	if (R.O)
	{
		particle_dir = vDir;
		particle_dir.invert( );

		//на текущем актере отметок не ставим
		if (Level( ).CurrentEntity( ) && Level( ).CurrentEntity( )->ID( ) == R.O->ID( ))
		{
			return;
		}

		ref_shader* pWallmarkShader = (!mtl_pair || mtl_pair->CollideMarks.empty( )) ? NULL : &mtl_pair->CollideMarks[::Random.randI(0, mtl_pair->CollideMarks.size( ))];;

		if (pWallmarkShader && ShowMark)
		{
			//добавить отметку на материале
			fVector3 p;
			p.mad(bullet->pos, bullet->dir, R.range - 0.01f);
			::Render->add_SkeletonWallmark(&R.O->renderable.xform,
										   PKinematics(R.O->Visual( )), *pWallmarkShader,
										   p, bullet->dir, bullet->wallmark_size);
		}
	}
	else
	{
		//вычислить нормаль к пораженной поверхности
		particle_dir = vNormal;
		fVector3* pVerts = Level( ).ObjectSpace.GetStaticVerts( );
		CDB::TRI* pTri = Level( ).ObjectSpace.GetStaticTris( ) + R.element;

		ref_shader* pWallmarkShader = (!mtl_pair || mtl_pair->CollideMarks.empty( )) ? NULL : &mtl_pair->CollideMarks[::Random.randI(0, mtl_pair->CollideMarks.size( ))];;

		if (pWallmarkShader && ShowMark)
		{
			//добавить отметку на материале
			::Render->add_StaticWallmark(*pWallmarkShader, vEnd, bullet->wallmark_size, pTri, pVerts);
		}
	}

	ref_sound* pSound = (!mtl_pair || mtl_pair->CollideSounds.empty( )) ? NULL : &mtl_pair->CollideSounds[::Random.randI(0, mtl_pair->CollideSounds.size( ))];

	//проиграть звук
	if (pSound && ShowMark)
	{
		CObject* O = Level( ).Objects.net_Find(bullet->parent_id);
		bullet->m_mtl_snd = *pSound;
		bullet->m_mtl_snd.play_at_pos(O, vEnd, 0);
	}

	pcstr ps_name = (!mtl_pair || mtl_pair->CollideParticles.empty( )) ? NULL : *mtl_pair->CollideParticles[::Random.randI(0, mtl_pair->CollideParticles.size( ))];

	SGameMtl* tgt_mtl = GMLib.GetMaterialByIdx(target_material);
	BOOL bStatic = !tgt_mtl->Flags.test(SGameMtl::flDynamic);

	if ((ps_name && ShowMark) || (bullet->flags.explosive && bStatic))
	{
		fMatrix4x4 pos;
		pos.k.normalize(particle_dir);
		fVector3::generate_orthonormal_basis(pos.k, pos.j, pos.i);
		pos.c.set(vEnd);
		if (ps_name && ShowMark)
		{
			//отыграть партиклы попадания в материал
			CParticlesObject* ps = CParticlesObject::Create(ps_name, TRUE);

			ps->UpdateParent(pos, zero_vel);
			GamePersistent( ).ps_needtoplay.push_back(ps);
		}

		if (bullet->flags.explosive && bStatic)
		{
			PlayExplodePS(pos);
		}
	}
}

void CBulletManager::StaticObjectHit(CBulletManager::_event& E)
{
	//	fVector3 hit_normal;
	FireShotmark(&E.bullet, E.bullet.dir, E.point, E.R, E.tgt_material, E.normal);
	//	ObjectHit	(&E.bullet,					E.point, E.R, E.tgt_material, hit_normal);
}

static bool g_clear = false;
void CBulletManager::DynamicObjectHit(CBulletManager::_event& E)
{
	//только для динамических объектов
	VERIFY(E.R.O);
	if (g_clear)
	{
		E.Repeated = false;
	}

	E.Repeated = false;
	bool NeedShootmark = true;//!E.Repeated;

	if (E.R.O->CLS_ID == CLSID_OBJECT_ACTOR)
	{
		game_PlayerState* ps = Game( ).GetPlayerByGameID(E.R.O->ID( ));
		if (ps && ps->testFlag(GAME_PLAYER_FLAG_INVINCIBLE))
		{
			NeedShootmark = false;
		}
	}

	//визуальное обозначение попадание на объекте
//	fVector3			hit_normal;
	FireShotmark(&E.bullet, E.bullet.dir, E.point, E.R, E.tgt_material, E.normal, NeedShootmark);

	fVector3 original_dir = E.bullet.dir;
	f32 power;
	f32 impulse;
	std::pair<f32, f32> hit_result = E.result; //ObjectHit(&E.bullet, E.end_point, E.R, E.tgt_material, hit_normal);
	power = hit_result.first;
	impulse = hit_result.second;

	// object-space
	//вычислить координаты попадания
	fVector3 p_in_object_space;
	fVector3 position_in_bone_space;
	fMatrix4x4 m_inv;
	m_inv.invert(E.R.O->XFORM( ));
	m_inv.transform_tiny(p_in_object_space, E.point);

	// bone-space
	CKinematics* V = smart_cast<CKinematics*>(E.R.O->Visual( ));
	if (V)
	{
		VERIFY3(V->LL_GetBoneVisible(u16(E.R.element)), *E.R.O->cNameVisual( ), V->LL_BoneName_dbg(u16(E.R.element)));
		fMatrix4x4& m_bone = (V->LL_GetBoneInstance(u16(E.R.element))).mTransform;
		fMatrix4x4 m_inv_bone;
		m_inv_bone.invert(m_bone);
		m_inv_bone.transform_tiny(position_in_bone_space, p_in_object_space);
	}
	else
	{
		position_in_bone_space.set(p_in_object_space);
	}

	//отправить хит пораженному объекту
	if (E.bullet.flags.allow_sendhit && !E.Repeated)
	{
		//-------------------------------------------------
		bool AddStatistic = false;

		SHit	Hit = SHit(power,
						   original_dir,
						   NULL,
						   u16(E.R.element),
						   position_in_bone_space,
						   impulse,
						   E.bullet.hit_type,
						   E.bullet.ap,
						   E.bullet.flags.aim_bullet);

		Hit.GenHeader(u16((AddStatistic) ? GE_HIT_STATISTIC : GE_HIT) & 0xffff, E.R.O->ID( ));
		Hit.whoID = E.bullet.parent_id;
		Hit.weaponID = E.bullet.weapon_id;
		Hit.BulletID = E.bullet.m_dwID;

		CNetPacket np;
		Hit.Write_Packet(np);

		//		Msg("Hit sended: %d[%d,%d]", Hit.whoID, Hit.weaponID, Hit.BulletID);
		CGameObject::u_EventSend(np);
	}
}

#ifdef DEBUG
FvectorVec g_hit[3];
#endif // def DEBUG

extern void random_dir(fVector3& tgt_dir, const fVector3& src_dir, f32 dispersion);

std::pair<f32, f32>  CBulletManager::ObjectHit(SBullet* bullet, const fVector3& end_point,
											   collide::rq_result& R, u16 target_material,
											   fVector3& hit_normal)
{
	//----------- normal - start
	if (R.O)
	{
		//вернуть нормаль по которой играть партиклы
		CCF_Skeleton* skeleton = smart_cast<CCF_Skeleton*>(R.O->CFORM( ));
		if (skeleton)
		{
			fVector3 e_center;
			hit_normal.set(0.0f, 0.0f, 0.0f);
			if (skeleton->_ElementCenter((u16) R.element, e_center))
			{
				hit_normal.sub(end_point, e_center);
			}

			f32 len = hit_normal.square_magnitude( );
			if (!fis_zero(len))
			{
				hit_normal.div(_sqrt(len));
			}
			else
			{
				hit_normal.invert(bullet->dir);
			}
		}
	}
	else
	{
		//вычислить нормаль к поверхности
		fVector3* pVerts = Level( ).ObjectSpace.GetStaticVerts( );
		CDB::TRI* pTri = Level( ).ObjectSpace.GetStaticTris( ) + R.element;
		hit_normal.mknormal(pVerts[pTri->verts[0]], pVerts[pTri->verts[1]], pVerts[pTri->verts[2]]);
	}
	//----------- normal - end
	f32 old_speed, energy_lost;
	old_speed = bullet->speed;

	//коэффициент уменьшение силы с падением скорости
	f32 speed_factor = bullet->speed / bullet->max_speed;
	//получить силу хита выстрела с учетом патрона
	f32 power = bullet->hit_power * speed_factor;

	SGameMtl* mtl = GMLib.GetMaterialByIdx(target_material);

	//shoot_factor: коеффициент указывающий на текущие свойства пули 
	//(Если меньше 1, то пуля либо рикошетит(если контакт идёт по касательной), либо застряёт в текущем 
	//объекте, если больше 1, то пуля прошивает объект)
	f32 shoot_factor = mtl->fShootFactor * bullet->pierce * speed_factor;

	f32 impulse = 0.0f;

#ifdef DEBUG
	fVector3 dbg_bullet_pos;
	dbg_bullet_pos.mad(bullet->pos, bullet->dir, R.range);
	s32 bullet_state = 0;
#endif // def DEBUG

	if (fsimilar(mtl->fShootFactor, 1.0f, EPSILON_5))//Если материал полностью простреливаемый, то
	{

#ifdef DEBUG
		bullet_state = 2;
#endif // def DEBUG

		return std::make_pair(power, impulse);
	}

	//рикошет
	fVector3 new_dir;
	new_dir.reflect(bullet->dir, hit_normal);
	fVector3 tgt_dir;
	random_dir(tgt_dir, new_dir, deg2rad(10.f));

	f32 ricoshet_factor = bullet->dir.dotproduct(tgt_dir);

	f32 f = Random.randF(0.5f, 1.f);

	if (((f + shoot_factor) < ricoshet_factor) && bullet->flags.allow_ricochet)
	{
		//уменьшение скорости полета в зависимости 
		//от угла падения пули (чем прямее угол, тем больше потеря)
		f32 scale = 1.0f - _abs(bullet->dir.dotproduct(hit_normal)) * m_fCollisionEnergyMin;
		clamp(scale, 0.0f, m_fCollisionEnergyMax);

		//вычисление рикошета, делается немного фейком,
		//т.к. пуля остается в точке столкновения
		//и сразу выходит из RayQuery()
		bullet->dir.set(tgt_dir);
		bullet->pos = end_point;
		bullet->flags.ricochet_was = 1;

		//уменьшить скорость в зависимости от простреливаемости
		bullet->speed *= (1 - mtl->fShootFactor) * scale;
		//сколько энергии в процентах потеряла пуля при столкновении
		f32 energy_lost = 1.f - bullet->speed / old_speed;
		//импульс переданный объекту равен прямопропорционален потерянной энергии
		impulse = bullet->hit_impulse * speed_factor * energy_lost;

#ifdef DEBUG
		bullet_state = 0;
#endif // def DEBUG

	}
	else if (shoot_factor < 1.0f)
	{
		//застрявание пули в материале
		bullet->speed = 0.0f;
		//передаем весь импульс целиком
		impulse = bullet->hit_impulse * speed_factor;

#ifdef DEBUG
		bullet_state = 1;
#endif // def DEBUG

	}
	else
	{
		//пробивание материала
		//уменьшить скорость пропорцианально потраченому импульсу
		bullet->speed *= mtl->fShootFactor;
		energy_lost = 1.0f - bullet->speed / old_speed;
		impulse = bullet->hit_impulse * speed_factor * energy_lost;

		bullet->pos.mad(bullet->pos, bullet->dir, EPSILON_5);//fake
		//ввести коэффициент случайности при простреливании
		fVector3 rand_normal;
		rand_normal.random_dir(bullet->dir, deg2rad(5.f) * energy_lost, Random);
		bullet->dir.set(rand_normal);

#ifdef DEBUG
		bullet_state = 2;
#endif // def DEBUG

	}

#ifdef DEBUG
	extern BOOL g_bDrawBulletHit;
	if (g_bDrawBulletHit)
	{
		g_hit[bullet_state].push_back(dbg_bullet_pos);
	}
#endif // def DEBUG

	return std::make_pair(power, impulse);
}
