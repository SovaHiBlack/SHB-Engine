#include "stdafx.h"

//#include "Actor_flags.h"
#include "hudmanager.h"

#ifdef DEBUG
#	include "ode_include.h"
#	include "..\XR_3DA\StatGraph.h"
#	include "PHDebug.h"
#endif // DEBUG

#include "alife_space.h"
#include "Hit.h"
#include "PHDestroyable.h"
#include "Car.h"
#include "xrserver_objects_alife_monsters.h"
#include "CameraLook.h"
#include "CameraFirstEye.h"
#include "effectorfall.h"
#include "EffectorBobbing.h"
#include "clsid_game.h"
#include "ActorEffector.h"
#include "EffectorZoomInertion.h"
#include "SleepEffector.h"
#include "CharacterInfo.h"
#include "CustomOutfit.h"
#include "actorcondition.h"
#include "ui/UIGameCustom.h"
#include "game_cl_base_weapon_usage_statistic.h"

// breakpoints
#include "..\XR_3DA\Input.h"

//
#include "Actor.h"
#include "ActorMemory.h"
#include "actor_anim_defs.h"
#include "HudItem.h"
#include "ai_sounds.h"
#include "AISpace.h"
#include "Trade.h"
#include "Inventory.h"
#include "Physics.h"
#include "Level.h"
#include "GamePersistent.h"
#include "game_cl_base.h"
#include "game_cl_single.h"
#include "xrmessages.h"
#include "StringTable.h"
#include "UsableScriptObject.h"
#include "..\XR_3DA\cl_intersect.h"
#include "ExtendedGeom.h"
#include "alife_registry_wrappers.h"
#include "..\XR_3DA\skeletonanimated.h"
#include "Artefact.h"
#include "CharacterPhysicsSupport.h"
#include "material_manager.h"
#include "IColisiondamageInfo.h"
#include "ui/UIMainIngameWnd.h"//
#include "ui/UIArtefactPanel.h"//
#include "MapManager.h"
#include "GameTaskManager.h"
#include "Script_Game_Object.h"
#include "GameObject_space.h"
#include "script_callback_ex.h"
#include "InventoryBox.h"
#include "location_manager.h"

const u32		patch_frames = 50;
const f32		respawn_delay = 1.0f;
const f32		respawn_auto = 7.0f;

static f32 IReceived = 0.0f;
static f32 ICoincidenced = 0.0f;

static fBox3		bbStandBox;
static fBox3		bbCrouchBox;

flags32			psActorFlags = {0};

CActor::CActor( ) : CEntityAlive( )
{
	encyclopedia_registry = xr_new<CEncyclopediaRegistryWrapper>( );
	game_news_registry = xr_new<CGameNewsRegistryWrapper>( );
	// Cameras
	cameras[eacFirstEye] = xr_new<CCameraFirstEye>(this);
	cameras[eacFirstEye]->Load("actor_firsteye_cam");

	if (strstr(Core.Params, "-psp"))
	{
		psActorFlags.set(AF_PSP, TRUE);
	}
	else
	{
		psActorFlags.set(AF_PSP, FALSE);
	}

	if (psActorFlags.test(AF_PSP))
	{
		cameras[eacLookAt] = xr_new<CCameraLook2>(this);
		cameras[eacLookAt]->Load("actor_look_cam_psp");
	}
	else
	{
		cameras[eacLookAt] = xr_new<CCameraLook>(this);
		cameras[eacLookAt]->Load("actor_look_cam");
	}

	cameras[eacFreeLook] = xr_new<CCameraLook>(this);
	cameras[eacFreeLook]->Load("actor_free_cam");

	cam_active = eacFirstEye;
	fPrevCamPos = 0.0f;
	vPrevCamDir.set(0.0f, 0.0f, 1.0f);
	fCurAVelocity = 0.0f;
	// ���������
	pCamBobbing = 0;
	m_pSleepEffector = NULL;
	m_pSleepEffectorPP = NULL;

	r_torso.yaw = 0.0f;
	r_torso.pitch = 0.0f;
	r_torso.roll = 0.0f;
	r_torso_tgt_roll = 0.0f;
	r_model_yaw = 0.0f;
	r_model_yaw_delta = 0.0f;
	r_model_yaw_dest = 0.0f;

	b_DropActivated = 0;
	f_DropPower = 0.0f;

	m_fRunFactor = 2.0f;
	m_fCrouchFactor = 0.2f;
	m_fClimbFactor = 1.0f;
	m_fCamHeightFactor = 0.87f;

	m_fFallTime = s_fFallTime;
	m_bAnimTorsoPlayed = false;

	m_pPhysicsShell = NULL;

	m_holder = NULL;
	m_holderID = u16(-1);

#ifdef DEBUG
	Device.seqRender.Add(this, REG_PRIORITY_LOW);
#endif // def DEBUG

	//��������� ������������� ����� � inventory
	inventory( ).SetBeltUseful(true);

	m_pPersonWeLookingAt = NULL;
	m_pVehicleWeLookingAt = NULL;
	m_pObjectWeLookingAt = NULL;
	m_bPickupMode = false;

	pStatGraph = NULL;

	m_pActorEffector = NULL;

	m_bZoomAimingMode = false;

	m_sDefaultObjAction = NULL;

	m_fSprintFactor = 4.0f;

	hFriendlyIndicator.create(FVF::F_LIT, RCache.Vertex.Buffer( ), RCache.QuadIB);

	m_pUsableObject = NULL;

	m_anims = xr_new<SActorMotions>( );
	m_vehicle_anims = xr_new<SActorVehicleAnims>( );
	m_entity_condition = NULL;
	m_iLastHitterID = u16(-1);
	m_iLastHittingWeaponID = u16(-1);
	m_game_task_manager = NULL;
	m_statistic_manager = NULL;
	//-----------------------------------------------------------------------------------
	m_memory = xr_new<CActorMemory>(this);
	m_bOutBorder = false;
	hit_probability = 1.0f;
	m_feel_touch_characters = 0;
	//-----------------------------------------------------------------------------------
	m_dwILastUpdateTime = 0;

	m_location_manager = xr_new<CLocationManager>(this);
}

CActor::~CActor( )
{
	xr_delete(m_location_manager);
	xr_delete(m_memory);
	xr_delete(encyclopedia_registry);
	xr_delete(game_news_registry);

#ifdef DEBUG
	Device.seqRender.Remove(this);
#endif // def DEBUG

	//xr_delete(Weapons);
	for (s32 i = 0; i < eacMaxCam; ++i)
	{
		xr_delete(cameras[i]);
	}

	m_HeavyBreathSnd.destroy( );
	m_BloodSnd.destroy( );

	xr_delete(m_pActorEffector);
	xr_delete(m_pSleepEffector);

	hFriendlyIndicator.destroy( );

	xr_delete(m_pPhysics_support);
	xr_delete(m_anims);
	xr_delete(m_vehicle_anims);
}

void CActor::reinit( )
{
	character_physics_support( )->movement( )->CreateCharacter( );
	character_physics_support( )->movement( )->SetPhysicsRefObject(this);
	CEntityAlive::reinit( );
	CInventoryOwner::reinit( );

	character_physics_support( )->in_Init( );
	material( ).reinit( );

	m_pUsableObject = NULL;

	memory( ).reinit( );

	set_input_external_handler(0);
	m_time_lock_accel = 0;
}

