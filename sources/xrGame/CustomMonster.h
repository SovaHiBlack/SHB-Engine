// CustomMonster.h: interface for the CCustomMonster class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "entity_alive.h"
#include "script_entity.h"
#include "..\XR_3DA\feel_vision.h"
#include "..\XR_3DA\feel_sound.h"
#include "..\XR_3DA\feel_touch.h"
#include "..\XR_3DA\skeletonanimated.h"
#include "associative_vector.h"

namespace MonsterSpace {
	struct SBoneRotation;
};

class CMotionDef;
class CKinematicsAnimated;
class CMemoryManager;
class CItemManager;
class CEnemyManager;
class CDangerManager;
class CMovementManager;
class CSoundPlayer;
class CStalker;
class CDangerObject;

class CCustomMonster : 
	public CEntityAlive, 
	public CScriptEntity,
	public Feel::Vision,
	public Feel::Sound,
	public Feel::Touch
{
private:
	typedef	CEntityAlive	inherited;

private:
	CMemoryManager		*m_memory_manager;
	CMovementManager	*m_movement_manager;
	CSoundPlayer		*m_sound_player;

private:
	u32					m_client_update_delta;
	u32					m_last_client_update_time;

protected:
	
	struct				SAnimState
	{
		MotionID		fwd;
		MotionID		back;
		MotionID		ls;
		MotionID		rs;

		void			Create(CKinematicsAnimated* K, pcstr base);
	};

private:
	xr_vector<CLASS_ID>	m_killer_clsids;

public:
	// Eyes
	fMatrix4x4				eye_matrix;
	s32					eye_bone;
	f32					eye_fov;
	f32					eye_range;

	f32					m_fCurSpeed;

	u32					eye_pp_stage;
	u32					eye_pp_timestamp;
	fVector3				m_tEyeShift;
	f32					m_fEyeShiftYaw;
	BOOL				NET_WasExtrapolating;

	fVector3				tWatchDirection;

	virtual void		Think() = 0;

	f32					m_fTimeUpdateDelta;
	u32					m_dwLastUpdateTime;
	u32					m_current_update;
//	fMatrix4x4				m_tServerTransform;
	
	u32					m_dwCurrentTime;		// time updated in UpdateCL 

	struct net_update	{
		u32				dwTimeStamp;			// server(game) timestamp
		f32				o_model;				// model yaw
		SRotation		o_torso;				// torso in world coords
		fVector3			p_pos;					// in world coords
		f32				fHealth;

		// non-exported (temporal)

		net_update()	{
			dwTimeStamp		= 0;
			o_model			= 0;
			o_torso.yaw		= 0;
			o_torso.pitch	= 0;
			p_pos.set		(0,0,0);
			fHealth			= 0.f;
		}
		void	lerp	(net_update& A,net_update& B, f32 f);
	};
	xr_deque<net_update>	NET;
	net_update				NET_Last;
	BOOL					NET_WasInterpolating;	// previous update was by interpolation or by extrapolation
	u32						NET_Time;				// server time of last update
//------------------------------

	virtual BOOL		feel_touch_on_contact	(CObject *);
	virtual BOOL		feel_touch_contact		(CObject *);
	// utils
	void				mk_orientation			(fVector3& dir, fMatrix4x4& mR );
	void				mk_rotation				(fVector3& dir, SRotation &R);

	// stream executors
	virtual void		Exec_Action				(f32 dt );
	virtual void		Exec_Look				(f32 dt );
	void	__stdcall	Exec_Visibility			( );
	void				eye_pp_s0				( );
	void				eye_pp_s1				( );
	void				eye_pp_s2				( );

	virtual void		UpdateCamera			( );

public:
						CCustomMonster			( );
	virtual				~CCustomMonster			( );

public:
	virtual CEntityAlive*				cast_entity_alive		()						{return this;}
	virtual CEntity*					cast_entity				()						{return this;}

public:
	virtual DLL_Pure	*_construct				();
	virtual BOOL		net_Spawn				( CSE_Abstract* DC);
	virtual void		Die						( CObject* who);

	virtual void		HitSignal				(f32 P, fVector3& vLocalDir, CObject* who);
	virtual void		g_WeaponBones			(s32&/**L/**/, s32&/**R1/**/, s32&/**R2/**/) {};
	virtual void		shedule_Update					( u32		DT		);
	virtual void		UpdateCL				( );

	// Network
	virtual void		net_Export				(CNetPacket& P);				// export to server
	virtual void		net_Import				(CNetPacket& P);				// import from server
	virtual void		net_Relcase				(CObject*	 O);

	virtual void		SelectAnimation			( const fVector3& _view, const fVector3& _move, f32 speed ) = 0;

	// debug
#ifdef DEBUG
	virtual void		OnRender				( );
	virtual void		OnHUDDraw				(CCustomHUD* hud);
#endif

	virtual bool		bfExecMovement			(){return(false);};


	IC	bool					angle_lerp_bounds		(f32& a, f32 b, f32 c, f32 d);
	IC	void					vfNormalizeSafe			(fVector3& Vector);

public:
	virtual	f32					ffGetFov				()	const								{return eye_fov;}
	virtual	f32					ffGetRange				()	const								{return eye_range;}
			void				set_fov					(f32 new_fov);
			void				set_range				(f32 new_range);
//	virtual	void				feel_touch_new			(CObject	*O);
	virtual BOOL				feel_visible_isRelevant	(CObject		*O);
	virtual	Feel::Sound*		dcast_FeelSound			()			{ return this;	}
	virtual	void				Hit						(SHit* pHDS);

	virtual void				OnEvent					(CNetPacket& P, u16 type		);
	virtual void				net_Destroy				();
	virtual BOOL				UsedAI_Locations		();
	///////////////////////////////////////////////////////////////////////
	virtual u16					PHGetSyncItemsNumber	()			{return inherited ::PHGetSyncItemsNumber();}
	virtual CPHSynchronize*		PHGetSyncItem			(u16 item)	{return inherited ::PHGetSyncItem(item);}
	virtual void				PHUnFreeze				()			{return inherited ::PHUnFreeze();}
	virtual void				PHFreeze				()			{return inherited ::PHFreeze();}
	///////////////////////////////////////////////////////////////////////
public:
	virtual void				Load					(pcstr	section);
	virtual	void				reinit					();
	virtual void				reload					(pcstr	section);
	virtual const SRotation		Orientation				() const;
	virtual f32					get_custom_pitch_speed	(f32 def_speed) {return def_speed;}
	virtual bool				human_being				() const
	{
		return					(false);
	}
	virtual	void				PitchCorrection			();

	virtual void				save					(CNetPacket& output_packet);
	virtual void				load					(IReader &input_packet);
	virtual BOOL				net_SaveRelevant		()							{return inherited::net_SaveRelevant();}
	
	virtual	const MonsterSpace::SBoneRotation &head_orientation	() const;
	
	virtual void				UpdatePositionAnimation	();
	virtual void				set_ready_to_save		();
	virtual CPhysicsShellHolder*cast_physics_shell_holder	()	{return this;}
	virtual CParticlesPlayer*	cast_particles_player	()	{return this;}
	virtual CCustomMonster*		cast_custom_monster		()	{return this;}
	virtual CScriptEntity*		cast_script_entity		()	{return this;}

			void				load_killer_clsids		(pcstr section);
			bool				is_special_killer		(CObject *obj);

	IC		CMemoryManager		&memory					() const;
	virtual f32					feel_vision_mtl_transp	(CObject* O, u32 element);
	virtual	void				feel_sound_new(CObject* who, s32 type, CSound_UserDataPtr user_data, const fVector3& Position, f32 power);

	virtual bool				useful					(const CItemManager *manager, const CGameObject *object) const;
	virtual f32					evaluate				(const CItemManager *manager, const CGameObject *object) const;
	virtual bool				useful					(const CEnemyManager *manager, const CEntityAlive *object) const;
	virtual f32					evaluate				(const CEnemyManager *manager, const CEntityAlive *object) const;
	virtual bool				useful					(const CDangerManager *manager, const CDangerObject &object) const;
	virtual f32					evaluate				(const CDangerManager *manager, const CDangerObject &object) const;

protected:
	f32							m_panic_threshold;

public:
	IC		f32					panic_threshold			() const;


private:
	CSound_UserDataVisitor			*m_sound_user_data_visitor;

protected:
	virtual CSound_UserDataVisitor	*create_sound_visitor	();
	virtual CMemoryManager			*create_memory_manager	();
	virtual CMovementManager		*create_movement_manager();

public:
	IC		CMovementManager		&movement				() const;
	IC		CSoundPlayer			&sound					() const;
	IC		CSound_UserDataVisitor	*sound_user_data_visitor() const;

protected:
	f32								m_far_plane_factor;
	f32								m_fog_density_factor;

public:
	virtual	void					update_range_fov		(f32& new_range, f32& new_fov, f32 start_range, f32 start_fov);

public:
			void __stdcall			update_sound_player		();
	virtual	void					on_restrictions_change	();
	virtual	pcstr					visual_name				(CSE_Abstract *server_entity);

private:
	bool							m_already_dead;

public:
	IC		const bool				&already_dead			() const {return (m_already_dead);};
	virtual	bool					use_simplified_visual	() const {return (already_dead());};
	virtual void					on_enemy_change			(const CEntityAlive *enemy);
	virtual	CVisualMemoryManager	*visual_memory			() const;

public:
	IC		f32						client_update_fdelta	() const;
	IC		const u32				&client_update_delta	() const;
	IC		const u32				&last_client_update_time() const;

	//////////////////////////////////////////////////////////////////////////
	// Critical Wounds
	//////////////////////////////////////////////////////////////////////////
public:
	typedef u32											CriticalWoundType;
private:
	typedef associative_vector<u16,CriticalWoundType>	BODY_PART;

protected:
	u32								m_last_hit_time;
	f32								m_critical_wound_threshold;
	f32								m_critical_wound_decrease_quant;
	f32								m_critical_wound_accumulator;
	CriticalWoundType				m_critical_wound_type;
	BODY_PART						m_bones_body_parts;

protected:
	virtual void					load_critical_wound_bones					() {}

	virtual bool					critical_wound_external_conditions_suitable	() {return true;}
	virtual void					critical_wounded_state_start				() {}

			bool					update_critical_wounded						(const u16 &bone_id, const f32& power);

public:
	IC		void					critical_wounded_state_stop					();
public:
	IC		bool					critically_wounded							();
	IC	const u32					&critical_wound_type						() const;

	//////////////////////////////////////////////////////////////////////////
private:
	bool							m_invulnerable;

public:
	IC		void					invulnerable								(const bool &invulnerable);
	IC		bool					invulnerable								() const;

protected:
	bool	m_update_rotation_on_frame;

private:
	bool	m_movement_enabled_before_animation_controller;

public:
	virtual	void					create_anim_mov_ctrl						(CBlend *b);
	virtual	void					destroy_anim_mov_ctrl						();
};

#include "custommonster_inline.h"