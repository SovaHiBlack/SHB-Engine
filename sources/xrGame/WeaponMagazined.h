#pragma once

#include "Weapon.h"
#include "HudSound.h"
#include "ai_sounds.h"

class ENGINE_API CMotionDef;

//размер очереди считается бесконечность
//заканчиваем стрельбу, только, если кончились патроны
#define WEAPON_ININITE_QUEUE -1

class CWeaponMagazined : public CWeapon
{
private:
	typedef CWeapon inherited;

protected:
	// Media :: sounds
	SHudSound		sndShow;
	SHudSound		sndHide;
	SHudSound		sndShot;
	SHudSound		sndEmptyClick;
	SHudSound		sndReload;
	//звук текущего выстрела
	SHudSound* m_pSndShotCurrent;

	virtual void	StopHUDSounds( );

	//дополнительная информация о глушителе
	pcstr			m_sSilencerFlameParticles;
	pcstr			m_sSilencerSmokeParticles;
	SHudSound		sndSilencerShot;

	ESoundTypes		m_eSoundShow;
	ESoundTypes		m_eSoundHide;
	ESoundTypes		m_eSoundShot;
	ESoundTypes		m_eSoundEmptyClick;
	ESoundTypes		m_eSoundReload;
	struct SWMmotions
	{
		MotionSVec		mhud_idle;
		MotionSVec		mhud_idle_aim;
		MotionSVec		mhud_reload;	//
		MotionSVec		mhud_hide;		//
		MotionSVec		mhud_show;		//
		MotionSVec		mhud_shots;		//
		MotionSVec		mhud_idle_sprint;
	};
	SWMmotions			mhud;

	// General
	//кадр момента пересчета UpdateSounds
	u32				dwUpdateSounds_Frame;

protected:
	virtual void	OnMagazineEmpty( );

	virtual void	switch2_Idle( );
	virtual void	switch2_Fire( );
	virtual void	switch2_Fire2( )
	{ }
	virtual void	switch2_Empty( );
	virtual void	switch2_Reload( );
	virtual void	switch2_Hiding( );
	virtual void	switch2_Hidden( );
	virtual void	switch2_Showing( );

	virtual void	OnShot( );

	virtual void	OnEmptyClick( );

	virtual void	OnAnimationEnd(u32 state);
	virtual void	OnStateSwitch(u32 S);

	virtual void	UpdateSounds( );

	bool			TryReload( );
	bool			TryPlayAnimIdle( );

protected:
	virtual void	ReloadMagazine( );
	void	ApplySilencerKoeffs( );

	virtual void	state_Fire(f32 dt);
	virtual void	state_MagEmpty(f32 dt);
	virtual void	state_Misfire(f32 dt);

public:
	CWeaponMagazined(pcstr name = "AK74", ESoundTypes eSoundType = SOUND_TYPE_WEAPON_SUBMACHINEGUN);
	virtual			~CWeaponMagazined( );

	virtual void	Load(pcstr section);
	virtual CWeaponMagazined* cast_weapon_magazined( )
	{
		return this;
	}

	virtual void	SetDefaults( );
	virtual void	FireStart( );
	virtual void	FireEnd( );
	virtual void	Reload( );


	virtual	void	UpdateCL( );
	virtual void	net_Destroy( );
	virtual void			net_Export(CNetPacket& P);
	virtual void			net_Import(CNetPacket& P);

	virtual void	OnH_A_Chield( );

	virtual bool	Attach(PIItem pIItem, bool b_send_event);
	virtual bool	Detach(pcstr item_section_name, bool b_spawn_item);
	virtual bool	CanAttach(PIItem pIItem);
	virtual bool	CanDetach(pcstr item_section_name);

	virtual void	InitAddons( );

	virtual bool	Action(s32 cmd, u32 flags);
	virtual void	onMovementChanged(ACTOR_DEFS::EMoveCommand cmd);
	bool			IsAmmoAvailable( );
	virtual void	UnloadMagazine(bool spawn_ammo = true);

	virtual void	GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count);

	//////////////////////////////////////////////
	// для стрельбы очередями или одиночными
	//////////////////////////////////////////////
public:
	virtual bool	SwitchMode( );
	virtual bool	SingleShotMode( )
	{
		return (1 == m_iQueueSize);
	}
	virtual void	SetQueueSize(s32 size);
	IC		s32		GetQueueSize( ) const
	{
		return m_iQueueSize;
	}
	virtual bool	StopedAfterQueueFired( )
	{
		return m_bStopedAfterQueueFired;
	}
	virtual void	StopedAfterQueueFired(bool value)
	{
		m_bStopedAfterQueueFired = value;
	}

protected:
	//максимальный размер очереди, которой можно стрельнуть
	s32				m_iQueueSize;
	//количество реально выстреляных патронов
	s32				m_iShotNum;

	//после какого патрона, при непрерывной стрельбе, начинается отдача (сделано из-зи Абакана)
	s32				m_iShootEffectorStart;
	fVector3			m_vStartPos;
	fVector3		m_vStartDir;

	//флаг того, что мы остановились после того как выстреляли
	//ровно столько патронов, сколько было задано в m_iQueueSize
	bool			m_bStopedAfterQueueFired;
	//флаг того, что хотя бы один выстрел мы должны сделать
	//(даже если очень быстро нажали на курок и вызвалось FireEnd)
	bool			m_bFireSingleShot;
	//режимы стрельбы
	bool			m_bHasDifferentFireModes;
	xr_vector<s32>	m_aFireModes;
	s32				m_iCurFireMode;
	string16		m_sCurFireMode;
	s32				m_iPrefferedFireMode;

	//переменная блокирует использование
	//только разных типов патронов
	bool m_bLockType;

	//////////////////////////////////////////////
	// режим приближения
	//////////////////////////////////////////////
public:
	virtual void	OnZoomIn( );
	virtual void	OnZoomOut( );
	virtual void	OnNextFireMode( );
	virtual void	OnPrevFireMode( );
	virtual bool	HasFireModes( )
	{
		return m_bHasDifferentFireModes;
	}
	virtual s32		GetCurrentFireMode( )
	{
		return m_aFireModes[m_iCurFireMode];
	}
	virtual pcstr	GetCurrentFireModeStr( )
	{
		return m_sCurFireMode;
	}

	virtual void	save(CNetPacket& output_packet);
	virtual void	load(IReader& input_packet);

protected:
	virtual bool	AllowFireWhileWorking( )
	{
		return false;
	}

	//виртуальные функции для проигрывания анимации HUD
	virtual void	PlayAnimShow( );
	virtual void	PlayAnimHide( );
	virtual void	PlayAnimReload( );
	virtual void	PlayAnimIdle( );
	virtual void	PlayAnimShoot( );
	virtual void	PlayReloadSound( );

	virtual void	StartIdleAnim( );
	virtual s32		ShotsFired( )
	{
		return m_iShotNum;
	}
	virtual f32	GetWeaponDeterioration( );
};