void CActor::reload(pcstr section)
{
	CEntityAlive::reload(section);
	CInventoryOwner::reload(section);
	material( ).reload(section);
	CStepManager::reload(section);
	memory( ).reload(section);
	m_location_manager->reload(section);
}

void CActor::Load(pcstr section)
{
	// Msg						("Loading actor: %s",section);
	inherited::Load(section);
	material( ).Load(section);
	CInventoryOwner::Load(section);
	m_location_manager->Load(section);

	OnDifficultyChanged( );
	//////////////////////////////////////////////////////////////////////////
	ISpatial* self = smart_cast<ISpatial*> (this);
	if (self)
	{
		self->spatial.type |= STYPE_VISIBLEFORAI;
		self->spatial.type &= ~STYPE_REACTTOSOUND;
	}
	//////////////////////////////////////////////////////////////////////////

	fBox3	bb;
	fVector3	vBOX_center;
	fVector3	vBOX_size;

	// m_PhysicMovementControl: BOX
	vBOX_center = pSettings->r_fvector3(section, "ph_box2_center");
	vBOX_size = pSettings->r_fvector3(section, "ph_box2_size");
	bb.set(vBOX_center, vBOX_center); bb.grow(vBOX_size);
	character_physics_support( )->movement( )->SetBox(2, bb);

	// m_PhysicMovementControl: BOX
	vBOX_center = pSettings->r_fvector3(section, "ph_box1_center");
	vBOX_size = pSettings->r_fvector3(section, "ph_box1_size");
	bb.set(vBOX_center, vBOX_center); bb.grow(vBOX_size);
	character_physics_support( )->movement( )->SetBox(1, bb);

	// m_PhysicMovementControl: BOX
	vBOX_center = pSettings->r_fvector3(section, "ph_box0_center");
	vBOX_size = pSettings->r_fvector3(section, "ph_box0_size");
	bb.set(vBOX_center, vBOX_center); bb.grow(vBOX_size);
	character_physics_support( )->movement( )->SetBox(0, bb);

	// m_PhysicMovementControl: Crash speed and mass
	f32	cs_min = pSettings->r_float(section, "ph_crash_speed_min");
	f32	cs_max = pSettings->r_float(section, "ph_crash_speed_max");
	f32	mass = pSettings->r_float(section, "ph_mass");
	character_physics_support( )->movement( )->SetCrashSpeeds(cs_min, cs_max);
	character_physics_support( )->movement( )->SetMass(mass);
	if (pSettings->line_exist(section, "stalker_restrictor_radius"))
	{
		character_physics_support( )->movement( )->SetActorRestrictorRadius(CPHCharacter::rtStalker, pSettings->r_float(section, "stalker_restrictor_radius"));
	}

	if (pSettings->line_exist(section, "stalker_small_restrictor_radius"))
	{
		character_physics_support( )->movement( )->SetActorRestrictorRadius(CPHCharacter::rtStalkerSmall, pSettings->r_float(section, "stalker_small_restrictor_radius"));
	}

	if (pSettings->line_exist(section, "medium_monster_restrictor_radius"))
	{
		character_physics_support( )->movement( )->SetActorRestrictorRadius(CPHCharacter::rtMonsterMedium, pSettings->r_float(section, "medium_monster_restrictor_radius"));
	}

	character_physics_support( )->movement( )->Load(section);

	m_fWalkAccel = pSettings->r_float(section, "walk_accel");
	m_fJumpSpeed = pSettings->r_float(section, "jump_speed");
	m_fRunFactor = pSettings->r_float(section, "run_coef");
	m_fRunBackFactor = pSettings->r_float(section, "run_back_coef");
	m_fWalkBackFactor = pSettings->r_float(section, "walk_back_coef");
	m_fCrouchFactor = pSettings->r_float(section, "crouch_coef");
	m_fClimbFactor = pSettings->r_float(section, "climb_coef");
	m_fSprintFactor = pSettings->r_float(section, "sprint_koef");

	m_fWalk_StrafeFactor = READ_IF_EXISTS(pSettings, r_float, section, "walk_strafe_coef", 1.0f);
	m_fRun_StrafeFactor = READ_IF_EXISTS(pSettings, r_float, section, "run_strafe_coef", 1.0f);

	m_fCamHeightFactor = pSettings->r_float(section, "camera_height_factor");
	character_physics_support( )->movement( )->SetJumpUpVelocity(m_fJumpSpeed);
	f32 AirControlParam = pSettings->r_float(section, "air_control_param");
	character_physics_support( )->movement( )->SetAirControlParam(AirControlParam);

	m_fPickupInfoRadius = pSettings->r_float(section, "pickup_info_radius");

	character_physics_support( )->in_Load(section);

	//��������� ��������� ���������
	LoadSleepEffector("sleep_effector");

	//��������� ��������� �������� firepoint
	m_vMissileOffset = pSettings->r_fvector3(section, "missile_throw_offset");

	pcstr hit_snd_sect = pSettings->r_string(section, "hit_sounds");
	for (s32 hit_type = 0; hit_type < (s32)ALife::eHitTypeMax; ++hit_type)
	{
		pcstr hit_name = ALife::g_cafHitType2String((ALife::EHitType)hit_type);
		pcstr hit_snds = READ_IF_EXISTS(pSettings, r_string, hit_snd_sect, hit_name, "");
		s32 cnt = _GetItemCount(hit_snds);
		string128 tmp;
		VERIFY(cnt != 0);
		for (s32 i = 0; i < cnt; ++i)
		{
			sndHit[hit_type].push_back(ref_sound( ));
			sndHit[hit_type].back( ).create(_GetItem(hit_snds, i, tmp), st_Effect, sg_SourceType);
		}

		string256 buf;
		::Sound->create(sndDie[0], strconcat(sizeof(buf), buf, *cName( ), "\\die0"), st_Effect, eST_MONSTER_DYING);
		::Sound->create(sndDie[1], strconcat(sizeof(buf), buf, *cName( ), "\\die1"), st_Effect, eST_MONSTER_DYING);
		::Sound->create(sndDie[2], strconcat(sizeof(buf), buf, *cName( ), "\\die2"), st_Effect, eST_MONSTER_DYING);
		::Sound->create(sndDie[3], strconcat(sizeof(buf), buf, *cName( ), "\\die3"), st_Effect, eST_MONSTER_DYING);

		m_HeavyBreathSnd.create(pSettings->r_string(section, "heavy_breath_snd"), st_Effect, eST_MONSTER_INJURING);
		m_BloodSnd.create(pSettings->r_string(section, "heavy_blood_snd"), st_Effect, eST_MONSTER_INJURING);
	}

	if (psActorFlags.test(AF_PSP))
	{
		cam_Set(eacLookAt);
	}
	else
	{
		cam_Set(eacFirstEye);
	}

	// sheduler
	shedule.t_min = shedule.t_max = 1;

	// ��������� ��������� ��������
	m_fDispBase = pSettings->r_float(section, "disp_base");
	m_fDispBase = deg2rad(m_fDispBase);

	m_fDispAim = pSettings->r_float(section, "disp_aim");
	m_fDispAim = deg2rad(m_fDispAim);

	m_fDispVelFactor = pSettings->r_float(section, "disp_vel_factor");
	m_fDispAccelFactor = pSettings->r_float(section, "disp_accel_factor");
	m_fDispCrouchFactor = pSettings->r_float(section, "disp_crouch_factor");
	m_fDispCrouchNoAccelFactor = pSettings->r_float(section, "disp_crouch_no_acc_factor");

	pcstr default_outfit = READ_IF_EXISTS(pSettings, r_string, section, "default_outfit", 0);
	SetDefaultVisualOutfit(default_outfit);

	invincibility_fire_shield_1st = READ_IF_EXISTS(pSettings, r_string, section, "Invincibility_Shield_1st", 0);
	invincibility_fire_shield_3rd = READ_IF_EXISTS(pSettings, r_string, section, "Invincibility_Shield_3rd", 0);
	//-----------------------------------------
	m_AutoPickUp_AABB = READ_IF_EXISTS(pSettings, r_fvector3, section, "AutoPickUp_AABB", fVector3( ).set(0.02f, 0.02f, 0.02f));
	m_AutoPickUp_AABB_Offset = READ_IF_EXISTS(pSettings, r_fvector3, section, "AutoPickUp_AABB_offs", fVector3( ).set(0.0f, 0.0f, 0.0f));

	CStringTable string_table;
	m_sCharacterUseAction = "character_use";
	m_sDeadCharacterUseAction = "dead_character_use";
	m_sDeadCharacterUseOrDragAction = "dead_character_use_or_drag";
	m_sCarCharacterUseAction = "car_character_use";
	m_sInventoryItemUseAction = "inventory_item_use";
	m_sInventoryBoxUseAction = "inventory_box_use";
	//---------------------------------------------------------------------
	m_sHeadShotParticle = READ_IF_EXISTS(pSettings, r_string, section, "HeadShotParticle", 0);
}

