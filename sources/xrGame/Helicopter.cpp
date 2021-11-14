#include "stdafx.h"

#include "Helicopter.h"////
#include "xrserver_objects_alife.h"
#include "PhysicsShell.h"////
#include "Level.h"////
#include "ai_sounds.h"
#include "clsid_game.h"////
#include "..\ENGINE\skeletonanimated.h"////
#include "script_callback_ex.h"////
#include "GameObject_space.h"////
#include "script_game_object.h"////
#include "..\ENGINE\LightAnimLibrary.h"////
#include "HUDManager.h"//
#include "PhysicsCommon.h"

#include "ai/stalker/Stalker.h"
#include "CustomZone.h"
#include "MathUtils.h"
#include "group_hierarchy_holder.h"
#include "seniority_hierarchy_holder.h"
#include "team_hierarchy_holder.h"
#include "squad_hierarchy_holder.h"

#include "ExtendedGeom.h"

//50fps fixed
float STEP=0.02f;

CHelicopter::CHelicopter()
{
	m_pParticle		= NULL;
	m_light_render	= NULL;
	m_lanim			= NULL;

	ISpatial*		self				=	smart_cast<ISpatial*> (this);
	if (self)		self->spatial.type  |=  STYPE_VISIBLEFORAI;

	m_movement.parent	= this;
	m_body.parent		= this;
}

CHelicopter::~CHelicopter()
{
	HUD_SOUND::DestroySound		(m_sndShot);
	HUD_SOUND::DestroySound		(m_sndShotRocket);
}

void CHelicopter::setState(CHelicopter::EHeliState s)
{
	m_curState = s;
}

void CHelicopter::init()
{
	m_cur_rot.set				(0.0f,0.0f);
	m_tgt_rot.set				(0.0f,0.0f);
	m_bind_rot.set				(0.0f,0.0f);

	m_allow_fire				= FALSE;
	m_use_rocket_on_attack		= TRUE;
	m_use_mgun_on_attack		= TRUE;	
	m_syncronize_rocket			= TRUE;
	m_min_rocket_dist			= 20.0f;
	m_max_rocket_dist			= 200.0f;
	m_time_between_rocket_attack = 0;
	m_last_rocket_attack		= Device.dwTimeGlobal;

	SetfHealth					(1.0f);
}

DLL_Pure* CHelicopter::_construct()
{
	CEntity::_construct	();
	init				();
	return				this;
}

void CHelicopter::reinit(){
	inherited::reinit	();
	m_movement.reinit	();
	m_body.reinit		(); 
	m_enemy.reinit		();
}

void CHelicopter::Load(const char* section)
{
	inherited::Load						(section);
	m_movement.Load						(section);
	m_body.Load							(section);
	m_enemy.Load						(section);

	m_death_ang_vel						= pSettings->r_fvector3(section,"death_angular_vel");
	m_death_lin_vel_k					= pSettings->r_float(section,"death_lin_vel_koeff");

	CHitImmunity::LoadImmunities		(pSettings->r_string(section,"immunities_sect"),pSettings);

	//weapons
	CShootingObject::Load				(section);
	HUD_SOUND::LoadSound				(section,"snd_shoot", m_sndShot, SOUND_TYPE_WEAPON_SHOOTING);
	HUD_SOUND::LoadSound				(section,"snd_shoot_rocket", m_sndShotRocket, SOUND_TYPE_WEAPON_SHOOTING);
	CRocketLauncher::Load				(section);

	UseFireTrail						(m_enemy.bUseFireTrail);//temp force reloar disp params

	m_sAmmoType							= pSettings->r_string(section, "ammo_class");
	m_CurrentAmmo.Load					(*m_sAmmoType, 0);

	m_sRocketSection					= pSettings->r_string	(section,"rocket_class");

	m_use_rocket_on_attack				= !!pSettings->r_bool(section,"use_rocket");
	m_use_mgun_on_attack				= !!pSettings->r_bool(section,"use_mgun");
	m_min_rocket_dist					= pSettings->r_float(section,"min_rocket_attack_dist");
	m_max_rocket_dist					= pSettings->r_float(section,"max_rocket_attack_dist");
	m_min_mgun_dist						= pSettings->r_float(section,"min_mgun_attack_dist");
	m_max_mgun_dist						= pSettings->r_float(section,"max_mgun_attack_dist");
	m_time_between_rocket_attack		= pSettings->r_u32(section,"time_between_rocket_attack");
	m_syncronize_rocket					= !!pSettings->r_bool(section,"syncronize_rocket");
	m_barrel_dir_tolerance				= pSettings->r_float(section,"barrel_dir_tolerance");

//lighting & sounds
	m_smoke_particle					= pSettings->r_string(section,"smoke_particle");

	m_light_range						= pSettings->r_float(section,"light_range");
	m_light_brightness					= pSettings->r_float(section,"light_brightness");

	m_light_color						= pSettings->r_fcolor(section,"light_color");
	m_light_color.a						= 1.f;
	m_light_color.mul_rgb				(m_light_brightness);
	const char* lanim						= pSettings->r_string	(section,"light_color_animmator");
	m_lanim								= LALib.FindItem(lanim);
}

