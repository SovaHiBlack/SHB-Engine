// Weapon.h: interface for the CWeapon class.
#pragma once

#include "PhysicsShell.h"
#include "WeaponAmmo.h"
#include "PHShellCreator.h"

#include "ShootingObject.h"
#include "HudItemObject.h"
#include "Actor_flags.h"
#include "..\XR_3DA\SkeletonAnimated.h"
#include "game_cl_single.h"

// refs
class CEntity;
class ENGINE_API CMotionDef;
class CSE_ALifeItemWeapon;
class CSE_ALifeItemWeaponAmmo;
class CWeaponMagazined;
class CParticlesObject;
class CUIStaticItem;

class CWeapon : public CHudItemObject, public CShootingObject
{
private:
	typedef CHudItemObject inherited;

public:
	CWeapon(pcstr name);
	virtual					~CWeapon( );

	// Generic
	virtual void			Load(pcstr section);

	virtual BOOL			net_Spawn(CSE_Abstract* DC);
	virtual void			net_Destroy( );
	virtual void			net_Export(CNetPacket& P);
	virtual void			net_Import(CNetPacket& P);

	virtual CWeapon* cast_weapon( )
	{
		return this;
	}
	virtual CWeaponMagazined* cast_weapon_magazined( )
	{
		return 0;
	}

	// serialization
	virtual void			save(CNetPacket& output_packet);
	virtual void			load(IReader& input_packet);
	virtual BOOL			net_SaveRelevant( )
	{
		return inherited::net_SaveRelevant( );
	}

	virtual void			UpdateCL( );
	virtual void			shedule_Update(u32 dt);

	virtual void			renderable_Render( );
	virtual void			OnDrawUI( );

	virtual void			OnH_B_Chield( );
	virtual void			OnH_A_Chield( );
	virtual void			OnH_B_Independent(bool just_before_destroy);
	virtual void			OnH_A_Independent( );
	virtual void			OnEvent(CNetPacket& P, u16 type);// {inherited::OnEvent(P,type);}

	virtual void			Hit(SHit* pHDS);

	virtual void			reinit( );
	virtual void			reload(pcstr section);
	virtual void			create_physic_shell( );
	virtual void			activate_physic_shell( );
	virtual void			setup_physic_shell( );

	virtual void			SwitchState(u32 S);
	virtual bool			Activate( );

	virtual void			Hide( );
	virtual void			Show( );

	// инициализация если вещь в активном слоте или спрятана на OnH_B_Chield
	virtual void			OnActiveItem( );
	virtual void			OnHiddenItem( );

	//////////////////////////////////////////////////////////////////////////
	//  Network
	//////////////////////////////////////////////////////////////////////////

public:
	virtual bool			can_kill( ) const;
	virtual CInventoryItem* can_kill(CInventory* inventory) const;
	virtual const CInventoryItem* can_kill(const xr_vector<const CGameObject*>& items) const;
	virtual bool			ready_to_kill( ) const;
	virtual bool			NeedToDestroyObject( ) const;
	virtual ALife::_TIME_ID	TimePassedAfterIndependant( ) const;

protected:
	// время удаления оружия
	ALife::_TIME_ID			m_dwWeaponRemoveTime;
	ALife::_TIME_ID			m_dwWeaponIndependencyTime;

	//////////////////////////////////////////////////////////////////////////
	//  Animation
	//////////////////////////////////////////////////////////////////////////
public:
	void					signal_HideComplete( );

//////////////////////////////////////////////////////////////////////////
//  InventoryItem methods
//////////////////////////////////////////////////////////////////////////
public:
	virtual bool			Action(s32 cmd, u32 flags);

	//////////////////////////////////////////////////////////////////////////
	//  Weapon state
	//////////////////////////////////////////////////////////////////////////
public:
	enum EWeaponStates
	{
		eIdle = 0,
		eFire,
		eFire2,
		eReload,
		eShowing,
		eHiding,
		eHidden,
		eMisfire,
		eMagEmpty,
		eSwitch
	};
	enum EWeaponSubStates
	{
		eSubstateReloadBegin = 0,
		eSubstateReloadInProcess,
		eSubstateReloadEnd
	};