void CActor::PHHit(f32 P, fVector3& dir, CObject* who, s16 element, fVector3 p_in_object_space, f32 impulse, ALife::EHitType hit_type)
{
	m_pPhysics_support->in_Hit(P, dir, who, element, p_in_object_space, impulse, hit_type, !g_Alive( ));
}

struct playing_pred
{
	IC bool	operator()			(ref_sound& s)
	{
		return (NULL != s._feedback( ));
	}
};

void	CActor::Hit(SHit* pHDS)
{
	pHDS->aim_bullet = false;

	SHit HDS = *pHDS;
	if (HDS.hit_type < ALife::eHitTypeBurn || HDS.hit_type >= ALife::eHitTypeMax)
	{
		string256 err;
		sprintf_s(err, "Unknown/unregistered hit type [%d]", HDS.hit_type);
		R_ASSERT2(0, err);
	}

#ifdef DEBUG
	if (ph_dbg_draw_mask.test(phDbgCharacterControl))
	{
		DBG_OpenCashedDraw( );
		fVector3 to;
		to.add(Position( ), fVector3( ).mul(HDS.dir, HDS.phys_impulse( )));
		DBG_DrawLine(Position( ), to, D3DCOLOR_XRGB(124, 124, 0));
		DBG_ClosedCashedDraw(500);
	}
#endif // DEBUG

	bool bPlaySound = true;
	if (!g_Alive( ))
	{
		bPlaySound = false;
	}

	if (!sndHit[HDS.hit_type].empty( ) && (ALife::eHitTypeTelepatic != HDS.hit_type))
	{
		ref_sound& S = sndHit[HDS.hit_type][Random.randI(sndHit[HDS.hit_type].size( ))];
		bool b_snd_hit_playing = sndHit[HDS.hit_type].end( ) != std::find_if(sndHit[HDS.hit_type].begin( ), sndHit[HDS.hit_type].end( ), playing_pred( ));

		if (ALife::eHitTypeExplosion == HDS.hit_type)
		{
			if (this == Level( ).CurrentControlEntity( ))
			{
				S.set_volume(10.0f);
				if (!m_sndShockEffector)
				{
					m_sndShockEffector = xr_new<CSndShockEffector>( );
					m_sndShockEffector->Start(this, f32(S._handle( )->length_ms( )), HDS.damage( ));
				}
			}
			else
			{
				bPlaySound = false;
			}
		}

		if (bPlaySound && !b_snd_hit_playing)
		{
			fVector3 point = Position( );
			point.y += CameraHeight( );
			S.play_at_pos(this, point);
		}
	}

	//slow actor, only when he gets hit
	if (HDS.hit_type == ALife::eHitTypeWound || HDS.hit_type == ALife::eHitTypeStrike)
	{
		hit_slowmo = HDS.damage( );
		clamp(hit_slowmo, 0.0f, 1.0f);
	}
	else
	{
		hit_slowmo = 0.0f;
	}

	if (Level( ).CurrentViewEntity( ) == this && HDS.hit_type == ALife::eHitTypeFireWound)
	{
		CObject* pLastHitter = Level( ).Objects.net_Find(m_iLastHitterID);
		CObject* pLastHittingWeapon = Level( ).Objects.net_Find(m_iLastHittingWeaponID);
		HitSector(pLastHitter, pLastHittingWeapon);
	}

	if ((mstate_real & mcSprint) && Level( ).CurrentControlEntity( ) == this && HDS.hit_type != ALife::eHitTypeTelepatic && HDS.hit_type != ALife::eHitTypeRadiation)
	{
		mstate_wishful &= ~mcSprint;
	}

	HitMark(HDS.damage( ), HDS.dir, HDS.who, HDS.bone( ), HDS.p_in_bone_space, HDS.impulse, HDS.hit_type);

	f32 hit_power = HitArtefactsOnBelt(HDS.damage( ), HDS.hit_type);

	if (GodMode( ))
	{
		HDS.power = 0.0f;
		inherited::Hit(&HDS);
		return;
	}
	else
	{
		HDS.power = hit_power;
		inherited::Hit(&HDS);
	}
}

void CActor::HitMark(f32 P, fVector3 dir, CObject* who, s16 element, fVector3 position_in_bone_space, f32 impulse, ALife::EHitType hit_type)
{
	// hit marker
	if ((hit_type == ALife::eHitTypeFireWound || hit_type == ALife::eHitTypeWound_2) && g_Alive( ) && Local( ) && (Level( ).CurrentEntity( ) == this))
	{
		HUD( ).Hit(0, P, dir);

		CEffectorCam* ce = Cameras( ).GetCamEffector((ECamEffectorType)effFireHit);
		if (!ce)
		{
			s32 id = -1;
			fVector3 cam_pos;
			fVector3 cam_dir;
			fVector3 cam_norm;
			cam_Active( )->Get(cam_pos, cam_dir, cam_norm);
			cam_dir.normalize_safe( );
			dir.normalize_safe( );

			f32 ang_diff = angle_difference(cam_dir.getH( ), dir.getH( ));
			fVector3 cp;
			cp.crossproduct(cam_dir, dir);
			bool bUp = (cp.y > 0.0f);

			fVector3 cross;
			cross.crossproduct(cam_dir, dir);
			VERIFY(ang_diff >= 0.0f && ang_diff <= PI);

			f32 _s1 = PI_DIV_8;
			f32 _s2 = _s1 + PI_DIV_4;
			f32 _s3 = _s2 + PI_DIV_4;
			f32 _s4 = _s3 + PI_DIV_4;

			if (ang_diff <= _s1)
			{
				id = 2;
			}
			else if (ang_diff > _s1 && ang_diff <= _s2)
			{
				id = (bUp) ? 5 : 7;
			}
			else if (ang_diff > _s2 && ang_diff <= _s3)
			{
				id = (bUp) ? 3 : 1;
			}
			else if (ang_diff > _s3 && ang_diff <= _s4)
			{
				id = (bUp) ? 4 : 6;
			}
			else if (ang_diff > _s4)
			{
				id = 0;
			}
			else
			{
				VERIFY(0);
			}

			string64 sect_name;
			sprintf_s(sect_name, "effector_fire_hit_%d", id);
			AddEffector(this, effFireHit, sect_name, P / 100.0f);
		}
	}
}

