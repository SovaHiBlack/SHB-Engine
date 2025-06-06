#include "stdafx.h"
#include "..\XR_3DA\cl_intersect.h"
#include "alife_space.h"
#include "PHMovementControl.h"
#include "Entity.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "PHAICharacter.h"
#include "PHActorCharacter.h"
#include "PHCapture.h"
#include "AISpace.h"
#include "detail_path_manager.h"
#include "GameMtlLib.h"
#include "Level.h"
#include "ElevatorState.h"
#include "CalculateTriangle.h"
#include "..\XR_3DA\SkeletonCustom.h"

#define def_X_SIZE_2	0.35f
#define def_Y_SIZE_2	0.8f
#define def_Z_SIZE_2	0.35f

const u64 after_creation_collision_hit_block_steps_number = 100;

CPHMovementControl::CPHMovementControl(CObject* parent)
{
	pObject = parent;

#ifdef DEBUG
	if (ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject) && (!!pObject->cName( )) && stricmp(PH_DBG_ObjectTrack( ), *pObject->cName( )) == 0)
	{
		Msg("CPHMovementControl::CPHMovementControl %s (constructor) %f,%f,%pObjectf", PH_DBG_ObjectTrack( ), pObject->Position( ).x, pObject->Position( ).y, pObject->Position( ).z);
	}
#endif

	m_material = 0;
	m_capture = NULL;
	b_exect_position = true;
	m_start_index = 0;
	eOldEnvironment = peInAir;
	eEnvironment = peInAir;
	aabb.set(-def_X_SIZE_2, 0.0f, -def_Z_SIZE_2, def_X_SIZE_2, def_Y_SIZE_2 * 2, def_Z_SIZE_2);
	fMass = 100.0f;
	fMinCrashSpeed = 12.0f;
	fMaxCrashSpeed = 25.0f;
	vVelocity.set(0.0f, 0.0f, 0.0f);
	vPosition.set(0.0f, 0.0f, 0.0f);
	vExternalImpulse.set(0.0f, 0.0f, 0.0f);
	bExernalImpulse = false;
	fLastMotionMag = 1.0f;
	SetPathDir(fVector3( ).set(0.0f, 0.0f, 1.0f));

	bIsAffectedByGravity = TRUE;
	fActualVelocity = 0.0f;
	m_fGroundDelayFactor = 1.0f;
	gcontact_HealthLost = 0.0f;

	fContactSpeed = 0.0f;
	fAirControlParam = 0.0f;
	m_character = NULL;
	m_dwCurBox = 0xffffffff;
	fCollisionDamageFactor = 1.0f;
	in_dead_area_count = 0;
}

CPHMovementControl::~CPHMovementControl(void)
{
	if (m_character)
	{
		m_character->Destroy( );
	}

	DeleteCharacterObject( );
	xr_delete(m_capture);
}

void CPHMovementControl::AddControlVel(const fVector3& vel)
{
	vExternalImpulse.add(vel);
	bExernalImpulse = true;
}
void CPHMovementControl::ApplyImpulse(const fVector3& dir, const dReal P)
{
	if (fis_zero(P))
	{
		return;
	}

	fVector3 force;
	force.set(dir);
	force.mul(P / fixed_step);

	AddControlVel(force);
	m_character->ApplyImpulse(dir, P);
}
void CPHMovementControl::SetVelocityLimit(f32 val)
{
	if (m_character)m_character->SetMaximumVelocity(val);
}
f32 CPHMovementControl::VelocityLimit( )
{
	if (!m_character || !m_character->b_exist) return 0.f;
	return m_character->GetMaximumVelocity( );
}

void CPHMovementControl::in_shedule_Update(u32 DT)
{
	if (m_capture)
	{
		if (m_capture->Failed( )) xr_delete(m_capture);
	}
}

void CPHMovementControl::Calculate(fVector3& vAccel, const fVector3& camDir, f32 /**ang_speed/**/, f32 jump, f32 /**dt/**/, bool /**bLight/**/)
{
	fVector3 previous_position;
	previous_position.set(vPosition);
	m_character->IPosition(vPosition);
	if (bExernalImpulse)
	{

		vAccel.add(vExternalImpulse);
		m_character->ApplyForce(vExternalImpulse);
		vExternalImpulse.set(0.f, 0.f, 0.f);

		bExernalImpulse = false;
	}
	//vAccel.y=jump;
	f32 mAccel = vAccel.magnitude( );
	m_character->SetCamDir(camDir);
	m_character->SetMaximumVelocity(mAccel / 10.f);
	//if(!fis_zero(mAccel))vAccel.mul(1.f/mAccel);
	m_character->SetAcceleration(vAccel);
	if (!fis_zero(jump)) m_character->Jump(vAccel);

	m_character->GetSavedVelocity(vVelocity);
	fActualVelocity = vVelocity.magnitude( );
	//Msg("saved avel %f", fActualVelocity);
	gcontact_Was = m_character->ContactWas( );
	fContactSpeed = 0.f;
	const ICollisionDamageInfo* di = m_character->CollisionDamageInfo( );
	{
		fContactSpeed = di->ContactVelocity( );

		gcontact_Power = fContactSpeed / fMaxCrashSpeed;

		gcontact_HealthLost = 0.0f;
		if (fContactSpeed > fMinCrashSpeed)
		{
			gcontact_HealthLost = ((fContactSpeed - fMinCrashSpeed) / (fMaxCrashSpeed - fMinCrashSpeed));
		}
	}

	if (m_character->LastMaterialIDX( ) != u16(-1))
	{
		const SGameMtl* last_material = GMLib.GetMaterialByIdx(m_character->LastMaterialIDX( ));
		if (last_material->Flags.test(SGameMtl::flInjurious))
		{
			gcontact_HealthLost += Device.fTimeDelta * last_material->fInjuriousSpeed;
		}
	}

	//CPhysicsShellHolder * O=di->DamageObject();
	//SCollisionHitCallback* cc= O ? O->get_collision_hit_callback() : NULL;
	const ICollisionDamageInfo* cdi = CollisionDamageInfo( );
	if (cdi->HitCallback( ))cdi->HitCallback( )->call(static_cast<CGameObject*>(m_character->PhysicsRefObject( )), fMinCrashSpeed, fMaxCrashSpeed, fContactSpeed, gcontact_HealthLost, CollisionDamageInfo( ));

	TraceBorder(previous_position);
	CheckEnvironment(vPosition);
	bSleep = false;
	m_character->Reinit( );
}

