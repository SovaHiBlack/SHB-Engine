#pragma once

#include "PHCharacter.h"
#include "MathUtils.h"

namespace ALife
{
	enum EHitType;
};

namespace DetailPathManager
{
	struct STravelPathPoint;
};

class CPHAICharacter;
class CPHSimpleCharacter;
class CPHCapture;
class CPHSynchronize;
class ICollisionDamageInfo;
class CElevatorState;

class CPHMovementControl
{
	collide::rq_results		storage;

	static const s32 path_few_point = 10;

public:
	CElevatorState* ElevatorState( );
	void 					in_shedule_Update(u32 DT);
	void					PHCaptureObject(CPhysicsShellHolder* object);
	void					PHCaptureObject(CPhysicsShellHolder* object, u16 element);
	CPHCapture* PHCapture( )
	{
		return m_capture;
	}
	CPHCharacter* PHCharacter( )
	{
		return m_character;
	}
	void					PHReleaseObject( );
	fVector3				PHCaptureGetNearestElemPos(const CPhysicsShellHolder* object);
	fMatrix4x4				PHCaptureGetNearestElemTransform(CPhysicsShellHolder* object);
	void					SetMaterial(u16 material);
	void					SetAirControlParam(f32 param)
	{
		fAirControlParam = param;
	}
	void					SetActorRestrictorRadius(CPHCharacter::ERestrictionType rt, f32 r);
	void					SetRestrictionType(CPHCharacter::ERestrictionType rt)
	{
		if (m_character)
		{
			m_character->SetRestrictionType(rt);
		}
	}
	void					SetActorMovable(bool v)
	{
		if (m_character)
		{
			m_character->SetActorMovable(v);
		}
	}
	void					SetForcedPhysicsControl(bool v)
	{
		if (m_character)
		{
			m_character->SetForcedPhysicsControl(v);
		}
	}
	bool					ForcedPhysicsControl( )
	{
		return (m_character && m_character->ForcedPhysicsControl( ));
	}
	void					UpdateObjectBox(CPHCharacter* ach);
	enum					JumpType
	{
		jtStrait, //end point before uppermost point
		jtCurved, //end point after uppermost point
		jtHigh	  //end point is uppermost point
	};
	void					JumpV(const fVector3& jump_velocity);
	void					Jump(const fVector3& start_point, const fVector3& end_point, f32 time);
	void					Jump(const fVector3& end_point, f32 time);
	f32					Jump(const fVector3& end_point);
	bool					JumpState( )
	{
		return (m_character && m_character->b_exist && m_character->IsEnabled( ) && m_character->JumpState( ));
	};
	///
	bool					PhyssicsOnlyMode( )
	{
		return (m_character && m_character->b_exist && m_character->IsEnabled( ) && (m_character->JumpState( ) || m_character->ForcedPhysicsControl( )));
	}
	void					GetJumpMinVelParam(fVector3& min_vel, f32& time, JumpType& type, const fVector3& end_point);	//returns vector of velocity of jump with minimal start speed
	//in min_vel and correspondent jump time in time
	f32					JumpMinVelTime(const fVector3& end_point); // return time of jump with min start speed
	// input: end_point and time; return velocity and type of jump
	void					GetJumpParam(fVector3& velocity, JumpType& type, const fVector3& end_point, f32 time);
	bool					b_exect_position;
	s32						in_dead_area_count;

public:
	enum EEnvironment
	{
		peOnGround,
		peAtWall,
		peInAir
	};
	enum CharacterType
	{
		actor,
		ai
	};
	bool				isOutBorder( )
	{
		return (in_dead_area_count > 0);
	}
	void				setOutBorder( )
	{
		in_dead_area_count = 1;
	}

private:
	void				TraceBorder(const fVector3& previous_position);
	void				CheckEnvironment(const fVector3& V);

	CharacterType		eCharacterType;
	CPHCharacter* m_character;
	CPHCapture* m_capture;

	f32				m_fGroundDelayFactor;
	BOOL				bIsAffectedByGravity;
	//------------------------------
	CObject* pObject;
	EEnvironment		eOldEnvironment;
	EEnvironment		eEnvironment;
	fBox3				aabb;
	fBox3				boxes[4];

	u32					trying_times[4];
	fVector3				trying_poses[4];
	DWORD				m_dwCurBox;

	f32				fMass;
	f32				fMinCrashSpeed;
	f32				fMaxCrashSpeed;
	f32				fCollisionDamageFactor;
	f32				fAirControlParam;

	fVector3				vVelocity;
	fVector3				vPosition;

	fVector3				vPathPoint;
	fVector3				_vPathDir;
	s32					m_path_size;
	s32					m_start_index;

	f32				m_path_distance;
	u16					m_material;

	f32				fLastMotionMag;

	f32				fActualVelocity;
	f32				fContactSpeed;
	f32				fLastUpdateTime;

public:
	fVector3				vExternalImpulse;
	bool				bExernalImpulse;
	BOOL				bSleep;