	virtual bool			IsHidden( ) const
	{	// Does weapon is in hidden state
		return (GetState( ) == eHidden);
	}
	virtual bool			IsHiding( ) const
	{
		return (GetState( ) == eHiding);
	}
	virtual bool			IsShowing( ) const
	{
		return (GetState( ) == eShowing);
	}

	IC BOOL					IsValid( ) const
	{
		return iAmmoElapsed;
	}
	// Does weapon need's update?
	BOOL					IsUpdating( );


	BOOL					IsMisfire( ) const;
	BOOL					CheckForMisfire( );


	BOOL					AutoSpawnAmmo( ) const
	{
		return m_bAutoSpawnAmmo;
	}
	bool					IsTriStateReload( ) const
	{
		return m_bTriStateReload;
	}
	EWeaponSubStates		GetReloadState( ) const
	{
		return (EWeaponSubStates)m_sub_state;
	}

protected:
	bool					m_bTriStateReload;
	u8						m_sub_state;
	// Weapon fires now
	bool					bWorking2;
	// a misfire happens, you'll need to rearm weapon
	bool					bMisfire;

	BOOL					m_bAutoSpawnAmmo;
	//////////////////////////////////////////////////////////////////////////
	//  Weapon Addons
	//////////////////////////////////////////////////////////////////////////
public:
	///////////////////////////////////////////
	// работа с аддонами к оружию
	//////////////////////////////////////////

	bool IsGrenadeLauncherAttached( ) const;
	bool IsScopeAttached( ) const;
	bool IsSilencerAttached( ) const;

	virtual bool GrenadeLauncherAttachable( );
	virtual bool ScopeAttachable( );
	virtual bool SilencerAttachable( );
	virtual bool UseScopeTexture( )
	{
		return true;
	}

	//обновление видимости для косточек аддонов
	void UpdateAddonsVisibility( );
	void UpdateHUDAddonsVisibility( );
	//инициализация свойств присоединенных аддонов
	virtual void InitAddons( );

	//для отоброажения иконок апгрейдов в интерфейсе
	s32 GetScopeX( )
	{
		return m_iScopeX;
	}
	s32 GetScopeY( )
	{
		return m_iScopeY;
	}
	s32 GetSilencerX( )
	{
		return m_iSilencerX;
	}
	s32 GetSilencerY( )
	{
		return m_iSilencerY;
	}
	s32 GetGrenadeLauncherX( )
	{
		return m_iGrenadeLauncherX;
	}
	s32 GetGrenadeLauncherY( )
	{
		return m_iGrenadeLauncherY;
	}

	const shared_str& GetGrenadeLauncherName( )
	{
		return m_sGrenadeLauncherName;
	}
	const shared_str& GetScopeName( )
	{
		return m_sScopeName;
	}
	const shared_str& GetSilencerName( )
	{
		return m_sSilencerName;
	}

	u8 GetAddonsState( ) const
	{
		return m_flagsAddOnState;
	}
	void	SetAddonsState(u8 st)
	{	// dont use!!! for buy menu only!!!
		m_flagsAddOnState = st;
	}

protected:
	//состояние подключенных аддонов
	u8 m_flagsAddOnState;

	//возможность подключения различных аддонов
	ALife::EWeaponAddonStatus	m_eScopeStatus;
	ALife::EWeaponAddonStatus	m_eSilencerStatus;
	ALife::EWeaponAddonStatus	m_eGrenadeLauncherStatus;

	//названия секций подключаемых аддонов
	shared_str		m_sScopeName;
	shared_str		m_sSilencerName;
	shared_str		m_sGrenadeLauncherName;

