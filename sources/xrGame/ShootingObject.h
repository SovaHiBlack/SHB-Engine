//////////////////////////////////////////////////////////////////////
// ShootingObject.h: интерфейс для семейства стреляющих объектов 
//					 (оружие и осколочные гранаты) 	
//					 обеспечивает набор хитов, звуков рикошетп
//////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "..\XR_3DA\Render.h"

class CCartridge;
class CParticlesObject;
class IRender_Sector;

extern const fVector3 zero_vel;

#define WEAPON_MATERIAL_NAME "objects\\bullet"

class CShootingObject
{
protected:
	CShootingObject( );
	virtual ~CShootingObject( );

	void	reinit( );
	void	reload(pcstr section)
	{ }
	void	Load(pcstr section);

	fVector3		m_vCurrentShootDir;
	fVector3		m_vCurrentShootPos;
	//ID персонажа который иницировал действие
	u16			m_iCurrentParentID;

//////////////////////////////////////////////////////////////////////////
// Fire Params
//////////////////////////////////////////////////////////////////////////
protected:
	virtual void			LoadFireParams(pcstr section, pcstr prefix);
	virtual bool			SendHitAllowed(CObject* pUser);
	virtual void			FireBullet(const fVector3& pos,
									   const fVector3& dir,
									   f32 fire_disp,
									   const CCartridge& cartridge,
									   u16 parent_id,
									   u16 weapon_id,
									   bool send_hit);

	virtual void			FireStart( );
	virtual void			FireEnd( );

public:
	IC BOOL					IsWorking( )	const
	{
		return bWorking;
	}
	virtual BOOL			ParentMayHaveAimBullet( )
	{
		return FALSE;
	}
	virtual BOOL			ParentIsActor( )
	{
		return FALSE;
	}

protected:
	// Weapon fires now
	bool					bWorking;

	f32					fTimeToFire;
	fVector4				fvHitPower;
	//f32					fHitPower;
	f32					fHitImpulse;

	//скорость вылета пули из ствола
	f32					m_fStartBulletSpeed;
	//максимальное расстояние стрельбы
	f32					fireDistance;

	//рассеивание во время стрельбы
	f32					fireDispersionBase;

	//счетчик времени, затрачиваемого на выстрел
	f32					fTime;

protected:
	//для сталкеров, чтоб они знали эффективные границы использования 
	//оружия
	f32					m_fMinRadius;
	f32					m_fMaxRadius;

//////////////////////////////////////////////////////////////////////////
// Lights
//////////////////////////////////////////////////////////////////////////
protected:
	fColor					light_base_color;
	f32					light_base_range;
	fColor					light_build_color;
	f32					light_build_range;
	ref_light				light_render;
	f32					light_var_color;
	f32					light_var_range;
	f32					light_lifetime;
	u32						light_frame;
	f32					light_time;
	//включение подсветки во время выстрела
	bool					m_bLightShotEnabled;

protected:
	void					Light_Create( );
	void					Light_Destroy( );

	void					Light_Start( );
	void					Light_Render(const fVector3& P);

	virtual	void			LoadLights(pcstr section, pcstr prefix);
	virtual void			RenderLight( );
	virtual void			UpdateLight( );
	virtual void			StopLight( );

//////////////////////////////////////////////////////////////////////////
// партикловая система
//////////////////////////////////////////////////////////////////////////
protected:
	//функции родительского объекта
	virtual const fVector3& get_CurrentFirePoint( ) = 0;
	virtual const fMatrix4x4& get_ParticlesXFORM( ) = 0;
	virtual void			ForceUpdateFireParticles( )
	{ }

////////////////////////////////////////////////
//общие функции для работы с партиклами оружия
	virtual void			StartParticles(CParticlesObject*& pParticles, pcstr particles_name, const fVector3& pos, const fVector3& vel = zero_vel, bool auto_remove_flag = false);
	virtual void			StopParticles(CParticlesObject*& pParticles);
	virtual void			UpdateParticles(CParticlesObject*& pParticles, const fVector3& pos, const fVector3& vel = zero_vel);

	virtual	void			LoadShellParticles(pcstr section, pcstr prefix);
	virtual	void			LoadFlameParticles(pcstr section, pcstr prefix);

	////////////////////////////////////////////////
	//спецефические функции для партиклов
	//партиклы огня
	virtual void			StartFlameParticles( );
	virtual void			StopFlameParticles( );
	virtual void			UpdateFlameParticles( );

	//партиклы дыма
	virtual void			StartSmokeParticles(const fVector3& play_pos, const fVector3& parent_vel);

	//партиклы полосы от пули
	virtual void			StartShotParticles( );

	//партиклы гильз
	virtual void			OnShellDrop(const fVector3& play_pos, const fVector3& parent_vel);

protected:
	//имя пратиклов для гильз
	shared_str				m_sShellParticles;

public:
	fVector3				vLoadedShellPoint;
	f32						m_fPredBulletTime;
	f32						m_fTimeToAim;
	BOOL					m_bUseAimBullet;

protected:
	//имя пратиклов для огня
	shared_str				m_sFlameParticlesCurrent;
	//для выстрела 1м и 2м видом стрельбы
	shared_str				m_sFlameParticles;
	//объект партиклов огня
	CParticlesObject* m_pFlameParticles;

	//имя пратиклов для дыма
	shared_str				m_sSmokeParticlesCurrent;
	shared_str				m_sSmokeParticles;

	//имя партиклов следа от пули
	shared_str				m_sShotParticles;
};
