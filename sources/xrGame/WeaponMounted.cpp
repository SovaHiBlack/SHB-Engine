#include "stdafx.h"

#include "WeaponMounted.h"
#include "xrServer_Objects_ALife.h"
#include "CameraFirstEye.h"
#include "Actor.h"
#include "WeaponAmmo.h"

#include "ActorEffector.h"
#include "effectorshot.h"
#include "ai_sounds.h"
#include "level.h"
#include "xr_level_controller.h"
#include "..\XR_3DA\skeletoncustom.h"
#include "GameObject_space.h"

void CWeaponMounted::BoneCallbackX(CBoneInstance* B)
{
	CWeaponMounted* P = static_cast<CWeaponMounted*>(B->Callback_Param);
	if (P->Owner( ))
	{
		fMatrix4x4 rX;
		rX.rotateX(P->camera->pitch + P->m_dAngle.y);
		B->mTransform.mulB_43(rX);
	}
}

void CWeaponMounted::BoneCallbackY(CBoneInstance* B)
{
	CWeaponMounted* P = static_cast<CWeaponMounted*>(B->Callback_Param);
	if (P->Owner( ))
	{
		fMatrix4x4 rY;
		rY.rotateY(P->camera->yaw + P->m_dAngle.x);
		B->mTransform.mulB_43(rY);
	}
}

CWeaponMounted::CWeaponMounted( )
{
	camera = xr_new<CCameraFirstEye>(this, CCameraBase::flRelativeLink | CCameraBase::flPositionRigid | CCameraBase::flDirectionRigid);
	camera->Load("mounted_weapon_cam");
}

CWeaponMounted::~CWeaponMounted( )
{
	xr_delete(camera);
}

void CWeaponMounted::Load(pcstr section)
{
	inherited::Load(section);
	CShootingObject::Load(section);

	SHudSound::LoadSound(section, "snd_shoot", sndShot, eST_WEAPON_SHOOTING);

	//��� ������������ ��������
	m_sAmmoType = pSettings->r_string(section, "ammo_class");
	m_CurrentAmmo.Load(*m_sAmmoType, 0);

	//������������� ������ �� ����� ������
	camMaxAngle = pSettings->r_float(section, "cam_max_angle");
	camMaxAngle = deg2rad(camMaxAngle);
	camRelaxSpeed = pSettings->r_float(section, "cam_relax_speed");
	camRelaxSpeed = deg2rad(camRelaxSpeed);
}

BOOL CWeaponMounted::net_Spawn(CSE_Abstract* DC)
{
	CSE_Abstract* e = (CSE_Abstract*)(DC);
	CSE_ALifeMountedWeapon* mw = smart_cast<CSE_ALifeMountedWeapon*>(e);
	R_ASSERT(mw);

	if (!inherited::net_Spawn(DC))
	{
		return FALSE;
	}

	R_ASSERT(Visual( ) && smart_cast<CKinematics*>(Visual( )));

	CKinematics* K = smart_cast<CKinematics*>(Visual( ));
	CIniFile* pUserData = K->LL_UserData( );

	R_ASSERT3(pUserData, "Empty MountedWeapon user data!", mw->get_visual( ));

	fire_bone = K->LL_BoneID(pUserData->r_string("mounted_weapon_definition", "fire_bone"));
	actor_bone = K->LL_BoneID(pUserData->r_string("mounted_weapon_definition", "actor_bone"));
	rotate_x_bone = K->LL_BoneID(pUserData->r_string("mounted_weapon_definition", "rotate_x_bone"));
	rotate_y_bone = K->LL_BoneID(pUserData->r_string("mounted_weapon_definition", "rotate_y_bone"));
	camera_bone = K->LL_BoneID(pUserData->r_string("mounted_weapon_definition", "camera_bone"));

	CBoneData& bdX = K->LL_GetData(rotate_x_bone); VERIFY(bdX.IK_data.type == jtJoint);
	camera->lim_pitch.set(bdX.IK_data.limits[0].limit.x, bdX.IK_data.limits[0].limit.y);
	CBoneData& bdY = K->LL_GetData(rotate_y_bone); VERIFY(bdY.IK_data.type == jtJoint);
	camera->lim_yaw.set(bdY.IK_data.limits[1].limit.x, bdY.IK_data.limits[1].limit.y);

	U16Vec fixed_bones;
	fixed_bones.push_back(K->LL_GetBoneRoot( ));
	PPhysicsShell( ) = P_build_Shell(this, false, fixed_bones);
	K->CalculateBones_Invalidate( );
	K->CalculateBones( );

	CShootingObject::Light_Create( );

	setVisible(TRUE);
	setEnabled(TRUE);

	return TRUE;
}