	//смещение иконов апгрейдов в инвентаре
	s32 m_iScopeX;
	s32 m_iScopeY;
	s32 m_iSilencerX;
	s32 m_iSilencerY;
	s32 m_iGrenadeLauncherX;
	s32 m_iGrenadeLauncherY;

	///////////////////////////////////////////////////
	//	для режима приближения и снайперского прицела
	///////////////////////////////////////////////////
protected:
	//разрешение режима приближения
	bool			m_bZoomEnabled;
	//текущий фактор приближения
	f32			m_fZoomFactor;
	//время приближения
	f32			m_fZoomRotateTime;
	//текстура для снайперского прицела, в режиме приближения
	CUIStaticItem* m_UIScope;
	//коэффициент увеличения прицеливания
	f32			m_fIronSightZoomFactor;
	//коэффициент увеличения прицела
	f32			m_fScopeZoomFactor;
	//когда режим приближения включен
	bool			m_bZoomMode;
	//от 0 до 1, показывает насколько процентов
	//мы перемещаем HUD
	f32			m_fZoomRotationFactor;
	bool			m_bHideCrosshairInZoom;

public:
	IC bool					IsZoomEnabled( ) const
	{
		return m_bZoomEnabled;
	}
	virtual void			ZoomInc( )
	{ }
	virtual void			ZoomDec( )
	{ }
	virtual void			OnZoomIn( );
	virtual void			OnZoomOut( );
	bool			IsZoomed( ) const
	{
		return m_bZoomMode;
	}
	CUIStaticItem* ZoomTexture( );
	bool			ZoomHideCrosshair( )
	{
		return (m_bHideCrosshairInZoom || ZoomTexture( ));
	}

	IC f32				GetZoomFactor( ) const
	{
		return m_fZoomFactor;
	}
	virtual	f32			CurrentZoomFactor( );
	//показывает, что оружие находится в соостоянии поворота для приближенного прицеливания
	bool			IsRotatingToZoom( ) const
	{
		return (m_fZoomRotationFactor < 1.0f);
	}

	void			LoadZoomOffset(pcstr section, pcstr prefix);

	virtual f32				Weight( );

public:
	virtual EHandDependence		HandDependence( ) const
	{
		return eHandDependence;
	}
	bool				IsSingleHanded( ) const
	{
		return m_bIsSingleHanded;
	}

public:
	IC pcstr			strap_bone0( ) const
	{
		return m_strap_bone0;
	}
	IC pcstr			strap_bone1( ) const
	{
		return m_strap_bone1;
	}
	IC void			strapped_mode(bool value)
	{
		m_strapped_mode = value;
	}
	IC bool			strapped_mode( ) const
	{
		return m_strapped_mode;
	}

protected:
	pcstr					m_strap_bone0;
	pcstr					m_strap_bone1;
	fMatrix4x4					m_StrapOffset;
	bool					m_strapped_mode;
	bool					m_can_be_strapped;

	fMatrix4x4					m_Offset;
	// 0-используется без участия рук, 1-одна рука, 2-две руки
	EHandDependence			eHandDependence;
	bool					m_bIsSingleHanded;

public:
	//загружаемые параметры
	fVector3					vLoadedFirePoint;
	fVector3					vLoadedFirePoint2;

private:
	//текущее положение и напрвление для партиклов
	struct					_firedeps
	{
		fMatrix4x4				m_FireParticlesXForm;	//направление для партиклов огня и дыма
		fVector3				vLastFP;	//огня
		fVector3				vLastFP2;	//огня2
		fVector3				vLastFD;	// direction
		fVector3				vLastSP;	//гильз	