void CHelicopter::reload(const char* section)
{
	inherited::reload	(section);
}

void CollisionCallbackAlife(bool& do_colide,bool bo1,dContact& c,SGameMtl* material_1,SGameMtl* material_2)
{	do_colide=false; }

void ContactCallbackAlife(CDB::TRI* T,dContactGeom* c)
{ }

BOOL CHelicopter::net_Spawn(CSE_Abstract*	DC)
{
	SetfHealth(100.0f);
	setState(CHelicopter::eAlive);
	m_flame_started					=false;
	m_light_started					=false;
	m_exploded						=false;
	m_ready_explode					=false;
	m_dead							=false;
	
	if (!inherited::net_Spawn(DC))
		return			(FALSE);

	CPHSkeleton::Spawn((CSE_Abstract*)(DC));
	for(u32 i=0; i<4; ++i)
		CRocketLauncher::SpawnRocket(*m_sRocketSection, smart_cast<CGameObject*>(this));

	// assigning m_animator here
	CSE_Abstract		*abstract	=(CSE_Abstract*)(DC);
	CSE_ALifeHelicopter	*heli		= smart_cast<CSE_ALifeHelicopter*>(abstract);
	VERIFY				(heli);

	R_ASSERT						(Visual()&&smart_cast<CKinematics*>(Visual()));
	CKinematics* K					= smart_cast<CKinematics*>(Visual());
	CConfigurationFile* pUserData				= K->LL_UserData();

	m_rotate_x_bone			= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_rotate_x_bone"));
	m_rotate_y_bone			= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_rotate_y_bone"));
	m_fire_bone				= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_fire_bone"));
	m_death_bones_to_hide	= pUserData->r_string("on_death_mode","scale_bone");
	m_left_rocket_bone		= K->LL_BoneID	(pUserData->r_string("helicopter_definition","left_rocket_bone"));
	m_right_rocket_bone		= K->LL_BoneID	(pUserData->r_string("helicopter_definition","right_rocket_bone"));

	m_smoke_bone 			= K->LL_BoneID	(pUserData->r_string("helicopter_definition","smoke_bone"));
	m_light_bone 			= K->LL_BoneID	(pUserData->r_string("helicopter_definition","light_bone"));

	CExplosive::Load		(pUserData,"explosion");
	CExplosive::SetInitiator(ID());

	const char* s = pUserData->r_string("helicopter_definition","hit_section");

	if( pUserData->section_exist(s) ){
		int lc = pUserData->line_count(s);
		const char* name;
		const char* value;
		signed short boneID;
		for (int i=0 ;i<lc; ++i){
			pUserData->r_line( s, i, &name, &value);
			boneID	=K->LL_BoneID(name);
			m_hitBones.insert( std::make_pair(boneID, (float)atof(value)) );
		}
	}
	
	CBoneInstance& biX		= smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(m_rotate_x_bone);	
	biX.set_callback		(bctCustom,BoneMGunCallbackX,this);
	CBoneInstance& biY		= smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(m_rotate_y_bone);	
	biY.set_callback		(bctCustom,BoneMGunCallbackY,this);
	CBoneData& bdX			= K->LL_GetData(m_rotate_x_bone); VERIFY(bdX.IK_data.type==jtJoint);
	m_lim_x_rot.set			(bdX.IK_data.limits[0].limit.x,bdX.IK_data.limits[0].limit.y);
	CBoneData& bdY			= K->LL_GetData(m_rotate_y_bone); VERIFY(bdY.IK_data.type==jtJoint);
	m_lim_y_rot.set			(bdY.IK_data.limits[1].limit.x,bdY.IK_data.limits[1].limit.y);

	xr_vector<Fmatrix> matrices;
	K->LL_GetBindTransform	(matrices);
	m_i_bind_x_xform.invert	(matrices[m_rotate_x_bone]);
	m_i_bind_y_xform.invert	(matrices[m_rotate_y_bone]);
	m_bind_rot.x			= matrices[m_rotate_x_bone].k.getP();
	m_bind_rot.y			= matrices[m_rotate_y_bone].k.getH();
	m_bind_x.set			(matrices[m_rotate_x_bone].c);
	m_bind_y.set			(matrices[m_rotate_y_bone].c);
	
	CKinematicsAnimated	*A	= smart_cast<CKinematicsAnimated*>(Visual());
	if (A) {
		A->PlayCycle		(*heli->startup_animation);
		A->CalculateBones	();
	}

	m_engineSound.create			(*heli->engine_sound,st_Effect,sg_SourceType);
	m_engineSound.play_at_pos		(0,XFORM().c,sm_Looped);
	
	CShootingObject::Light_Create	();

	setVisible						(TRUE);
	setEnabled						(TRUE);

	m_stepRemains						= 0.0f;

//lighting
	m_light_render						= ::Render->light_create();
	m_light_render->set_shadow			(false);
	m_light_render->set_type			(IRender_Light::POINT);
	m_light_render->set_range			(m_light_range);
	m_light_render->set_color			(m_light_color);

	if(g_Alive())processing_activate	();
	TurnEngineSound(false);
	if(pUserData->section_exist("destroyed"))
		CPHDestroyable::Load(pUserData,"destroyed");

#ifdef DEBUG
	Device.seqRender.Add(this,REG_PRIORITY_LOW-1);
#endif

	return TRUE;
}