void CWeaponMounted::net_Destroy( )
{
	CShootingObject::Light_Destroy( );

	inherited::net_Destroy( );
	xr_delete(m_pPhysicsShell);
}

void CWeaponMounted::net_Export(CNetPacket& P)
{
	inherited::net_Export(P);
}

void CWeaponMounted::net_Import(CNetPacket& P)
{
	inherited::net_Import(P);
}

void CWeaponMounted::UpdateCL( )
{
	inherited::UpdateCL( );
	if (Owner( ))
	{
		CKinematics* K = smart_cast<CKinematics*>(Visual( ));
		K->CalculateBones( );
		// update fire pos & fire_dir
		fire_bone_xform = K->LL_GetTransform(fire_bone);
		fire_bone_xform.mulA_43(XFORM( ));
		fire_pos.set(0, 0, 0);
		fire_bone_xform.transform_tiny(fire_pos);
		fire_dir.set(0, 0, 1);
		fire_bone_xform.transform_dir(fire_dir);

		UpdateFire( );

		if (OwnerActor( ) && OwnerActor( )->IsMyCamera( ))
		{
			cam_Update(Device.fTimeDelta, g_fov);
			OwnerActor( )->Cameras( ).Update(Camera( ));
			OwnerActor( )->Cameras( ).ApplyDevice(VIEWPORT_NEAR);
		}
	}
}

void CWeaponMounted::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
}

void CWeaponMounted::renderable_Render( )
{
	//���������� ���������
	RenderLight( );

	inherited::renderable_Render( );
}

void CWeaponMounted::OnMouseMove(s32 dx, s32 dy)
{
	if (Remote( ))
	{
		return;
	}

	CCameraBase* C = camera;
	f32 scale = (C->f_fov / g_fov) * psMouseSens * psMouseSensScale / 50.0f;
	if (dx)
	{
		f32 d = f32(dx) * scale;
		C->Move((d < 0) ? kLEFT : kRIGHT, _abs(d));
	}

	if (dy)
	{
		f32 d = ((psMouseInvert.test(1)) ? -1 : 1) * f32(dy) * scale * 3.0f / 4.0f;
		C->Move((d > 0) ? kUP : kDOWN, _abs(d));
	}
}

void CWeaponMounted::OnKeyboardPress(s32 dik)
{
	if (Remote( ))
	{
		return;
	}

	switch (dik)
	{
		case kWPN_FIRE:
		{
			FireStart( );
		}
		break;
	}
}

void CWeaponMounted::OnKeyboardRelease(s32 dik)
{
	if (Remote( ))
	{
		return;
	}

	switch (dik)
	{
		case kWPN_FIRE:
		{
			FireEnd( );
		}
		break;
	}
}

void CWeaponMounted::OnKeyboardHold(s32 dik)
{
	if (Remote( ))
	{
		return;
	}
}

void CWeaponMounted::cam_Update(f32 dt, f32 fov)
{
	fVector3 P;
	fVector3 Da;
	Da.set(0.0f, 0.0f, 0.0f);

	CKinematics* K = smart_cast<CKinematics*>(Visual( ));
	K->CalculateBones_Invalidate( );
	K->CalculateBones( );
	const fMatrix4x4& C = K->LL_GetTransform(camera_bone);
	XFORM( ).transform_tiny(P, C.c);

	if (OwnerActor( ))
	{
		// rotate head
		OwnerActor( )->Orientation( ).yaw = -Camera( )->yaw;
		OwnerActor( )->Orientation( ).pitch = -Camera( )->pitch;
	}

	Camera( )->Update(P, Da);
	Level( ).Cameras( ).Update(Camera( ));
}

bool CWeaponMounted::Use(const fVector3& pos, const fVector3& dir, const fVector3& foot_pos)
{
	return !Owner( );
}