void CPHMovementControl::Calculate(const xr_vector<DetailPathManager::STravelPathPoint>& path, f32 speed, u32& travel_point, f32& precision)
{

#ifdef DEBUG
	if (ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject) && (!!pObject->cName( )) && stricmp(PH_DBG_ObjectTrack( ), *pObject->cName( )) == 0)
	{
		Msg("CPHMovementControl::Calculate in %s (Object Position) %f,%f,%f", PH_DBG_ObjectTrack( ), pObject->Position( ).x, pObject->Position( ).y, pObject->Position( ).z);
		Msg("CPHMovementControl::Calculate in %s (CPHMovementControl::vPosition) %f,%f,%f", PH_DBG_ObjectTrack( ), vPosition.x, vPosition.y, vPosition.z);
	}
#endif

	if (!m_character->b_exist)
	{
		return;
	}

	fVector3 new_position;
	m_character->IPosition(new_position);

	int index = 0;//nearest point

	bool  near_line;
	m_path_size = path.size( );
	fVector3 dir;
	dir.set(0.0f, 0.0f, 0.0f);
	if (m_path_size == 0)
	{
		speed = 0;
		vPosition.set(new_position);
	}
	else if (b_exect_position)
	{
		m_start_index = travel_point;

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		if ((m_path_size - 1) > (int)travel_point)
			dir.sub(path[travel_point + 1].position, path[travel_point].position);
		else
			dir.sub(path[travel_point].position, new_position);
		m_start_index = travel_point;
		dir.y = 0.f;
		dir.normalize_safe( );
		vPosition.set(new_position);
		m_path_distance = 0;
		SetPathDir(dir);
		vPathPoint.set(vPosition);
	}
	else
	{
		fVector3 dif;

		dif.sub(new_position, vPathPoint);
		f32 radius = dif.magnitude( ) * 2.f;
		if (m_path_size == 1)
		{
			speed = 0.f;
			vPosition.set(new_position);	//todo - insert it in PathNearestPoint
			index = 0;
			vPathPoint.set(path[0].position);
			fVector3 _d;
			_d.sub(path[0].position, new_position);
			SetPathDir(_d);
			m_path_distance = GetPathDir( ).magnitude( );
			if (m_path_distance > EPSILON_5)
			{
				fVector3 _d = GetPathDir( );
				_d.mul(1.0f / m_path_distance);
				SetPathDir(_d);
			}

			near_line = false;
		}
		else
		{
			m_path_distance = dInfinity;
			near_line = true;
			if (m_start_index < m_path_size)
			{
				PathNearestPointFindUp(path, new_position, index, radius, near_line);
				PathNearestPointFindDown(path, new_position, index, radius, near_line);
			}
			if (m_path_distance > radius)
			{
				m_start_index = 0;
				PathNearestPoint(path, new_position, index, near_line);
			}
			vPosition.set(new_position);//for PathDirLine && PathDirPoint
			if (near_line) PathDIrLine(path, index, m_path_distance, precision, dir);
			else		  PathDIrPoint(path, index, m_path_distance, precision, dir);

			travel_point = (u32)index;
			m_start_index = index;
			if (fis_zero(speed)) dir.set(0, 0, 0);
		}
	}

	dir.y = 0.f;
	//VERIFY(!(fis_zero(dir.magnitude())&&!fis_zero(speed)));
	dir.normalize_safe( );

	/////////////////////////////////////////////////////////////////
	if (bExernalImpulse)
	{
		//vAccel.add(vExternalImpulse);
		fVector3 V;
		V.set(dir);
		//V.mul(speed*fMass/fixed_step);
		V.mul(speed * 10.f);
		V.add(vExternalImpulse);
		m_character->ApplyForce(vExternalImpulse);
		speed = V.magnitude( );

		if (!fis_zero(speed))
		{
			dir.set(V);
			dir.mul(1.f / speed);
		}
		speed /= 10.f;
		vExternalImpulse.set(0.f, 0.f, 0.f);
		bExernalImpulse = false;
	}
	/////////////////////////
	//if(!PhyssicsOnlyMode()){
	//	fVector3	v;//m_character->GetVelocity(v);
	//	v.mul(dir,speed);
	//	SetVelocity(v);//hk
	//	
	//}
	/////////////////////////
	m_character->SetMaximumVelocity(speed);
	m_character->SetAcceleration(dir);
	//////////////////////////////////////////////////////
	m_character->GetSmothedVelocity(vVelocity);
	fActualVelocity = vVelocity.magnitude( );

	gcontact_Was = m_character->ContactWas( );
	const ICollisionDamageInfo* di = m_character->CollisionDamageInfo( );
	fContactSpeed = 0.f;
	{
		fContactSpeed = di->ContactVelocity( );
		gcontact_Power = fContactSpeed / fMaxCrashSpeed;
		gcontact_HealthLost = 0;
		if (fContactSpeed > fMinCrashSpeed)
		{
			gcontact_HealthLost =
				((fContactSpeed - fMinCrashSpeed)) / (fMaxCrashSpeed - fMinCrashSpeed);
		}
	}

	CheckEnvironment(vPosition);
	bSleep = false;
	b_exect_position = false;
	//m_character->Reinit();
}

