#pragma once

#include "shootingobject.h"
#include "weaponammo.h"
#include "rocketlauncher.h"
#include "entity.h"
#include "phskeleton.h"
#include "hit_immunity.h"
#include "script_export_space.h"
#include "memory_manager.h"
#include "HudSound.h"

#include "patrol_path.h"
#include "PHDestroyable.h"
#include "Explosive.h"

class CScriptGameObject;
class CLightAnimItem;
class CHelicopterMovManager;
class CHelicopter;

enum EHeliHuntState
{
	eEnemyNone, eEnemyPoint, eEnemyEntity
};
struct SHeliEnemy
{
	EHeliHuntState					type;
	fVector3							destEnemyPos;
	u32								destEnemyID;
	f32							fire_trail_length_curr;
	f32							fire_trail_length_des;
	bool							bUseFireTrail;
	f32							fStartFireTime;
	void reinit( );
	void Update( );
	void save(CNetPacket& output_packet);
	void load(IReader& input_packet);
	void Load(pcstr section);
};

enum EHeliBodyState
{
	eBodyByPath, eBodyToPoint
};
struct SHeliBodyState
{
	CHelicopter* parent;
	EHeliBodyState					type;
	//settings, constants
	f32							model_pitch_k;
	f32							model_bank_k;
	f32							model_angSpeedBank;
	f32							model_angSpeedPitch;

	//runtime params
	fVector3							currBodyHPB;

	bool							b_looking_at_point;
	fVector3							looking_point;
	void		reinit( );
	void		LookAtPoint(fVector3 point, bool do_it);

	void save(CNetPacket& output_packet);
	void load(IReader& input_packet);
	void Load(pcstr section);
};

enum EHeilMovementState
{
	eMovNone, eMovToPoint, eMovPatrolPath, eMovRoundPath, eMovLanding, eMovTakeOff
};
struct SHeliMovementState
{
	struct STmpPt
	{
		fVector3		point;
		f32		dir_h;
		STmpPt(const fVector3& p, const f32 h) : point(p), dir_h(h)
		{ }
	};
	~SHeliMovementState( );
	CHelicopter* parent;
	EHeilMovementState				type;
	//specified path
	const CPatrolPath* currPatrolPath;
	const CPatrolPath::CVertex* currPatrolVertex;

	s32								patrol_begin_idx;
	shared_str						patrol_path_name;
	bool							need_to_del_path;
	f32							safe_altitude_add;
	f32							maxLinearSpeed;
	f32							LinearAcc_fw;
	f32							LinearAcc_bk;
	f32							isAdnAcc;

protected:
	f32							HeadingSpK, HeadingSpB;
	f32							PitchSpK, PitchSpB, AngSP, AngSH;
	f32							speedInDestPoint;
	void							SetPointFlags(u32 idx, u32 new_flags);

public:
	f32							min_altitude;
	//runtime values
	fVector3							desiredPoint;

	f32							curLinearSpeed;
	f32							curLinearAcc;

	fVector3							currP;
	f32							currPathH;
	f32							currPathP;

	fVector3							round_center;
	f32							round_radius;
	bool							round_reverse;

	f32							onPointRangeDist;
	f32	GetSpeedInDestPoint( );
	void	SetSpeedInDestPoint(f32);
	f32	GetAngSpeedPitch(f32 speed);
	f32	GetAngSpeedHeading(f32 speed);

	f32	GetSafeAltitude( );
	void	reinit( );
	void	Update( );
	void	UpdateMovToPoint( );
	void	UpdatePatrolPath( );
	bool	AlreadyOnPoint( );
	void	goByRoundPath(fVector3 center, f32 radius, bool clockwise);
	f32	GetDistanceToDestPosition( );
	void	getPathAltitude(fVector3& point, f32 base_altitude);
	void	SetDestPosition(fVector3* pos);
	void	goPatrolByPatrolPath(pcstr path_name, s32 start_idx);
	void	CreateRoundPoints(fVector3 center, f32 radius, f32 start_h, f32 end_h, xr_vector<STmpPt>& round_points);
	void	save(CNetPacket& output_packet);
	void	load(IReader& input_packet);
	void	Load(pcstr section);
	void	net_Destroy( );
};

class CHelicopter : public CEntity,
	public CShootingObject,
	public CRocketLauncher,
	public CPHSkeleton,
	public CPHDestroyable,
	public CHitImmunity,
	public CExplosive
#ifdef DEBUG
	, public pureRender