void CHelicopter::net_Destroy()
{
	inherited::net_Destroy				();
	CExplosive::net_Destroy				();
	CShootingObject::Light_Destroy		();
	CShootingObject::StopFlameParticles	();
	CPHSkeleton::RespawnInit			();
	CPHDestroyable::RespawnInit			();
	m_engineSound.stop					();
	m_brokenSound.stop					();
	CParticlesObject::Destroy			(m_pParticle);
	m_light_render.destroy				();
	m_movement.net_Destroy				();

#ifdef DEBUG
	Device.seqRender.Remove(this);
#endif

}

void CHelicopter::SpawnInitPhysics	(CSE_Abstract	*D)	
{
	
	PPhysicsShell()=P_build_Shell	(this,false);
	if(g_Alive())
	{
		PPhysicsShell()->EnabledCallbacks				(FALSE);
		PPhysicsShell()->set_ObjectContactCallback		(CollisionCallbackAlife);
		PPhysicsShell()->set_ContactCallback			(ContactCallbackAlife);
		PPhysicsShell()->Disable						();
	}
}

void	CHelicopter::net_Save			(CNetPacket& P)	
{
	inherited::net_Save					(P);
	CPHSkeleton::SaveNetState			(P);
}

float GetCurrAcc(float V0, float V1, float dist, float a0, float a1);