void CPHMovementControl::PathNearestPoint(const xr_vector<DetailPathManager::STravelPathPoint>& path,			//in path
										  const fVector3& new_position,  //in position
										  int& index,			//in start from; out nearest
										  bool& near_line       //out type
)
{

	fVector3 from_first;
	fVector3 from_second;
	fVector3 dir;
	bool after_line = true;//to check first point

	fVector3 path_point;
	fVector3 vtemp;
	f32 temp;

	for (int i = 0; i < m_path_size - 1; ++i)
	{
		const fVector3& first = path[i].position;
		const fVector3& second = path[i + 1].position;
		from_first.sub(new_position, first);
		from_second.sub(new_position, second);
		dir.sub(second, first);
		dir.normalize_safe( );

		if (from_first.dotproduct(dir) < 0.f)//befor this line
		{
			if (after_line)//after previous line && befor this line = near first point
			{
				vtemp.sub(new_position, first);
				temp = vtemp.magnitude( );
				if (temp < m_path_distance)
				{
					m_path_distance = temp;
					index = i;
					vPathPoint.set(first);
					SetPathDir(dir);
					near_line = false;
				}
			}
			after_line = false;
		}
		else //after first 
		{
			if (from_second.dotproduct(dir) < 0.f) //befor second && after first = near line
			{
				//temp=dir.dotproduct(new_position); seems to be wrong
				temp = dir.dotproduct(from_first);
				vtemp.set(dir);
				vtemp.mul(temp);
				path_point.add(vtemp, first);
				vtemp.sub(path_point, new_position);
				temp = vtemp.magnitude( );
				if (temp < m_path_distance)
				{
					m_path_distance = temp;
					index = i;
					vPathPoint.set(path_point);
					SetPathDir(dir);
					near_line = true;
				}
			}
			else							//after second = after this line
			{
				after_line = true;
			}
		}
	}

	if (m_path_distance == dInfinity)	//after whall path
	{
		R_ASSERT2(after_line, "Must be after line");
		vtemp.sub(new_position, path[i].position);
		m_path_distance = vtemp.magnitude( );
		SetPathDir(dir);
		vPathPoint.set(path[i].position);
		index = i;
		near_line = false;
	}
#ifdef DEBUG
	if (ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject) && (!!pObject->cName( )) && stricmp(PH_DBG_ObjectTrack( ), *pObject->cName( )) == 0)
	{
		Msg("CPHMovementControl::Calculate out %s (Object Position) %f,%f,%f", PH_DBG_ObjectTrack( ), pObject->Position( ).x, pObject->Position( ).y, pObject->Position( ).z);
		Msg("CPHMovementControl::Calculate out %s (CPHMovementControl::vPosition) %f,%f,%f", PH_DBG_ObjectTrack( ), vPosition.x, vPosition.y, vPosition.z);
	}
#endif
	return;
}



void CPHMovementControl::PathNearestPointFindUp(const xr_vector<DetailPathManager::STravelPathPoint>& path,			//in path
												const fVector3& new_position,  //in position
												int& index,			//in start from; out nearest
												f32							radius,	//out m_path_distance in exit radius
												bool& near_line      //out type
)
{

	fVector3 from_first;
	fVector3 from_second;
	fVector3 dir;
	bool after_line = true;//to check first point

	fVector3 path_point;
	fVector3 vtemp;
	f32 temp;
	dir.set(0.0f, 0.0f, 1.0f);

	for (int i = m_start_index; i < m_path_size - 1; ++i)
	{
		const fVector3& first = path[i].position;
		const fVector3& second = path[i + 1].position;
		from_first.sub(new_position, first);
		from_second.sub(new_position, second);
		dir.sub(second, first);
		dir.normalize_safe( );
		f32 from_first_dir = from_first.dotproduct(dir);
		f32 from_second_dir = from_second.dotproduct(dir);

		if (from_first_dir < 0.f)//before this line
		{
			temp = from_first.magnitude( );
			if (after_line)//after previous line && before this line = near first point
			{
				if (temp < m_path_distance)
				{
					m_path_distance = temp;
					index = i;
					vPathPoint.set(first);
					SetPathDir(dir);
					near_line = false;
				}
			}

			if (temp > radius) break;//exit test
			after_line = false;

		}
		else //after first 
		{
			if (from_second_dir < 0.f) //befor second && after first = near line
			{
				vtemp.set(dir);
				vtemp.mul(from_first_dir);
				path_point.add(vtemp, first);
				vtemp.sub(path_point, new_position);
				temp = vtemp.magnitude( );
				if (temp < m_path_distance)
				{
					m_path_distance = temp;
					index = i;
					vPathPoint.set(path_point);
					SetPathDir(dir);
					near_line = true;
				}
				if (temp > radius) break;//exit test
			}
			else							//after second = after this line
			{
				after_line = true;
				if (from_second.magnitude( ) > radius) break;//exit test
			}
		}
	}

	if (m_path_distance == dInfinity && i == m_path_size - 1)
	{
		R_ASSERT2(after_line, "Must be after line");
		vtemp.sub(new_position, path[i].position);
		m_path_distance = vtemp.magnitude( );
		SetPathDir(dir);
		vPathPoint.set(path[i].position);
		index = i;
		near_line = false;
	}

	return;
}

