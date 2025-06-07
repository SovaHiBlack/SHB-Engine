//////////////////////////////////////////////////////////////////////
// ShootingObject.cpp:  ��������� ��� ��������� ���������� �������� 
//						(������ � ���������� �������) 	
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ShootingObject.h"

#include "ParticlesObject.h"
#include "WeaponAmmo.h"

#include "Actor.h"
#include "game_cl_base.h"
#include "level.h"
#include "BulletManager.h"
#include "clsid_game.h"
#include "game_cl_single.h"

#define HIT_POWER_EPSILON 0.05f
#define WALLMARK_SIZE 0.04f

CShootingObject::CShootingObject(void)
{
	fTime = 0.0f;
	fTimeToFire = 0.0f;

	fvHitPower.set(0.0f, 0.0f, 0.0f, 0.0f);
	m_fStartBulletSpeed = 1000.0f;

	m_vCurrentShootDir.set(0.0f, 0.0f, 0.0f);
	m_vCurrentShootPos.set(0.0f, 0.0f, 0.0f);
	m_iCurrentParentID = 0xFFFF;

	m_fPredBulletTime = 0.0f;
	m_bUseAimBullet = false;
	m_fTimeToAim = 0.0f;

	//particles
	m_sFlameParticlesCurrent = m_sFlameParticles = NULL;
	m_sSmokeParticlesCurrent = m_sSmokeParticles = NULL;
	m_sShellParticles = NULL;

	bWorking = false;

	light_render = 0;

	reinit( );
}

CShootingObject::~CShootingObject( )
{ }

void CShootingObject::reinit( )
{
	m_pFlameParticles = NULL;
}

void CShootingObject::Load(pcstr section)
{
	if (pSettings->line_exist(section, "light_disabled"))
	{
		m_bLightShotEnabled = !pSettings->r_bool(section, "light_disabled");
	}
	else
	{
		m_bLightShotEnabled = true;
	}

	//����� ������������� �� �������
	fTimeToFire = pSettings->r_float(section, "rpm");
	VERIFY(fTimeToFire > 0.0f);
	fTimeToFire = 60.0f / fTimeToFire;

	LoadFireParams(section, "");
	LoadLights(section, "");
	LoadShellParticles(section, "");
	LoadFlameParticles(section, "");
}

void CShootingObject::Light_Create( )
{
	//lights
	light_render = ::Render->light_create( );
	if (::Render->get_generation( ) == IRender_interface::GENERATION_R2)
	{
		light_render->set_shadow(true);
	}
	else
	{
		light_render->set_shadow(false);
	}
}

void CShootingObject::Light_Destroy( )
{
	light_render.destroy( );
}

