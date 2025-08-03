// WeaponFire.cpp: implementation of the CWeapon class.
// function responsible for firing with CWeapon
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Weapon.h"
#include "WeaponHUD.h"
#include "ParticlesObject.h"
#include "HUDManager.h"
#include "Entity.h"
#include "Actor.h"

#include "ActorEffector.h"
#include "effectorshot.h"

#include "BulletManager.h"

#define FLAME_TIME 0.05f

f32 _nrand(f32 sigma)
{
#define ONE_OVER_SIGMA_EXP (1.0f / 0.7975f)

	if (sigma == 0.0f)
	{
		return 0.0f;
	}

	f32 y;
	do
	{
		y = -logf(Random.randF( ));
	}
	while (Random.randF( ) > expf(-_sqr(y - 1.0f) * 0.5f));

	if (rand( ) & 0x1)
	{
		return (y * sigma * ONE_OVER_SIGMA_EXP);
	}
	else
	{
		return (-y * sigma * ONE_OVER_SIGMA_EXP);
	}
}

void random_dir(fVector3& tgt_dir, const fVector3& src_dir, f32 dispersion)
{
	f32 sigma = dispersion / 3.0f;
	f32 alpha = clampr(_nrand(sigma), -dispersion, dispersion);
	f32 theta = Random.randF(0.0f, PI);
	f32 r = tan(alpha);
	fVector3 U;
	fVector3 V;
	fVector3 T;
	fVector3::generate_orthonormal_basis(src_dir, U, V);
	U.mul(r * _sin(theta));
	V.mul(r * _cos(theta));
	T.add(U, V);
	tgt_dir.add(src_dir, T).normalize( );
}

f32 CWeapon::GetWeaponDeterioration( )
{
	return conditionDecreasePerShot;
}

void CWeapon::FireTrace(const fVector3& P, const fVector3& D)
{
	VERIFY(m_magazine.size( ));

	CCartridge& l_cartridge = m_magazine.back( );
//	Msg("ammo - %s", l_cartridge.m_ammoSect.c_str());
	VERIFY(u16(-1) != l_cartridge.bullet_material_idx);
	//-------------------------------------------------------------	
	l_cartridge.m_flags.set(CCartridge::cfTracer, (m_bHasTracers & !!l_cartridge.m_flags.test(CCartridge::cfTracer)));
	if (m_u8TracerColorID != u8(-1))
	{
		l_cartridge.m_u8ColorID = m_u8TracerColorID;
	}
	//-------------------------------------------------------------
	//повысить изношенность оружия с учетом влияния конкретного патрона
	ChangeCondition(-GetWeaponDeterioration( ) * l_cartridge.m_impair);

	f32 fire_disp = GetFireDispersion(true);

	bool SendHit = SendHitAllowed(H_Parent( ));
	//выстерлить пулю (с учетом возможной стрельбы дробью)
	for (s32 i = 0; i < l_cartridge.m_buckShot; ++i)
	{
		FireBullet(P, D, fire_disp, l_cartridge, H_Parent( )->ID( ), ID( ), SendHit);
	}

	StartShotParticles( );

	if (m_bLightShotEnabled)
	{
		Light_Start( );
	}

	// Ammo
	m_magazine.pop_back( );
	--iAmmoElapsed;

	//проверить не произошла ли осечка
	CheckForMisfire( );

	VERIFY((u32)iAmmoElapsed == m_magazine.size( ));
}

void CWeapon::Fire2Start( )
{
	bWorking2 = true;
}

void CWeapon::Fire2End( )
{
	//принудительно останавливать зацикленные партиклы
	if (m_pFlameParticles2 && m_pFlameParticles2->IsLooped( ))
	{
		StopFlameParticles2( );
	}

	bWorking2 = false;
}

void CWeapon::StopShooting( )
{
	m_bPending = true;

	//принудительно останавливать зацикленные партиклы
	if (m_pFlameParticles && m_pFlameParticles->IsLooped( ))
	{
		StopFlameParticles( );
	}

	SwitchState(eIdle);

	bWorking = false;
}

void CWeapon::FireEnd( )
{
	CShootingObject::FireEnd( );
	ClearShotEffector( );
}

void CWeapon::StartFlameParticles2( )
{
	CShootingObject::StartParticles(m_pFlameParticles2, *m_sFlameParticles2, get_LastFP2( ));
}

void CWeapon::StopFlameParticles2( )
{
	CShootingObject::StopParticles(m_pFlameParticles2);
}

void CWeapon::UpdateFlameParticles2( )
{
	if (m_pFlameParticles2)
	{
		CShootingObject::UpdateParticles(m_pFlameParticles2, get_LastFP2( ));
	}
}