void CPHMovementControl::PathNearestPointFindDown(const xr_vector<DetailPathManager::STravelPathPoint>& path,			//in path
												  const fVector3& new_position,  //in position
												  int& index,			//in start from; out nearest
												  f32							radius,	//out m_path_distance in exit radius
												  bool& near_line      //out type
)
{

	fVector3 from_first;
	fVector3 from_second;
	fVector3 dir;
	bool after_line = true;//to check first point

	fVector3 path_point;
	fVector3 vtemp;
	f32 temp;
	//(going down)
	dir.set(0, 0, 1);
	for (int i = m_start_index; i > 1; --i)
	{
		const fVector3& first = path[i - 1].position;
		const fVector3& second = path[i].position;
		from_first.sub(new_position, first);
		from_second.sub(new_position, second);
		dir.sub(second, first);
		dir.normalize_safe( );
		f32 from_first_dir = from_first.dotproduct(dir);
		f32 from_second_dir = from_second.dotproduct(dir);

		if (from_second_dir > 0.f)//befor this line
		{
			temp = from_second.magnitude( );
			if (after_line)//after previous line && befor this line = near second point (going down)
			{
				if (temp < m_path_distance)
				{
					m_path_distance = temp;
					index = i;
					vPathPoint.set(second);
					SetPathDir(dir);
					near_line = false;
				}
			}

			if (temp > radius) break;//exit test
			after_line = false;
		}
		else //after second
		{
			if (from_first_dir > 0.f) //after second && before first = near line (going down)
			{
				vtemp.set(dir);
				vtemp.mul(from_second_dir);
				path_point.add(second, vtemp); //from_second_dir <0.f !!
				vtemp.sub(path_point, new_position);
				temp = vtemp.magnitude( );
				if (temp < m_path_distance)
				{
					m_path_distance = temp;
					index = i - 1;
					vPathPoint.set(path_point);
					SetPathDir(dir);
					near_line = true;
				}
				if (temp > radius) break;//exit test
			}
			else							//after first = after this line(going down)
			{
				after_line = true;
				if (from_first.magnitude( ) > radius) break;//exit test
			}
		}
	}

	if (m_path_distance == dInfinity && i == 1)
	{

		R_ASSERT2(after_line, "Must be after line");
		vtemp.sub(new_position, path[i].position);
		m_path_distance = vtemp.magnitude( );
		SetPathDir(dir);
		vPathPoint.set(path[i].position);
		index = i;
		near_line = false;
	}

	return;
}

void		CPHMovementControl::CorrectPathDir(const fVector3& real_path_dir, const xr_vector<DetailPathManager::STravelPathPoint>& path, int index, fVector3& corrected_path_dir)
{
	const f32 epsilon = 0.1f;
	f32 plane_motion = dXZMag(real_path_dir);
	if (fis_zero(plane_motion, epsilon))
	{
		if (!fis_zero(plane_motion, EPSILON_5))
		{
			corrected_path_dir.set(real_path_dir);
			corrected_path_dir.y = 0.f;
			corrected_path_dir.mul(1.f / plane_motion);
		}
		else if (index != m_path_size - 1)
		{
			corrected_path_dir.sub(path[index + 1].position, path[index].position);
			corrected_path_dir.normalize_safe( );
			CorrectPathDir(corrected_path_dir, path, index + 1, corrected_path_dir);
		}
		else
		{
			corrected_path_dir.set(real_path_dir);
		}
	}
	else
	{
		corrected_path_dir.set(real_path_dir);
	}
}

void CPHMovementControl::PathDIrLine(const xr_vector<DetailPathManager::STravelPathPoint>& path, int index, f32 distance, f32 precesition, fVector3& dir)
{
	fVector3 to_path_point;
	fVector3 corrected_path_dir;
	CorrectPathDir(GetPathDir( ), path, index, corrected_path_dir);
	to_path_point.sub(vPathPoint, vPosition);	//_new position
	f32 mag = to_path_point.magnitude( );
	if (mag < EPSILON_5)
	{
		dir.set(corrected_path_dir);
		return;
	}
	to_path_point.mul(1.f / mag);
	if (mag > FootRadius( ))to_path_point.mul(precesition);
	else to_path_point.mul(mag * precesition);
	dir.add(corrected_path_dir, to_path_point);
	dir.normalize_safe( );
}

