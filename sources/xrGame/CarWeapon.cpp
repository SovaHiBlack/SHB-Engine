#include "stdafx.h"
#include "CarWeapon.h"
#include "PhysicsShell.h"
#include "PhysicsShellHolder.h"
#include "..\XR_3DA\skeletoncustom.h"
#include "object_broker.h"
#include "ai_sounds.h"
#include "WeaponAmmo.h"
#include "xr_level_controller.h"
#include "GameObject_space.h"

void CCarWeapon::BoneCallbackX(CBoneInstance* B)
{
	CCarWeapon* P = static_cast<CCarWeapon*>(B->Callback_Param);
	fMatrix4x4 rX;
	rX.rotateX(P->m_cur_x_rot);
	B->mTransform.mulB_43(rX);
}

void CCarWeapon::BoneCallbackY(CBoneInstance* B)
{
	CCarWeapon* P = static_cast<CCarWeapon*>(B->Callback_Param);
	fMatrix4x4 rY;
	rY.rotateY(P->m_cur_y_rot);
	B->mTransform.mulB_43(rY);
}

CCarWeapon::CCarWeapon(CPhysicsShellHolder* obj)
{
	m_bActive = false;
	m_bAutoFire = false;
	m_object = obj;
	m_Ammo = xr_new<CCartridge>( );

	CKinematics* K = smart_cast<CKinematics*>(m_object->Visual( ));
	CIniFile* pUserData = K->LL_UserData( );

	m_rotate_x_bone = K->LL_BoneID(pUserData->r_string("mounted_weapon_definition", "rotate_x_bone"));
	m_rotate_y_bone = K->LL_BoneID(pUserData->r_string("mounted_weapon_definition", "rotate_y_bone"));
	m_fire_bone = K->LL_BoneID(pUserData->r_string("mounted_weapon_definition", "fire_bone"));
	m_min_gun_speed = pUserData->r_float("mounted_weapon_definition", "min_gun_speed");
	m_max_gun_speed = pUserData->r_float("mounted_weapon_definition", "max_gun_speed");
	CBoneData& bdX = K->LL_GetData(m_rotate_x_bone);
	//VERIFY(bdX.IK_data.type==jtJoint);
	m_lim_x_rot.set(bdX.IK_data.limits[0].limit.x, bdX.IK_data.limits[0].limit.y);
	CBoneData& bdY = K->LL_GetData(m_rotate_y_bone);
	//VERIFY(bdY.IK_data.type==jtJoint);
	m_lim_y_rot.set(bdY.IK_data.limits[1].limit.x, bdY.IK_data.limits[1].limit.y);

	xr_vector<fMatrix4x4> matrices;
	K->LL_GetBindTransform(matrices);
	m_i_bind_x_xform.invert(matrices[m_rotate_x_bone]);
	m_i_bind_y_xform.invert(matrices[m_rotate_y_bone]);
	m_bind_x_rot = matrices[m_rotate_x_bone].k.getP( );
	m_bind_y_rot = matrices[m_rotate_y_bone].k.getH( );
	m_bind_x.set(matrices[m_rotate_x_bone].c);
	m_bind_y.set(matrices[m_rotate_y_bone].c);

	m_cur_x_rot = m_bind_x_rot;
	m_cur_y_rot = m_bind_y_rot;
	m_destEnemyDir.setHP(m_bind_y_rot, m_bind_x_rot);
	m_object->XFORM( ).transform_dir(m_destEnemyDir);

	inheritedShooting::Light_Create( );
	Load(pUserData->r_string("mounted_weapon_definition", "wpn_section"));
	SetBoneCallbacks( );
	m_object->processing_activate( );

	m_weapon_h = matrices[m_rotate_y_bone].c.y;
	m_fire_norm.set(0, 1, 0);
	m_fire_dir.set(0, 0, 1);
	m_fire_pos.set(0, 0, 0);
}

CCarWeapon::~CCarWeapon( )
{
	delete_data(m_Ammo);
//.	m_object->processing_deactivate		();
}