#endif

{
	typedef CEntity inherited;
public:
	enum EHeliState
	{
		eAlive = u32(0),
		eDead,
		eForce = u32(-1)
	};

	// heli weapons
	bool							m_use_rocket_on_attack;
	bool							m_use_mgun_on_attack;
	f32							m_min_rocket_dist, m_max_rocket_dist;
	f32							m_min_mgun_dist, m_max_mgun_dist;
	u32								m_time_between_rocket_attack;
	bool							m_syncronize_rocket;
	f32							m_barrel_dir_tolerance;
	HUD_SOUND						m_sndShot;
	HUD_SOUND						m_sndShotRocket;

	fVector3							m_fire_dir;
	fVector3						m_fire_pos;

	u16								m_left_rocket_bone, m_right_rocket_bone, m_fire_bone, m_rotate_x_bone, m_rotate_y_bone;

	fMatrix4x4							m_fire_bone_xform;
	fMatrix4x4							m_i_bind_x_xform;
	fMatrix4x4						m_i_bind_y_xform;
	fVector2						m_lim_x_rot;
	fVector2						m_lim_y_rot;
	fVector2						m_tgt_rot;
	fVector2						m_cur_rot;
	fVector2						m_bind_rot;
	fVector3							m_bind_x;
	fVector3						m_bind_y;
	bool							m_allow_fire;
	u16								m_last_launched_rocket;
	u32								m_last_rocket_attack;

	shared_str						m_sAmmoType, m_sRocketSection;
	CCartridge						m_CurrentAmmo;
	f32							delta_t;
	f32							flag_by_fire;
	fMatrix4x4						m_left_rocket_bone_xform;
	fMatrix4x4						m_right_rocket_bone_xform;

	static void 					BoneMGunCallbackX(CBoneInstance* B);
	static void						BoneMGunCallbackY(CBoneInstance* B);
	void							startRocket(u16 idx);

	//CShootingObject
	virtual const fMatrix4x4& ParticlesXFORM( )const
	{
		return m_fire_bone_xform;
	}

	virtual const fVector3& CurrentFirePoint( )
	{
		return m_fire_pos;
	}

	void							MGunFireStart( );
	void							MGunFireEnd( );
	void							MGunUpdateFire( );
	virtual	void					OnShot( );

	void							UpdateMGunDir( );
	void							UpdateWeapons( );

	bool							m_flame_started;
	bool							m_light_started;
	bool							m_ready_explode;
	bool							m_exploded;
	bool							m_dead;

protected:
	SHeliEnemy						m_enemy;
	SHeliBodyState					m_body;
	SHeliMovementState				m_movement;

	// on death...
	fVector3							m_death_ang_vel;
	f32							m_death_lin_vel_k;
	shared_str						m_death_bones_to_hide;

	//////////////////////////////////////////////////

	// sound, light, particles...
	ref_sound						m_engineSound;
	ref_sound						m_brokenSound;
	//	ref_sound						m_explodeSound;
	ref_light						m_light_render;
	CLightAnimItem* m_lanim;
	u16								m_light_bone, m_smoke_bone;
	f32							m_light_range, m_light_brightness;
	fColor							m_light_color;
	shared_str						m_smoke_particle;
	CParticlesObject* m_pParticle;
	fMatrix4x4							m_particleXFORM;

	void							StartFlame( );
	void							UpdateHeliParticles( );
	void							DieHelicopter( );
	void							TurnLighting(bool bOn);
	void							TurnEngineSound(bool bOn);
	//explosive
	virtual void					OnAfterExplosion( )
	{ }
	virtual void					GetRayExplosionSourcePos(fVector3& pos)
	{
		random_point_in_object_box(pos, this);
	}
	virtual void					ActivateExplosionBox(const fVector3& size, fVector3& in_out_pos)
	{ }
	//general
	EHeliState						m_curState;

	xr_map<s16, f32>				m_hitBones;
	typedef xr_map<s16, f32>::iterator bonesIt;
	f32							m_stepRemains;

	void	UpdateState( );

public:
	void							ExplodeHelicopter( );

	CHelicopter( );
	virtual							~CHelicopter( );

	CHelicopter::EHeliState			state( )
	{
		return m_curState;
	}
	s32								state_script( )
	{
		return m_curState;
	}

	void							setState(CHelicopter::EHeliState s);
	void							setState_script(u32 s)
	{
		setState((CHelicopter::EHeliState)s);
	}

	void							init( );
	virtual	void					reinit( );

	virtual	void					Load(pcstr		section);
	virtual	void					reload(pcstr		section);

	virtual BOOL					net_Spawn(CSE_Abstract* DC);
	virtual void					net_Destroy( );
	virtual void					net_Export(CNetPacket& P)
	{ }
	virtual void					net_Import(CNetPacket& P)
	{ }
	virtual void					net_Relcase(CObject* O);
	virtual void					save(CNetPacket& output_packet);
	virtual void					load(IReader& input_packet);

	virtual void					SpawnInitPhysics(CSE_Abstract* D);
	virtual CPhysicsShellHolder* PPhysicsShellHolder( )
	{
		return PhysicsShellHolder( );
	}
	virtual void					net_Save(CNetPacket& P);
	virtual	BOOL					net_SaveRelevant( )
	{
		return (inherited::net_SaveRelevant( ) && BOOL(PPhysicsShell( ) != NULL)) || m_exploded;
	}

	virtual void					renderable_Render( )
	{
		inherited::renderable_Render( );
	}
	virtual BOOL					renderable_ShadowGenerate( )
	{
		return FALSE;
	}
	virtual BOOL					renderable_ShadowReceive( )
	{
		return TRUE;
	}

	virtual void					OnEvent(CNetPacket& P, u16 type);
	virtual void					UpdateCL( );
	virtual void					shedule_Update(u32		time_delta);
	void					MoveStep( );

	virtual	void					Hit(SHit* pHDS);
	virtual void					PHHit(f32 P, fVector3& dir, CObject* who, s16 element, fVector3 p_in_object_space, f32 impulse, ALife::EHitType hit_type);
	//CEntity
	virtual void					HitSignal(f32 P, fVector3& local_dir, CObject* who, s16 element)
	{ }
	virtual void					HitImpulse(f32 P, fVector3& vWorldDir, fVector3& vLocalDir)
	{ }

	virtual const fMatrix4x4& get_ParticlesXFORM( );
	virtual const fVector3& get_CurrentFirePoint( );

	virtual CGameObject* cast_game_object( )
	{
		return this;
	}
	virtual CExplosive* cast_explosive( )
	{
		return this;
	}
	virtual CPHSkeleton* PHSkeleton( )
	{
		return this;
	}

public:
	//for scripting
	bool					isVisible(CScriptGameObject* O);
	bool					isObjectVisible(CObject* O);
	bool			 		isOnAttack( )
	{
		return m_enemy.type != eEnemyNone;
	}

	void					goPatrolByPatrolPath(pcstr path_name, s32 start_idx);
	void					goByRoundPath(fVector3 center, f32 radius, bool clockwise);
	void					LookAtPoint(fVector3 point, bool do_it);
	void					SetDestPosition(fVector3* pos);
	f32					GetDistanceToDestPosition( );

	void					SetSpeedInDestPoint(f32 sp);
	f32					GetSpeedInDestPoint(f32 sp);

	void					SetOnPointRangeDist(f32 d);
	f32					GetOnPointRangeDist( );

	f32					GetCurrVelocity( );
	f32					GetMaxVelocity( );
	void					SetMaxVelocity(f32 v);
	//////////////////////Start By JoHnY///////////////////////
	void					SetLinearAcc(f32 LAcc_fw, f32 LAcc_bw);
	//////////////////////End By JoHnY/////////////////////////
	fVector3					GetCurrVelocityVec( );
	void					SetBarrelDirTolerance(f32 val)
	{
		m_barrel_dir_tolerance = val;
	}
	void					SetEnemy(CScriptGameObject* e);
	void					SetEnemy(fVector3* pos);
	void					UnSetEnemy( );
	void					SetFireTrailLength(f32 val);
	bool					UseFireTrail( );
	void					UseFireTrail(bool val);

	f32					GetRealAltitude( );

	s32						GetMovementState( );
	s32						GetHuntState( );
	s32						GetBodyState( );

	virtual DLL_Pure* _construct( );
	f32					GetSafeAltitude( )
	{
		return m_movement.GetSafeAltitude( );
	}
	f32					GetHeliHealth( ) const
	{
		return inherited::GetfHealth( );
	}
	f32					SetHeliHealth(f32 value)
	{
		return inherited::SetfHealth(value);
	}

#ifdef DEBUG
public:
	virtual void			OnRender( );
#endif

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CHelicopter)
#undef script_type_list
#define script_type_list save_type_list(CHelicopter)