void CPHMovementControl::PathDIrPoint(const xr_vector<DetailPathManager::STravelPathPoint>& path, int index, f32 distance, f32 precesition, fVector3& dir)
{
	fVector3 to_path_point;
	fVector3 corrected_path_dir;
	CorrectPathDir(GetPathDir( ), path, index, corrected_path_dir);
	to_path_point.sub(vPathPoint, vPosition);	//_new position
	f32 mag = to_path_point.magnitude( );

	if (mag < EPSILON_5) //near the point
	{
		if (0 == index || m_path_size - 1 == index) //on path eidge
		{
			dir.set(corrected_path_dir);//??
			return;
		}
		dir.sub(path[index].position, path[index - 1].position);
		dir.normalize_safe( );
		dir.add(corrected_path_dir);
		dir.normalize_safe( );
	}
	to_path_point.mul(1.f / mag);
	if (m_path_size - 1 == index)//on_path_edge
	{
		dir.set(to_path_point);
		return;
	}

	if (mag < EPSILON_5 || fis_zero(dXZMag(to_path_point), EPSILON_5))
	{
		dir.set(corrected_path_dir);
		return;//mean dir
	}

	fVector3 tangent;
	tangent.crossproduct(fVector3( ).set(0.0f, 1.0f, 0.0f), to_path_point);

	VERIFY(!fis_zero(tangent.magnitude( )));
	tangent.normalize( );
	if (dir.square_magnitude( ) > EPSILON_5)
	{
		if (tangent.dotproduct(dir) < 0.0f)
		{
			tangent.invert( );
		}
	}
	else
	{
		if (tangent.dotproduct(corrected_path_dir) < 0.0f)
		{
			tangent.invert( );
		}
	}

	if (mag > FootRadius( ))
	{
		to_path_point.mul(precesition);
	}
	else
	{
		to_path_point.mul(mag * precesition);
	}

	dir.add(tangent, to_path_point);
	dir.normalize_safe( );
}

void CPHMovementControl::SetActorRestrictorRadius(CPHCharacter::ERestrictionType rt, f32 r)
{
	if (m_character && eCharacterType == actor)
	{
		static_cast<CPHActorCharacter*>(m_character)->SetRestrictorRadius(rt, r);
	}
}

void CPHMovementControl::Load(pcstr section)
{
	fBox3	bb;

	// m_PhysicMovementControl: BOX
	fVector3	vBOX1_center = pSettings->r_fvector3(section, "ph_box1_center");
	fVector3	vBOX1_size = pSettings->r_fvector3(section, "ph_box1_size");
	bb.set(vBOX1_center, vBOX1_center); bb.grow(vBOX1_size);
	SetBox(1, bb);

	// m_PhysicMovementControl: BOX
	fVector3	vBOX0_center = pSettings->r_fvector3(section, "ph_box0_center");
	fVector3	vBOX0_size = pSettings->r_fvector3(section, "ph_box0_size");
	bb.set(vBOX0_center, vBOX0_center); bb.grow(vBOX0_size);
	SetBox(0, bb);

	// m_PhysicMovementControl: Crash speed and mass
	f32	cs_min = pSettings->r_float(section, "ph_crash_speed_min");
	f32	cs_max = pSettings->r_float(section, "ph_crash_speed_max");
	f32	mass = pSettings->r_float(section, "ph_mass");

	xr_token retrictor_types[ ] = {
		{ "actor",			CPHCharacter::rtActor},
		{ "medium_monster",	CPHCharacter::rtMonsterMedium},
		{ "stalker",		CPHCharacter::rtStalker	},
		{ "none",			CPHCharacter::rtNone	},
		{ 0,							0}
	};

	if (pSettings->line_exist(section, "actor_restrictor"))
	{
		SetRestrictionType(CPHCharacter::ERestrictionType(pSettings->r_token(section, "actor_restrictor", retrictor_types)));
	}

	fCollisionDamageFactor = READ_IF_EXISTS(pSettings, r_float, section, "ph_collision_damage_factor", fCollisionDamageFactor);
	R_ASSERT3(fCollisionDamageFactor <= 1.0f, "ph_collision_damage_factor >1.", section);
	SetCrashSpeeds(cs_min, cs_max);
	SetMass(mass);
}

void CPHMovementControl::CheckEnvironment(const fVector3&/**V/**/)
{
	eOldEnvironment = eEnvironment;
	switch (m_character->CheckInvironment( ))
	{
		case peOnGround:
		{
			eEnvironment = peOnGround;
		}
		break;
		case peInAir:
		{
			eEnvironment = peInAir;
		}
		break;
		case peAtWall:
		{
			eEnvironment = peAtWall;
		}
		break;
	}
}

void CPHMovementControl::GroundNormal(fVector3& norm)
{
	if (m_character && m_character->b_exist)
	{
		m_character->GroundNormal(norm);
	}
	else
	{
		norm.set(0.0f, 1.0f, 0.0f);
	}
}

void CPHMovementControl::SetEnvironment(s32 enviroment, s32 old_enviroment)
{
	switch (enviroment)
	{
		case 0:
		{
			eEnvironment = peOnGround;
		}
		break;
		case 1:
		{
			eEnvironment = peAtWall;
		}
		break;
		case 2:
		{
			eEnvironment = peInAir;
		}
	}

	switch (old_enviroment)
	{
		case 0:
		{
			eOldEnvironment = peOnGround;
		}
		break;
		case 1:
		{
			eOldEnvironment = peAtWall;
		}
		break;
		case 2:
		{
			eOldEnvironment = peInAir;
		}
	}
}