void CCarWeapon::Load(pcstr section)
{
	inheritedShooting::Load(section);
	SHudSound::LoadSound(section, "snd_shoot", m_sndShot, eST_WEAPON_SHOOTING);
	m_Ammo->Load(pSettings->r_string(section, "ammo_class"), 0);
}

void CCarWeapon::UpdateCL( )
{
	if (!m_bActive)
	{
		return;
	}

	UpdateBarrelDir( );
	CKinematics* K = smart_cast<CKinematics*>(m_object->Visual( ));
	K->CalculateBones_Invalidate( );
	K->CalculateBones( );
	UpdateFire( );
}

void CCarWeapon::UpdateFire( )
{
	fTime -= Device.fTimeDelta;

	inheritedShooting::UpdateFlameParticles( );
	inheritedShooting::UpdateLight( );

	if (m_bAutoFire)
	{
		if (m_allow_fire)
		{
			FireStart( );
		}
		else
		{
			FireEnd( );
		}
	}

	if (!IsWorking( ))
	{
		if (fTime < 0.0f)
		{
			fTime = 0.0f;
		}

		return;
	}

	if (fTime <= 0.0f)
	{
		OnShot( );
		fTime += fTimeToFire;
	}
}

void CCarWeapon::Render_internal( )
{
	RenderLight( );
}

void CCarWeapon::SetBoneCallbacks( )
{
//	m_object->PPhysicsShell()->EnabledCallbacks(FALSE);

	CBoneInstance& biX = smart_cast<CKinematics*>(m_object->Visual( ))->LL_GetBoneInstance(m_rotate_x_bone);
	biX.set_callback(bctCustom, BoneCallbackX, this);
	CBoneInstance& biY = smart_cast<CKinematics*>(m_object->Visual( ))->LL_GetBoneInstance(m_rotate_y_bone);
	biY.set_callback(bctCustom, BoneCallbackY, this);
}

void CCarWeapon::ResetBoneCallbacks( )
{
	CBoneInstance& biX = smart_cast<CKinematics*>(m_object->Visual( ))->LL_GetBoneInstance(m_rotate_x_bone);
	biX.reset_callback( );
	CBoneInstance& biY = smart_cast<CKinematics*>(m_object->Visual( ))->LL_GetBoneInstance(m_rotate_y_bone);
	biY.reset_callback( );

//	m_object->PPhysicsShell()->EnabledCallbacks(TRUE);
}

void CCarWeapon::UpdateBarrelDir( )
{
	CKinematics* K = smart_cast<CKinematics*>(m_object->Visual( ));
	m_fire_bone_xform = K->LL_GetTransform(m_fire_bone);

	m_fire_bone_xform.mulA_43(m_object->XFORM( ));
	m_fire_pos.set(0.0f, 0.0f, 0.0f);
	m_fire_bone_xform.transform_tiny(m_fire_pos);
	m_fire_dir.set(0.0f, 0.0f, 1.0f);
	m_fire_bone_xform.transform_dir(m_fire_dir);
	m_fire_norm.set(0.0f, 1.0f, 0.0f);
	m_fire_bone_xform.transform_dir(m_fire_norm);

	m_allow_fire = true;
	fMatrix4x4 XFi;
	XFi.invert(m_object->XFORM( ));
	fVector3 dep;
	XFi.transform_dir(dep, m_destEnemyDir);
	{// x angle
		m_i_bind_x_xform.transform_dir(dep); dep.normalize( );
		m_tgt_x_rot = angle_normalize_signed(m_bind_x_rot - dep.getP( ));
		clamp(m_tgt_x_rot, -m_lim_x_rot.y, -m_lim_x_rot.x);
	}
	{// y angle
		m_i_bind_y_xform.transform_dir(dep); dep.normalize( );
		m_tgt_y_rot = angle_normalize_signed(m_bind_y_rot - dep.getH( ));
		clamp(m_tgt_y_rot, -m_lim_y_rot.y, -m_lim_y_rot.x);
	}

	m_cur_x_rot = angle_inertion_var(m_cur_x_rot, m_tgt_x_rot, m_min_gun_speed, m_max_gun_speed, PI, Device.fTimeDelta);
	m_cur_y_rot = angle_inertion_var(m_cur_y_rot, m_tgt_y_rot, m_min_gun_speed, m_max_gun_speed, PI, Device.fTimeDelta);
	static f32 dir_eps = deg2rad(5.0f);
	if (!fsimilar(m_cur_x_rot, m_tgt_x_rot, dir_eps) || !fsimilar(m_cur_y_rot, m_tgt_y_rot, dir_eps))
	{
		m_allow_fire = FALSE;
	}

#if (0)
	if (Device.dwFrame % 200 == 0)
	{
		Msg("m_cur_x_rot=[%f]", m_cur_x_rot);
		Msg("m_cur_y_rot=[%f]", m_cur_y_rot);
	}
#endif
}
bool CCarWeapon::AllowFire( )
{
	return m_allow_fire;
}

