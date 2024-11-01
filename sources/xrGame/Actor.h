#pragma once

#include "..\XR_3DA\feel_touch.h"
#include "..\XR_3DA\feel_sound.h"
#include "..\XR_3DA\iinputreceiver.h"
#include "..\XR_3DA\SkeletonAnimated.h"
#include "actor_flags.h"
#include "actor_defs.h"
#include "entity_alive.h"
#include "PHMovementControl.h"
#include "PhysicsShell.h"
#include "InventoryOwner.h"
#include "..\XR_3DA\StatGraph.h"
#include "PhraseDialogManager.h"
#include "step_manager.h"

using namespace ACTOR_DEFS;

// refs
class CInfoPortion;
struct GAME_NEWS_DATA;
class CActorCondition;
class CCustomOutfit;
class CKnownContactsRegistryWrapper;
class CEncyclopediaRegistryWrapper;
class CGameNewsRegistryWrapper;
class CCharacterPhysicsSupport;
class ENGINE_API CCameraBase;
class ENGINE_API CBoneInstance;
class ENGINE_API CBlend;
class CEffectorBobbing;
class CHolderCustom;
class CUsableScriptObject;
struct SSleepEffector;
class CSleepEffectorPP;
class CInventoryBox;
class CHudItem;
class CArtefact;
struct SActorMotions;
struct SActorVehicleAnims;
class CActorCondition;
class CSndShockEffector;
class CGameTaskManager;
class CCameraShotEffector;
class CActorInputHandler;
class CActorMemory;
class CActorStatisticMgr;
class CLocationManager;

class CActor :
	public CEntityAlive,
	public IInputReceiver,
	public Feel::Touch,
	public CInventoryOwner,
	public CPhraseDialogManager,
	public CStepManager,
	public Feel::Sound

#ifdef DEBUG
	, public pureRender
#endif // def DEBUG