void CActor::HitSignal(f32 perc, fVector3& vLocalDir, CObject* who, s16 element)
{
	if (g_Alive( ))
	{
		// stop-motion
		if (character_physics_support( )->movement( )->Environment( ) == CPHMovementControl::peOnGround || character_physics_support( )->movement( )->Environment( ) == CPHMovementControl::peAtWall)
		{
			fVector3 zeroV;
			zeroV.set(0.0f, 0.0f, 0.0f);
			character_physics_support( )->movement( )->SetVelocity(zeroV);
		}

		// check damage bone
		fVector3 D;
		XFORM( ).transform_dir(D, vLocalDir);

		f32 yaw;
		f32 pitch;
		D.getHP(yaw, pitch);
		CKinematicsAnimated* tpKinematics = smart_cast<CKinematicsAnimated*>(Visual( ));
		VERIFY(tpKinematics);
		MotionID motion_ID = m_anims->m_normal.m_damage[iFloor(tpKinematics->LL_GetBoneInstance(element).get_param(1) + (angle_difference(r_model_yaw + r_model_yaw_delta, yaw) <= PI_DIV_2 ? 0 : 1))];
		f32 power_factor = perc / 100.0f;
		clamp(power_factor, 0.0f, 1.0f);
		VERIFY(motion_ID.valid( ));
		tpKinematics->PlayFX(motion_ID, power_factor);
	}
}

void start_tutorial(pcstr name);
void CActor::Die(CObject* who)
{
	inherited::Die(who);

	if (OnServer( ))
	{
		xr_vector<CInventorySlot>::iterator I = inventory( ).m_slots.begin( );
		xr_vector<CInventorySlot>::iterator E = inventory( ).m_slots.end( );
		for (u32 slot_idx = 0; I != E; ++I, ++slot_idx)
		{
			if (slot_idx == inventory( ).GetActiveSlot( ))
			{
				if ((*I).m_pIItem)
				{
					(*I).m_pIItem->SetDropManual(TRUE);
				}

				continue;
			}
			else
			{
				CCustomOutfit* pOutfit = smart_cast<CCustomOutfit*> ((*I).m_pIItem);
				if (pOutfit)
				{
					continue;
				}
			}

			if ((*I).m_pIItem)
			{
				inventory( ).Ruck((*I).m_pIItem);
			}
		}

		///!!! ������ �����
		TIItemContainer& l_blist = inventory( ).m_belt;
		while (!l_blist.empty( ))
		{
			inventory( ).Ruck(l_blist.front( ));
		}
	}

	cam_Set(eacFreeLook);
	mstate_wishful &= ~mcAnyMove;
	mstate_real &= ~mcAnyMove;

	::Sound->play_at_pos(sndDie[Random.randI(SND_DIE_COUNT)], this, Position( ));

	m_HeavyBreathSnd.stop( );
	m_BloodSnd.stop( );

	start_tutorial("game_over");

	xr_delete(m_sndShockEffector);
}

void CActor::SwitchOutBorder(bool new_border_state)
{
	if (new_border_state)
	{
		callback(GameObject::eExitLevelBorder)(lua_game_object( ));
	}
	else
	{
		//.		Msg("enter level border");
		callback(GameObject::eEnterLevelBorder)(lua_game_object( ));
	}

	m_bOutBorder = new_border_state;
}

void CActor::g_Physics(fVector3& _accel, f32 jump, f32 dt)
{
	// Correct accel
	fVector3 accel;
	accel.set(_accel);
	hit_slowmo -= dt;
	if (hit_slowmo < 0)
	{
		hit_slowmo = 0.0f;
	}

	accel.mul(1.0f - hit_slowmo);

	if (g_Alive( ))
	{
		if (mstate_real & mcClimb && !cameras[eacFirstEye]->bClampYaw)
		{
			accel.set(0.0f, 0.0f, 0.0f);
		}

		character_physics_support( )->movement( )->Calculate(accel, cameras[cam_active]->vDirection, 0, jump, dt, false);
		bool new_border_state = character_physics_support( )->movement( )->isOutBorder( );
		if (m_bOutBorder != new_border_state && Level( ).CurrentControlEntity( ) == this)
		{
			SwitchOutBorder(new_border_state);
		}

		character_physics_support( )->movement( )->GetPosition(Position( ));
		character_physics_support( )->movement( )->bSleep = false;
	}

	if (Local( ) && g_Alive( ))
	{
		if (character_physics_support( )->movement( )->gcontact_Was)
		{
			Cameras( ).AddCamEffector(xr_new<CEffectorFall>(character_physics_support( )->movement( )->gcontact_Power));
		}

		if (!fis_zero(character_physics_support( )->movement( )->gcontact_HealthLost))
		{
			const ICollisionDamageInfo* di = character_physics_support( )->movement( )->CollisionDamageInfo( );
			fVector3 hdir;
			di->HitDir(hdir);
			SetHitInfo(this, NULL, 0, fVector3( ).set(0.0f, 0.0f, 0.0f), hdir);
			if (Level( ).CurrentControlEntity( ) == this)
			{
				SHit HDS = SHit(character_physics_support( )->movement( )->gcontact_HealthLost, hdir, di->DamageInitiator( ), character_physics_support( )->movement( )->ContactBone( ), di->HitPos( ), 0.0f, di->HitType( ));

				CNetPacket	l_P;
				HDS.GenHeader(GE_HIT, ID( ));
				HDS.whoID = di->DamageInitiator( )->ID( );
				HDS.weaponID = di->DamageInitiator( )->ID( );
				HDS.Write_Packet(l_P);

				u_EventSend(l_P);
			}
		}
	}
}

f32 g_fov = 67.5f;

f32 CActor::currentFOV( )
{
	CWeapon* pWeapon = smart_cast<CWeapon*>(inventory( ).ActiveItem( ));

	if (eacFirstEye == cam_active && pWeapon && pWeapon->IsZoomed( ) && (!pWeapon->ZoomTexture( ) || (!pWeapon->IsRotatingToZoom( ) && pWeapon->ZoomTexture( ))))
	{
		return pWeapon->GetZoomFactor( ) * (0.75f);
	}
	else
	{
		return g_fov;
	}
}

