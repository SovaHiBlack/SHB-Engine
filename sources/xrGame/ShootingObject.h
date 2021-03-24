// ShootingObject.h: интерфейс дл€ семейства стрел€ющих объектов (оружие и осколочные гранаты) обеспечивает набор хитов, звуков рикошета

#pragma once

#include "alife_space.h"
#include "..\ENGINE\render.h"

class CCartridge;
class CParticlesObject;
class IRender_Sector;

extern const Fvector zero_vel;

#define BULLET_MATERIAL_NAME "objects\\bullet"

class CShootingObject
{
protected:
	CShootingObject( );
	virtual ~CShootingObject( );

	void	reinit( );
	void	reload(const char* section)
	{ }
	void	Load(const char* section);

	Fvector		m_vCurrentShootDir;
	Fvector		m_vCurrentShootPos;
	//ID персонажа который иницировал действие
	u16			m_iCurrentParentID;

//////////////////////////////////////////////////////////////////////////
// Fire Params
//////////////////////////////////////////////////////////////////////////
protected:
	virtual void			LoadFireParams(const char* section, const char* prefix);
	virtual bool			SendHitAllowed(CObject* pUser);
	virtual void			FireBullet(const Fvector& pos,
									   const Fvector& dir,
									   float fire_disp,
									   const CCartridge& cartridge,
									   u16 parent_id,
									   u16 weapon_id,
									   bool send_hit);

	virtual void			FireStart( );
	virtual void			FireEnd( );

public:
	inline BOOL					IsWorking( ) const
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

	float					fTimeToFire;
	Fvector4				fvHitPower;
	//float					fHitPower;
	float					fHitImpulse;

	//скорость вылета пули из ствола
	float					m_fStartBulletSpeed;
	//максимальное рассто€ние стрельбы
	float					fireDistance;

	//рассеивание во врем€ стрельбы
	float					fireDispersionBase;

	//счетчик времени, затрачиваемого на выстрел
	float					fTime;

//////////////////////////////////////////////////////////////////////////
// Lights
//////////////////////////////////////////////////////////////////////////
protected:
	Fcolor					light_base_color;
	float					light_base_range;
	Fcolor					light_build_color;
	float					light_build_range;
	ref_light				light_render;
	float					light_var_color;
	float					light_var_range;
	float					light_lifetime;
	u32						light_frame;
	float					light_time;
	//включение подсветки во врем€ выстрела
	bool					m_bLightShotEnabled;

	void					Light_Create( );
	void					Light_Destroy( );

	void					Light_Start( );
	void					Light_Render(const Fvector& P);

	virtual	void			LoadLights(const char* section, const char* prefix);
	virtual void			RenderLight( );
	virtual void			UpdateLight( );
	virtual void			StopLight( );

//////////////////////////////////////////////////////////////////////////
// партиклова€ система
//////////////////////////////////////////////////////////////////////////
protected:
	//функции родительского объекта
	virtual const Fvector& get_CurrentFirePoint( ) = 0;
	virtual const Fmatrix& get_ParticlesXFORM( ) = 0;
	virtual void			ForceUpdateFireParticles( )
	{ }

////////////////////////////////////////////////
//общие функции дл€ работы с партиклами оружи€
	virtual void			StartParticles(CParticlesObject*& pParticles, const char* particles_name, const Fvector& pos, const Fvector& vel = zero_vel, bool auto_remove_flag = false);
	virtual void			StopParticles(CParticlesObject*& pParticles);
	virtual void			UpdateParticles(CParticlesObject*& pParticles, const Fvector& pos, const  Fvector& vel = zero_vel);

	virtual	void			LoadShellParticles(const char* section, const char* prefix);
	virtual	void			LoadFlameParticles(const char* section, const char* prefix);

	////////////////////////////////////////////////
	//спецефические функции дл€ партиклов
	//партиклы огн€
	virtual void			StartFlameParticles( );
	virtual void			StopFlameParticles( );
	virtual void			UpdateFlameParticles( );

	//партиклы дыма
	virtual void			StartSmokeParticles(const Fvector& play_pos, const Fvector& parent_vel);

	//партиклы полосы от пули
	virtual void			StartShotParticles( );

	//партиклы гильз
	virtual void			OnShellDrop(const Fvector& play_pos, const Fvector& parent_vel);

protected:
	//им€ пратиклов дл€ гильз
	shared_str				m_sShellParticles;

public:
	Fvector					vLoadedShellPoint;
	float					m_fPredBulletTime;
	float					m_fTimeToAim;
	BOOL					m_bUseAimBullet;

protected:
	//им€ пратиклов дл€ огн€
	shared_str				m_sFlameParticlesCurrent;
	//дл€ выстрела 1м и 2м видом стрельбы
	shared_str				m_sFlameParticles;
	//объект партиклов огн€
	CParticlesObject* m_pFlameParticles;

	//им€ пратиклов дл€ дыма
	shared_str				m_sSmokeParticlesCurrent;
	shared_str				m_sSmokeParticles;

	//им€ партиклов следа от пули
	shared_str				m_sShotParticles;
};