f32 CCarWeapon::FireDirDiff( )
{
	fVector3 d1;
	fVector3 d2;
	d1.set(m_cur_x_rot, m_cur_y_rot, 0).normalize_safe( );
	d2.set(m_tgt_x_rot, m_tgt_y_rot, 0).normalize_safe( );
	return rad2deg(acos(d1.dotproduct(d2)));
}

const fVector3& CCarWeapon::get_CurrentFirePoint( )
{
	return m_fire_pos;
}

const fMatrix4x4& CCarWeapon::get_ParticlesXFORM( )
{
	return m_fire_bone_xform;
}

void CCarWeapon::FireStart( )
{
	inheritedShooting::FireStart( );
}

void CCarWeapon::FireEnd( )
{
	inheritedShooting::FireEnd( );
	StopFlameParticles( );
}

void CCarWeapon::OnShot( )
{
	FireBullet(m_fire_pos, m_fire_dir, fireDispersionBase, *m_Ammo, m_object->ID( ), m_object->ID( ), SendHitAllowed(m_object));

	StartShotParticles( );

	if (m_bLightShotEnabled)
	{
		Light_Start( );
	}

	StartFlameParticles( );
	StartSmokeParticles(m_fire_pos, zero_vel);
//	OnShellDrop				(m_fire_pos, zero_vel);

	SHudSound::PlaySound(m_sndShot, m_fire_pos, m_object, false);
}

void CCarWeapon::Action(s32 id, u32 flags)
{
	switch (id)
	{
		case eWpnFire:
		{
			if (flags == 1)
			{
				FireStart( );
			}
			else
			{
				FireEnd( );
			}
		}
		break;
		case eWpnActivate:
		{
			if (flags == 1)
			{
				m_bActive = true;
			}
			else
			{
				m_bActive = false; FireEnd( );
			}
		}
		break;
		case eWpnAutoFire:
		{
			if (flags == 1)
			{
				m_bAutoFire = true;
			}
			else
			{
				m_bAutoFire = false;
			}
		}
		break;
		case eWpnToDefaultDir:
		{
			SetParam(eWpnDesiredDir, fVector2( ).set(m_bind_y_rot, m_bind_x_rot));
		}
		break;
	}
}

void CCarWeapon::SetParam(s32 id, fVector2 val)
{
	switch (id)
	{
		case eWpnDesiredDir:
		{
			m_destEnemyDir.setHP(val.x, val.y);
		}
		break;
	}
}

void CCarWeapon::SetParam(s32 id, fVector3 val)
{
	switch (id)
	{
		case eWpnDesiredPos:
		{
			m_destEnemyDir.sub(val, m_fire_pos).normalize_safe( );
		}
		break;
	}
}

const fVector3& CCarWeapon::ViewCameraPos( )
{
	return m_fire_pos;
}

const fVector3& CCarWeapon::ViewCameraDir( )
{
	return m_fire_dir;
}

const fVector3& CCarWeapon::ViewCameraNorm( )
{
	return m_fire_norm;
}