	BOOL				gcontact_Was;			// Приземление
	f32				gcontact_Power;			// Насколько сильно ударились
	f32				gcontact_HealthLost;	// Скоко здоровья потеряли

public:
	void				AllocateCharacterObject(CharacterType type);
	void				DeleteCharacterObject( );
	void				CreateCharacter( );
	void				DestroyCharacter( );
	void				Load(pcstr section);

#ifdef DEBUG
	void				dbg_Draw( )
	{
		if (m_character)
		{
			m_character->OnRender( );
		}
	}
#endif

	void				SetPLastMaterialIDX(u16* p)
	{
		m_character->SetPLastMaterialIDX(p);
	}
	dBodyID				GetBody( )
	{
		if (m_character)
		{
			return m_character->get_body( );
		}
		else
		{
			return NULL;
		}
	}
	const fVector3& GetVelocity( )
	{
		return vVelocity;
	}
	const fVector3& GetPathDir( )
	{
		return _vPathDir;
	}
	void				SetPathDir(const fVector3& v);

	void				GetCharacterVelocity(fVector3& velocity)
	{
		if (m_character)
		{
			m_character->GetVelocity(velocity);
		}
		else
		{
			velocity.set(0.0f, 0.0f, 0.0f);
		}
	}
	f32				GetVelocityMagnitude( )
	{
		return vVelocity.magnitude( );
	}
	f32				GetVelocityActual( )
	{
		return fActualVelocity;
	}
	f32				GetXZVelocityActual( )
	{
		return dXZMag(vVelocity);
	}
	f32				GetActVelProj(const fVector3& dir)
	{
		return vVelocity.dotproduct(dir);
	}
	f32				GetActVelInGoingDir( )
	{
		f32 r = GetActVelProj(GetPathDir( ));
		return (r > 0.0f ? r : 0.0f);
	}
	f32				GetXZActVelInGoingDir( )
	{
		f32 r = dXZDot(GetPathDir( ), vVelocity);
		return (r > EPSILON_3 ? r : 0.0f);
	}
	void				GetSmoothedVelocity(fVector3& v)
	{
		if (m_character)
		{
			m_character->GetSmothedVelocity(v);
		}
		else
		{
			v.set(0.0f, 0.0f, 0.0f);
		}
	}
	f32				GetContactSpeed( )
	{
		return fContactSpeed;
	}
	void				GroundNormal(fVector3& norm);
	CPHSynchronize* GetSyncItem( );
	void				Freeze( );
	void				UnFreeze( );
	void				SetVelocity(f32 x, f32 y, f32 z)
	{
		SetVelocity(fVector3( ).set(x, y, z));
	}
	void				SetVelocity(const fVector3& v)
	{
		vVelocity.set(v);
		SetCharacterVelocity(v);
	}
	void				SetCharacterVelocity(const fVector3& v)
	{
		if (m_character)
		{
			m_character->SetVelocity(v);
		}
	}
	void				SetPhysicsRefObject(CPhysicsShellHolder* ref_object)
	{
		m_character->SetPhysicsRefObject(ref_object);
	}

	void				CalcMaximumVelocity(fVector3& /**dest/**/, fVector3& /**accel/**/, f32 /**friction/**/)
	{ }
	void				CalcMaximumVelocity(f32& /**dest/**/, f32 /**accel/**/, f32 /**friction/**/)
	{ }
	void				ActivateBox(DWORD id, BOOL Check = false);
	bool				ActivateBoxDynamic(DWORD id, s32 num_it = 9, s32 num_steps = 5, f32 resolve_depth = 0.01f);
	void				InterpolateBox(DWORD id, f32 k);
	EEnvironment		Environment( )
	{
		return eEnvironment;
	}
	EEnvironment		OldEnvironment( )
	{
		return eOldEnvironment;
	}
	const fBox3& Box( )
	{
		return aabb;
	}
	DWORD				BoxID( )const
	{
		return m_dwCurBox;
	}
	const fBox3* Boxes( )
	{
		return boxes;
	}
	f32				FootRadius( )
	{
		if (m_character)
		{
			return m_character->FootRadius( );
		}
		else
		{
			return 0.0f;
		}
	};
	void				CollisionEnable(BOOL enable)
	{
		if (m_character)
		{
			if (enable)
			{
				m_character->collision_enable( );
			}
			else
			{
				m_character->collision_disable( );
			}
		}
	}
	void				SetBox(DWORD id, const fBox3& BB)
	{
		boxes[id].set(BB);
		aabb.set(BB);
	}
	void				SetMass(f32 M)
	{
		fMass = M;
		if (m_character)
		{
			m_character->SetMas(fMass);
		}
	}
	f32				GetMass( )
	{
		return fMass;
	}
	void				SetCrashSpeeds(f32 min, f32 max)
	{
		fMinCrashSpeed = min;
		fMaxCrashSpeed = max;
	}

