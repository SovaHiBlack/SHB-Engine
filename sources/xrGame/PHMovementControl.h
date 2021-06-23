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
	collide::rq_results														storage;

public:
	CElevatorState* ElevatorState( );
	void 							in_shedule_Update(u32 DT);
	void							PHCaptureObject(CPHShellHolder* object);
	void							PHCaptureObject(CPHShellHolder* object, U16 element);
	CPHCapture* PHCapture( )
	{
		return m_capture;
	}
	CPHCharacter* PHCharacter( )
	{
		return m_character;
	}
	void							PHReleaseObject( );
	Fvector3							PHCaptureGetNearestElemPos(const CPHShellHolder* object);
	Fmatrix							PHCaptureGetNearestElemTransform(CPHShellHolder* object);
	void							SetMaterial(U16 material);
	void							SetAirControlParam(float param)
	{
		fAirControlParam = param;
	}
	void							SetActorRestrictorRadius(CPHCharacter::ERestrictionType rt, float r);
	void							SetRestrictionType(CPHCharacter::ERestrictionType rt)
	{
		if (m_character)
		{
			m_character->SetRestrictionType(rt);
		}
	}
	void							SetActorMovable(bool v)
	{
		if (m_character)
		{
			m_character->SetActorMovable(v);
		}
	}
	void							SetForcedPhysicsControl(bool v)
	{
		if (m_character)
		{
			m_character->SetForcedPhysicsControl(v);
		}
	}
	bool							ForcedPhysicsControl( )
	{
		return (m_character && m_character->ForcedPhysicsControl( ));
	}
	void							UpdateObjectBox(CPHCharacter* ach);
	enum							JumpType
	{
		jtStrait,	//end point before uppermost point
		jtCurved,	//end point after uppermost point
		jtHigh		//end point is uppermost point
	};
	void							JumpV(const Fvector3& jump_velocity);
	void							Jump(const Fvector3& start_point, const Fvector3& end_point, float time);
	void							Jump(const Fvector3& end_point, float time);
	float							Jump(const Fvector3& end_point);
	bool							JumpState( )
	{
		return (m_character && m_character->b_exist && m_character->IsEnabled( ) && m_character->JumpState( ));
	};

	bool							PhyssicsOnlyMode( )
	{
		return (m_character && m_character->b_exist && m_character->IsEnabled( ) && (m_character->JumpState( ) || m_character->ForcedPhysicsControl( )));
	}
	void							GetJumpMinVelParam(Fvector3& min_vel, float& time, JumpType& type, const Fvector3& end_point);	//returns vector of velocity of jump with minimal start speed in min_vel and correspondent jump time in time
	float							JumpMinVelTime(const Fvector3& end_point);													//return time of jump with min start speed. input: end_point and time; return velocity and type of jump
	void							GetJumpParam(Fvector3& velocity, JumpType& type, const Fvector3& end_point, float time);
	bool																	b_exect_position;
	int																		in_dead_area_count;

	enum							EEnvironment
	{
		peOnGround,
		peAtWall,
		peInAir
	};
	enum							CharacterType
	{
		actor,
		ai
	};
	bool							isOutBorder( )
	{
		return (in_dead_area_count > 0);
	}
	void							setOutBorder( )
	{
		in_dead_area_count = 1;
	}

private:
	void							TraceBorder(const Fvector3& previous_position);
	void							CheckEnvironment(const Fvector3& V);

	CharacterType															eCharacterType;
	CPHCharacter* m_character;
	CPHCapture* m_capture;

	float																	m_fGroundDelayFactor;
	BOOL																	bIsAffectedByGravity;

	CObject* pObject;
	EEnvironment															eOldEnvironment;
	EEnvironment															eEnvironment;
	Fbox3																	aabb;
	Fbox3																	boxes[4];

	u32																		trying_times[4];
	Fvector3																	trying_poses[4];
	DWORD																	m_dwCurBox;

	float																	fMass;
	float																	fMinCrashSpeed;
	float																	fMaxCrashSpeed;
	float																	fCollisionDamageFactor;
	float																	fAirControlParam;

	Fvector3																	vVelocity;
	Fvector3																	vPosition;

	Fvector3																	vPathPoint;
	Fvector3																	_vPathDir;
	int																		m_path_size;
	int																		m_start_index;

	float																	m_path_distance;
	U16																		m_material;

	float																	fLastMotionMag;

	float																	fActualVelocity;
	float																	fContactSpeed;
	float																	fLastUpdateTime;