void CActor::UpdateCL( )
{
	if (m_feel_touch_characters > 0)
	{
		for (xr_vector<CObject*>::iterator it = feel_touch.begin( ); it != feel_touch.end( ); it++)
		{
			CPhysicsShellHolder* sh = smart_cast<CPhysicsShellHolder*>(*it);
			if (sh && sh->character_physics_support( ))
			{
				sh->character_physics_support( )->movement( )->UpdateObjectBox(character_physics_support( )->movement( )->PHCharacter( ));
			}
		}
	}

	if (m_holder)
	{
		m_holder->UpdateEx(currentFOV( ));
	}

	m_snd_noise -= 0.3f * Device.fTimeDelta;

	VERIFY2(_valid(renderable.xform), *cName( ));
	inherited::UpdateCL( );
	VERIFY2(_valid(renderable.xform), *cName( ));
	m_pPhysics_support->in_UpdateCL( );
	VERIFY2(_valid(renderable.xform), *cName( ));

	if (g_Alive( ))
	{
		PickupModeUpdate( );
	}

	PickupModeUpdate_COD( );

	m_bZoomAimingMode = false;
	CWeapon* pWeapon = smart_cast<CWeapon*>(inventory( ).ActiveItem( ));

	Device.Statistic->TEST1.Begin( );
	cam_Update(f32(Device.dwTimeDelta) / 1000.0f, currentFOV( ));
	Device.Statistic->TEST1.End( );

	if (Level( ).CurrentEntity( ) && this->ID( ) == Level( ).CurrentEntity( )->ID( ))
	{
		psHUD_Flags.set(HUD_CROSSHAIR_RT2, true);
		psHUD_Flags.set(HUD_DRAW_RT, true);
	}

	if (pWeapon)
	{
		if (pWeapon->IsZoomed( ))
		{
			f32 full_fire_disp = pWeapon->GetFireDispersion(true);

			CEffectorZoomInertion* S = smart_cast<CEffectorZoomInertion*>	(Cameras( ).GetCamEffector(eCEZoom));
			if (S)
			{
				S->SetParams(full_fire_disp);
			}

			m_bZoomAimingMode = true;
		}

		if (Level( ).CurrentEntity( ) && this->ID( ) == Level( ).CurrentEntity( )->ID( ))
		{
			f32 fire_disp_full = pWeapon->GetFireDispersion(true);

			HUD( ).SetCrosshairDisp(fire_disp_full, 0.02f);
			HUD( ).ShowCrosshair(pWeapon->use_crosshair( ));

			psHUD_Flags.set(HUD_CROSSHAIR_RT2, pWeapon->show_crosshair( ));
			psHUD_Flags.set(HUD_DRAW_RT, pWeapon->show_indicators( ));
		}
	}
	else
	{
		if (Level( ).CurrentEntity( ) && this->ID( ) == Level( ).CurrentEntity( )->ID( ))
		{
			HUD( ).SetCrosshairDisp(0.f);
			HUD( ).ShowCrosshair(false);
		}
	}

	UpdateDefferedMessages( );

	if (g_Alive( ))
	{
		CStepManager::update( );
	}

	spatial.type |= STYPE_REACTTOSOUND;

	if (m_sndShockEffector)
	{
		if (this == Level( ).CurrentViewEntity( ))
		{
			m_sndShockEffector->Update( );

			if (!m_sndShockEffector->InWork( ) || !g_Alive( ))
			{
				xr_delete(m_sndShockEffector);
			}
		}
		else
		{
			xr_delete(m_sndShockEffector);
		}
	}
}

f32 NET_Jump = 0.0f;
void CActor::shedule_Update(u32 DT)
{
	setSVU(OnServer( ));

	//���������� ����� ������ HUD ��� �������� ��������� �����
	CHudItem* pHudItem = smart_cast<CHudItem*>(inventory( ).ActiveItem( ));
	if (pHudItem)
	{
		pHudItem->SetHUDmode(HUDview( ));
	}

	//���������� ���������
	UpdateInventoryOwner(DT);
	GameTaskManager( ).UpdateTasks( );

	if (m_holder || !getEnabled( ) || !Ready( ))
	{
		m_sDefaultObjAction = NULL;
		inherited::shedule_Update(DT);

		return;
	}

	// 
	clamp(DT, 0u, 100u);
	f32 dt = f32(DT) / 1000.0f;

	// Check controls, create accel, prelimitary setup "mstate_real"

	//----------- for E3 -----------------------------
	if (Level( ).CurrentControlEntity( ) == this && (!Level( ).IsDemoPlay( ) || Level( ).IsServerDemo( )))
	{
		g_cl_CheckControls(mstate_wishful, NET_SavedAccel, NET_Jump, dt);

		g_cl_Orientate(mstate_real, dt);
		g_Orientate(mstate_real, dt);

		g_Physics(NET_SavedAccel, NET_Jump, dt);

		g_cl_ValidateMState(dt, mstate_wishful);
		g_SetAnimation(mstate_real);

		// Check for game-contacts
		fVector3 C;
		f32 R;
		Center(C);
		R = Radius( );
		feel_touch_update(C, R);

		// Dropping
		if (b_DropActivated)
		{
			f_DropPower += dt * 0.1f;
			clamp(f_DropPower, 0.0f, 1.0f);
		}
		else
		{
			f_DropPower = 0.0f;
		}

		if (!Level( ).IsDemoPlay( ))
		{
			mstate_wishful &= ~mcAccel;
			mstate_wishful &= ~mcLStrafe;
			mstate_wishful &= ~mcRStrafe;
			mstate_wishful &= ~mcLLookout;
			mstate_wishful &= ~mcRLookout;
			mstate_wishful &= ~mcFwd;
			mstate_wishful &= ~mcBack;
			extern bool g_bAutoClearCrouch;
			if (g_bAutoClearCrouch)
			{
				mstate_wishful &= ~mcCrouch;
			}
		}
	}
	else
	{
		make_Interpolation( );

		if (NET.size( ))
		{
			g_sv_Orientate(mstate_real, dt);
			g_Orientate(mstate_real, dt);
			g_Physics(NET_SavedAccel, NET_Jump, dt);
			if (!m_bInInterpolation)
			{
				g_cl_ValidateMState(dt, mstate_wishful);
			}

			g_SetAnimation(mstate_real);

			if (NET_Last.mstate & mcCrouch)
			{
				if (isActorAccelerated(mstate_real, IsZoomAimingMode( )))
				{
					character_physics_support( )->movement( )->ActivateBox(1, true);
				}
				else
				{
					character_physics_support( )->movement( )->ActivateBox(2, true);
				}
			}
			else
			{
				character_physics_support( )->movement( )->ActivateBox(0, true);
			}
		}

		mstate_old = mstate_real;
	}

	if (this == Level( ).CurrentViewEntity( ))
	{
		UpdateMotionIcon(mstate_real);
	}

	NET_Jump = 0.0f;

	inherited::shedule_Update(DT);

	//�������� ���������� ��� ������
	if (!pCamBobbing)
	{
		pCamBobbing = xr_new<CEffectorBobbing>( );
		Cameras( ).AddCamEffector(pCamBobbing);
	}

	pCamBobbing->SetState(mstate_real, conditions( ).IsLimping( ), IsZoomAimingMode( ));

	//���� �������� ������� ��� �������� � ��������
	if (this == Level( ).CurrentControlEntity( ))
	{
		if (conditions( ).IsLimping( ) && g_Alive( ))
		{
			if (!m_HeavyBreathSnd._feedback( ))
			{
				m_HeavyBreathSnd.play_at_pos(this, fVector3( ).set(0.0f, ACTOR_HEIGHT, 0.0f), sm_Looped | sm_2D);
			}
			else
			{
				m_HeavyBreathSnd.set_position(fVector3( ).set(0.0f, ACTOR_HEIGHT, 0.0f));
			}
		}
		else if (m_HeavyBreathSnd._feedback( ))
		{
			m_HeavyBreathSnd.stop( );
		}

		f32 bs = conditions( ).BleedingSpeed( );
		if (bs > 0.6f)
		{
			fVector3 snd_pos;
			snd_pos.set(0.0f, ACTOR_HEIGHT, 0.0f);
			if (!m_BloodSnd._feedback( ))
			{
				m_BloodSnd.play_at_pos(this, snd_pos, sm_Looped | sm_2D);
			}
			else
			{
				m_BloodSnd.set_position(snd_pos);
			}

			f32 v = bs + 0.25f;

			m_BloodSnd.set_volume(v);
		}
		else
		{
			if (m_BloodSnd._feedback( ))
			{
				m_BloodSnd.stop( );
			}
		}

		if (!g_Alive( ) && m_BloodSnd._feedback( ))
		{
			m_BloodSnd.stop( );
		}
	}

	//���� � ������ HUD, �� ���� ������ ������ �� ��������
	if (!character_physics_support( )->IsRemoved( ))
	{
		setVisible(!HUDview( ));
	}

	//��� ����� ����� ����� �����
	collide::rq_result& RQ = HUD( ).GetCurrentRayQuery( );

	if (!input_external_handler_installed( ) && RQ.O && RQ.range < inventory( ).GetTakeDist( ))
	{
		m_pObjectWeLookingAt = smart_cast<CGameObject*>(RQ.O);
		CGameObject* game_object = smart_cast<CGameObject*>(RQ.O);
		m_pUsableObject = smart_cast<CUsableScriptObject*>(game_object);
		m_pInvBoxWeLookingAt = smart_cast<CInventoryBox*>(game_object);
		inventory( ).m_pTarget = smart_cast<PIItem>(game_object);
		m_pPersonWeLookingAt = smart_cast<CInventoryOwner*>(game_object);
		m_pVehicleWeLookingAt = smart_cast<CHolderCustom*>(game_object);
		CEntityAlive* pEntityAlive = smart_cast<CEntityAlive*>(game_object);

		if (m_pUsableObject && m_pUsableObject->tip_text( ))
		{
			m_sDefaultObjAction = CStringTable( ).translate(m_pUsableObject->tip_text( ));
		}
		else
		{
			if (m_pPersonWeLookingAt && pEntityAlive->g_Alive( ))
			{
				m_sDefaultObjAction = m_sCharacterUseAction;
			}
			else if (pEntityAlive && !pEntityAlive->g_Alive( ))
			{
				bool b_allow_drag = !!pSettings->line_exist("ph_capture_visuals", pEntityAlive->cNameVisual( ));

				if (b_allow_drag)
				{
					m_sDefaultObjAction = m_sDeadCharacterUseOrDragAction;
				}
				else
				{
					m_sDefaultObjAction = m_sDeadCharacterUseAction;
				}
			}
			else if (m_pVehicleWeLookingAt)
			{
				m_sDefaultObjAction = m_sCarCharacterUseAction;
			}
			else if (inventory( ).m_pTarget && inventory( ).m_pTarget->CanTake( ))
			{
				m_sDefaultObjAction = m_sInventoryItemUseAction;
			}
			else
			{
				m_sDefaultObjAction = NULL;
			}
		}
	}
	else
	{
		inventory( ).m_pTarget = NULL;
		m_pPersonWeLookingAt = NULL;
		m_sDefaultObjAction = NULL;
		m_pUsableObject = NULL;
		m_pObjectWeLookingAt = NULL;
		m_pVehicleWeLookingAt = NULL;
		m_pInvBoxWeLookingAt = NULL;
	}

	//��� ������ ����������, ����������� �� �����
	UpdateArtefactsOnBelt( );
	m_pPhysics_support->in_shedule_Update(DT);
	Check_for_AutoPickUp( );
}