void CHelicopter::MoveStep()
{
	Fvector3 dir;
	Fvector3 pathDir;
	float desired_H = m_movement.currPathH;
	float desired_P;
	if(m_movement.type != eMovNone){

		float dist = m_movement.currP.distance_to(m_movement.desiredPoint);

		dir.sub(m_movement.desiredPoint,m_movement.currP);
		dir.normalize_safe();
		pathDir = dir;
		dir.getHP(desired_H, desired_P);
		float speed_ = _min(m_movement.GetSpeedInDestPoint(), GetMaxVelocity() );

		static float ang = pSettings->r_float	(cNameSect(),"magic_angle");
		if(m_movement.curLinearSpeed>GetMaxVelocity() || angle_difference(m_movement.currPathH,desired_H)>ang)
			m_movement.curLinearAcc = -m_movement.LinearAcc_bk;
		else
			m_movement.curLinearAcc = GetCurrAcc(	m_movement.curLinearSpeed,
													speed_,
													dist*0.95f,
													m_movement.LinearAcc_fw,
													-m_movement.LinearAcc_bk);

		angle_lerp	(m_movement.currPathH, desired_H, m_movement.GetAngSpeedHeading(m_movement.curLinearSpeed), STEP);
		angle_lerp	(m_movement.currPathP, desired_P, m_movement.GetAngSpeedPitch(m_movement.curLinearSpeed), STEP);
		
		dir.setHP(m_movement.currPathH, m_movement.currPathP);

		float vp = m_movement.curLinearSpeed*STEP+(m_movement.curLinearAcc*STEP*STEP)/2.0f;
		m_movement.currP.mad	(dir, vp);
		m_movement.curLinearSpeed += m_movement.curLinearAcc*STEP;
		static bool aaa = false;
		if(aaa)
			Log("1-m_movement.curLinearSpeed=",m_movement.curLinearSpeed);
		clamp(m_movement.curLinearSpeed,0.0f,1000.0f);
		if(aaa)
			Log("2-m_movement.curLinearSpeed=",m_movement.curLinearSpeed);
	}else{ //go stopping
		if( !fis_zero(m_movement.curLinearSpeed) ){
			m_movement.curLinearAcc = -m_movement.LinearAcc_bk;

			float vp = m_movement.curLinearSpeed*STEP+(m_movement.curLinearAcc*STEP*STEP)/2.0f;
			dir.setHP(m_movement.currPathH, m_movement.currPathP);
			dir.normalize_safe();
			m_movement.currP.mad	(dir, vp);
			m_movement.curLinearSpeed += m_movement.curLinearAcc*STEP;
			clamp(m_movement.curLinearSpeed,0.0f,1000.0f);
//			clamp(m_movement.curLinearSpeed,0.0f,m_movement.maxLinearSpeed);

		}else{
			m_movement.curLinearAcc		= 0.0f;
			m_movement.curLinearSpeed	= 0.0f;
		}
	}

	if(	m_body.b_looking_at_point){
		Fvector3 desired_dir;
		desired_dir.sub(m_body.looking_point, m_movement.currP ).normalize_safe();

		float center_desired_H,tmp_P;
		desired_dir.getHP(center_desired_H, tmp_P);
		angle_lerp			(m_body.currBodyHPB.x, center_desired_H, m_movement.GetAngSpeedHeading(m_movement.curLinearSpeed), STEP);
	}else{
		angle_lerp			(m_body.currBodyHPB.x, m_movement.currPathH, m_movement.GetAngSpeedHeading(m_movement.curLinearSpeed), STEP);
	}

	float needBodyP = -m_body.model_pitch_k*m_movement.curLinearSpeed;
	if(m_movement.curLinearAcc < 0) needBodyP*=-1;
	angle_lerp	(m_body.currBodyHPB.y, needBodyP, m_body.model_angSpeedPitch, STEP);

	float sign;
	Fvector3 cp;
	cp.crossproduct (pathDir,dir);
	(cp.y>0.0)?sign=1.0f:sign=-1.0f;
	float ang_diff = angle_difference (m_movement.currPathH, desired_H);
	
	float needBodyB = -ang_diff*sign*m_body.model_bank_k*m_movement.curLinearSpeed;
	angle_lerp	(m_body.currBodyHPB.z, needBodyB, m_body.model_angSpeedBank, STEP);

	XFORM().setHPB(m_body.currBodyHPB.x,m_body.currBodyHPB.y,m_body.currBodyHPB.z);

	XFORM().translate_over(m_movement.currP);
}

