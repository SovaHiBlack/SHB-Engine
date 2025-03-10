#pragma once

#include "PHObject.h"
#include "PHInterpolation.h"
#include "PHSynchronize.h"
#include "PHDisabling.h"

class CPhysicsShellHolder;
class CClimableObject;
class CGameObject;
class ICollisionDamageInfo;
class CElevatorState;
class CPHActorCharacter;
class CPHAICharacter;

static enum EEnvironment
{
	peOnGround,
	peAtWall,
	peInAir
};


class CPHCharacter :
	public CPHObject,
	public CPHSynchronize,
	public CPHDisablingTranslational
#ifdef DEBUG
	, public pureRender
#endif
{
public:

	u64						m_creation_step;
	bool					b_exist;
protected:

	////////////////////////// dynamic

	CPHInterpolation m_body_interpolation;
	dBodyID				m_body;
	CPhysicsShellHolder* m_phys_ref_object;

	dReal					m_mass;

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////
	u16* p_lastMaterialIDX;
	u16 lastMaterialIDX;
	///////////////////////////////////////////////////////////////////////////
	dVector3 m_safe_velocity;
	dVector3 m_safe_position;
	dReal	 m_mean_y;
public:
	enum ERestrictionType
	{

		rtStalker = 0,
		rtStalkerSmall,
		rtMonsterMedium,
		rtNone,
		rtActor
	};

protected:
	ERestrictionType		m_new_restriction_type;
	ERestrictionType		m_restriction_type;
	bool					b_actor_movable;

	bool					b_in_touch_resrtrictor;
	f32					m_current_object_radius;

public:
	virtual ECastType	CastType( )
	{
		return CPHObject::tpCharacter;
	}
	virtual CPHActorCharacter* CastActorCharacter( )
	{
		return NULL;
	}
	virtual CPHAICharacter* CastAICharacter( )
	{
		return NULL;
	}
	ERestrictionType	RestrictionType( )
	{
		return m_restriction_type;
	}
	void				SetNewRestrictionType(ERestrictionType rt)
	{
		m_new_restriction_type = rt;
	}
	void				SetRestrictionType(ERestrictionType rt)
	{
		m_new_restriction_type = m_restriction_type = rt;
	}
	void				SetObjectRadius(f32 R)
	{
		m_current_object_radius = R;
	}
	f32				ObjectRadius( )
	{
		return m_current_object_radius;
	}
	virtual	void		ChooseRestrictionType(ERestrictionType my_type, f32 my_depth, CPHCharacter* ch)
	{ }
	virtual bool		UpdateRestrictionType(CPHCharacter* ach) = 0;
	virtual void		FreezeContent( );
	virtual void		UnFreezeContent( );
	virtual	dBodyID		get_body( )
	{
		return m_body;
	}
	virtual	dSpaceID	dSpace( ) = 0;
	virtual	void		Disable( );
	virtual	void		ReEnable( )
	{ }
	virtual	void		Enable( );											//!!
	virtual	void		SwitchOFFInitContact( ) = 0;
	virtual	void		SwitchInInitContact( ) = 0;
	bool		IsEnabled( )
	{
		if (!b_exist)return false; return !!dBodyIsEnabled(m_body);
	}
	bool		ActorMovable( )
	{
		return b_actor_movable;
	}
	void		SetActorMovable(bool v)
	{
		b_actor_movable = v;
	}
	virtual		const ICollisionDamageInfo* CollisionDamageInfo( )const = 0;
	virtual		void		Reinit( ) = 0;
	void					SetPLastMaterialIDX(u16* p)
	{
		p_lastMaterialIDX = p;
	}
	const	u16& LastMaterialIDX( )const
	{
		return *p_lastMaterialIDX;
	}
	virtual bool			TouchRestrictor(ERestrictionType rttype) = 0;
	virtual void			SetElevator(CClimableObject* climable)
	{ }
	virtual void			SetMaterial(u16 material) = 0;
	virtual void			SetMaximumVelocity(dReal /**vel/**/)
	{ }																			//!!
	virtual		dReal		GetMaximumVelocity( )
	{
		return 0;
	}
	virtual	void			SetJupmUpVelocity(dReal /**velocity/**/)
	{ }																			//!!
	virtual	void			IPosition(fVector3& /**pos/**/)
	{ }
	virtual u16				ContactBone( )
	{
		return 0;
	}
	virtual void			DeathPosition(fVector3& /**deathPos/**/)
	{ }
	virtual		void		ApplyImpulse(const fVector3& /**dir/**/, const dReal /**P/**/)
	{ }
	virtual		void		ApplyForce(const fVector3& force) = 0;
	virtual		void		ApplyForce(const fVector3& dir, f32 force) = 0;
	virtual		void		ApplyForce(f32 x, f32 y, f32 z) = 0;
	virtual		void		AddControlVel(const fVector3& vel) = 0;
	virtual		void		Jump(const fVector3& jump_velocity) = 0;
	virtual		bool		JumpState( ) = 0;
	virtual		EEnvironment CheckInvironment( ) = 0;
	virtual		bool		ContactWas( ) = 0;
	virtual		void		GroundNormal(fVector3& norm) = 0;
	virtual		void		Create(dVector3 /**sizes/**/) = 0;
	virtual		void		Destroy(void) = 0;
	virtual		void		SetBox(const dVector3& sizes) = 0;
	virtual		void		SetAcceleration(fVector3 accel) = 0;
	virtual		void		SetForcedPhysicsControl(bool v)
	{ }
	virtual		bool		ForcedPhysicsControl( )
	{
		return false;
	}
	virtual     void		SetCamDir(const fVector3& cam_dir) = 0;
	virtual	const fVector3& CamDir( )const = 0;
	virtual		fVector3		GetAcceleration( ) = 0;
	virtual		void		SetPosition(fVector3 pos) = 0;
	virtual		void		SetApplyGravity(BOOL flag)
	{
		dBodySetGravityMode(m_body, flag);
	}
	virtual		void		SetObjectContactCallback(ObjectContactCallbackFun* callback) = 0;
	virtual		void		SetWheelContactCallback(ObjectContactCallbackFun* callback) = 0;
	virtual		ObjectContactCallbackFun* ObjectContactCallBack( )
	{
		return NULL;
	}
	virtual		void		GetVelocity(fVector3& vvel) = 0;
	virtual		void		GetSavedVelocity(fVector3& vvel);
	virtual		void		GetSmothedVelocity(fVector3& vvel) = 0;
	virtual		void		SetVelocity(fVector3 vel) = 0;
	virtual		void		SetAirControlFactor(f32 factor) = 0;
	virtual		void		GetPosition(fVector3& vpos) = 0;
	virtual		void		GetFootCenter(fVector3& vpos)
	{
		vpos.set(*(fVector3*)dBodyGetPosition(m_body));
	}
	virtual		void		SetMas(dReal mass) = 0;
	virtual		void		SetCollisionDamageFactor(f32 f) = 0;
	virtual		f32		Mass( ) = 0;
	virtual		void		SetPhysicsRefObject(CPhysicsShellHolder* ref_object) = 0;
	virtual		CPhysicsShellHolder* PhysicsRefObject( )
	{
		return m_phys_ref_object;
	}

	//AICharacter
	virtual		void		GetDesiredPosition(fVector3& /**dpos/**/)
	{ }
	virtual		void		SetDesiredPosition(const fVector3& /**pos/**/)
	{ }
	virtual		void		BringToDesired(f32 /**time/**/, f32 /**velocity/**/, f32 force = 1.f)
	{ }
	virtual		bool		TryPosition(fVector3 /**pos/**/, bool)
	{
		return false;
	}
	virtual		bool		TouchBorder( )
	{
		return false;
	}
	virtual		void		getForce(fVector3& force);
	virtual		void		setForce(const	fVector3& force);
	virtual		f32		FootRadius( ) = 0;
	virtual		void		get_State(SPHNetState& state);
	virtual		void		set_State(const	SPHNetState& state);
	virtual		void		cv2obj_Xfrom(const fQuaternion& q, const fVector3& pos, fMatrix4x4& xform)
	{ }
	virtual		void		cv2bone_Xfrom(const fQuaternion& q, const fVector3& pos, fMatrix4x4& xform)
	{ }
	virtual	const fVector3& ControlAccel( )const = 0;
	virtual		f32& FrictionFactor( ) = 0;
	virtual		void		CutVelocity(f32 l_limit, f32 a_limit);
	virtual		u16				get_elements_number( )
	{
		return 1;
	}
	virtual		CPHSynchronize* get_element_sync(u16 element)
	{
		VERIFY(element == 0); return static_cast<CPHSynchronize*>(this);
	}
	virtual		CElevatorState* ElevatorState( ) = 0;
	CPHCharacter( );
	virtual		~CPHCharacter( );
};