void	CPHMovementControl::SetPosition(const fVector3& P)
{

#ifdef DEBUG
	if (ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject) && (!!pObject->cName( )) && stricmp(PH_DBG_ObjectTrack( ), *pObject->cName( )) == 0)
	{
		Msg("CPHMovementControl::SetPosition %s (Object Position) %f,%f,%f", PH_DBG_ObjectTrack( ), pObject->Position( ).x, pObject->Position( ).y, pObject->Position( ).z);
		Msg("CPHMovementControl::SetPosition %s (CPHMovementControl::vPosition) %f,%f,%f", PH_DBG_ObjectTrack( ), vPosition.x, vPosition.y, vPosition.z);
	}
#endif

	vPosition.set(P);  m_character->SetPosition(vPosition);
}

bool		CPHMovementControl::TryPosition(fVector3& pos)
{
	VERIFY_BOUNDARIES2(pos, phBoundaries, m_character->PhysicsRefObject( ), "CPHMovementControl::TryPosition	arqument pos");

#ifdef DEBUG
	if (ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject) && (!!pObject->cName( )) && stricmp(PH_DBG_ObjectTrack( ), *pObject->cName( )) == 0)
	{
		Msg("CPHMovementControl::TryPosition %s (Object Position) %f,%f,%f", PH_DBG_ObjectTrack( ), pObject->Position( ).x, pObject->Position( ).y, pObject->Position( ).z);
		Msg("CPHMovementControl::TryPosition %s (CPHMovementControl::vPosition) %f,%f,%f", PH_DBG_ObjectTrack( ), vPosition.x, vPosition.y, vPosition.z);
	}
#endif

	if (m_character->b_exist)
	{
		bool					ret = m_character->TryPosition(pos, b_exect_position) && !bExernalImpulse;
		m_character->GetPosition(vPosition);
		return					(ret);
	}

	vPosition.set(pos);
	return			(true);
}

void		CPHMovementControl::GetPosition(fVector3& P)
{
	VERIFY_BOUNDARIES2(P, phBoundaries, m_character->PhysicsRefObject( ), "CPHMovementControl::GetPosition	arqument pos");

#ifdef DEBUG
	if (ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject) && (!!pObject->cName( )) && stricmp(PH_DBG_ObjectTrack( ), *pObject->cName( )) == 0)
	{
		Msg("CPHMovementControl::GetPosition %s (Object Position) %f,%f,%f", PH_DBG_ObjectTrack( ), pObject->Position( ).x, pObject->Position( ).y, pObject->Position( ).z);
		Msg("CPHMovementControl::GetPosition %s (CPHMovementControl::vPosition) %f,%f,%f", PH_DBG_ObjectTrack( ), vPosition.x, vPosition.y, vPosition.z);
	}
#endif
	P.set(vPosition);
	VERIFY_BOUNDARIES2(vPosition, phBoundaries, m_character->PhysicsRefObject( ), "CPHMovementControl::GetPosition	out pos");
}

void	CPHMovementControl::AllocateCharacterObject(CharacterType type)
{
	switch (type)
	{
		case actor:			m_character = xr_new<CPHActorCharacter>( );	break;
		case ai:			m_character = xr_new<CPHAICharacter>( );	break;
	}
	eCharacterType = type;
	m_character->SetMas(fMass);
	m_character->SetPosition(vPosition);
#ifdef DEBUG
	if (ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject) && (!!pObject->cName( )) && stricmp(PH_DBG_ObjectTrack( ), *pObject->cName( )) == 0)
	{
		Msg("CPHMovementControl::AllocateCharacterObject %s (Object Position) %f,%f,%f", PH_DBG_ObjectTrack( ), pObject->Position( ).x, pObject->Position( ).y, pObject->Position( ).z);
		Msg("CPHMovementControl::AllocateCharacterObject %s (CPHMovementControl::vPosition) %f,%f,%f", PH_DBG_ObjectTrack( ), vPosition.x, vPosition.y, vPosition.z);
	}
#endif
}

void	CPHMovementControl::PHCaptureObject(CPhysicsShellHolder* object)
{
	if (m_capture) return;

	if (!object || !object->PPhysicsShell( ) || !object->m_pPhysicsShell->isActive( )) return;
	m_capture = xr_new<CPHCapture>(m_character,
								   object
	);
}

void	CPHMovementControl::PHCaptureObject(CPhysicsShellHolder* object, u16 element)
{
	if (m_capture) return;

	if (!object || !object->PPhysicsShell( ) || !object->PPhysicsShell( )->isActive( )) return;
	m_capture = xr_new<CPHCapture>(m_character,
								   object,
								   element
	);
}

fVector3 CPHMovementControl::PHCaptureGetNearestElemPos(const CPhysicsShellHolder* object)
{
	R_ASSERT3((object->m_pPhysicsShell != NULL), "NO Phisics Shell for object ", *object->cName( ));

	CPhysicsElement* ph_elem = object->m_pPhysicsShell->NearestToPoint(vPosition);

	fVector3 v;
	ph_elem->GetGlobalPositionDynamic(&v);

	return v;
}

fMatrix4x4 CPHMovementControl::PHCaptureGetNearestElemTransform(CPhysicsShellHolder* object)
{
	CPhysicsElement* ph_elem = object->m_pPhysicsShell->NearestToPoint(vPosition);

	fMatrix4x4 m;
	ph_elem->GetGlobalTransformDynamic(&m);

	return m;
}

void CPHMovementControl::PHReleaseObject( )
{
	if (m_capture) m_capture->Release( );
}

void	CPHMovementControl::DestroyCharacter( )
{
	m_character->Destroy( );
	xr_delete(m_capture);
	//xr_delete<CPHSimpleCharacter>(m_character);
}