void CActor::renderable_Render( )
{
	inherited::renderable_Render( );
	if (!HUDview( ))
	{
		CInventoryOwner::renderable_Render( );
	}
}

BOOL CActor::renderable_ShadowGenerate( )
{
	if (m_holder)
	{
		return FALSE;
	}

	return inherited::renderable_ShadowGenerate( );
}

void CActor::g_PerformDrop( )
{
	b_DropActivated = FALSE;

	PIItem pItem = inventory( ).ActiveItem( );
	if (0 == pItem)
	{
		return;
	}

	u32 s = inventory( ).GetActiveSlot( );
	if (inventory( ).m_slots[s].m_bPersistent)
	{
		return;
	}

	pItem->SetDropManual(TRUE);
}

#ifdef DEBUG
extern BOOL	g_ShowAnimationInfo;
#endif // DEBUG

// HUD
void CActor::OnHUDDraw(CCustomHUD* /**hud/**/)
{
	CHudItem* pHudItem = smart_cast<CHudItem*>(inventory( ).ActiveItem( ));
	if (pHudItem && pHudItem->GetHUDmode( ))
	{
		inventory( ).ActiveItem( )->renderable_Render( );
	}

#if 0//ndef NDEBUG
	if (Level( ).CurrentControlEntity( ) == this && g_ShowAnimationInfo)
	{
		string128 buf;
		HUD( ).Font( ).pFontSmall->SetColor(0xffffffff);
		HUD( ).Font( ).pFontSmall->OutSet(170, 530);
		HUD( ).Font( ).pFontSmall->OutNext("Position:      [%3.2f, %3.2f, %3.2f]", VPUSH(Position( )));
		HUD( ).Font( ).pFontSmall->OutNext("Velocity:      [%3.2f, %3.2f, %3.2f]", VPUSH(m_PhysicMovementControl->GetVelocity( )));
		HUD( ).Font( ).pFontSmall->OutNext("Vel Magnitude: [%3.2f]", m_PhysicMovementControl->GetVelocityMagnitude( ));
		HUD( ).Font( ).pFontSmall->OutNext("Vel Actual:    [%3.2f]", m_PhysicMovementControl->GetVelocityActual( ));
		switch (m_PhysicMovementControl->Environment( ))
		{
			case CPHMovementControl::peOnGround:	strcpy(buf, "ground");			break;
			case CPHMovementControl::peInAir:		strcpy(buf, "air");				break;
			case CPHMovementControl::peAtWall:		strcpy(buf, "wall");			break;
		}
		HUD( ).Font( ).pFontSmall->OutNext(buf);

		if (IReceived != 0)
		{
			f32 Size = 0;
			Size = HUD( ).Font( ).pFontSmall->GetSize( );
			HUD( ).Font( ).pFontSmall->SetSize(Size * 2);
			HUD( ).Font( ).pFontSmall->SetColor(0xffff0000);
			HUD( ).Font( ).pFontSmall->OutNext("Input :		[%3.2f]", ICoincidenced / IReceived * 100.0f);
			HUD( ).Font( ).pFontSmall->SetSize(Size);
		}
	}
#endif

}

void CActor::RenderIndicator(fVector3 dpos, f32 r1, f32 r2, ref_shader IndShader)
{
	if (!g_Alive( ))
	{
		return;
	}

	u32 dwOffset = 0;
	u32 dwCount = 0;
	FVF::LIT* pv_start = (FVF::LIT*)RCache.Vertex.Lock(4, hFriendlyIndicator->vb_stride, dwOffset);
	FVF::LIT* pv = pv_start;
	// base rect

	CBoneInstance& BI = smart_cast<CKinematics*>(Visual( ))->LL_GetBoneInstance(u16(m_head));
	fMatrix4x4 M;
	smart_cast<CKinematics*>(Visual( ))->CalculateBones( );
	M.mul(XFORM( ), BI.mTransform);

	fVector3 pos = M.c; pos.add(dpos);
	const fVector3& T = Device.vCameraTop;
	const fVector3& R = Device.vCameraRight;
	fVector3 Vr;
	fVector3 Vt;
	Vr.x = R.x * r1;
	Vr.y = R.y * r1;
	Vr.z = R.z * r1;
	Vt.x = T.x * r2;
	Vt.y = T.y * r2;
	Vt.z = T.z * r2;

	fVector3 a;
	fVector3 b;
	fVector3 c;
	fVector3 d;
	a.sub(Vt, Vr);
	b.add(Vt, Vr);
	c.invert(a);
	d.invert(b);
	pv->set(d.x + pos.x, d.y + pos.y, d.z + pos.z, 0xffffffff, 0.0f, 1.0f);
	pv++;
	pv->set(a.x + pos.x, a.y + pos.y, a.z + pos.z, 0xffffffff, 0.0f, 0.0f);
	pv++;
	pv->set(c.x + pos.x, c.y + pos.y, c.z + pos.z, 0xffffffff, 1.0f, 1.0f);
	pv++;
	pv->set(b.x + pos.x, b.y + pos.y, b.z + pos.z, 0xffffffff, 1.0f, 0.0f);
	pv++;

	// render
	dwCount = u32(pv - pv_start);
	RCache.Vertex.Unlock(dwCount, hFriendlyIndicator->vb_stride);

	RCache.set_xform_world(Fidentity);
	RCache.set_Shader(IndShader);
	RCache.set_Geometry(hFriendlyIndicator);
	RCache.Render(D3DPT_TRIANGLESTRIP, dwOffset, 0, dwCount, 0, 2);
}

