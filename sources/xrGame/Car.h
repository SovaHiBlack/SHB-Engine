#pragma once

#include "entity.h"
#include "PHDynamicData.h"
#include "PhysicsShell.h"
#include "script_entity.h"
#include "CarLights.h"
#include "phobject.h"
#include "holder_custom.h"
#include "PHSkeleton.h"
#include "DamagableItem.h"
#include "phcollisiondamagereceiver.h"
#include "CarDamageParticles.h"
#include "xrserver_objects_alife.h"
#include "HitImmunity.h"
#include "Explosive.h"
#include "PHDestroyable.h"
#include "DelayedActionFuse.h"

// refs
class ENGINE_API			CBoneInstance;
class						CActor;
class						CInventory;
class						CSE_PHSkeleton;
class						CCarWeapon;
struct						dxGeomUserData;
struct						dSurfaceParameters;
// defs

#ifdef DEBUG
#include "..\XR_3DA\StatGraph.h"
#include "PHDebug.h"
#endif

class CScriptEntityAction;
class car_memory;

class CCar :
	public CEntity,
	public CScriptEntity,
	public CPHUpdateObject,
	public CHolderCustom,
	public CPHSkeleton,
	public CDamagableItem,
	public CPHDestroyable,
	public CPHCollisionDamageReceiver,
	public CHitImmunity,
	public CExplosive,
	public CDelayedActionFuse
{
private:
	collide::rq_results		RQR;

#ifdef DEBUG
	CFunctionGraph 					m_dbg_power_rpm;
	CFunctionGraph 					m_dbg_torque_rpm;
	CStatGraph* m_dbg_dynamic_plot;
	bool							b_plots;
	f32 _stdcall			TorqueRpmFun(f32 rpm)
	{
		return Parabola(rpm) / rpm;
	}
	void 					InitDebug( );
	void 					DbgSheduleUpdate( );
	void 					DbgUbdateCl( );
	void 					DbgCreatePlots( );
	void 					DBgClearPlots( );
#endif
////////////////////////////////////////////////////////////////////	
	flags16						async_calls;
	static	const u16				cAsCallsnum = 3;
	enum EAsyncCalls
	{
		ascSndTransmission = 1 << 0,
		ascSndStall = 1 << 1,
		ascExhoustStop = 1 << 2,
		ascLast = 1 << cAsCallsnum
	};
	void					ASCUpdate( );
	void					ASCUpdate(EAsyncCalls c);
	void					AscCall(EAsyncCalls c);
////////////////////////////////////////////////////////////////////////////////////////
	virtual bool						CanRemoveObject( );
////////////////////////////////////////////////////////////////////////
	static	BONE_P_MAP					bone_map;					//interface for PhysicsShell
	static	void 						ActorObstacleCallback(bool& do_colide, bool bo1, dContact& c, SGameMtl* material_1, SGameMtl* material_2);
	virtual void						PhDataUpdate(dReal step);
	virtual void						PhTune(dReal step);
/////////////////////////////////////////////////////////////////////////
	virtual void						ApplyDamage(u16 level);
	virtual	f32						Health( )
	{
		return GetfHealth( );
	}
	virtual void						ChangeCondition(f32 fDeltaCondition);
	virtual void						StartTimerEffects( )
	{ };
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual CPhysicsShellHolder* PPhysicsShellHolder( )
	{
		return static_cast<CPhysicsShellHolder*>(this);
	}
	virtual CPHCollisionDamageReceiver* PHCollisionDamageReceiver( )
	{
		return static_cast<CPHCollisionDamageReceiver*>(this);
	}

////////////////////////////////////////////////////////////////////////
	CCarDamageParticles					m_damage_particles;
///////////////////////////////////////////////////////////////////////

protected:
	enum ECarCamType
	{
		ectFirst = 0,
		ectChase,
		ectFree
	};

public:
	bool rsp, lsp, fwp, bkp, brp;
	fMatrix4x4 m_root_transform;
	fVector3 m_exit_position;

	enum eStateDrive
	{
		drive,
		neutral
	};

	eStateDrive e_state_drive;

	enum eStateSteer
	{
		right,
		idle,
		left
	};

	eStateSteer e_state_steer;

	bool b_wheels_limited;
	bool b_engine_on;
	bool b_clutch;
	bool b_starting;
	bool b_stalling;
	bool b_breaks;
	bool b_transmission_switching;

	u32	 m_dwStartTime;
	f32 m_fuel;
	f32 m_fuel_tank;
	f32 m_fuel_consumption;
	u16	  m_driver_anim_type;

	f32   m_break_start;
	f32	m_break_time;
	f32	m_breaks_to_back_rate;
	f32	m_power_neutral_factor;//multiplier for power when accelerator is not pressed (0-1,0.25)
	bool	b_exploded;

	struct SWheel :
		public CDamagableHealthItem
	{
		typedef		CDamagableHealthItem inherited;
		u16									bone_id;
		bool								inited;
		f32								radius;
		CPhysicsJoint* joint;
		CCar* car;
		struct			SWheelCollisionParams
		{
			f32							spring_factor;
			f32							damping_factor;
			f32							mu_factor;
			SWheelCollisionParams( );
		}									collision_params;

		IC	static	void			applywheelCollisionParams(const dxGeomUserData* ud, bool& do_colide, dContact& c, SGameMtl* material_1, SGameMtl* material_2);
		static	void			WheellCollisionCallback(bool& do_colide, bool bo1, dContact& c, SGameMtl* material_1, SGameMtl* material_2);

		void 			Init( );//asumptions: bone_map is 1. ini parsed 2. filled in 3. bone_id is set 
		void			Load(pcstr section);
		void 			RestoreNetState(const CSE_ALifeCar::SWheelState& a_state);
		void 			SaveNetState(CNetPacket& P);
		void 			ApplyDriveAxisVel(f32 vel);
		void 			ApplyDriveAxisTorque(f32 torque);
		void 			ApplyDriveAxisVelTorque(f32 vel, f32 torque);
		void 			ApplySteerAxisVel(f32 vel);
		void 			ApplySteerAxisTorque(f32 torque);
		void 			ApplySteerAxisVelTorque(f32 vel, f32 torque);
		void 			SetSteerLoLimit(f32 lo);
		void			SetSteerHiLimit(f32 hi);
		void			SetSteerLimits(f32 hi, f32 lo);

		virtual void ApplyDamage(u16 level);
		SWheel(CCar* acar)
		{
			bone_id = BI_NONE;
			car = acar;
			joint = NULL;
			inited = false;
		}
	};
	struct SWheelDrive
	{
		SWheel* pwheel;
		f32	pos_fvd;
		f32	gear_factor;
		void	Init( );
		void	Drive( );
		void	Neutral( );
		void	UpdatePower( );
		f32	ASpeed( );
		void	Load(pcstr /*section*/)
		{ };
	};
	struct SWheelSteer
	{
		SWheel* pwheel;
		f32 pos_right;
		f32 lo_limit;
		f32 hi_limit;
		f32 steering_velocity;
		f32 steering_torque;
		bool  limited;			//zero limited for idle steering drive
		f32 GetSteerAngle( )
		{
			return -pos_right * dJointGetHinge2Angle1(pwheel->joint->GetDJoint( ));
		}
		void	 Init( );
		void	 SteerRight( );
		void	 SteerLeft( );
		void	 SteerIdle( );
		void	 Limit( );
		void	 Load(pcstr /*section*/)
		{ };
	};
	struct SWheelBreak
	{
		SWheel* pwheel;
		f32			break_torque;
		f32			hand_break_torque;
		void		 Init( );
		void		 Break(f32 k);
		void		 HandBreak( );
		void		 Neutral( );
		void		 Load(pcstr section);
	};

	struct SExhaust
	{
		u16					bone_id;
		fMatrix4x4				transform;
		//fVector3				velocity;
		CParticlesObject* p_pgobject;
		CPhysicsElement* pelement;
		CCar* pcar;
		void Init( );
		void Play( );
		void Stop( );
		void Update( );
		void Clear( );
		SExhaust(CCar* acar)
		{
			bone_id = BI_NONE;
			pcar = acar;
			p_pgobject = NULL;
			pelement = NULL;
		}
		~SExhaust( );
	};

	struct SDoor;
	struct SDoor :
		public CDamagableHealthItem
	{
		typedef CDamagableHealthItem inherited;
		u16 bone_id;
		CCar* pcar;
		bool  update;
		CPhysicsJoint* joint;
		f32			torque;
		f32			a_vel;
		f32			pos_open;
		f32			opened_angle;
		f32			closed_angle;
		u32				open_time;
		struct SDoorway
		{
			fVector2		door_plane_ext;
			_vector2<s32>	door_plane_axes;
			SDoor* door;
			SDoorway( );
			void SPass( );
			void Init(SDoor* adoor);
			void Trace(const fVector3& point, const fVector3& dir);
		};
		fVector2		door_plane_ext;
		_vector2<s32>	door_plane_axes;
		fVector3			door_dir_in_door;
		fMatrix4x4			closed_door_form_in_object;
		void Use( );
		void Switch( );
		void Init( );
		void Open( );
		void Close( );
		void Break( );
		virtual void ApplyDamage(u16 level);
		void Update( );
		f32 GetAngle( );
		bool CanEnter(const fVector3& pos, const fVector3& dir, const fVector3& foot_pos);
		bool IsInArea(const fVector3& pos, const fVector3& dir);
		bool IsFront(const fVector3& pos, const fVector3& dir);
		bool CanExit(const fVector3& pos, const fVector3& dir);
		bool TestPass(const fVector3& pos, const fVector3& dir);
		//bool TestPass1(const fVector3& pos,const fVector3& dir);
		void GetExitPosition(fVector3& pos);
		void ApplyOpenTorque( );
		void ApplyTorque(f32 atorque, f32 aa_vel);
		void ApplyCloseTorque( );
		void NeutralTorque(f32 atorque);
		void ClosingToClosed( );
		void ClosedToOpening( );
		void PlaceInUpdate( );
		void RemoveFromUpdate( );
		void SaveNetState(CNetPacket& P);
		void RestoreNetState(const CSE_ALifeCar::SDoorState& a_state);
		void SetDefaultNetState( );
		enum eState
		{
			opening,
			closing,
			opened,
			closed,
			broken
		};
		eState state;
		SDoor(CCar* acar)
		{
			bone_id = BI_NONE;
			pcar = acar;
			joint = NULL;
			state = closed;
			torque = 500.f;
			a_vel = M_PI;
		}
	};

	struct SCarSound
	{
		ref_sound					snd_engine;
		ref_sound					snd_engine_start;
		ref_sound					snd_engine_stop;
		ref_sound					snd_transmission;

		enum ESoundState
		{
			sndOff,
			sndStalling,
			sndStoping,
			sndStarting,
			sndDrive
		} eCarSound;
		void	Update( );
		void	UpdateStarting( );
		void	UpdateStoping( );
		void	UpdateStalling( );
		void	UpdateDrive( );
		void	SwitchState(ESoundState new_state);
		void	SetSoundPosition(ref_sound& snd);
		void	SwitchOff( );
		void	SwitchOn( );
		void	Init( );
		void	Destroy( );
		void	Start( );
		void	Stop( );
		void	Stall( );
		void	Drive( );
		void	TransmissionSwitch( );

		SCarSound(CCar* car);
		~SCarSound( );
		fVector3	relative_pos;
		f32	volume;
		u32		engine_start_delay;//snd_engine starts after engine_start_delay ms by snd_engine_start
		u32		time_state_start;
		CCar* pcar;
	} *m_car_sound;

private:
	typedef CEntity			inherited;
private:
	CCarWeapon* m_car_weapon;
	f32					m_steer_angle;
	bool					m_repairing;
	u16						m_bone_steer;
	CCameraBase* camera[3];
	CCameraBase* active_camera;

	fVector3					m_camera_position;

	////////////////////////////////////////////////////
	friend struct SWheel;
	friend struct SDoor;

	xr_map   <u16, SWheel>	m_wheels_map;
	xr_vector <SWheelDrive> m_driving_wheels;
	xr_vector <SWheelSteer> m_steering_wheels;
	xr_vector <SWheelBreak> m_breaking_wheels;
	xr_vector <SExhaust>	m_exhausts;
	shared_str				m_exhaust_particles;
	xr_map	  <u16, SDoor>	m_doors;
	xr_vector <SDoor*>		m_doors_update;
	xr_vector <fVector3>		m_gear_ratious;
	xr_vector <fMatrix4x4>		m_sits_transforms;// m_sits_transforms[0] - driver_place
	f32					m_current_gear_ratio;

	/////////////////////////////////////////////////////////////
	bool					b_auto_switch_transmission;

	/////////////////////////////////////////////////////////////
	f32					m_doors_torque_factor;
	/////////////////////////////////////////////////////////////

	f32					m_max_power;//best rpm
	f32					m_power_increment_factor;
	f32					m_power_decrement_factor;
	f32					m_rpm_increment_factor;
	f32					m_rpm_decrement_factor;
	/////////////////////porabola
	f32 m_a;
	f32 m_b;
	f32 m_c;

	f32					m_current_engine_power;
	f32					m_current_rpm;

	f32					m_axle_friction;

	f32					m_fSaveMaxRPM;
	f32					m_max_rpm;
	f32					m_min_rpm;
	f32					m_power_rpm;//max power
	f32					m_torque_rpm;//max torque

	f32					m_steering_speed;
	f32					m_ref_radius;
	size_t					m_current_transmission_num;
	///////////////////////////////////////////////////
	CCarLights				m_lights;
	////////////////////////////////////////////////////
	/////////////////////////////////////////////////
	void				InitParabola( );
	f32	_stdcall	Parabola(f32 rpm);

	void				 LimitWheels( );
	void				 Drive( );
	void				 Starter( );
	void				 StartEngine( );
	void				 StopEngine( );
	void				 Stall( );
	void				 Clutch( );
	void				 Unclutch( );
	void				 SwitchEngine( );
	void				 NeutralDrive( );
	void				 UpdatePower( );
	void				 ReleasePedals( );
	void				 ResetKeys( );

	////////////////////////////////////////////////////////////////////////////
	f32				RefWheelMaxSpeed( );
	f32				EngineCurTorque( );
	f32				RefWheelCurTorque( );
	f32	 			EnginePower( );
	f32	 			EngineDriveSpeed( );
	f32	 			DriveWheelsMeanAngleRate( );
	IC	f32	 			EngineRpmFromWheels( )
	{
		return dFabs(DriveWheelsMeanAngleRate( ) * m_current_gear_ratio);
	}
/////////////////////////////////////////////////////////////////////////	
	void				SteerRight( );
	void				SteerLeft( );
	void				SteerIdle( );
	void				Transmission(size_t num);
	void				CircleSwitchTransmission( );
	void				TransmissionUp( );
	void				TransmissionDown( );
	IC	size_t				CurrentTransmission( )
	{
		return m_current_transmission_num;
	}
	void				PressRight( );
	void				PressLeft( );
	void				PressForward( );
	void				PressBack( );
	void				PressBreaks( );

	void				ReleaseRight( );
	void				ReleaseLeft( );
	void				ReleaseForward( );
	void				ReleaseBack( );
	void				ReleaseBreaks( );
	void				Revert( );
	f32				EffectiveGravity( );
	f32				AntiGravityAccel( );
	f32				GravityFactorImpulse( );
	void StartBreaking( );
	void StopBreaking( );
	void UpdateBack( );

	void HandBreak( );
	void ReleaseHandBreak( );
	void DriveForward( );
	void DriveBack( );
	void ParseDefinitions( );
	void CreateSkeleton(CSE_Abstract* po);//creates m_pPhysicsShell
	void Init( );

	void PlayExhausts( );
	void StopExhausts( );
	void UpdateExhausts( );
	void ClearExhausts( );
	void UpdateFuel(f32 time_delta);
	f32 AddFuel(f32 ammount); //ammount - fuel to load, ret - fuel loaded
	void CarExplode( );
	////////////////////////////////////////////		////////

	void					OnCameraChange(s32 type);

	bool					HUDview( )
	{
		return IsFocused( );
	}

	static void				cb_Steer(CBoneInstance* B);
	virtual	void			Hit(SHit* pHDS);
	virtual void			Die(CObject* who);
	virtual void PHHit(f32 P, fVector3& dir, CObject* who, s16 element, fVector3 p_in_object_space, f32 impulse, ALife::EHitType hit_type/* =ALife::eHitTypeWound */);
	bool WheelHit(f32 P, s16 element, ALife::EHitType hit_type);
	bool DoorHit(f32 P, s16 element, ALife::EHitType hit_type);
public:
	virtual bool			allowWeapon( ) const
	{
		return true;
	};
	virtual bool			HUDView( ) const;
	virtual fVector3			ExitPosition( )
	{
		return m_exit_position;
	}
	virtual fVector3			ExitVelocity( );
	void					GetVelocity(fVector3& vel)
	{
		m_pPhysicsShell->get_LinearVel(vel);
	}
	void					cam_Update(f32 dt, f32 fov);
	void					detach_Actor( );
	bool					attach_Actor(CGameObject* actor);
	bool					is_Door(u16 id, xr_map<u16, SDoor>::iterator& i);
	bool					is_Door(u16 id);
	bool					DoorOpen(u16 id);
	bool					DoorClose(u16 id);
	bool					DoorUse(u16 id);
	bool					DoorSwitch(u16 id);
	bool					Enter(const fVector3& pos, const fVector3& dir, const fVector3& foot_pos);
	bool					Exit(const fVector3& pos, const fVector3& dir);
	bool					Use(const fVector3& pos, const fVector3& dir, const fVector3& foot_pos);
	u16						DriverAnimationType( );
	// Core events
	virtual DLL_Pure* _construct( );
	virtual void			Load(pcstr section);
	virtual BOOL			net_Spawn(CSE_Abstract* DC);
	virtual void			net_Destroy( );
	virtual void			UpdateCL( );
	virtual	void			UpdateEx(f32 fov); //called by owner

	virtual void			shedule_Update(u32 dt);
	virtual void			renderable_Render( );
	virtual	bool			bfAssignMovement(CScriptEntityAction* tpEntityAction);
	virtual	bool			bfAssignObject(CScriptEntityAction* tpEntityAction);

	// Network
	virtual void			net_Export(CNetPacket& P);				// export to server
	virtual void			net_Import(CNetPacket& P);				// import from server
	virtual BOOL			net_Relevant( )
	{
		return getLocal( );
	};		// relevant for export to server
	virtual BOOL			UsedAI_Locations( );
	virtual	void			net_Relcase(CObject* O);
	// Input
	virtual void			OnMouseMove(s32 x, s32 y);
	virtual void			OnKeyboardPress(s32 dik);
	virtual void			OnKeyboardRelease(s32 dik);
	virtual void			OnKeyboardHold(s32 dik);
	virtual void			vfProcessInputKey(s32 iCommand, bool bPressed);
	virtual void			OnEvent(CNetPacket& P, u16 type);
	virtual void			OnAfterExplosion( );
	virtual void			OnBeforeExplosion( );
	virtual void			GetRayExplosionSourcePos(fVector3& pos);
	virtual void			ActivateExplosionBox(const fVector3& size, fVector3& in_out_pos)
	{ };
	virtual void			ResetScriptData(pvoid P = 0);

	virtual void			Action(s32 id, u32 flags);
	virtual void			SetParam(s32 id, fVector2 val);
	virtual void			SetParam(s32 id, fVector3 val);
	bool			HasWeapon( );
	bool			WpnCanHit( );
	f32				FireDirDiff( );
	bool			isObjectVisible(CScriptGameObject* O);
	fVector3			CurrentVel( );
	virtual f32				GetfHealth( ) const
	{
		return CEntity::GetfHealth( );
	}
	virtual f32				SetfHealth(f32 value)
	{
		return CEntity::SetfHealth(value);
	}

// Hits
	virtual void			HitSignal(f32 /**HitAmount/**/, fVector3& /**local_dir/**/, CObject* /**who/**/, s16 /**element/**/)
	{ }
	virtual void			HitImpulse(f32 /**amount/**/, fVector3& /**vWorldDir/**/, fVector3& /**vLocalDir/**/)
	{ }
	virtual void			g_fireParams(const CHudItem* /**pHudItem/**/, fVector3& /**P/**/, fVector3& /**D/**/)
	{ }
	virtual u16				Initiator( );
	// HUD
	virtual void			OnHUDDraw(CCustomHUD* hud);

	CCameraBase* Camera( )
	{
		return active_camera;
	}
	void					SetExplodeTime(u32 et);
	u32				ExplodeTime( );
// Inventory for the car	
	CInventory* GetInventory( )
	{
		return inventory;
	}
	void						VisualUpdate(f32 fov = 90.0f);

protected:
	virtual void					SpawnInitPhysics(CSE_Abstract* D);
	virtual void					net_Save(CNetPacket& P);
	virtual	BOOL					net_SaveRelevant( );
	void					SaveNetState(CNetPacket& P);
	virtual	void					RestoreNetState(CSE_PHSkeleton* po);
	void					SetDefaultNetState(CSE_PHSkeleton* po);

public:
	CCar( );
	virtual ~CCar( );
	virtual BOOL					AlwaysTheCrow( );

public:
	virtual CEntity* cast_entity( )
	{
		return this;
	}

private:
	template <class T> IC void fill_wheel_vector(pcstr S, xr_vector<T>& type_wheels);
	IC void fill_exhaust_vector(pcstr S, xr_vector<SExhaust>& exhausts);
	IC void fill_doors_map(pcstr S, xr_map<u16, SDoor>& doors);

	//Inventory for the car
	CInventory* inventory;

	virtual	void reinit( );
	virtual	void reload(pcstr section);
	virtual CGameObject* cast_game_object( )
	{
		return this;
	}
	virtual CExplosive* cast_explosive( )
	{
		return this;
	}
	virtual CPhysicsShellHolder* cast_physics_shell_holder( )
	{
		return this;
	}
	virtual CParticlesPlayer* cast_particles_player( )
	{
		return this;
	}
	virtual CScriptEntity* cast_script_entity( )
	{
		return this;
	}
	virtual IDamageSource* cast_IDamageSource( )
	{
		return this;
	}
	virtual CHolderCustom* cast_holder_custom( )
	{
		return this;
	}

private:
	car_memory* m_memory;

public:
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CCar)
#undef script_type_list
#define script_type_list save_type_list(CCar)
