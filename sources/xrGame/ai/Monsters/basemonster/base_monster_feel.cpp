////////////////////////////////////////////////////////////////////////////
//	Module 		: base_monster_feel.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Visibility and look for all the biting monsters
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "base_monster.h"
#include "../../../Actor.h"
#include "../../../ActorEffector.h"
#include "../ai_monster_effector.h"
#include "../../../hudmanager.h"
#include "../../../clsid_game.h"
#include "../../../..\XR_3DA\skeletonanimated.h"
#include "../../../sound_player.h"
#include "../../../level.h"
#include "../../../script_callback_ex.h"
#include "../../../script_game_object.h"
#include "../../../GameObject_space.h"
#include "../../../ai_monster_space.h"
#include "../control_animation_base.h"
#include "../../../ui/UIGameCustom.h"
#include "../../../UI/UIStatic.h"
#include "../../../ai_object_location.h"
#include "../../../Profiler.h"
#include "../../../..\XR_3DA\CameraBase.h"

void CBaseMonster::feel_sound_new(CObject* who, s32 eType, CSound_UserDataPtr user_data, const fVector3& Position, f32 power)
{
	if (!g_Alive())		return;

	// ignore my sounds
	if (this == who)	return;

	if (user_data)
		user_data->accept	(sound_user_data_visitor());

	// ignore unknown sounds
	if (eType == 0xffffffff) return;

	// ignore distant sounds
	fVector3 center;
	Center	(center);
	f32 dist	= center.distance_to(Position);
	if (dist > db().m_max_hear_dist)	return;

	// ignore sounds if not from enemies and not help sounds
	CEntityAlive* entity = smart_cast<CEntityAlive*> (who);

	// ignore sound if enemy drop a weapon on death
	if (!entity && ((eType & eST_ITEM_HIDING) == eST_ITEM_HIDING)) return;

	if (entity && (!EnemyMan.is_enemy(entity))) {
		SoundMemory.check_help_sound(eType, entity->ai_location().level_vertex_id());
		return;
	}
	
	if ((eType & eST_WEAPON_SHOOTING) == eST_WEAPON_SHOOTING) power = 1.f;

	if (((eType & eST_WEAPON_BULLET_HIT) == eST_WEAPON_BULLET_HIT) && (dist < 2.f))
		HitMemory.add_hit(who,eSideFront);

	// execute callback
	sound_callback	(who,eType,Position,power);
	
	// register in sound memory
	if (power >= db().m_fSoundThreshold) {
		SoundMemory.HearSound(who,eType,Position,power,Device.dwTimeGlobal);
 	}
}
#define MAX_LOCK_TIME 2.f