	void				SetPosition(const fVector3& P);
	void				GetPosition(fVector3& P);
	void				GetCharacterPosition(fVector3& P)
	{
		m_character->GetPosition(P);
	}
	void				InterpolatePosition(fVector3& P)
	{
		VERIFY(m_character && m_character->b_exist);
		m_character->IPosition(P);
	}
	bool				TryPosition(fVector3& pos);
	bool				IsCharacterEnabled( )
	{
		return (m_character->IsEnabled( ) || bExernalImpulse);
	}
	void				DisableCharacter( )
	{
		m_character->Disable( );
	}
	void				Calculate(fVector3& vAccel, const fVector3& camDir, f32 ang_speed, f32 jump, f32 dt, bool bLight);
	void				Calculate(const xr_vector<DetailPathManager::STravelPathPoint>& path, //in path
								  f32 speed,						 //in speed
								  u32& travel_point,					 //in- travel start, out - current trev point
								  f32& precesition					 //in- tolerance, out - precesition
	);
	void				AddControlVel(const fVector3& vel);
	void				SetVelocityLimit(f32 val);
	f32				VelocityLimit( );
	void				PathNearestPoint(const xr_vector<DetailPathManager::STravelPathPoint>& path,		//in path
										 const fVector3& new_position,  //in position
										 s32& index,			//out nearest
										 bool& type          //out type
	);	//return nearest point
	void				PathNearestPointFindUp(const xr_vector<DetailPathManager::STravelPathPoint>& path,			//in path
											   const fVector3& new_position,  //in position
											   s32& index,			//out nearest
											   f32							radius,			//in exit radius
											   bool& near_line      //out type
	);
	void				PathNearestPointFindDown(const xr_vector<DetailPathManager::STravelPathPoint>& path,			//in path
												 const fVector3& new_position,  //in position
												 s32& index,			//out nearest
												 f32							radius,			//in exit radius
												 bool& near_line      //out type
	);

	void				PathDIrPoint(const xr_vector<DetailPathManager::STravelPathPoint>& path,		//in path
									 s32							index,			//in index
									 f32							distance,	//in distance
									 f32							precesition,//in precesition
									 fVector3& dir        //out dir
	);
	void				PathDIrLine(const xr_vector<DetailPathManager::STravelPathPoint>& path,		//in path
									s32								index,		//in point
									f32							distance,	//in distance
									f32							precesition,//in precesition
									fVector3& dir        //out dir
	);
	void				CorrectPathDir(const fVector3& real_path_dir, const xr_vector<DetailPathManager::STravelPathPoint>& path, s32 index, fVector3& corrected_path_dir);

	//	void				Move					(fVector3& Dest, fVector3& Motion, BOOL bDynamic=FALSE){};
	void				SetApplyGravity(BOOL flag)
	{
		bIsAffectedByGravity = flag;
		if (m_character && m_character->b_exist)
		{
			m_character->SetApplyGravity(flag);
		}
	}
	void				GetDeathPosition(fVector3& pos)
	{
		m_character->DeathPosition(pos);
	}
	void				SetEnvironment(s32 enviroment, s32 old_enviroment);
	void				SetFrictionFactor(f32 f);
	f32				GetFrictionFactor( );
	void				MulFrictionFactor(f32 f);
	void				ApplyImpulse(const fVector3& dir, const dReal P);
	void				ApplyHit(const fVector3& dir, const dReal P, ALife::EHitType hit_type);
	void				SetJumpUpVelocity(f32 velocity)
	{
		m_character->SetJupmUpVelocity(velocity);
	}
	void				EnableCharacter( )
	{
		if (m_character && m_character->b_exist)
		{
			m_character->Enable( );
		}
	}
	void				SetOjectContactCallback(ObjectContactCallbackFun* callback)
	{
		if (m_character)
		{
			m_character->SetObjectContactCallback(callback);
		}
	}
	void				SetFootCallBack(ObjectContactCallbackFun* callback)
	{
		VERIFY(m_character);
		m_character->SetWheelContactCallback(callback);
	}
	static BOOL CPHMovementControl::BorderTraceCallback(collide::rq_result& result, LPVOID params);
	ObjectContactCallbackFun* ObjectContactCallback( )
	{
		if (m_character)
		{
			return m_character->ObjectContactCallBack( );
		}
		else
		{
			return NULL;
		}
	}
	u16					ContactBone( )
	{
		return m_character->ContactBone( );
	}
	const ICollisionDamageInfo* CollisionDamageInfo( )const
	{
		VERIFY(m_character);
		return m_character->CollisionDamageInfo( );
	}
	void				GetDesiredPos(fVector3& dpos)
	{
		m_character->GetDesiredPosition(dpos);
	}
	bool				CharacterExist( )
	{
		return (m_character && m_character->b_exist);
	}
	CPHMovementControl(CObject* parent);
	~CPHMovementControl( );
};