		_firedeps( )
		{
			m_FireParticlesXForm.identity( );
			vLastFP.set(0.0f, 0.0f, 0.0f);
			vLastFP2.set(0.0f, 0.0f, 0.0f);
			vLastFD.set(0.0f, 0.0f, 0.0f);
			vLastSP.set(0.0f, 0.0f, 0.0f);
		}
	}						m_firedeps;

protected:
	virtual void			UpdateFireDependencies_internal( );
	virtual void			UpdatePosition(const fMatrix4x4& transform);	//.
	virtual void			UpdateXForm( );
	virtual void			UpdateHudAdditonal(fMatrix4x4&);
	IC void			UpdateFireDependencies( )
	{
		if (dwFP_Frame == Device.dwFrame)
		{
			return;
		}

		UpdateFireDependencies_internal( );
	}

	virtual void			LoadFireParams(pcstr section, pcstr prefix);

public:
	IC const fVector3& get_LastFP( )
	{
		UpdateFireDependencies( );
		return m_firedeps.vLastFP;
	}
	IC const fVector3& get_LastFP2( )
	{
		UpdateFireDependencies( );
		return m_firedeps.vLastFP2;
	}
	IC const fVector3& get_LastFD( )
	{
		UpdateFireDependencies( );
		return m_firedeps.vLastFD;
	}
	IC const fVector3& get_LastSP( )
	{
		UpdateFireDependencies( );
		return m_firedeps.vLastSP;
	}

	virtual const fVector3& get_CurrentFirePoint( )
	{
		return get_LastFP( );
	}
	virtual const fVector3& get_CurrentFirePoint2( )
	{
		return get_LastFP2( );
	}
	virtual const fMatrix4x4& get_ParticlesXFORM( )
	{
		UpdateFireDependencies( );
		return m_firedeps.m_FireParticlesXForm;
	}
	virtual void			ForceUpdateFireParticles( );

	//////////////////////////////////////////////////////////////////////////
	// Weapon fire
	//////////////////////////////////////////////////////////////////////////
protected:
	virtual void			SetDefaults( );

	//трассирование полета пули
	void			FireTrace(const fVector3& P, const fVector3& D);
	virtual f32			GetWeaponDeterioration( );

	virtual void			FireStart( )
	{
		CShootingObject::FireStart( );
	}
	virtual void			FireEnd( );// {CShootingObject::FireEnd();}

	virtual void			Fire2Start( );
	virtual void			Fire2End( );
	virtual void			Reload( );
	void			StopShooting( );

	// обработка визуализации выстрела
	virtual void			OnShot( )
	{ }
	virtual void			AddShotEffector( );
	virtual void			RemoveShotEffector( );
	virtual	void			ClearShotEffector( );

public:
	//текущая дисперсия (в радианах) оружия с учетом используемого патрона
	f32					GetFireDispersion(bool with_cartridge);
	f32					GetFireDispersion(f32 cartridge_k);
	virtual s32				ShotsFired( )
	{
		return 0;
	}

	//параметы оружия в зависимоти от его состояния исправности
	f32					GetConditionDispersionFactor( ) const;
	f32					GetConditionMisfireProbability( ) const;
	virtual	f32			GetConditionToShow( ) const;

public:
	//отдача при стрельбе 
	f32					camMaxAngle;
	f32					camRelaxSpeed;
	f32					camRelaxSpeed_AI;
	f32					camDispersion;
	f32					camDispersionInc;
	f32					camDispertionFrac;
	f32					camMaxAngleHorz;
	f32					camStepAngleHorz;

protected:
	//фактор увеличения дисперсии при максимальной изношености 
	//(на сколько процентов увеличится дисперсия)
	f32					fireDispersionConditionFactor;
	//вероятность осечки при максимальной изношености
	f32					misfireProbability;
	f32					misfireConditionK;
	//увеличение изношености при выстреле
	f32					conditionDecreasePerShot;

	f32					m_fPDM_disp_base;
	f32					m_fPDM_disp_vel_factor;
	f32					m_fPDM_disp_accel_factor;
	f32					m_fPDM_disp_crouch;
	f32					m_fPDM_disp_crouch_no_acc;

protected:
	//для отдачи оружия
	fVector3					m_vRecoilDeltaAngle;