void CBaseMonster::HitEntity(const CEntity *pEntity, f32 fDamage, f32 impulse, fVector3& dir)
{
	if (!g_Alive()) return;
	if (!pEntity || pEntity->getDestroy()) return;

	if (!EnemyMan.get_enemy()) return;

	if (EnemyMan.get_enemy() == pEntity) {
		fVector3 position_in_bone_space;
		position_in_bone_space.set(0.0f,0.0f,0.0f);

		// ������� �� ��������� ��������� � ������� ������� ����������� ��������
		fVector3 hit_dir;
		XFORM().transform_dir	(hit_dir,dir);
		hit_dir.normalize		();

		CEntity		*pEntityNC	= const_cast<CEntity*>(pEntity);
		VERIFY		(pEntityNC);
		
		CNetPacket	l_P;
		SHit		HS;
		HS.GenHeader(GE_HIT, pEntityNC->ID());													//		u_EventGen	(l_P,GE_HIT, pEntityNC->ID());
		HS.whoID			= (ID());															//		l_P.w_u16	(ID());
		HS.weaponID			= (ID());															//		l_P.w_u16	(ID());
		HS.dir				= (hit_dir);														//		l_P.w_dir	(hit_dir);
		HS.power			= (fDamage);														//		l_P.w_float	(fDamage);
		HS.boneID			= (smart_cast<CKinematics*>(pEntityNC->Visual())->LL_GetBoneRoot());//		l_P.w_s16	(smart_cast<CKinematics*>(pEntityNC->Visual())->LL_GetBoneRoot());
		HS.p_in_bone_space	= (position_in_bone_space);											//		l_P.w_vec3	(position_in_bone_space);
		HS.impulse			= (impulse);														//		l_P.w_float	(impulse);
		HS.hit_type			= (ALife::eHitTypeWound);											//		l_P.w_u16	( u16(ALife::eHitTypeWound) );
		HS.Write_Packet(l_P);
		u_EventSend	(l_P);
		
		if (pEntityNC == Actor()) {
			START_PROFILE("BaseMonster/Animation/HitEntity");
			SDrawStaticStruct* s = HUD().GetUI()->UIGame()->AddCustomStatic("monster_claws", false);
			s->m_endTime = Device.fTimeGlobal+3.0f;// 3sec
			
			f32 h1;
			f32 p1;
			Device.vCameraDirection.getHP	(h1,p1);

			fVector3 hd = hit_dir;
			hd.mul(-1);
			f32 d = -h1 + hd.getH();
			s->wnd()->SetHeading	(d);
			s->wnd()->SetHeadingPivot(fVector2().set(256.0f, 512.0f));
			STOP_PROFILE;

			//SetAttackEffector			();
			
			f32 time_to_lock		= fDamage * MAX_LOCK_TIME;
			clamp					(time_to_lock, 0.f, MAX_LOCK_TIME);
			Actor()->lock_accel_for	(s32(time_to_lock * 1000));

			//////////////////////////////////////////////////////////////////////////
			//
			//////////////////////////////////////////////////////////////////////////
			
			CEffectorCam* ce = Actor()->Cameras().GetCamEffector((ECamEffectorType)effBigMonsterHit);
			if(!ce)
			{
				const shared_str&	eff_sect = pSettings->r_string(cNameSect(), "actor_hit_effect");	
				if(eff_sect.c_str())
				{
					s32 id						= -1;
					fVector3					cam_pos;
					fVector3					cam_dir;
					fVector3					cam_norm;
					Actor()->cam_Active()->Get	(cam_pos,cam_dir,cam_norm);
					cam_dir.normalize_safe		();
					dir.normalize_safe			();

					f32 ang_diff				= angle_difference	(cam_dir.getH(), dir.getH());
					fVector3						cp;
					cp.crossproduct				(cam_dir,dir);
					bool bUp					=(cp.y>0.0f);

					fVector3 cross;
					cross.crossproduct			(cam_dir, dir);
					VERIFY						(ang_diff>=0.0f && ang_diff<=PI);

					f32 _s1 = PI_DIV_8;
					f32 _s2 = _s1+PI_DIV_4;
					f32 _s3 = _s2+PI_DIV_4;
					f32 _s4 = _s3+PI_DIV_4;

					if(ang_diff<=_s1){
						id = 2;
					}else {
						if(ang_diff>_s1 && ang_diff<=_s2){
							id = (bUp)?5:7;
						}else
							if(ang_diff>_s2 && ang_diff<=_s3){
								id = (bUp)?3:1;
							}else
								if(ang_diff>_s3 && ang_diff<=_s4){
									id = (bUp)?4:6;
								}else
									if(ang_diff>_s4){
										id = 0;
									}else{
										VERIFY(0);
									}
					}
					
					string64				sect_name;

					sprintf_s					(sect_name,"%s_%d",eff_sect.c_str(), id);
					AddEffector				(Actor(), effBigMonsterHit, sect_name, fDamage);
				}
			}
			//////////////////////////////////////////////////////////////////////////
			

		}

		Morale.on_attack_success();
		
		m_time_last_attack_success	= Device.dwTimeGlobal;
	}
}


BOOL  CBaseMonster::feel_vision_isRelevant(CObject* O)
{
	if (!g_Alive())					return FALSE;
	if (0==smart_cast<CEntity*>(O))	return FALSE;
	
	if ((O->spatial.type & STYPE_VISIBLEFORAI) != STYPE_VISIBLEFORAI) return FALSE;
	
	// ���� ����, �� ������ �� �����
	if (m_bSleep) return FALSE;
	
	// ���� �� ���� - �� �����
	CEntityAlive* entity = smart_cast<CEntityAlive*> (O);
	if (entity && entity->g_Alive()) {
		if (!EnemyMan.is_enemy(entity)) {
			// ���� ����� ����� - ��������� ������� � ���� ������
			CBaseMonster *monster = smart_cast<CBaseMonster *>(entity);
			if (monster && !m_skip_transfer_enemy) EnemyMan.transfer_enemy(monster);
			return FALSE;
		}
	}

	return TRUE;
}