bool CWeaponMounted::attach_Actor(CGameObject* actor)
{
	m_dAngle.set(0.0f, 0.0f);
	CHolderCustom::attach_Actor(actor);
	CKinematics* K = smart_cast<CKinematics*>(Visual( ));
	// ������ ������ �� ���	
	// disable shell callback
	m_pPhysicsShell->EnabledCallbacks(FALSE);
	// enable actor rotate callback
	CBoneInstance& biX = smart_cast<CKinematics*>(Visual( ))->LL_GetBoneInstance(rotate_x_bone);
	biX.set_callback(bctCustom, BoneCallbackX, this);
	CBoneInstance& biY = smart_cast<CKinematics*>(Visual( ))->LL_GetBoneInstance(rotate_y_bone);
	biY.set_callback(bctCustom, BoneCallbackY, this);
	// set actor to mounted position
	const fMatrix4x4& A = K->LL_GetTransform(actor_bone);
	fVector3 ap;
	XFORM( ).transform_tiny(ap, A.c);
	fMatrix4x4 AP;
	AP.translate(ap);
	if (OwnerActor( ))
	{
		OwnerActor( )->SetPhPosition(AP);
	}

	processing_activate( );
	return true;
}

void CWeaponMounted::detach_Actor( )
{
	CHolderCustom::detach_Actor( );
	// disable actor rotate callback
	CBoneInstance& biX = smart_cast<CKinematics*>(Visual( ))->LL_GetBoneInstance(rotate_x_bone);
	biX.reset_callback( );
	CBoneInstance& biY = smart_cast<CKinematics*>(Visual( ))->LL_GetBoneInstance(rotate_y_bone);
	biY.reset_callback( );
	// enable shell callback
	m_pPhysicsShell->EnabledCallbacks(TRUE);

	//��������� ��������
	FireEnd( );

	processing_deactivate( );
}

fVector3 CWeaponMounted::ExitPosition( )
{
	return XFORM( ).c;
}

CCameraBase* CWeaponMounted::Camera( )
{
	return camera;
}

void CWeaponMounted::FireStart( )
{
	m_dAngle.set(0.0f, 0.0f);
	CShootingObject::FireStart( );
}

void CWeaponMounted::FireEnd( )
{
	m_dAngle.set(0.0f, 0.0f);
	CShootingObject::FireEnd( );
	StopFlameParticles( );
	RemoveShotEffector( );
}

void CWeaponMounted::OnShot( )
{
	VERIFY(Owner( ));

	FireBullet(get_CurrentFirePoint( ), fire_dir, fireDispersionBase, m_CurrentAmmo, Owner( )->ID( ), ID( ), SendHitAllowed(Owner( )));

	StartShotParticles( );

	if (m_bLightShotEnabled)
	{
		Light_Start( );
	}

	StartFlameParticles( );
	StartSmokeParticles(fire_pos, zero_vel);
	OnShellDrop(fire_pos, zero_vel);

	bool b_hud_mode = (Level( ).CurrentEntity( ) == smart_cast<CObject*>(Owner( )));
	SHudSound::PlaySound(sndShot, fire_pos, Owner( ), b_hud_mode);

	//�������� �������� ��������
	AddShotEffector( );
	m_dAngle.set(::Random.randF(-fireDispersionBase, fireDispersionBase), ::Random.randF(-fireDispersionBase, fireDispersionBase));
}

void CWeaponMounted::UpdateFire( )
{
	fTime -= Device.fTimeDelta;

	CShootingObject::UpdateFlameParticles( );
	CShootingObject::UpdateLight( );

	if (!IsWorking( ))
	{
		if (fTime < 0.0f)
		{
			fTime = 0.f;
		}

		return;
	}

	if (fTime <= 0.0f)
	{
		OnShot( );
		fTime += fTimeToFire;
	}
	else
	{
		angle_lerp(m_dAngle.x, 0.0f, 5.0f, Device.fTimeDelta);
		angle_lerp(m_dAngle.y, 0.0f, 5.0f, Device.fTimeDelta);
	}
}

const fMatrix4x4& CWeaponMounted::get_ParticlesXFORM( )
{
	return fire_bone_xform;
}

void CWeaponMounted::AddShotEffector( )
{
	if (OwnerActor( ))
	{
		CCameraShotEffector* S = smart_cast<CCameraShotEffector*>(OwnerActor( )->Cameras( ).GetCamEffector(eCEShot));
		if (!S)
		{
			S = (CCameraShotEffector*)OwnerActor( )->Cameras( ).AddCamEffector(xr_new<CCameraShotEffector>(camMaxAngle, camRelaxSpeed, 0.25f, 0.01f, 0.7f));
		}

		R_ASSERT(S);
		S->Shot(0.01f);
	}
}

void CWeaponMounted::RemoveShotEffector( )
{
	if (OwnerActor( ))
	{
		OwnerActor( )->Cameras( ).RemoveCamEffector(eCEShot);
	}
}