void CHelicopter::UpdateCL()
{
	inherited::UpdateCL	();
	CExplosive::UpdateCL();
	if(PPhysicsShell() && (state() == CHelicopter::eDead) ){

		PPhysicsShell()->InterpolateGlobalTransform(&XFORM());

		CKinematics* K		= smart_cast<CKinematics*>(Visual());
		K->CalculateBones	();
		//smoke
		UpdateHeliParticles();

		if(m_brokenSound._feedback())
			m_brokenSound.set_position(XFORM().c);

		return;
	}
	else
		PPhysicsShell()->SetTransform(XFORM());

	m_movement.Update();

	m_stepRemains+=Device.fTimeDelta;
	while(m_stepRemains>STEP){
		MoveStep();
		m_stepRemains-=STEP;
	}

#ifdef DEBUG
	if (bDebug)
	{
		CGameFont* F = UI( )->Font( )->pFontDI;
		F->SetAligment(CGameFont::alCenter);
//		F->SetSizeI(0.02f);
		F->OutSetI(0.0f, -0.8f);
		F->SetColor(0xffffffff);
		F->OutNext("Heli: speed=%4.4f acc=%4.4f dist=%4.4f", m_movement.curLinearSpeed, m_movement.curLinearAcc, m_movement.GetDistanceToDestPosition( ));
	}
#endif

	if(m_engineSound._feedback())
		m_engineSound.set_position(XFORM().c);

	m_enemy.Update();
	//weapon
	UpdateWeapons();
	UpdateHeliParticles();

	CKinematics* K		= smart_cast<CKinematics*>(Visual());
	K->CalculateBones	();
}

void CHelicopter::shedule_Update(u32 time_delta)
{
	if (!getEnabled())	return;

	inherited::shedule_Update	(time_delta);
	if(CPHDestroyable::Destroyed())CPHDestroyable::SheduleUpdate(time_delta);
	else	CPHSkeleton::Update(time_delta);
	
	if(state() != CHelicopter::eDead){
		for(u32 i=getRocketCount(); i<4; ++i)
			CRocketLauncher::SpawnRocket(*m_sRocketSection, this);
	}
	if(m_ready_explode)ExplodeHelicopter();
}

void CHelicopter::goPatrolByPatrolPath (const char* path_name, int start_idx)
{
	m_movement.goPatrolByPatrolPath (path_name, start_idx);
}

void CHelicopter::goByRoundPath(Fvector3 center, float radius, bool clockwise)
{
	m_movement.goByRoundPath(center, radius, clockwise);
}

void CHelicopter::LookAtPoint(Fvector3 point, bool do_it)
{
	m_body.LookAtPoint(point,do_it);
}

void CHelicopter::save(CNetPacket &output_packet)
{
	m_movement.save	(output_packet);
	m_body.save		(output_packet);
	m_enemy.save	(output_packet);
	output_packet.w_vec3(XFORM().c);
	output_packet.w_float(m_barrel_dir_tolerance);
	save_data		(m_use_rocket_on_attack, output_packet);
	save_data		(m_use_mgun_on_attack, output_packet);
	save_data		(m_min_rocket_dist, output_packet);
	save_data		(m_max_rocket_dist, output_packet);
	save_data		(m_min_mgun_dist, output_packet);
	save_data		(m_max_mgun_dist, output_packet);
	save_data		(m_time_between_rocket_attack, output_packet);
	save_data		(m_syncronize_rocket, output_packet);
}