void CShootingObject::LoadFireParams(pcstr section, pcstr prefix)
{
	string256	full_name;
	string32	buffer;
	shared_str	s_sHitPower;
	//������� ��������� ������
	fireDispersionBase = pSettings->r_float(section, "fire_dispersion_base");
	fireDispersionBase = deg2rad(fireDispersionBase);
	//���� �������� � ��� ���������
	s_sHitPower = pSettings->r_string_wb(section, strconcat(sizeof(full_name), full_name, prefix, "hit_power"));//������ ������ ���� ���� ���� ������
	fvHitPower[egdMaster] = (f32)atof(_GetItem(*s_sHitPower, 0, buffer));//������ �������� - ��� ��� ��� ������ ���� ������

	fvHitPower[egdVeteran] = fvHitPower[egdMaster];//���������� ��������� ��� ������ �������
	fvHitPower[egdStalker] = fvHitPower[egdMaster];//���������
	fvHitPower[egdNovice] = fvHitPower[egdMaster];//����� ��

	s32 num_game_diff_param = _GetItemCount(*s_sHitPower);//����� ����������� ���������� ��� �����
	if (num_game_diff_param > 1)//���� ����� ������ �������� ����
	{
		fvHitPower[egdVeteran] = (f32)atof(_GetItem(*s_sHitPower, 1, buffer));//�� ���������� ��� ��� ������ ��������
	}

	if (num_game_diff_param > 2)//���� ����� ������ �������� ����
	{
		fvHitPower[egdStalker] = (f32)atof(_GetItem(*s_sHitPower, 2, buffer));//�� ���������� ��� ��� ������ ��������
	}

	if (num_game_diff_param > 3)//���� ����� �������� �������� ����
	{
		fvHitPower[egdNovice] = (f32)atof(_GetItem(*s_sHitPower, 3, buffer));//�� ���������� ��� ��� ������ �������
	}

	fHitImpulse = pSettings->r_float(section, strconcat(sizeof(full_name), full_name, prefix, "hit_impulse"));
	//������������ ���������� ������ ����
	fireDistance = pSettings->r_float(section, strconcat(sizeof(full_name), full_name, prefix, "fire_distance"));
	//��������� �������� ����
	m_fStartBulletSpeed = pSettings->r_float(section, strconcat(sizeof(full_name), full_name, prefix, "bullet_speed"));
	m_bUseAimBullet = pSettings->r_bool(section, strconcat(sizeof(full_name), full_name, prefix, "use_aim_bullet"));
	if (m_bUseAimBullet)
	{
		m_fTimeToAim = pSettings->r_float(section, strconcat(sizeof(full_name), full_name, prefix, "time_to_aim"));
	}
}

void CShootingObject::LoadLights(pcstr section, pcstr prefix)
{
	string256 full_name;
	// light
	if (m_bLightShotEnabled)
	{
		fVector3 clr = pSettings->r_fvector3(section, strconcat(sizeof(full_name), full_name, prefix, "light_color"));
		light_base_color.set(clr.x, clr.y, clr.z, 1);
		light_base_range = pSettings->r_float(section, strconcat(sizeof(full_name), full_name, prefix, "light_range"));
		light_var_color = pSettings->r_float(section, strconcat(sizeof(full_name), full_name, prefix, "light_var_color"));
		light_var_range = pSettings->r_float(section, strconcat(sizeof(full_name), full_name, prefix, "light_var_range"));
		light_lifetime = pSettings->r_float(section, strconcat(sizeof(full_name), full_name, prefix, "light_time"));
		light_time = -1.f;
	}
}

void CShootingObject::Light_Start( )
{
	if (!light_render)
	{
		Light_Create( );
	}

	if (Device.dwFrame != light_frame)
	{
		light_frame = Device.dwFrame;
		light_time = light_lifetime;

		light_build_color.set(Random.randFs(light_var_color, light_base_color.r), Random.randFs(light_var_color, light_base_color.g), Random.randFs(light_var_color, light_base_color.b), 1);
		light_build_range = Random.randFs(light_var_range, light_base_range);
	}
}

void CShootingObject::Light_Render(const fVector3& P)
{
	f32 light_scale = light_time / light_lifetime;
	R_ASSERT(light_render);

	light_render->set_position(P);
	light_render->set_color(light_build_color.r * light_scale, light_build_color.g * light_scale, light_build_color.b * light_scale);
	light_render->set_range(light_build_range * light_scale);

	if (!light_render->get_active( ))
	{
		light_render->set_active(true);
	}
}

//////////////////////////////////////////////////////////////////////////
// Particles
//////////////////////////////////////////////////////////////////////////
void CShootingObject::StartParticles(CParticlesObject*& pParticles, pcstr particles_name, const fVector3& pos, const  fVector3& vel, bool auto_remove_flag)
{
	if (!particles_name)
	{
		return;
	}

	if (pParticles != NULL)
	{
		UpdateParticles(pParticles, pos, vel);
		return;
	}

	pParticles = CParticlesObject::Create(particles_name, (BOOL)auto_remove_flag);

	UpdateParticles(pParticles, pos, vel);
	pParticles->Play( );
}

void CShootingObject::StopParticles(CParticlesObject*& pParticles)
{
	if (pParticles == NULL)
	{
		return;
	}

	pParticles->Stop( );
	CParticlesObject::Destroy(pParticles);
}