	//для сталкеров, чтоб они знали эффективные границы использования 
	//оружия
	f32					m_fMinRadius;
	f32					m_fMaxRadius;

	//////////////////////////////////////////////////////////////////////////
	// партиклы
	//////////////////////////////////////////////////////////////////////////

protected:
	//для второго ствола
	void			StartFlameParticles2( );
	void			StopFlameParticles2( );
	void			UpdateFlameParticles2( );

protected:
	shared_str					m_sFlameParticles2;
	//объект партиклов для стрельбы из 2-го ствола
	CParticlesObject* m_pFlameParticles2;

	//////////////////////////////////////////////////////////////////////////
	// Weapon and ammo
	//////////////////////////////////////////////////////////////////////////
public:
	IC s32					GetAmmoElapsed( ) const
	{
		return /*int(m_magazine.size())*/iAmmoElapsed;
	}
	IC s32					GetAmmoMagSize( ) const
	{
		return iMagazineSize;
	}
	s32						GetAmmoCurrent(bool use_item_to_spawn = false) const;

	void					SetAmmoElapsed(s32 ammo_count);

	virtual void			OnMagazineEmpty( );
	void			SpawnAmmo(u32 boxCurr = 0xffffffff, pcstr ammoSect = NULL, u32 ParentID = 0xffffffff);

	virtual f32			Get_PDM_Base( ) const
	{
		return m_fPDM_disp_base;
	}
	virtual f32			Get_PDM_Vel_F( ) const
	{
		return m_fPDM_disp_vel_factor;
	}
	virtual f32			Get_PDM_Accel_F( ) const
	{
		return m_fPDM_disp_accel_factor;
	}
	virtual f32			Get_PDM_Crouch( ) const
	{
		return m_fPDM_disp_crouch;
	}
	virtual f32			Get_PDM_Crouch_NA( ) const
	{
		return m_fPDM_disp_crouch_no_acc;
	}

protected:
	s32						iAmmoElapsed;		// ammo in magazine, currently
	s32						iMagazineSize;		// size (in bullets) of magazine

	//для подсчета в GetAmmoCurrent
	mutable s32				iAmmoCurrent;
	mutable u32				m_dwAmmoCurrentCalcFrame;	//кадр на котором просчитали кол-во патронов

	bool					m_bAmmoWasSpawned;

	virtual bool			IsNecessaryItem(const shared_str& item_sect);

public:
	xr_vector<shared_str>	m_ammoTypes;

	CWeaponAmmo* m_pAmmo;
	u32						m_ammoType;
	shared_str				m_ammoName;
	BOOL					m_bHasTracers;
	u8						m_u8TracerColorID;
	u32						m_set_next_ammoType_on_reload;
	// Multitype ammo support
	xr_vector<CCartridge>	m_magazine;
	CCartridge				m_DefaultCartridge;
	f32					m_fCurrentCartirdgeDisp;

	bool				unlimited_ammo( );
	IC	bool				can_be_strapped( ) const
	{
		return m_can_be_strapped;
	}

	pcstr					GetCurrentAmmo_ShortName( );

protected:
	u32						m_ef_main_weapon_type;
	u32						m_ef_weapon_type;

public:
	virtual u32				ef_main_weapon_type( ) const;
	virtual u32				ef_weapon_type( ) const;

protected:
	// This is because when scope is attached we can't ask scope for these params
	// therefore we should hold them by ourself :-((
	f32					m_addon_holder_range_modifier;
	f32					m_addon_holder_fov_modifier;

public:
	virtual void			modify_holder_params(f32& range, f32& fov) const;
	virtual bool			use_crosshair( )	const
	{
		return true;
	}
	bool			show_crosshair( );
	bool			show_indicators( );
	virtual BOOL			ParentMayHaveAimBullet( );
	virtual BOOL			ParentIsActor( );

private:
	f32					m_hit_probability[egdCount];

public:
	const f32& hit_probability( ) const;
};