{
	friend class CActorCondition;

private:
	typedef CEntityAlive				inherited;

public:
	CActor( );
	virtual								~CActor( );

public:
	virtual BOOL						AlwaysTheCrow( )
	{
		return TRUE;
	}

	virtual CAttachmentOwner* cast_attachment_owner( )
	{
		return this;
	}
	virtual CInventoryOwner* cast_inventory_owner( )
	{
		return this;
	}
	virtual CActor* cast_actor( )
	{
		return this;
	}
	virtual CGameObject* cast_game_object( )
	{
		return this;
	}
	virtual IInputReceiver* cast_input_receiver( )
	{
		return this;
	}
	virtual CCharacterPhysicsSupport* character_physics_support( )
	{
		return m_pPhysics_support;
	}
	virtual CCharacterPhysicsSupport* character_physics_support( ) const
	{
		return m_pPhysics_support;
	}
	virtual CPHDestroyable* ph_destroyable( );
	CHolderCustom* Holder( )
	{
		return m_holder;
	}

public:
	virtual void						Load(pcstr section);

	virtual void						shedule_Update(u32 T);
	virtual void						UpdateCL( );

	virtual void						OnEvent(NET_Packet& P, u16 type);

	// Render
	virtual void						renderable_Render( );
	virtual BOOL						renderable_ShadowGenerate( );
	virtual void						feel_sound_new(CObject* who, int type, CSound_UserDataPtr user_data, const fVector3& Position, f32 power);
	virtual Feel::Sound* dcast_FeelSound( )
	{
		return this;
	}
	f32														m_snd_noise;

#ifdef DEBUG
	virtual void						OnRender( );
#endif // def DEBUG

	/////////////////////////////////////////////////////////////////
	// Inventory Owner
public:
	//information receive & dialogs
	virtual bool						OnReceiveInfo(shared_str info_id) const;
	virtual void						OnDisableInfo(shared_str info_id) const;

	virtual void						NewPdaContact(CInventoryOwner*);
	virtual void						LostPdaContact(CInventoryOwner*);

protected:
	virtual void						AddEncyclopediaArticle(const CInfoPortion* info_portion) const;
	virtual void						AddGameTask(const CInfoPortion* info_portion) const;

protected:
	struct SDefNewsMsg
	{
		GAME_NEWS_DATA* news_data;
		u32															time;
		bool operator < (const SDefNewsMsg& other) const
		{
			return time > other.time;
		}
	};

	xr_vector<SDefNewsMsg>									m_defferedMessages;
	void						UpdateDefferedMessages( );

public:
	void						AddGameNews_deffered(GAME_NEWS_DATA& news_data, u32 delay);
	virtual void						AddGameNews(GAME_NEWS_DATA& news_data);

protected:
	CGameTaskManager* m_game_task_manager;
	CActorStatisticMgr* m_statistic_manager;

public:
	virtual void						StartTalk(CInventoryOwner* talk_partner);
	virtual void						RunTalkDialog(CInventoryOwner* talk_partner);
	CGameTaskManager& GameTaskManager( ) const
	{
		return *m_game_task_manager;
	}
	CActorStatisticMgr& StatisticMgr( )
	{
		return *m_statistic_manager;
	}
	CEncyclopediaRegistryWrapper* encyclopedia_registry;
	CGameNewsRegistryWrapper* game_news_registry;
	CCharacterPhysicsSupport* m_pPhysics_support;

	virtual pcstr						Name( ) const
	{
		return CInventoryOwner::Name( );
	}

public:
	//PhraseDialogManager
	virtual void						ReceivePhrase(DIALOG_SHARED_PTR& phrase_dialog);
	virtual void						UpdateAvailableDialogs(CPhraseDialogManager* partner);
	virtual void						TryToTalk( );
	bool						OnDialogSoundHandlerStart(CInventoryOwner* inv_owner, pcstr phrase);
	bool						OnDialogSoundHandlerStop(CInventoryOwner* inv_owner);

	virtual void						reinit( );
	virtual void						reload(pcstr section);
	virtual bool						use_bolts( ) const;

	virtual void						OnItemTake(CInventoryItem* inventory_item);

	virtual void						OnItemRuck(CInventoryItem* inventory_item, EItemPlace previous_place);
	virtual void						OnItemBelt(CInventoryItem* inventory_item, EItemPlace previous_place);

	virtual void						OnItemDrop(CInventoryItem* inventory_item);
	virtual void						OnItemDropUpdate( );

	virtual void						OnPlayHeadShotParticle(NET_Packet P);

	virtual void						Die(CObject* who);
	virtual void						Hit(SHit* pHDS);
	virtual void						PHHit(f32 P, fVector3& dir, CObject* who, s16 element, fVector3 p_in_object_space, f32 impulse, ALife::EHitType hit_type);
	virtual void						HitSignal(f32 P, fVector3& vLocalDir, CObject* who, s16 element);
	void						HitSector(CObject* who, CObject* weapon);
	void						HitMark(f32 P, fVector3 dir, CObject* who, s16 element, fVector3 position_in_bone_space, f32 impulse, ALife::EHitType hit_type);

	virtual f32							GetMass( );
	virtual f32							Radius( ) const;
	virtual void						g_PerformDrop( );

	virtual bool						NeedToDestroyObject( ) const;
	virtual ALife::_TIME_ID				TimePassedAfterDeath( ) const;

public:
	//свойства артефактов
	virtual void		UpdateArtefactsOnBelt( );
	virtual void		MoveArtefactBelt(const CArtefact* artefact, bool on_belt);
	virtual f32			HitArtefactsOnBelt(f32 hit_power, ALife::EHitType hit_type);
	const xr_vector<const CArtefact*>& ArtefactsOnBelt( )
	{
		return m_ArtefactsOnBelt;
	}

protected:
	//звук тяжелого дыхания
	ref_sound				m_HeavyBreathSnd;
	ref_sound				m_BloodSnd;

	xr_vector<const CArtefact*> m_ArtefactsOnBelt;

	/////////////////////////////////////////////////////////////////
	// misc properties
protected:
	// Death
	f32						hit_slowmo;
	f32						hit_probability;

	// media
	CSndShockEffector* m_sndShockEffector;
	xr_vector<ref_sound>	sndHit[ALife::eHitTypeMax];
	ref_sound				sndDie[SND_DIE_COUNT];

	f32						m_fLandingTime;
	f32						m_fJumpTime;
	f32						m_fFallTime;
	f32						m_fCamHeightFactor;

	// Dropping
	BOOL					b_DropActivated;
	f32						f_DropPower;

	//random seed для Zoom mode
	s32						m_ZoomRndSeed;
	//random seed для Weapon Effector Shot
	s32						m_ShotRndSeed;

	bool					m_bOutBorder;
	//сохраняет счетчик объектов в feel_touch, для которых необходимо обновлять размер колижена с актером 
	u32						m_feel_touch_characters;
	//разрешения на удаление трупа актера 
	//после того как контролирующий его игрок зареспавнился заново. 
	//устанавливается в game

private:
	void					SwitchOutBorder(bool new_border_state);

public:
	bool					m_bAllowDeathRemove;

	////////////////////////////////////////////////////////
	void					SetZoomRndSeed(s32 Seed = 0);
	s32						GetZoomRndSeed( )
	{
		return m_ZoomRndSeed;
	};
	////////////////////////////////////////////////////////
	void					SetShotRndSeed(s32 Seed = 0);
	s32						GetShotRndSeed( )
	{
		return m_ShotRndSeed;
	};

public:
	void					detach_Vehicle( );
	void					steer_Vehicle(f32 angle);
	void					attach_Vehicle(CHolderCustom* vehicle);

	virtual bool			can_attach(const CInventoryItem* inventory_item) const;

protected:
	CHolderCustom* m_holder;
	u16						m_holderID;
	bool					use_Holder(CHolderCustom* holder);

	bool					use_Vehicle(CHolderCustom* object);
	bool					use_MountedWeapon(CHolderCustom* object);
	void					ActorUse( );

	/////////////////////////////////////////////////////////
	// actor model & animations
	/////////////////////////////////////////////////////////
protected:
	BOOL					m_bAnimTorsoPlayed;
	static void				AnimTorsoPlayCallBack(CBlend* B);

	// Rotation
	SRotation				r_torso;
	f32						r_torso_tgt_roll;
	//положение торса без воздействия эффекта отдачи оружия
	SRotation				unaffected_r_torso;

	//ориентация модели
	f32						r_model_yaw_dest;
	f32						r_model_yaw;			// orientation of model
	f32						r_model_yaw_delta;		// effect on multiple "strafe"+"something"

public:
	SActorMotions* m_anims;
	SActorVehicleAnims* m_vehicle_anims;

	CBlend* m_current_legs_blend;
	CBlend* m_current_torso_blend;
	CBlend* m_current_jump_blend;
	MotionID				m_current_legs;
	MotionID				m_current_torso;
	MotionID				m_current_head;

	// callback на анимации модели актера
	void					SetCallbacks( );
	void					ResetCallbacks( );
	static void				Spin0Callback(CBoneInstance*);
	static void				Spin1Callback(CBoneInstance*);
	static void				ShoulderCallback(CBoneInstance*);
	static void				HeadCallback(CBoneInstance*);
	static void				VehicleHeadCallback(CBoneInstance*);

	virtual const SRotation	Orientation( ) const
	{
		return r_torso;
	}
	SRotation& Orientation( )
	{
		return r_torso;
	}

	void					g_SetAnimation(u32 mstate_rl);
	void					g_SetSprintAnimation(u32 mstate_rl, MotionID& head, MotionID& torso, MotionID& legs);
	//////////////////////////////////////////////////////////////////////////
	// HUD
	//////////////////////////////////////////////////////////////////////////
public:
	virtual void			OnHUDDraw(CCustomHUD* hud);
	BOOL			HUDview( ) const;

	//visiblity 
	virtual f32				ffGetFov( )	const
	{
		return 90.0f;
	}
	virtual f32				ffGetRange( )	const
	{
		return 500.0f;
	}

	//////////////////////////////////////////////////////////////////////////
	// Cameras and effectors
	//////////////////////////////////////////////////////////////////////////
public:
	CCameraManager& Cameras( )
	{
		VERIFY(m_pActorEffector); return *m_pActorEffector;
	}
	IC CCameraBase* cam_Active( )
	{
		return cameras[cam_active];
	}
	IC CCameraBase* cam_FirstEye( )
	{
		return cameras[eacFirstEye];
	}

protected:
	void					cam_Set(EActorCameras style);
	void					cam_Update(f32 dt, f32 fFOV);
	void					camUpdateLadder(f32 dt);
	void					cam_SetLadder( );
	void					cam_UnsetLadder( );
	f32						currentFOV( );

	// Cameras
	CCameraBase* cameras[eacMaxCam];
	EActorCameras			cam_active;
	f32						fPrevCamPos;
	fVector3					vPrevCamDir;
	f32						fCurAVelocity;
	CEffectorBobbing* pCamBobbing;

	void					LoadSleepEffector(pcstr section);
	SSleepEffector* m_pSleepEffector;
	CSleepEffectorPP* m_pSleepEffectorPP;

	//менеджер эффекторов, есть у каждого актрера
	CCameraManager* m_pActorEffector;
	static f32				f_Ladder_cam_limit;
	////////////////////////////////////////////
	// для взаимодействия с другими персонажами 
	// или предметами
	///////////////////////////////////////////
public:
	virtual void			feel_touch_new(CObject* O);
	virtual void			feel_touch_delete(CObject* O);
	virtual BOOL			feel_touch_contact(CObject* O);
	virtual BOOL			feel_touch_on_contact(CObject* O);

	CGameObject* ObjectWeLookingAt( )
	{
		return m_pObjectWeLookingAt;
	}
	CInventoryOwner* PersonWeLookingAt( )
	{
		return m_pPersonWeLookingAt;
	}
	pcstr					GetDefaultActionForObject( )
	{
		return *m_sDefaultObjAction;
	}

protected:
	CUsableScriptObject* m_pUsableObject;
	// Person we're looking at
	CInventoryOwner* m_pPersonWeLookingAt;
	CHolderCustom* m_pVehicleWeLookingAt;
	CGameObject* m_pObjectWeLookingAt;
	CInventoryBox* m_pInvBoxWeLookingAt;

	// Tip for action for object we're looking at
	shared_str				m_sDefaultObjAction;
	shared_str				m_sCharacterUseAction;
	shared_str				m_sDeadCharacterUseAction;
	shared_str				m_sDeadCharacterUseOrDragAction;
	shared_str				m_sCarCharacterUseAction;
	shared_str				m_sInventoryItemUseAction;
	shared_str				m_sInventoryBoxUseAction;

	//режим подбирания предметов
	bool					m_bPickupMode;
	//расстояние подсветки предметов
	f32						m_fPickupInfoRadius;

	void					PickupModeUpdate( );
	void					PickupInfoDraw(CObject* object);
	void					PickupModeUpdate_COD( );

public:
	void					PickupModeOn( );
	void					PickupModeOff( );

	//////////////////////////////////////////////////////////////////////////
	// Motions (передвижения актрера)
	//////////////////////////////////////////////////////////////////////////
public:
	void					g_cl_CheckControls(u32 mstate_wf, fVector3& vControlAccel, f32& Jump, f32 dt);
	void					g_cl_ValidateMState(f32 dt, u32 mstate_wf);
	void					g_cl_Orientate(u32 mstate_rl, f32 dt);
	void					g_sv_Orientate(u32 mstate_rl, f32 dt);
	void					g_Orientate(u32 mstate_rl, f32 dt);
	bool					g_LadderOrient( );
	void					UpdateMotionIcon(u32 mstate_rl);

	bool					CanAccelerate( );
	bool					CanJump( );
	bool					CanMove( );
	f32						CameraHeight( );
	bool					CanSprint( );
	bool					CanRun( );
	void					StopAnyMove( );

	bool					AnyAction( )
	{
		return (mstate_real & mcAnyAction) != 0;
	}

	bool					is_jump( );

protected:
	u32						mstate_wishful;
	u32						mstate_old;
	u32						mstate_real;

	BOOL					m_bJumpKeyPressed;

	f32						m_fWalkAccel;
	f32						m_fJumpSpeed;
	f32						m_fRunFactor;
	f32						m_fRunBackFactor;
	f32						m_fWalkBackFactor;
	f32						m_fCrouchFactor;
	f32						m_fClimbFactor;
	f32						m_fSprintFactor;

	f32						m_fWalk_StrafeFactor;
	f32						m_fRun_StrafeFactor;
	//////////////////////////////////////////////////////////////////////////
	// User input/output
	//////////////////////////////////////////////////////////////////////////
public:
	virtual void			IR_OnMouseMove(int x, int y);
	virtual void			IR_OnKeyboardPress(int dik);
	virtual void			IR_OnKeyboardRelease(int dik);
	virtual void			IR_OnKeyboardHold(int dik);
	virtual void			IR_OnMouseWheel(int direction);
	virtual	f32				GetLookFactor( );

	//////////////////////////////////////////////////////////////////////////
	// Weapon fire control (оружие актрера)
	//////////////////////////////////////////////////////////////////////////
public:
	virtual void						g_WeaponBones(int& L, int& R1, int& R2);
	virtual void						g_fireParams(const CHudItem* pHudItem, fVector3& P, fVector3& D);
	virtual BOOL						g_State(SEntityState& state) const;
	virtual	f32							GetWeaponAccuracy( ) const;
	bool						IsZoomAimingMode( ) const
	{
		return m_bZoomAimingMode;
	}

protected:
	//если актер целится в прицел
	bool								m_bZoomAimingMode;

	//настройки аккуратности стрельбы
	//базовая дисперсия (когда игрок стоит на месте)
	f32									m_fDispBase;
	f32									m_fDispAim;
	//коэффициенты на сколько процентов увеличится базовая дисперсия
	//учитывает скорость актера 
	f32									m_fDispVelFactor;
	//если актер бежит
	f32									m_fDispAccelFactor;
	//если актер сидит
	f32									m_fDispCrouchFactor;
	//crouch+no acceleration
	f32									m_fDispCrouchNoAccelFactor;
	//смещение firepoint относительно default firepoint для бросания болтов и гранат
	fVector3								m_vMissileOffset;

public:
	// Получение, и запись смещения для гранат
	fVector3								GetMissileOffset( ) const;
	void								SetMissileOffset(const fVector3& vNewOffset);

protected:
	//косточки используемые при стрельбе
	int									m_r_hand;
	int									m_l_finger1;
	int									m_r_finger2;
	int									m_head;

	int									m_l_clavicle;
	int									m_r_clavicle;
	int									m_spine2;
	int									m_spine1;
	int									m_spine;
	int									m_neck;

	//////////////////////////////////////////////////////////////////////////
	// Network
	//////////////////////////////////////////////////////////////////////////
	void						ConvState(u32 mstate_rl, string128* buf);

public:
	virtual BOOL						net_Spawn(CSE_Abstract* DC);
	virtual void						net_Export(NET_Packet& P);				// export to server
	virtual void						net_Import(NET_Packet& P);				// import from server
	virtual void						net_Destroy( );
	virtual BOOL						net_Relevant( );								// relevant for export to server
	virtual	void						net_Relcase(CObject* O);					//
	virtual void xr_stdcall				on_requested_spawn(CObject* object);
	//object serialization
	virtual void						save(NET_Packet& output_packet);
	virtual void						load(IReader& input_packet);
	virtual void						net_Save(NET_Packet& P);
	virtual	BOOL						net_SaveRelevant( );

protected:
	xr_deque<net_update>	NET;
	fVector3					NET_SavedAccel;
	net_update				NET_Last;
	BOOL					NET_WasInterpolating;	// previous update was by interpolation or by extrapolation
	u32						NET_Time;				// server time of last update

	//---------------------------------------------
	void					net_Import_Base(NET_Packet& P);
	void					net_Import_Physic(NET_Packet& P);
	void					net_Import_Base_proceed( );
	void					net_Import_Physic_proceed( );
	//---------------------------------------------

////////////////////////////////////////////////////////////////////////////
	virtual bool				can_validate_position_on_spawn( )
	{
		return false;
	}
	///////////////////////////////////////////////////////
	// апдайт с данными физики
	xr_deque<net_update_A>	NET_A;

	//---------------------------------------------
	/// spline coeff /////////////////////
	f32				SCoeff[3][4];			//коэффициэнты для сплайна Бизье
	f32				HCoeff[3][4];			//коэффициэнты для сплайна Эрмита
	fVector3		IPosS;	//
	fVector3		IPosH;	//
	fVector3		IPosL;	//положение актера после интерполяции Бизье, Эрмита, линейной

#ifdef DEBUG
	DEF_DEQUE(VIS_POSITION, fVector3);

	VIS_POSITION	LastPosS;
	VIS_POSITION	LastPosH;
	VIS_POSITION	LastPosL;
#endif // def DEBUG

	SPHNetState				LastState;
	SPHNetState				RecalculatedState;
	SPHNetState				PredictedState;

	InterpData				IStart;
	InterpData				IRec;
	InterpData				IEnd;

	bool					m_bInInterpolation;
	bool					m_bInterpolate;
	u32						m_dwIStartTime;
	u32						m_dwIEndTime;
	u32						m_dwILastUpdateTime;

	//---------------------------------------------
	DEF_DEQUE(PH_STATES, SPHNetState);
	PH_STATES				m_States;
	u16						m_u16NumBones;
	void					net_ExportDeadBody(NET_Packet& P);
	//---------------------------------------------
	void					CalculateInterpolationParams( );
	//---------------------------------------------
	virtual void			make_Interpolation( );

#ifdef DEBUG
	//---------------------------------------------
	virtual void			OnRender_Network( );
	//---------------------------------------------
#endif // def DEBUG

	ref_geom 				hFriendlyIndicator;
	//////////////////////////////////////////////////////////////////////////
	// Actor physics
	//////////////////////////////////////////////////////////////////////////
public:
	void			g_Physics(fVector3& accel, f32 jump, f32 dt);
	virtual void			ForceTransform(const fMatrix4x4& m);
	void			SetPhPosition(const fMatrix4x4& pos);
	virtual void			PH_B_CrPr( ); // actions & operations before physic correction-prediction steps
	virtual void			PH_I_CrPr( ); // actions & operations after correction before prediction steps
	virtual void			PH_A_CrPr( ); // actions & operations after phisic correction-prediction steps
	virtual void			MoveActor(fVector3 NewPos, fVector3 NewDir);

	virtual void			SpawnAmmoForWeapon(CInventoryItem* pIItem);
	virtual void			RemoveAmmoForWeapon(CInventoryItem* pIItem);
	virtual	void			spawn_supplies( );
	virtual bool			human_being( ) const
	{
		return				(true);
	}

	virtual	shared_str		GetDefaultVisualOutfit( ) const
	{
		return m_DefaultVisualOutfit;
	}
	virtual	void			SetDefaultVisualOutfit(shared_str DefaultOutfit)
	{
		m_DefaultVisualOutfit = DefaultOutfit;
	}
	virtual void			UpdateAnimation( )
	{
		g_SetAnimation(mstate_real);
	}

	virtual void			ChangeVisual(shared_str NewVisual);
	virtual void			OnChangeVisual( );

	virtual void			RenderIndicator(fVector3 dpos, f32 r1, f32 r2, ref_shader IndShader);
	virtual void			RenderText(pcstr Text, fVector3 dpos, f32* pdup, u32 color);

	//////////////////////////////////////////////////////////////////////////
	// Controlled Routines
	//////////////////////////////////////////////////////////////////////////

	void			set_input_external_handler(CActorInputHandler* handler);
	bool			input_external_handler_installed( ) const
	{
		return (m_input_external_handler != 0);
	}

	IC		void			lock_accel_for(u32 time)
	{
		m_time_lock_accel = Device.dwTimeGlobal + time;
	}

private:
	CActorInputHandler* m_input_external_handler;
	u32						m_time_lock_accel;

	/////////////////////////////////////////
	// DEBUG INFO
protected:
	CStatGraph* pStatGraph;

	shared_str				m_DefaultVisualOutfit;

	pcstr					invincibility_fire_shield_3rd;
	pcstr					invincibility_fire_shield_1st;
	shared_str				m_sHeadShotParticle;
	u32						last_hit_frame;

#ifdef DEBUG
	friend class CLevelGraph;
#endif // def DEBUG

	fVector3							m_AutoPickUp_AABB;
	fVector3							m_AutoPickUp_AABB_Offset;

	void							Check_for_AutoPickUp( );

public:
	void							SetWeaponHideState(u32 State, bool bSet);
	virtual CCustomOutfit* GetOutfit( ) const;

private:
	CActorCondition* m_entity_condition;

protected:
	virtual	CEntityConditionSimple* create_entity_condition(CEntityConditionSimple* ec);

public:
	IC		CActorCondition& conditions( ) const;
	virtual DLL_Pure* _construct( );
	virtual bool				natural_weapon( ) const
	{
		return false;
	}
	virtual bool				natural_detector( ) const
	{
		return false;
	}
	virtual bool				use_center_to_aim( ) const;

protected:
	u16							m_iLastHitterID;
	u16							m_iLastHittingWeaponID;
	s16							m_s16LastHittedElement;
	fVector3						m_vLastHitDir;
	fVector3						m_vLastHitPos;
	f32							m_fLastHealth;
	bool						m_bWasHitted;
	bool						m_bWasBackStabbed;

	virtual		bool			Check_for_BackStab_Bone(u16 element);

public:
	virtual void				SetHitInfo(CObject* who, CObject* weapon, s16 element, fVector3 Pos, fVector3 Dir);

	virtual	bool				InventoryAllowSprint( );
	virtual void				OnNextWeaponSlot( );
	virtual void				OnPrevWeaponSlot( );

public:
	virtual	void				on_weapon_shot_start(CWeapon* weapon);
	virtual	void				on_weapon_shot_stop(CWeapon* weapon);
	virtual	void				on_weapon_hide(CWeapon* weapon);
	fVector3				weapon_recoil_delta_angle( );
	fVector3				weapon_recoil_last_delta( );

protected:
	virtual	void				update_camera(CCameraShotEffector* effector);
	//step manager
	virtual bool				is_on_ground( );

private:
	CActorMemory* m_memory;

public:
	void				SetActorVisibility(u16 who, f32 value);
	IC		CActorMemory& memory( ) const
	{
		VERIFY(m_memory);
		return *m_memory;
	}

	void						OnDifficultyChanged( );

	IC f32						HitProbability( )
	{
		return hit_probability;
	}
	virtual CVisualMemoryManager* visual_memory( ) const;

	virtual BOOL				BonePassBullet(int boneID);
	virtual void				On_B_NotCurrentEntity( );

private:
	collide::rq_results			RQR;
	BOOL				CanPickItem(const CFrustum& frustum, const fVector3& from, CObject* item);
	xr_vector<ISpatial*>		ISpatialResult;

private:
	CLocationManager* m_location_manager;

public:
	IC		const CLocationManager& locations( ) const
	{
		VERIFY(m_location_manager);
		return						*m_location_manager;
	}

private:
	ALife::_OBJECT_ID	m_holder_id;

public:
	virtual bool				register_schedule( ) const
	{
		return false;
	}
};

extern bool		isActorAccelerated(u32 mstate, bool ZoomMode);

IC CActorCondition& CActor::conditions( ) const
{
	VERIFY(m_entity_condition);
	return *m_entity_condition;
}

extern CActor* g_actor;
CActor* Actor( );
extern const f32	s_fFallTime;