void CShootingObject::UpdateParticles(CParticlesObject*& pParticles, const fVector3& pos, const fVector3& vel)
{
	if (!pParticles)
	{
		return;
	}

	fMatrix4x4 particles_pos;
	particles_pos.set(get_ParticlesXFORM( ));
	particles_pos.c.set(pos);

	pParticles->SetXFORM(particles_pos);

	if (!pParticles->IsAutoRemove( ) && !pParticles->IsLooped( ) && !pParticles->PSI_alive( ))
	{
		pParticles->Stop( );
		CParticlesObject::Destroy(pParticles);
	}
}

void CShootingObject::LoadShellParticles(pcstr section, pcstr prefix)
{
	string256 full_name;
	strconcat(sizeof(full_name), full_name, prefix, "shell_particles");

	if (pSettings->line_exist(section, full_name))
	{
		m_sShellParticles = pSettings->r_string(section, full_name);
		vLoadedShellPoint = pSettings->r_fvector3(section, strconcat(sizeof(full_name), full_name, prefix, "shell_point"));
	}
}

void CShootingObject::LoadFlameParticles(pcstr section, pcstr prefix)
{
	string256 full_name;

	// flames
	strconcat(sizeof(full_name), full_name, prefix, "flame_particles");
	if (pSettings->line_exist(section, full_name))
	{
		m_sFlameParticles = pSettings->r_string(section, full_name);
	}

	strconcat(sizeof(full_name), full_name, prefix, "smoke_particles");
	if (pSettings->line_exist(section, full_name))
	{
		m_sSmokeParticles = pSettings->r_string(section, full_name);
	}

	strconcat(sizeof(full_name), full_name, prefix, "shot_particles");
	if (pSettings->line_exist(section, full_name))
	{
		m_sShotParticles = pSettings->r_string(section, full_name);
	}

	//������� ��������
	m_sFlameParticlesCurrent = m_sFlameParticles;
	m_sSmokeParticlesCurrent = m_sSmokeParticles;
}

void CShootingObject::OnShellDrop(const fVector3& play_pos, const fVector3& parent_vel)
{
	if (!m_sShellParticles)
	{
		return;
	}

	if (Device.vCameraPosition.distance_to_sqr(play_pos) > 2 * 2)
	{
		return;
	}

	CParticlesObject* pShellParticles = CParticlesObject::Create(*m_sShellParticles, TRUE);

	fMatrix4x4 particles_pos;
	particles_pos.set(get_ParticlesXFORM( ));
	particles_pos.c.set(play_pos);

	pShellParticles->UpdateParent(particles_pos, parent_vel);
	pShellParticles->Play( );
}

//�������� ����
void CShootingObject::StartSmokeParticles(const fVector3& play_pos, const fVector3& parent_vel)
{
	CParticlesObject* pSmokeParticles = NULL;
	StartParticles(pSmokeParticles, *m_sSmokeParticlesCurrent, play_pos, parent_vel, true);
}

void CShootingObject::StartFlameParticles( )
{
	if (0 == m_sFlameParticlesCurrent.size( ))
	{
		return;
	}

	//���� �������� �����������
	if (m_pFlameParticles && m_pFlameParticles->IsLooped( ) && m_pFlameParticles->IsPlaying( ))
	{
		UpdateFlameParticles( );
		return;
	}

	StopFlameParticles( );
	m_pFlameParticles = CParticlesObject::Create(*m_sFlameParticlesCurrent, FALSE);
	UpdateFlameParticles( );
	m_pFlameParticles->Play( );

}
void CShootingObject::StopFlameParticles( )
{
	if (0 == m_sFlameParticlesCurrent.size( ))
	{
		return;
	}

	if (m_pFlameParticles == NULL)
	{
		return;
	}

	m_pFlameParticles->SetAutoRemove(true);
	m_pFlameParticles->Stop( );
	m_pFlameParticles = NULL;
}