static f32 mid_size = 0.097f;
static f32 fontsize = 15.0f;
static f32 upsize = 0.33f;
void CActor::RenderText(pcstr Text, fVector3 dpos, f32* pdup, u32 color)
{
	if (!g_Alive( ))
	{
		return;
	}

	CBoneInstance& BI = smart_cast<CKinematics*>(Visual( ))->LL_GetBoneInstance(u16(m_head));
	fMatrix4x4 M;
	smart_cast<CKinematics*>(Visual( ))->CalculateBones( );
	M.mul(XFORM( ), BI.mTransform);
	//------------------------------------------------
	fVector3 v0;
	fVector3 v1;
	v0.set(M.c);
	v1.set(M.c);
	fVector3 T = Device.vCameraTop;
	v1.add(T);

	fVector3 v0r;
	fVector3 v1r;
	Device.mFullTransform.transform(v0r, v0);
	Device.mFullTransform.transform(v1r, v1);
	f32 size = v1r.distance_to(v0r);
	CGameFont* pFont = HUD( ).Font( ).pFontArial14;
	if (!pFont)
	{
		return;
	}

	f32 delta_up = 0.0f;
	if (size < mid_size)
	{
		delta_up = upsize;
	}
	else
	{
		delta_up = upsize * (mid_size / size);
	}

	dpos.y += delta_up;
	if (size > mid_size)
	{
		size = mid_size;
	}

	//------------------------------------------------
	M.c.y += dpos.y;

	fVector4 v_res;
	Device.mFullTransform.transform(v_res, M.c);

	if (v_res.z < 0 || v_res.w < 0)
	{
		return;
	}

	if (v_res.x < -1.0f || v_res.x > 1.0f || v_res.y < -1.0f || v_res.y > 1.0f)
	{
		return;
	}

	f32 x = (1.0f + v_res.x) / 2.0f * (Device.dwWidth);
	f32 y = (1.0f - v_res.y) / 2.0f * (Device.dwHeight);

	pFont->SetAligment(CGameFont::alCenter);
	pFont->SetColor(color);
	pFont->Out(x, y, Text);
	//-------------------------------------------------
	*pdup = delta_up;
}

void CActor::SetPhPosition(const fMatrix4x4& transform)
{
	if (!m_pPhysicsShell)
	{
		character_physics_support( )->movement( )->SetPosition(transform.c);
	}
}

void CActor::ForceTransform(const fMatrix4x4& m)
{
	if (!g_Alive( ))
	{
		return;
	}

	XFORM( ).set(m);
	if (character_physics_support( )->movement( )->CharacterExist( ))
	{
		character_physics_support( )->movement( )->EnableCharacter( );
	}

	character_physics_support( )->set_movement_position(m.c);
	character_physics_support( )->movement( )->SetVelocity(0.0f, 0.0f, 0.0f);
}

ENGINE_API extern f32		psHUD_FOV;
f32 CActor::Radius( )const
{
	f32 R = inherited::Radius( );
	CWeapon* W = smart_cast<CWeapon*>(inventory( ).ActiveItem( ));
	if (W)
	{
		R += W->Radius( );
	}

	return R;
}

bool CActor::use_bolts( ) const
{
	return CInventoryOwner::use_bolts( );
}

s32 g_iCorpseRemove = 1;

bool CActor::NeedToDestroyObject( ) const
{
	if (g_Alive( ))
	{
		return false;
	}

	if (g_iCorpseRemove == -1)
	{
		return false;
	}

	if (g_iCorpseRemove == 0 && m_bAllowDeathRemove)
	{
		return true;
	}

	return (TimePassedAfterDeath( ) > m_dwBodyRemoveTime && m_bAllowDeathRemove);
}

ALife::_TIME_ID CActor::TimePassedAfterDeath( ) const
{
	if (!g_Alive( ))
	{
		return Level( ).timeServer( ) - GetLevelDeathTime( );
	}
	else
	{
		return 0;
	}
}

void CActor::OnItemTake(CInventoryItem* inventory_item)
{
	CInventoryOwner::OnItemTake(inventory_item);
	if (OnClient( ))
	{
		return;
	}
}

void CActor::OnItemDrop(CInventoryItem* inventory_item)
{
	CInventoryOwner::OnItemDrop(inventory_item);

	CArtefact* artefact = smart_cast<CArtefact*>(inventory_item);
	if (artefact && artefact->m_eItemPlace == eItemPlaceBelt)
	{
		MoveArtefactBelt(artefact, false);
	}
}

void CActor::OnItemDropUpdate( )
{
	CInventoryOwner::OnItemDropUpdate( );

	TIItemContainer::iterator I = inventory( ).m_all.begin( );
	TIItemContainer::iterator E = inventory( ).m_all.end( );

	for (; I != E; ++I)
	{
		if (!(*I)->IsInvalid( ) && !attached(*I))
		{
			attach(*I);
		}
	}
}

void CActor::OnItemRuck(CInventoryItem* inventory_item, EItemPlace previous_place)
{
	CInventoryOwner::OnItemRuck(inventory_item, previous_place);

	CArtefact* artefact = smart_cast<CArtefact*>(inventory_item);
	if (artefact && previous_place == eItemPlaceBelt)
	{
		MoveArtefactBelt(artefact, false);
	}
}

void CActor::OnItemBelt(CInventoryItem* inventory_item, EItemPlace previous_place)
{
	CInventoryOwner::OnItemBelt(inventory_item, previous_place);

	CArtefact* artefact = smart_cast<CArtefact*>(inventory_item);
	if (artefact)
	{
		MoveArtefactBelt(artefact, true);
	}
}

void CActor::MoveArtefactBelt(const CArtefact* artefact, bool on_belt)
{
	VERIFY(artefact);

	//�������� �������� �� ����
	if (on_belt)
	{
		VERIFY(m_ArtefactsOnBelt.end( ) == std::find(m_ArtefactsOnBelt.begin( ), m_ArtefactsOnBelt.end( ), artefact));
		m_ArtefactsOnBelt.push_back(artefact);
	}
	else
	{
		xr_vector<const CArtefact*>::iterator it = std::remove(m_ArtefactsOnBelt.begin( ), m_ArtefactsOnBelt.end( ), artefact);
		VERIFY(it != m_ArtefactsOnBelt.end( ));
		m_ArtefactsOnBelt.erase(it);
	}

	if (Level( ).CurrentViewEntity( ) && Level( ).CurrentViewEntity( ) == this)
	{
		HUD( ).GetUI( )->UIMainIngameWnd->m_artefactPanel->InitIcons(m_ArtefactsOnBelt);
	}
}