void CBaseMonster::HitSignal(f32 amount, fVector3& vLocalDir, CObject* who, s16 element)
{
	if (!g_Alive()) return;
	
	feel_sound_new(who, eST_WEAPON_SHOOTING,0,who->Position(),1.0f);
	if (g_Alive()) sound().play(MonsterSound::eMonsterSoundTakeDamage);

	if (element < 0) return;

	// ���������� ����������� ���� (����� || ��� || ���� || �����)
	f32 yaw;
	f32 pitch;
	vLocalDir.getHP(yaw,pitch);
	
	yaw = angle_normalize(yaw);
	
	EHitSide hit_side = eSideFront;
	if ((yaw >= PI_DIV_4) && (yaw <= 3*PI_DIV_4)) hit_side = eSideLeft;
	else if ((yaw >= 3 * PI_DIV_4) && (yaw <= 5*PI_DIV_4)) hit_side = eSideBack;
	else if ((yaw >= 5 * PI_DIV_4) && (yaw <= 7*PI_DIV_4)) hit_side = eSideRight;

	anim().FX_Play	(hit_side, 1.0f);

	HitMemory.add_hit	(who,hit_side);

	Morale.on_hit		();

	callback(GameObject::eHit)(
		lua_game_object(), 
		amount,
		vLocalDir,
		smart_cast<const CGameObject*>(who)->lua_game_object(),
		element
	);

	// ���� ������� - �������� ��� �����
	CEntityAlive	*obj = smart_cast<CEntityAlive*>(who);
	if (obj && (tfGetRelationType(obj) == ALife::eRelationTypeNeutral)) EnemyMan.add_enemy(obj);
}

void CBaseMonster::SetAttackEffector() 
{
	CActor *pA = smart_cast<CActor *>(Level().CurrentEntity());
	if (pA) {
		Actor()->Cameras().AddCamEffector(xr_new<CMonsterEffectorHit>(db().m_attack_effector.ce_time,db().m_attack_effector.ce_amplitude,db().m_attack_effector.ce_period_number,db().m_attack_effector.ce_power));
		Actor()->Cameras().AddPPEffector(xr_new<CMonsterEffector>(db().m_attack_effector.ppi, db().m_attack_effector.time, db().m_attack_effector.time_attack, db().m_attack_effector.time_release));
	}
}

void CBaseMonster::Hit_Psy(CObject *object, f32 value)
{
	CNetPacket		P;
	SHit			HS;
	HS.GenHeader		(GE_HIT, object->ID());				//					//	u_EventGen		(P,GE_HIT, object->ID());				// 
	HS.whoID			= (ID());									// own		//	P.w_u16			(ID());									// own
	HS.weaponID			= (ID());									// own		//	P.w_u16			(ID());									// own
	HS.dir				= (fVector3().set(0.0f,1.0f,0.0f));			// direction	//	P.w_dir			(fVector3().set(0.f,1.f,0.f));			// direction
	HS.power			= (value);								// hit value	//	P.w_float		(value);								// hit value
	HS.boneID			= (BI_NONE);								// bone		//	P.w_s16			(BI_NONE);								// bone
	HS.p_in_bone_space	= (fVector3().set(0.0f,0.0f,0.0f));							//	P.w_vec3		(fVector3().set(0.f,0.f,0.f));			
	HS.impulse			= (0.f);												//	P.w_float		(0.f);									
	HS.hit_type			= (ALife::eHitTypeTelepatic);							//	P.w_u16			(u16(ALife::eHitTypeTelepatic));
	HS.Write_Packet	(P);
	u_EventSend		(P);
}

void CBaseMonster::Hit_Wound(CObject *object, f32 value, const fVector3& dir, f32 impulse)
{
	CNetPacket	P;
	SHit		HS;
	HS.GenHeader(GE_HIT, object->ID());														//	u_EventGen	(P,GE_HIT, object->ID());
	HS.whoID			= (ID());															//	P.w_u16		(ID());
	HS.weaponID			= (ID());															//	P.w_u16		(ID());
	HS.dir				= (dir);															//	P.w_dir		(dir);
	HS.power			= (value);															//	P.w_float	(value);
	HS.boneID			= (smart_cast<CKinematics*>(object->Visual())->LL_GetBoneRoot());	//	P.w_s16		(smart_cast<CKinematics*>(object->Visual())->LL_GetBoneRoot());
	HS.p_in_bone_space	= (fVector3().set(0.0f,0.0f,0.0f));										//	P.w_vec3	(fVector3().set(0.f,0.f,0.f));
	HS.impulse			= (impulse);														//	P.w_float	(impulse);
	HS.hit_type			= (ALife::eHitTypeWound);											//	P.w_u16		(u16(ALife::eHitTypeWound));
	HS.Write_Packet(P);
	u_EventSend	(P);
}

bool CBaseMonster::critical_wound_external_conditions_suitable	()
{
	if (!control().check_start_conditions(ControlCom::eControlSequencer)) 
		return false;

	if (!anim().IsStandCurAnim()) return false;

	return true;
}

void CBaseMonster::critical_wounded_state_start() 
{
	VERIFY	(m_critical_wound_type != u32(-1));

	pcstr anim = 0;
	switch (m_critical_wound_type)	{
	case critical_wound_type_head:
		anim = m_critical_wound_anim_head;
		break;
	case critical_wound_type_torso:
		anim = m_critical_wound_anim_torso;
		break;
	case critical_wound_type_legs:
		anim = m_critical_wound_anim_legs;
		break;
	}

	VERIFY	(anim);
	com_man().critical_wound(anim);	
}