void	CPHMovementControl::DeleteCharacterObject( )
{
	xr_delete(m_character);
	xr_delete(m_capture);
}

void CPHMovementControl::JumpV(const fVector3& jump_velocity)
{
	m_character->Enable( );
	m_character->Jump(jump_velocity);
}

void CPHMovementControl::Jump(const fVector3& end_point, f32 time)
{
	//vPosition
	Jump(smart_cast<CGameObject*>(m_character->PhysicsRefObject( ))->Position( ), end_point, time);
}

void CPHMovementControl::Jump(const fVector3& start_point, const fVector3& end_point, f32 time)
{
	fVector3 velosity;
	velosity.sub(end_point, start_point);
	TransferenceToThrowVel(velosity, time, ph_world->Gravity( ));
	JumpV(velosity);
}

f32 CPHMovementControl::Jump(const fVector3& end_point)
{
	f32 time = JumpMinVelTime(end_point);
	Jump(smart_cast<CGameObject*>(m_character->PhysicsRefObject( ))->Position( ), end_point, time);
	return time;
}

void CPHMovementControl::GetJumpMinVelParam(fVector3& min_vel, f32& time, JumpType& type, const fVector3& end_point)
{
	time = JumpMinVelTime(end_point);
	GetJumpParam(min_vel, type, end_point, time);
}

f32 CPHMovementControl::JumpMinVelTime(const fVector3& end_point)
{
	return ThrowMinVelTime(fVector3( ).sub(end_point, smart_cast<CGameObject*>(m_character->PhysicsRefObject( ))->Position( )), ph_world->Gravity( ));
}

void CPHMovementControl::GetJumpParam(fVector3& velocity, JumpType& type, const fVector3& end_point, f32 time)
{
	fVector3 velosity;
	velosity.sub(smart_cast<CGameObject*>(m_character->PhysicsRefObject( ))->Position( ), end_point);
	TransferenceToThrowVel(velosity, time, ph_world->Gravity( ));
	if (velocity.y < 0.0f)
	{
		type = jtStrait;
		return;
	}

	f32 rise_time = velosity.y / ph_world->Gravity( );
	if (_abs(rise_time - time) < EPSILON_3)
	{
		type = jtHigh;
	}
	else if (rise_time > time)
	{
		type = jtStrait;
	}
	else
	{
		type = jtCurved;
	}
}

void CPHMovementControl::SetMaterial(u16 material)
{
	m_material = material;
	if (m_character)
	{
		m_character->SetMaterial(material);
	}
}

void CPHMovementControl::CreateCharacter( )
{
	dVector3 size = { aabb.x2 - aabb.x1,aabb.y2 - aabb.y1,aabb.z2 - aabb.z1 };
	m_character->Create(size);
	m_character->SetMaterial(m_material);
	m_character->SetAirControlFactor(fAirControlParam);
#ifdef DEBUG
	if (ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject) && (!!pObject->cName( )) && stricmp(PH_DBG_ObjectTrack( ), *pObject->cName( )) == 0)
	{
		Msg("CPHMovementControl::CreateCharacter %s (Object Position) %f,%f,%f", PH_DBG_ObjectTrack( ), pObject->Position( ).x, pObject->Position( ).y, pObject->Position( ).z);
		Msg("CPHMovementControl::CreateCharacter %s (CPHMovementControl::vPosition) %f,%f,%f", PH_DBG_ObjectTrack( ), vPosition.x, vPosition.y, vPosition.z);
	}
#endif
	m_character->SetPosition(vPosition);
	m_character->SetCollisionDamageFactor(fCollisionDamageFactor * fCollisionDamageFactor);
	trying_times[0] = trying_times[1] = trying_times[2] = trying_times[3] = u32(-1);
	trying_poses[0].set(vPosition);
	trying_poses[1].set(vPosition);
	trying_poses[2].set(vPosition);
	trying_poses[3].set(vPosition);
}

CPHSynchronize* CPHMovementControl::GetSyncItem( )
{
	if (m_character)	return smart_cast<CPHSynchronize*>(m_character);
	else			return 0;
}

void CPHMovementControl::Freeze( )
{
	if (m_character)m_character->Freeze( );
}

void CPHMovementControl::UnFreeze( )
{
	if (m_character) m_character->UnFreeze( );
}

void CPHMovementControl::ActivateBox(DWORD id, BOOL Check/*false*/)
{
	if (Check && (m_dwCurBox == id)) return;
	m_dwCurBox = id;
	aabb.set(boxes[id]);
	if (!m_character || !m_character->b_exist) return;
	dVector3 size = { aabb.x2 - aabb.x1,aabb.y2 - aabb.y1,aabb.z2 - aabb.z1 };
	m_character->SetBox(size);
	//fVector3 v;
	//m_character->GetVelocity(v);
	//m_character->Destroy();
	//CreateCharacter();	
	//m_character->SetVelocity(v);
	//m_character->SetPosition(vPosition);	
}

void CPHMovementControl::InterpolateBox(DWORD id, f32 k)
{
	if (m_dwCurBox == id) return;
	if (!m_character || !m_character->b_exist) return;
	dVector3 size = { aabb.x2 - aabb.x1,aabb.y2 - aabb.y1,aabb.z2 - aabb.z1 };
	dVector3 to_size = { boxes[id].x2 - boxes[id].x1,boxes[id].y2 - boxes[id].y1,boxes[id].z2 - boxes[id].z1 };
	dVectorInterpolate(size, to_size, k);
	m_character->SetBox(size);
}