void CShootingObject::UpdateFlameParticles( )
{
	if (0 == m_sFlameParticlesCurrent.size( ))
	{
		return;
	}

	if (!m_pFlameParticles)
	{
		return;
	}

	fMatrix4x4 pos;
	pos.set(get_ParticlesXFORM( ));
	pos.c.set(get_CurrentFirePoint( ));

	m_pFlameParticles->SetXFORM(pos);

	if (!m_pFlameParticles->IsLooped( ) && !m_pFlameParticles->IsPlaying( ) && !m_pFlameParticles->PSI_alive( ))
	{
		m_pFlameParticles->Stop( );
		CParticlesObject::Destroy(m_pFlameParticles);
	}
}

//��������� �� ��������
void CShootingObject::UpdateLight( )
{
	if (light_render && light_time > 0)
	{
		light_time -= Device.fTimeDelta;
		if (light_time <= 0.0f)
		{
			StopLight( );
		}
	}
}

void CShootingObject::StopLight( )
{
	if (light_render)
	{
		light_render->set_active(false);
	}
}

void CShootingObject::RenderLight( )
{
	if (light_render && light_time > 0)
	{
		Light_Render(get_CurrentFirePoint( ));
	}
}

bool CShootingObject::SendHitAllowed(CObject* pUser)
{
	if (Game( ).IsServerControlHits( ))
	{
		return OnServer( );
	}

	if (OnServer( ))
	{
		if (pUser->CLS_ID == CLSID_OBJECT_ACTOR)
		{
			if (Level( ).CurrentControlEntity( ) != pUser)
			{
				return false;
			}
		}

		return true;
	}
	else
	{
		if (pUser->CLS_ID == CLSID_OBJECT_ACTOR)
		{
			if (Level( ).CurrentControlEntity( ) == pUser)
			{
				return true;
			}
		}

		return false;
	}
}

extern void random_dir(fVector3& tgt_dir, const fVector3& src_dir, f32 dispersion);

void CShootingObject::FireBullet(const fVector3& pos,
								 const fVector3& shot_dir,
								 f32 fire_disp,
								 const CCartridge& cartridge,
								 u16 parent_id,
								 u16 weapon_id,
								 bool send_hit)
{
	fVector3 dir;
	random_dir(dir, shot_dir, fire_disp);

	m_vCurrentShootDir = dir;
	m_vCurrentShootPos = pos;
	m_iCurrentParentID = parent_id;

	bool aim_bullet;
	if (m_bUseAimBullet)
	{
		if (ParentMayHaveAimBullet( ))
		{
			if (m_fPredBulletTime == 0.0f)
			{
				aim_bullet = true;
			}
			else
			{
				if ((Device.fTimeGlobal - m_fPredBulletTime) >= m_fTimeToAim)
				{
					aim_bullet = true;
				}
				else
				{
					aim_bullet = false;
				}
			}
		}
		else
		{
			aim_bullet = false;
		}
	}
	else
	{
		aim_bullet = false;
	}

	m_fPredBulletTime = Device.fTimeGlobal;

	f32 l_fHitPower;
	if (ParentIsActor( ))//���� �� ������ �������� ����(�����)
	{
		l_fHitPower = fvHitPower[g_SingleGameDifficulty];
	}
	else
	{
		l_fHitPower = fvHitPower[egdMaster];
	}

	Level( ).BulletManager( ).AddBullet(pos, dir, m_fStartBulletSpeed, l_fHitPower,
										fHitImpulse, parent_id, weapon_id,
										ALife::eHitTypeFireWound, fireDistance, cartridge, send_hit, aim_bullet);
}

void CShootingObject::FireStart( )
{
	bWorking = true;
}

void CShootingObject::FireEnd( )
{
	bWorking = false;
}

void CShootingObject::StartShotParticles( )
{
	CParticlesObject* pSmokeParticles = NULL;
	StartParticles(pSmokeParticles, *m_sShotParticles, m_vCurrentShootPos, m_vCurrentShootDir, true);
}