#define ARTEFACTS_UPDATE_TIME 0.1f

void CActor::UpdateArtefactsOnBelt( )
{
	static f32 update_time = 0;
	f32 f_update_time = 0;

	if (update_time < ARTEFACTS_UPDATE_TIME)
	{
		update_time += conditions( ).fdelta_time( );
		return;
	}
	else
	{
		f_update_time = update_time;
		update_time = 0.0f;
	}

	for (TIItemContainer::iterator it = inventory( ).m_belt.begin( ); inventory( ).m_belt.end( ) != it; ++it)
	{
		CArtefact* artefact = smart_cast<CArtefact*>(*it);
		if (artefact)
		{
			conditions( ).ChangeBleeding(artefact->m_fBleedingRestoreSpeed * f_update_time);
			conditions( ).ChangeHealth(artefact->m_fHealthRestoreSpeed * f_update_time);
			conditions( ).ChangePower(artefact->m_fPowerRestoreSpeed * f_update_time);
			conditions( ).ChangeRadiation(artefact->m_fRadiationRestoreSpeed * f_update_time);
		}
	}
}

f32	CActor::HitArtefactsOnBelt(f32 hit_power, ALife::EHitType hit_type)
{
	f32 res_hit_power_k = 1.0f;
	f32 _af_count = 0.0f;
	for (TIItemContainer::iterator it = inventory( ).m_belt.begin( ); inventory( ).m_belt.end( ) != it; ++it)
	{
		CArtefact* artefact = smart_cast<CArtefact*>(*it);
		if (artefact)
		{
			res_hit_power_k += artefact->m_ArtefactHitImmunities.AffectHit(1.0f, hit_type);
			_af_count += 1.0f;
		}
	}

	res_hit_power_k -= _af_count;

	return res_hit_power_k * hit_power;
}

void CActor::SetZoomRndSeed(s32 Seed)
{
	if (0 != Seed)
	{
		m_ZoomRndSeed = Seed;
	}
	else
	{
		m_ZoomRndSeed = s32(Level( ).timeServer_Async( ));
	}
}

void CActor::SetShotRndSeed(s32 Seed)
{
	if (0 != Seed)
	{
		m_ShotRndSeed = Seed;
	}
	else
	{
		m_ShotRndSeed = s32(Level( ).timeServer_Async( ));
	}
}

fVector3 CActor::GetMissileOffset( ) const
{
	return m_vMissileOffset;
}

void CActor::SetMissileOffset(const fVector3& vNewOffset)
{
	m_vMissileOffset.set(vNewOffset);
}

void CActor::spawn_supplies( )
{
	inherited::spawn_supplies( );
	CInventoryOwner::spawn_supplies( );
}

void CActor::AnimTorsoPlayCallBack(CBlend* B)
{
	CActor* actor = (CActor*)B->CallbackParam;
	actor->m_bAnimTorsoPlayed = FALSE;
}

void CActor::SetActorVisibility(u16 who, f32 value)
{
	CUIMotionIcon& motion_icon = HUD( ).GetUI( )->UIMainIngameWnd->MotionIcon( );
	motion_icon.SetActorVisibility(who, value);
}

void CActor::UpdateMotionIcon(u32 mstate_rl)
{
	CUIMotionIcon& motion_icon = HUD( ).GetUI( )->UIMainIngameWnd->MotionIcon( );
	if (mstate_rl & mcClimb)
	{
		motion_icon.ShowState(CUIMotionIcon::stClimb);
	}
	else
	{
		if (mstate_rl & mcCrouch)
		{
			if (!isActorAccelerated(mstate_rl, IsZoomAimingMode( )))
			{
				motion_icon.ShowState(CUIMotionIcon::stCreep);
			}
			else
			{
				motion_icon.ShowState(CUIMotionIcon::stCrouch);
			}
		}
		else if (mstate_rl & mcSprint)
		{
			motion_icon.ShowState(CUIMotionIcon::stSprint);
		}
		else if (mstate_rl & mcAnyMove && isActorAccelerated(mstate_rl, IsZoomAimingMode( )))
		{
			motion_icon.ShowState(CUIMotionIcon::stRun);
		}
		else
		{
			motion_icon.ShowState(CUIMotionIcon::stNormal);
		}
	}

	/*
							stNormal, --
							stCrouch, --
							stCreep,  --
							stClimb,  --
							stRun,    --
							stSprint, --
	*/
}


CPHDestroyable* CActor::ph_destroyable( )
{
	return smart_cast<CPHDestroyable*>(character_physics_support( ));
}

CEntityConditionSimple* CActor::create_entity_condition(CEntityConditionSimple* ec)
{
	if (!ec)
	{
		m_entity_condition = xr_new<CActorCondition>(this);
	}
	else
	{
		m_entity_condition = smart_cast<CActorCondition*>(ec);
	}

	return inherited::create_entity_condition(m_entity_condition);
}

DLL_Pure* CActor::_construct( )
{
	m_pPhysics_support = xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::etActor, this);
	CEntityAlive::_construct( );
	CInventoryOwner::_construct( );
	CStepManager::_construct( );

	return this;
}

bool CActor::use_center_to_aim( ) const
{
	return !(mstate_real & mcCrouch);
}

bool CActor::can_attach(const CInventoryItem* inventory_item) const
{
	const CAttachableItem* item = smart_cast<const CAttachableItem*>(inventory_item);
	if (!item || !item->can_be_attached( ))
	{
		return false;
	}

	//����� �� ������������ ������� ������ ����
	if (m_attach_item_sections.end( ) == std::find(m_attach_item_sections.begin( ), m_attach_item_sections.end( ), inventory_item->object( ).cNameSect( )))
	{
		return false;
	}

	//���� ��� ���� �������������� ����� ������ ����
	if (attached(inventory_item->object( ).cNameSect( )))
	{
		return false;
	}

	return true;
}

void CActor::OnDifficultyChanged( )
{
	// immunities
	VERIFY(g_SingleGameDifficulty >= egdNovice && g_SingleGameDifficulty <= egdMaster);
	pcstr diff_name = get_token_name(difficulty_type_token, g_SingleGameDifficulty);
	string128 tmp;
	strconcat(sizeof(tmp), tmp, "actor_immunities_", diff_name);
	conditions( ).LoadImmunities(tmp, pSettings);

	// hit probability
	strconcat(sizeof(tmp), tmp, "hit_probability_", diff_name);
	hit_probability = pSettings->r_float(*cNameSect( ), tmp);
}

CVisualMemoryManager* CActor::visual_memory( ) const
{
	return &memory( ).visual( );
}

f32 CActor::GetMass( )
{
	return (g_Alive( ) ? character_physics_support( )->movement( )->GetMass( ) : m_pPhysicsShell ? m_pPhysicsShell->getMass( ) : 0);
}

bool CActor::is_on_ground( )
{
	return (character_physics_support( )->movement( )->Environment( ) != CPHMovementControl::peInAir);
}

CCustomOutfit* CActor::GetOutfit( ) const
{
	PIItem _of = inventory( ).m_slots[OUTFIT_SLOT].m_pIItem;
	return (_of ? smart_cast<CCustomOutfit*>(_of) : NULL);
}