public:
	Fvector3																	vExternalImpulse;
	bool																	bExernalImpulse;
	BOOL																	bSleep;

	BOOL																	gcontact_Was;			// Приземление
	float																	gcontact_Power;			// Насколько сильно ударились
	float																	gcontact_HealthLost;	// Скоко здоровья потеряли

	void							AllocateCharacterObject(CharacterType type);
	void							DeleteCharacterObject( );
	void							CreateCharacter( );
	void							DestroyCharacter( );
	void							Load(const char* section);

#ifdef DEBUG
	void							dbg_Draw( )
	{
		if (m_character)
		{
			m_character->OnRender( );
		}
	};
#endif // def DEBUG

	void							SetPLastMaterialIDX(U16* p)
	{
		m_character->SetPLastMaterialIDX(p);
	}
	dBodyID							GetBody( )
	{
		if (m_character)
		{
			return m_character->get_body( );
		}
		else
		{
			return nullptr;
		}
	}
	const Fvector3& GetVelocity( )
	{
		return vVelocity;
	}
	const Fvector3& GetPathDir( )
	{
		return _vPathDir;
	}
	void							SetPathDir(const Fvector3& v);

	void							GetCharacterVelocity(Fvector3& velocity)
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
	float							GetVelocityMagnitude( )
	{
		return vVelocity.magnitude( );
	}
	float							GetVelocityActual( )
	{
		return fActualVelocity;
	}
	float							GetXZVelocityActual( )
	{
		return dXZMag(vVelocity);
	}
	float							GetActVelProj(const Fvector3& dir)
	{
		return vVelocity.dotproduct(dir);
	}
	float							GetActVelInGoingDir( )
	{
		float r = GetActVelProj(GetPathDir( ));
		return r > 0.0f ? r : 0.0f;
	}
	float							GetXZActVelInGoingDir( )
	{
		float r = dXZDot(GetPathDir( ), vVelocity);
		return r > EPS_L ? r : 0.0f;
	}
	void							GetSmoothedVelocity(Fvector3& v)
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
	float							GetContactSpeed( )
	{
		return fContactSpeed;
	}
	void							GroundNormal(Fvector3& norm);
	CPHSynchronize* GetSyncItem( );
	void							Freeze( );
	void							UnFreeze( );
	void							SetVelocity(float x, float y, float z)
	{
		SetVelocity(Fvector3( ).set(x, y, z));
	}
	void							SetVelocity(const Fvector3& v)
	{
		vVelocity.set(v);
		SetCharacterVelocity(v);
	}
	void							SetCharacterVelocity(const Fvector3& v)
	{
		if (m_character)
		{
			m_character->SetVelocity(v);
		}
	}
	void							SetPhysicsRefObject(CPHShellHolder* ref_object)
	{
		m_character->SetPhysicsRefObject(ref_object);
	}

	void							CalcMaximumVelocity(Fvector3& /**dest/**/, Fvector3& /**accel/**/, float /**friction/**/)
	{ }
	void							CalcMaximumVelocity(float& /**dest/**/, float /**accel/**/, float /**friction/**/)
	{ }
	void							ActivateBox(DWORD id, BOOL Check = false);
	bool							ActivateBoxDynamic(DWORD id, int num_it = 9, int num_steps = 5, float resolve_depth = 0.01f);
	void							InterpolateBox(DWORD id, float k);
	EEnvironment					Environment( )
	{
		return eEnvironment;
	}
	EEnvironment					OldEnvironment( )
	{
		return eOldEnvironment;
	}
	const Fbox3& Box( )
	{
		return aabb;
	}
	DWORD							BoxID( ) const
	{
		return m_dwCurBox;
	}
	const Fbox3* Boxes( )
	{
		return boxes;
	}
	float							FootRadius( )
	{
		if (m_character)
		{
			return m_character->FootRadius( );
		}
		else
		{
			return 0.0f;
		}
	}
	void							CollisionEnable(BOOL enable)
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
	void							SetBox(DWORD id, const Fbox3& BB)
	{
		boxes[id].set(BB);
		aabb.set(BB);
	}
	void							SetMass(float M)
	{
		fMass = M;
		if (m_character)
		{
			m_character->SetMas(fMass);
		}
	}
	float							GetMass( )
	{
		return fMass;
	}
	void							SetCrashSpeeds(float min, float max)
	{
		fMinCrashSpeed = min;
		fMaxCrashSpeed = max;
	}

	void							SetPosition(const Fvector3& P);
	void							GetPosition(Fvector3& P);
	void							GetCharacterPosition(Fvector3& P)
	{
		m_character->GetPosition(P);
	}
	void							InterpolatePosition(Fvector3& P)
	{
		VERIFY(m_character && m_character->b_exist);
		m_character->IPosition(P);
	}
	bool							TryPosition(Fvector3& pos);
	bool							IsCharacterEnabled( )
	{
		return (m_character->IsEnabled( ) || bExernalImpulse);
	}
	void							DisableCharacter( )
	{
		m_character->Disable( );
	}
	void							Calculate(Fvector3& vAccel, const Fvector3& camDir, float ang_speed, float jump, float dt, bool bLight);
	void							Calculate(const xr_vector<DetailPathManager::STravelPathPoint>& path,		//in path
											  float speed,														//in speed
											  u32& travel_point,													//in- travel start, out - current trev point
											  float& precesition													//in- tolerance, out - precesition
	);
	void							AddControlVel(const Fvector3& vel);
	void							SetVelocityLimit(float val);
	float							VelocityLimit( );
	void							PathNearestPoint(const xr_vector<DetailPathManager::STravelPathPoint>& path,		//in path
													 const Fvector3& new_position,										//in position
													 int& index,															//out nearest
													 bool& type															//out type
	);	//return nearest point
	void							PathNearestPointFindUp(const xr_vector<DetailPathManager::STravelPathPoint>& path,		//in path
														   const Fvector3& new_position,										//in position
														   int& index,															//out nearest
														   float radius,														//in exit radius
														   bool& near_line														//out type
	);
	void							PathNearestPointFindDown(const xr_vector<DetailPathManager::STravelPathPoint>& path,		//in path
															 const Fvector3& new_position,										//in position
															 int& index,															//out nearest
															 float radius,														//in exit radius
															 bool& near_line														//out type
	);

	void							PathDIrPoint(const xr_vector<DetailPathManager::STravelPathPoint>& path,		//in path
												 int index,															//in index
												 float distance,														//in distance
												 float precesition,													//in precesition
												 Fvector3& dir														//out dir
	);
	void							PathDIrLine(const xr_vector<DetailPathManager::STravelPathPoint>& path,		//in path
												int index,															//in point
												float distance,														//in distance
												float precesition,													//in precesition
												Fvector3& dir														//out dir
	);
	void							CorrectPathDir(const Fvector3& real_path_dir, const xr_vector<DetailPathManager::STravelPathPoint>& path, int index, Fvector3& corrected_path_dir);

	void							SetApplyGravity(BOOL flag)
	{
		bIsAffectedByGravity = flag;
		if (m_character && m_character->b_exist)
		{
			m_character->SetApplyGravity(flag);
		}
	}
	void							GetDeathPosition(Fvector3& pos)
	{
		m_character->DeathPosition(pos);
	}
	void							SetEnvironment(int enviroment, int old_enviroment);
	void							SetFrictionFactor(float f);
	float							GetFrictionFactor( );
	void							MulFrictionFactor(float f);
	void							ApplyImpulse(const Fvector3& dir, const dReal P);
	void							ApplyHit(const Fvector3& dir, const dReal P, ALife::EHitType hit_type);
	void							SetJumpUpVelocity(float velocity)
	{
		m_character->SetJupmUpVelocity(velocity);
	}
	void							EnableCharacter( )
	{
		if (m_character && m_character->b_exist)
		{
			m_character->Enable( );
		}
	}
	void							SetOjectContactCallback(ObjectContactCallbackFun* callback)
	{
		if (m_character)
		{
			m_character->SetObjectContactCallback(callback);
		}
	}
	void							SetFootCallBack(ObjectContactCallbackFun* callback)
	{
		VERIFY(m_character);
		m_character->SetWheelContactCallback(callback);
	}
	static BOOL						CPHMovementControl::BorderTraceCallback(collide::rq_result& result, LPVOID params);
	ObjectContactCallbackFun* ObjectContactCallback( )
	{
		if (m_character)
		{
			return m_character->ObjectContactCallBack( );
		}
		else
		{
			return nullptr;
		}
	}
	U16								ContactBone( )
	{
		return m_character->ContactBone( );
	}
	const ICollisionDamageInfo* CollisionDamageInfo( ) const
	{
		VERIFY(m_character);
		return m_character->CollisionDamageInfo( );
	}
	void							GetDesiredPos(Fvector3& dpos)
	{
		m_character->GetDesiredPosition(dpos);
	}
	bool							CharacterExist( )
	{
		return (m_character && m_character->b_exist);
	}
	CPHMovementControl(CObject* parent);
	~CPHMovementControl( );
};