void CHelicopter::load(IReader &input_packet)
{
	m_movement.load				(input_packet);
	m_body.load					(input_packet);
	m_enemy.load				(input_packet);
	input_packet.r_fvector3		(XFORM().c);
	m_barrel_dir_tolerance		= input_packet.r_float();
	UseFireTrail				(m_enemy.bUseFireTrail);//force reloar disp params

	load_data		(m_use_rocket_on_attack, input_packet);
	load_data		(m_use_mgun_on_attack, input_packet);
	load_data		(m_min_rocket_dist, input_packet);
	load_data		(m_max_rocket_dist, input_packet);
	load_data		(m_min_mgun_dist, input_packet);
	load_data		(m_max_mgun_dist, input_packet);
	load_data		(m_time_between_rocket_attack, input_packet);
	load_data		(m_syncronize_rocket, input_packet);
}

void CHelicopter::net_Relcase(CObject* O )
{
	CExplosive::net_Relcase(O);
	inherited::net_Relcase(O);
}

bool CHelicopter::isObjectVisible(CObject* O)
{
	Fvector3					dir_to_object;
	Fvector3					to_point;
	O->Center(to_point);
	Fvector3 from_point = XFORM( ).c;
	dir_to_object.sub(to_point, from_point).normalize_safe( );
	float ray_length = from_point.distance_to(to_point);

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

void  CHelicopter::TurnEngineSound(bool bOn)
{
	if (bOn)
		m_engineSound.set_volume(1.0f);
	else
		m_engineSound.set_volume(0.0f);
}

void CHelicopter::StartFlame( )
{
	if (m_pParticle)return;
	m_pParticle = CParticlesObject::Create(*m_smoke_particle, FALSE);

	Fvector3 zero_vector;
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
		Fvector3 vel;

		Fvector3 last_pos = PositionStack.back( ).vPosition;
		vel.sub(Position( ), last_pos);
		vel.mul(5.0f);

		m_pParticle->UpdateParent(m_particleXFORM, vel);
	}
//lighting
	if (m_light_render->get_active( ))
	{
		Fmatrix xf;
		Fmatrix& M = K->LL_GetTransform(U16(m_light_bone));
		xf.mul(XFORM( ), M);
		VERIFY(!fis_zero(DET(xf)));

		m_light_render->set_rotation(xf.k, xf.i);
		m_light_render->set_position(xf.c);

		if (m_lanim)
		{
			int frame;
			u32 clr = m_lanim->CalculateBGR(Device.fTimeGlobal, frame); // тючтЁр•рхЄ т ЇюЁьрЄх BGR
			Fcolor					fclr;
			fclr.set((float) color_get_B(clr), (float) color_get_G(clr), (float) color_get_R(clr), 1.f);
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
		CPHDestroyable::Destroy(ID( ), "physic_destroyable_object");

	CExplosive::SetInitiator(ID( ));
	CExplosive::GenExplodeEvent(Position( ), Fvector3( ).set(0.0f, 1.0f, 0.0f));
	m_brokenSound.stop( );
}

void CHelicopter::SetDestPosition(Fvector3* pos)
{
	m_movement.SetDestPosition(pos);
	if (bDebug)
	{
		Msg("---SetDestPosition %f %f %f", pos->x, pos->y, pos->z);
	}
}

float CHelicopter::GetDistanceToDestPosition( )
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

void CHelicopter::SetEnemy(Fvector3* pos)
{
	m_enemy.type = eEnemyPoint;
	m_enemy.destEnemyPos = *pos;
}

float CHelicopter::GetCurrVelocity( )
{
	return m_movement.curLinearSpeed;
}

void CHelicopter::SetMaxVelocity(float v)
{
	m_movement.maxLinearSpeed = v;
}

float CHelicopter::GetMaxVelocity( )
{
	return m_movement.maxLinearSpeed;
}
//////////////////////Start By JoHnY///////////////////////
void CHelicopter::SetLinearAcc(float LAcc_fw, float LAcc_bw)
{
	m_movement.LinearAcc_fw = LAcc_fw;	//ускорение разгона
	m_movement.LinearAcc_bk = LAcc_bw;	//ускорение торможения

}
//////////////////////End By JoHnY/////////////////////////
void CHelicopter::SetSpeedInDestPoint(float sp)
{
	m_movement.SetSpeedInDestPoint(sp);
	if (bDebug)
	{
		Msg("---SetSpeedInDestPoint %f", sp);
	}
}

float CHelicopter::GetSpeedInDestPoint(float sp)
{
	return m_movement.GetSpeedInDestPoint( );
}

void CHelicopter::SetOnPointRangeDist(float d)
{
	m_movement.onPointRangeDist = d;
	if (bDebug)
	{
		Msg("---SetOnPointRangeDist %f", d);
	}
}

float CHelicopter::GetOnPointRangeDist( )
{
	return m_movement.onPointRangeDist;
}

float CHelicopter::GetRealAltitude( )
{
	collide::rq_result		cR;
	Fvector3 down_dir;

	down_dir.set(0.0f, -1.0f, 0.0f);


	Level( ).ObjectSpace.RayPick(XFORM( ).c, down_dir, 1000.0f, collide::rqtStatic, cR, NULL);

	return cR.range;
}

void	CHelicopter::Hit(SHit* pHDS)
{
	if (GetfHealth( ) < 0.005f)
		return;

	if (state( ) == CHelicopter::eDead) return;

	if (pHDS->who == this)
		return;

	bonesIt It = m_hitBones.find(pHDS->bone( ));
	if (It != m_hitBones.end( ) && pHDS->hit_type == ALife::eHitTypeFireWound)
	{
		float curHealth = GetfHealth( );
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
		float hit_power = pHDS->damage( );
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

void CHelicopter::PHHit(float P, Fvector3& dir, CObject* who, signed short element, Fvector3 p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	if (!g_Alive( ))inherited::PHHit(P, dir, who, element, p_in_object_space, impulse, hit_type);
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
		const char* bone;

		U16 bone_id;
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
	Fvector3 lin_vel;

	Fvector3 prev_pos = PositionStack.front( ).vPosition;
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

void SHeliEnemy::Load(const char* section)
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
	output_packet.w_s16((signed short) type);
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

void CHelicopter::SetFireTrailLength(float val)
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

void SHeliBodyState::Load(const char* section)
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

void SHeliBodyState::LookAtPoint(Fvector3 point, bool do_it)
{
	b_looking_at_point = do_it;
	looking_point = point;
	type = (do_it) ? eBodyToPoint : eBodyByPath;
}

void SHeliBodyState::save(CNetPacket& output_packet)
{
	output_packet.w_s16((signed short) type);
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

float t_xx(float V0, float V1, float a0, float a1, float d, float fSign)
{
	return (V1 + _sqrt(V1 * V1 - (a1 / (a1 - a0)) * (V1 * V1 - V0 * V0 - 2 * a0 * d)) * fSign) / a1;
}

float t_1(float t10, float t11)
{
	if (t10 < 0)
		return t11;
	else
		if (t11 < 0)
			return t10;
		else
			return _min(t10, t11);
}

float t_0(float V0, float V1, float a0, float a1, float t1)
{
	return (V1 - V0 - a1 * t1) / a0;
}

float getA(float t0, float a1, float a0)
{
	float eps = 0.001f;
	return (t0 < eps) ? a1 : a0;
}

float GetCurrAcc(float V0, float V1, float dist, float a0, float a1)
{
	float t10, t11, t0, t1;

	t10 = t_xx(V0, V1, a0, a1, dist, 1.0f);
	t11 = t_xx(V0, V1, a0, a1, dist, -1.0f);
	t1 = t_1(t10, t11);
	t0 = t_0(V0, V1, a0, a1, t1);
	return getA(t0, a1, a0);
}