void CPHMovementControl::ApplyHit(const fVector3& dir, const dReal P, ALife::EHitType hit_type)
{
	if (hit_type == ALife::eHitTypeExplosion || hit_type == ALife::eHitTypeWound)
		ApplyImpulse(dir, P);
}

void CPHMovementControl::SetFrictionFactor(f32 f)
{
	m_character->FrictionFactor( ) = f;
}

f32 CPHMovementControl::GetFrictionFactor( )
{
	return m_character->FrictionFactor( );
}

void CPHMovementControl::MulFrictionFactor(f32 f)
{
	m_character->FrictionFactor( ) *= f;
}

CElevatorState* CPHMovementControl::ElevatorState( )
{
	if (!m_character || !m_character->b_exist)return NULL;
	return m_character->ElevatorState( );
	//m_character->SetElevator()
}

struct STraceBorderQParams
{
	CPHMovementControl* m_movement;
	const fVector3& m_dir;
	STraceBorderQParams(CPHMovementControl* movement, const fVector3& dir) : m_dir(dir)
	{
		m_movement = movement;
	}
	STraceBorderQParams& operator = (STraceBorderQParams& p)
	{
		VERIFY(FALSE);
		return p;
	}
};

BOOL CPHMovementControl::BorderTraceCallback(collide::rq_result& result, LPVOID params)
{
	STraceBorderQParams& p = *(STraceBorderQParams*)params;
	u16 mtl_idx = GAMEMTL_NONE_IDX;
	CDB::TRI* T = NULL;
	if (result.O)
	{
		return true;
	}
	else
	{
		//�������� ����������� � ������ ��� ��������
		T = Level( ).ObjectSpace.GetStaticTris( ) + result.element;
		mtl_idx = T->material;
	}

	VERIFY(T);
	SGameMtl* mtl = GMLib.GetMaterialByIdx(mtl_idx);
	if (mtl->Flags.test(SGameMtl::flInjurious))
	{
		fVector3 tri_norm;
		GetNormal(T, tri_norm);
		if (p.m_dir.dotproduct(tri_norm) < 0.0f)
		{
			p.m_movement->in_dead_area_count++;
		}
		else
		{
			p.m_movement->in_dead_area_count--;
		}
	}

	return true;
}

void	CPHMovementControl::TraceBorder(const fVector3& prev_position)
{
	const fVector3& from_pos = prev_position;
	const fVector3& to_position = vPosition;
	fVector3 dir;
	dir.sub(to_position, from_pos);
	f32 sq_mag = dir.square_magnitude( );
	if (sq_mag == 0.0f)
	{
		return;
	}

	f32 mag = _sqrt(sq_mag);
	dir.mul(1.0f / mag);
	collide::ray_defs	RD(from_pos, dir, mag, 0, collide::rqtStatic);
	VERIFY(!fis_zero(RD.dir.square_magnitude( )));

	STraceBorderQParams			p(this, dir);
	storage.r_clear( );
	g_pGameLevel->ObjectSpace.RayQuery(storage, RD, BorderTraceCallback, &p, NULL, static_cast<CObject*>(m_character->PhysicsRefObject( )));
}

void	CPHMovementControl::UpdateObjectBox(CPHCharacter* ach)
{
	if (!m_character || !m_character->b_exist) return;
	if (!ach || !ach->b_exist) return;
	fVector3 cbox;
	PKinematics(pObject->Visual( ))->CalculateBones( );
	pObject->BoundingBox( ).getradius(cbox);
	const fVector3& pa = cast_fv(dBodyGetPosition(ach->get_body( )));
	const fVector3& p = cast_fv(dBodyGetPosition(m_character->get_body( )));
	fVector2 poses_dir;
	poses_dir.set(p.x - pa.x, p.z - pa.z);
	f32 plane_dist = poses_dir.magnitude( );
	if (plane_dist > 2.f) return;
	if (plane_dist > EPSILON_7)poses_dir.mul(1.f / plane_dist);
	fVector2 plane_cam;
	plane_cam.set(Device.vCameraDirection.x, Device.vCameraDirection.z);
	plane_cam.normalize_safe( );
	fVector2 plane_i;
	plane_i.set(pObject->XFORM( ).i.x, pObject->XFORM( ).i.z);
	fVector2 plane_k;
	plane_k.set(pObject->XFORM( ).k.x, pObject->XFORM( ).k.z);
	f32 R = _abs(poses_dir.dotproduct(plane_i) * cbox.x) + _abs(poses_dir.dotproduct(plane_k) * cbox.z);
	R *= poses_dir.dotproduct(plane_cam); //(poses_dir.x*plane_cam.x+poses_dir.y*plane_cam.z);
	Calculate(fVector3( ).set(0.0f, 0.0f, 0.0f), fVector3( ).set(1.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, false);
	m_character->SetObjectRadius(R);
	ach->ChooseRestrictionType(CPHCharacter::rtStalker, 1.0f, m_character);
	m_character->UpdateRestrictionType(ach);
}

void CPHMovementControl::SetPathDir(const fVector3& v)
{
	_vPathDir = v;

	if (_abs(_vPathDir.x) > 1000 || _abs(_vPathDir.y) > 1000 || _abs(_vPathDir.z) > 1000)
	{
		Log("_vPathDir", _vPathDir);
	}

	VERIFY2(_abs(_vPathDir.x) < 1000, " incorrect SetPathDir ");
}
