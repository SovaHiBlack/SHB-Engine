#include "stdafx.h"

//#include "..\ENGINE\cl_intersect.h"
//#include "alife_space.h"
#include "PHMovementControl.h"
//#include "entity.h"
//#include "PHDynamicData.h"
//#include "Physics.h"
#include "PHAICharacter.h"
#include "PHActorCharacter.h"
#include "PHCapture.h"
//#include "ai_space.h"
#include "detail_path_manager.h"
//#include "GameMtlLib.h"
//#include "Level.h"
//#include "ElevatorState.h"
#include "CalculateTriangle.h"
#include "..\ENGINE\SkeletonCustom.h"

#define def_X_SIZE_2	0.35f
#define def_Y_SIZE_2	0.8f
#define def_Z_SIZE_2	0.35f

CPHMovementControl::CPHMovementControl(CObject* parent)
{
	pObject = parent;

#ifdef DEBUG
	if (ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject) && (!!pObject->cName( )) && stricmp(PH_DBG_ObjectTrack( ), *pObject->cName( )) == 0)
	{
		Msg("CPHMovementControl::CPHMovementControl %s (constructor) %f,%f,%pObjectf", PH_DBG_ObjectTrack( ), pObject->Position( ).x, pObject->Position( ).y, pObject->Position( ).z);
	}
#endif // def DEBUG

	m_material = 0;
	m_capture = nullptr;
	b_exect_position = true;
	m_start_index = 0;
	eOldEnvironment = peInAir;
	eEnvironment = peInAir;
	aabb.set(-def_X_SIZE_2, 0, -def_Z_SIZE_2, def_X_SIZE_2, def_Y_SIZE_2 * 2, def_Z_SIZE_2);
	fMass = 100;
	fMinCrashSpeed = 12.0f;
	fMaxCrashSpeed = 25.0f;
	vVelocity.set(0.0f, 0.0f, 0.0f);
	vPosition.set(0.0f, 0.0f, 0.0f);
	vExternalImpulse.set(0.0f, 0.0f, 0.0f);
	bExernalImpulse = false;
	fLastMotionMag = 1.0f;
	SetPathDir(Fvector( ).set(0.0f, 0.0f, 1.0f));
	bIsAffectedByGravity = TRUE;
	fActualVelocity = 0.0f;
	m_fGroundDelayFactor = 1.0f;
	gcontact_HealthLost = 0.0f;
	fContactSpeed = 0.0f;
	fAirControlParam = 0.0f;
	m_character = nullptr;
	m_dwCurBox = 0xffffffff;
	fCollisionDamageFactor = 1.0f;
	in_dead_area_count = 0;
}

CPHMovementControl::~CPHMovementControl( )
{
	if (m_character)
	{
		m_character->Destroy( );
	}

	DeleteCharacterObject( );
	xr_delete(m_capture);
}

void CPHMovementControl::AddControlVel(const Fvector& vel)
{
	vExternalImpulse.add(vel);
	bExernalImpulse = true;
}

void CPHMovementControl::ApplyImpulse(const Fvector& dir, const dReal P)
{
	if (fis_zero(P))
	{
		return;
	}

	Fvector force;
	force.set(dir);
	force.mul(P / fixed_step);

	AddControlVel(force);
	m_character->ApplyImpulse(dir, P);
}

void CPHMovementControl::SetVelocityLimit(float val)
{
	if (m_character)
	{
		m_character->SetMaximumVelocity(val);
	}
}

float CPHMovementControl::VelocityLimit( )
{
	if (!m_character || !m_character->b_exist)
	{
		return 0.0f;
	}

	return m_character->GetMaximumVelocity( );
}

void CPHMovementControl::in_shedule_Update(u32 DT)
{
	if (m_capture)
	{
		if (m_capture->Failed( ))
		{
			xr_delete(m_capture);
		}
	}
}

void CPHMovementControl::Calculate(Fvector& vAccel, const Fvector& camDir, float /**ang_speed/**/, float jump, float /**dt/**/, bool /**bLight/**/)
{
	Fvector previous_position;
	previous_position.set(vPosition);
	m_character->IPosition(vPosition);
	if (bExernalImpulse)
	{
		vAccel.add(vExternalImpulse);
		m_character->ApplyForce(vExternalImpulse);
		vExternalImpulse.set(0.0f, 0.0f, 0.0f);

		bExernalImpulse = false;
	}

	float mAccel = vAccel.magnitude( );
	m_character->SetCamDir(camDir);
	m_character->SetMaximumVelocity(mAccel / 10.0f);
	m_character->SetAcceleration(vAccel);
	if (!fis_zero(jump))
	{
		m_character->Jump(vAccel);
	}

	m_character->GetSavedVelocity(vVelocity);
	fActualVelocity = vVelocity.magnitude( );
	//Msg("saved avel %f", fActualVelocity);
	gcontact_Was = m_character->ContactWas( );
	fContactSpeed = 0.0f;
	const ICollisionDamageInfo* di = m_character->CollisionDamageInfo( );

	{
		fContactSpeed = di->ContactVelocity( );
		gcontact_Power = fContactSpeed / fMaxCrashSpeed;
		gcontact_HealthLost = 0.0f;
		if (fContactSpeed > fMinCrashSpeed)
		{
			gcontact_HealthLost = (fContactSpeed - fMinCrashSpeed) / (fMaxCrashSpeed - fMinCrashSpeed);
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

	const ICollisionDamageInfo* cdi = CollisionDamageInfo( );
	if (cdi->HitCallback( ))
	{
		cdi->HitCallback( )->call(static_cast<CGameObject*>(m_character->PhysicsRefObject( )), fMinCrashSpeed, fMaxCrashSpeed, fContactSpeed, gcontact_HealthLost, CollisionDamageInfo( ));
	}

	TraceBorder(previous_position);
	CheckEnvironment(vPosition);
	bSleep = false;
	m_character->Reinit( );
}

void CPHMovementControl::Calculate(const xr_vector<DetailPathManager::STravelPathPoint>& path, float speed, u32& travel_point, float& precision)
{

#ifdef DEBUG
	if (ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject) && (!!pObject->cName( )) && stricmp(PH_DBG_ObjectTrack( ), *pObject->cName( )) == 0)
	{
		Msg("CPHMovementControl::Calculate in %s (Object Position) %f,%f,%f", PH_DBG_ObjectTrack( ), pObject->Position( ).x, pObject->Position( ).y, pObject->Position( ).z);
		Msg("CPHMovementControl::Calculate in %s (CPHMovementControl::vPosition) %f,%f,%f", PH_DBG_ObjectTrack( ), vPosition.x, vPosition.y, vPosition.z);
	}
#endif // def DEBUG

	if (!m_character->b_exist)
	{
		return;
	}

	Fvector new_position;
	m_character->IPosition(new_position);

	int index = 0;//nearest point

	bool near_line;
	m_path_size = path.size( );
	Fvector dir;
	dir.set(0.0f, 0.0f, 0.0f);
	if (m_path_size == 0)
	{
		speed = 0.0f;
		vPosition.set(new_position);
	}
	else if (b_exect_position)
	{
		m_start_index = travel_point;

		if ((m_path_size - 1) > (int) travel_point)
		{
			dir.sub(path[travel_point + 1].position, path[travel_point].position);
		}
		else
		{
			dir.sub(path[travel_point].position, new_position);
		}

		m_start_index = travel_point;
		dir.y = 0.0f;
		dir.normalize_safe( );
		vPosition.set(new_position);
		m_path_distance = 0.0f;
		SetPathDir(dir);
		vPathPoint.set(vPosition);
	}
	else
	{
		Fvector dif;
		dif.sub(new_position, vPathPoint);
		float radius = dif.magnitude( ) * 2.0f;
		if (m_path_size == 1)
		{
			speed = 0.0f;
			vPosition.set(new_position);	//todo - insert it in PathNearestPoint
			index = 0;
			vPathPoint.set(path[0].position);
			Fvector _d;
			_d.sub(path[0].position, new_position);
			SetPathDir(_d);
			m_path_distance = GetPathDir( ).magnitude( );
			if (m_path_distance > EPS)
			{
				Fvector _d = GetPathDir( );
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
			if (near_line)
			{
				PathDIrLine(path, index, m_path_distance, precision, dir);
			}
			else
			{
				PathDIrPoint(path, index, m_path_distance, precision, dir);
			}

			travel_point = (u32) index;
			m_start_index = index;
			if (fis_zero(speed))
			{
				dir.set(0.0f, 0.0f, 0.0f);
			}
		}
	}

	dir.y = 0.0f;
	//VERIFY(!(fis_zero(dir.magnitude( )) && !fis_zero(speed)));
	dir.normalize_safe( );

	/////////////////////////////////////////////////////////////////
	if (bExernalImpulse)
	{
		Fvector V;
		V.set(dir);
		V.mul(speed * 10.0f);
		V.add(vExternalImpulse);
		m_character->ApplyForce(vExternalImpulse);
		speed = V.magnitude( );

		if (!fis_zero(speed))
		{
			dir.set(V);
			dir.mul(1.0f / speed);
		}

		speed /= 10.0f;
		vExternalImpulse.set(0.0f, 0.0f, 0.0f);
		bExernalImpulse = false;
	}

	m_character->SetMaximumVelocity(speed);
	m_character->SetAcceleration(dir);
	//////////////////////////////////////////////////////
	m_character->GetSmothedVelocity(vVelocity);
	fActualVelocity = vVelocity.magnitude( );

	gcontact_Was = m_character->ContactWas( );
	const ICollisionDamageInfo* di = m_character->CollisionDamageInfo( );
	fContactSpeed = 0.0f;
	{
		fContactSpeed = di->ContactVelocity( );
		gcontact_Power = fContactSpeed / fMaxCrashSpeed;
		gcontact_HealthLost = 0.0f;
		if (fContactSpeed > fMinCrashSpeed)
		{
			gcontact_HealthLost = (fContactSpeed - fMinCrashSpeed) / (fMaxCrashSpeed - fMinCrashSpeed);
		}
	}

	CheckEnvironment(vPosition);
	bSleep = false;
	b_exect_position = false;
}

void CPHMovementControl::PathNearestPoint(const xr_vector<DetailPathManager::STravelPathPoint>& path,			//in path
										  const Fvector& new_position,											//in position
										  int& index,															//in start from; out nearest
										  bool& near_line														//out type
)
{
	Fvector from_first;
	Fvector from_second;
	Fvector dir;
	bool after_line = true;//to check first point

	Fvector path_point;
	Fvector vtemp;
	float temp;

	for (int i = 0; i < m_path_size - 1; ++i)
	{
		const Fvector& first = path[i].position, & second = path[i + 1].position;
		from_first.sub(new_position, first);
		from_second.sub(new_position, second);
		dir.sub(second, first);
		dir.normalize_safe( );

		if (from_first.dotproduct(dir) < 0.0f)//befor this line
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
			if (from_second.dotproduct(dir) < 0.0f) //befor second && after first = near line
			{
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
			else //after second = after this line
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
#endif // def DEBUG

	return;
}

void CPHMovementControl::PathNearestPointFindUp(const xr_vector<DetailPathManager::STravelPathPoint>& path,			//in path
												const Fvector& new_position,										//in position
												int& index,															//in start from; out nearest
												float radius,														//out m_path_distance in exit radius
												bool& near_line														//out type
)
{
	Fvector from_first;
	Fvector from_second;
	Fvector dir;
	bool after_line = true;//to check first point

	Fvector path_point;
	Fvector vtemp;
	float temp;
	dir.set(0.0f, 0.0f, 1.0f);

	for (int i = m_start_index; i < m_path_size - 1; ++i)
	{
		const Fvector& first = path[i].position, & second = path[i + 1].position;
		from_first.sub(new_position, first);
		from_second.sub(new_position, second);
		dir.sub(second, first);
		dir.normalize_safe( );
		float from_first_dir = from_first.dotproduct(dir);
		float from_second_dir = from_second.dotproduct(dir);

		if (from_first_dir < 0.0f)//before this line
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

			if (temp > radius)
			{	//exit test
				break;
			}

			after_line = false;
		}
		else //after first
		{
			if (from_second_dir < 0.0f) //befor second && after first = near line
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

				if (temp > radius)
				{	//exit test
					break;
				}
			}
			else //after second = after this line
			{
				after_line = true;
				if (from_second.magnitude( ) > radius)
				{	//exit test
					break;
				}
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
												  const Fvector& new_position,											//in position
												  int& index,															//in start from; out nearest
												  float radius,															//out m_path_distance in exit radius
												  bool& near_line														//out type
)
{
	Fvector from_first;
	Fvector from_second;
	Fvector dir;
	bool after_line = true;//to check first point

	Fvector path_point;
	Fvector vtemp;
	float temp;
	//(going down)
	dir.set(0.0f, 0.0f, 1.0f);
	for (int i = m_start_index; i > 1; --i)
	{
		const Fvector& first = path[i - 1].position, & second = path[i].position;
		from_first.sub(new_position, first);
		from_second.sub(new_position, second);
		dir.sub(second, first);
		dir.normalize_safe( );
		float from_first_dir = from_first.dotproduct(dir);
		float from_second_dir = from_second.dotproduct(dir);

		if (from_second_dir > 0.0f)//befor this line
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

			if (temp > radius)
			{	//exit test
				break;
			}

			after_line = false;
		}
		else //after second
		{
			if (from_first_dir > 0.0f) //after second && before first = near line (going down)
			{
				vtemp.set(dir);
				vtemp.mul(from_second_dir);
				path_point.add(second, vtemp); //from_second_dir < 0.0f !!
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

				if (temp > radius)
				{	//exit test
					break;
				}
			}
			else //after first = after this line(going down)
			{
				after_line = true;
				if (from_first.magnitude( ) > radius)
				{	//exit test
					break;
				}
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

void CPHMovementControl::CorrectPathDir(const Fvector& real_path_dir, const xr_vector<DetailPathManager::STravelPathPoint>& path, int index, Fvector& corrected_path_dir)
{
	const float epsilon = 0.1f;
	float plane_motion = dXZMag(real_path_dir);
	if (fis_zero(plane_motion, epsilon))
	{
		if (!fis_zero(plane_motion, EPS))
		{
			corrected_path_dir.set(real_path_dir);
			corrected_path_dir.y = 0.0f;
			corrected_path_dir.mul(1.0f / plane_motion);
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

void CPHMovementControl::PathDIrLine(const xr_vector<DetailPathManager::STravelPathPoint>& path, int index, float distance, float precesition, Fvector& dir)
{
	Fvector to_path_point;
	Fvector corrected_path_dir;
	CorrectPathDir(GetPathDir( ), path, index, corrected_path_dir);
	to_path_point.sub(vPathPoint, vPosition);	//_new position
	float mag = to_path_point.magnitude( );
	if (mag < EPS)
	{
		dir.set(corrected_path_dir);
		return;
	}

	to_path_point.mul(1.0f / mag);
	if (mag > FootRadius( ))
	{
		to_path_point.mul(precesition);
	}
	else
	{
		to_path_point.mul(mag * precesition);
	}

	dir.add(corrected_path_dir, to_path_point);
	dir.normalize_safe( );
}

void CPHMovementControl::PathDIrPoint(const xr_vector<DetailPathManager::STravelPathPoint>& path, int index, float distance, float precesition, Fvector& dir)
{
	Fvector to_path_point;
	Fvector corrected_path_dir;
	CorrectPathDir(GetPathDir( ), path, index, corrected_path_dir);
	to_path_point.sub(vPathPoint, vPosition);	//_new position
	float mag = to_path_point.magnitude( );

	if (mag < EPS) //near the point
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

	to_path_point.mul(1.0f / mag);
	if (m_path_size - 1 == index)//on_path_edge
	{
		dir.set(to_path_point);
		return;
	}

	if (mag < EPS || fis_zero(dXZMag(to_path_point), EPS))
	{
		dir.set(corrected_path_dir);
		return;//mean dir
	}

	Fvector tangent;
	tangent.crossproduct(Fvector( ).set(0, 1, 0), to_path_point);

	VERIFY(!fis_zero(tangent.magnitude( )));
	tangent.normalize( );
	if (dir.square_magnitude( ) > EPS)
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

void CPHMovementControl::SetActorRestrictorRadius(CPHCharacter::ERestrictionType rt, float r)
{
	if (m_character && eCharacterType == actor)
	{
		static_cast<CPHActorCharacter*>(m_character)->SetRestrictorRadius(rt, r);
	}
}

void CPHMovementControl::Load(const char* section)
{
	Fbox3 bb;

	// m_PhysicMovementControl: BOX
	Fvector vBOX1_center = pSettings->r_fvector3(section, "ph_box1_center");
	Fvector vBOX1_size = pSettings->r_fvector3(section, "ph_box1_size");
	bb.set(vBOX1_center, vBOX1_center);
	bb.grow(vBOX1_size);
	SetBox(1, bb);

	// m_PhysicMovementControl: BOX
	Fvector	vBOX0_center = pSettings->r_fvector3(section, "ph_box0_center");
	Fvector	vBOX0_size = pSettings->r_fvector3(section, "ph_box0_size");
	bb.set(vBOX0_center, vBOX0_center);
	bb.grow(vBOX0_size);
	SetBox(0, bb);

	// m_PhysicMovementControl: Crash speed and mass
	float cs_min = pSettings->r_float(section, "ph_crash_speed_min");
	float cs_max = pSettings->r_float(section, "ph_crash_speed_max");
	float mass = pSettings->r_float(section, "ph_mass");
	xr_token retrictor_types[ ] =
	{
		{ "actor",			CPHCharacter::rtActor },
		{ "medium_monster",	CPHCharacter::rtMonsterMedium },
		{ "stalker",		CPHCharacter::rtStalker },
		{ "none",			CPHCharacter::rtNone },
		{ 0,				0 }
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

void CPHMovementControl::CheckEnvironment(const Fvector&/**V/**/)
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

void CPHMovementControl::GroundNormal(Fvector& norm)
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

void CPHMovementControl::SetEnvironment(int enviroment, int old_enviroment)
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

void CPHMovementControl::SetPosition(const Fvector& P)
{

#ifdef DEBUG
	if (ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject) && (!!pObject->cName( )) && stricmp(PH_DBG_ObjectTrack( ), *pObject->cName( )) == 0)
	{
		Msg("CPHMovementControl::SetPosition %s (Object Position) %f,%f,%f", PH_DBG_ObjectTrack( ), pObject->Position( ).x, pObject->Position( ).y, pObject->Position( ).z);
		Msg("CPHMovementControl::SetPosition %s (CPHMovementControl::vPosition) %f,%f,%f", PH_DBG_ObjectTrack( ), vPosition.x, vPosition.y, vPosition.z);
	}
#endif // def DEBUG

	vPosition.set(P);  m_character->SetPosition(vPosition);
}

bool CPHMovementControl::TryPosition(Fvector& pos)
{
	VERIFY_BOUNDARIES2(pos, phBoundaries, m_character->PhysicsRefObject( ), "CPHMovementControl::TryPosition	arqument pos");

#ifdef DEBUG
	if (ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject) && (!!pObject->cName( )) && stricmp(PH_DBG_ObjectTrack( ), *pObject->cName( )) == 0)
	{
		Msg("CPHMovementControl::TryPosition %s (Object Position) %f,%f,%f", PH_DBG_ObjectTrack( ), pObject->Position( ).x, pObject->Position( ).y, pObject->Position( ).z);
		Msg("CPHMovementControl::TryPosition %s (CPHMovementControl::vPosition) %f,%f,%f", PH_DBG_ObjectTrack( ), vPosition.x, vPosition.y, vPosition.z);
	}
#endif // def DEBUG

	if (m_character->b_exist)
	{
		bool ret = m_character->TryPosition(pos, b_exect_position) && !bExernalImpulse;
		m_character->GetPosition(vPosition);
		return ret;
	}

	vPosition.set(pos);
	return true;
}

void CPHMovementControl::GetPosition(Fvector& P)
{
	VERIFY_BOUNDARIES2(P, phBoundaries, m_character->PhysicsRefObject( ), "CPHMovementControl::GetPosition	arqument pos");

#ifdef DEBUG
	if (ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject) && (!!pObject->cName( )) && stricmp(PH_DBG_ObjectTrack( ), *pObject->cName( )) == 0)
	{
		Msg("CPHMovementControl::GetPosition %s (Object Position) %f,%f,%f", PH_DBG_ObjectTrack( ), pObject->Position( ).x, pObject->Position( ).y, pObject->Position( ).z);
		Msg("CPHMovementControl::GetPosition %s (CPHMovementControl::vPosition) %f,%f,%f", PH_DBG_ObjectTrack( ), vPosition.x, vPosition.y, vPosition.z);
	}
#endif // def DEBUG

	P.set(vPosition);
	VERIFY_BOUNDARIES2(vPosition, phBoundaries, m_character->PhysicsRefObject( ), "CPHMovementControl::GetPosition	out pos");
}

void CPHMovementControl::AllocateCharacterObject(CharacterType type)
{
	switch (type)
	{
		case actor:
		{
			m_character = xr_new<CPHActorCharacter>( );
		}
		break;
		case ai:
		{
			m_character = xr_new<CPHAICharacter>( );
		}
		break;
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
#endif // def DEBUG

}

void CPHMovementControl::PHCaptureObject(CPHShellHolder* object)
{
	if (m_capture)
	{
		return;
	}

	if (!object || !object->PPhysicsShell( ) || !object->m_pPhysicsShell->isActive( ))
	{
		return;
	}

	m_capture = xr_new<CPHCapture>(m_character, object);
}

void CPHMovementControl::PHCaptureObject(CPHShellHolder* object, u16 element)
{
	if (m_capture)
	{
		return;
	}

	if (!object || !object->PPhysicsShell( ) || !object->PPhysicsShell( )->isActive( ))
	{
		return;
	}

	m_capture = xr_new<CPHCapture>(m_character, object, element);
}

Fvector CPHMovementControl::PHCaptureGetNearestElemPos(const CPHShellHolder* object)
{
	R_ASSERT3((object->m_pPhysicsShell != NULL), "NO Phisics Shell for object ", *object->cName( ));

	CPhysicsElement* ph_elem = object->m_pPhysicsShell->NearestToPoint(vPosition);

	Fvector v;
	ph_elem->GetGlobalPositionDynamic(&v);

	return v;
}

Fmatrix CPHMovementControl::PHCaptureGetNearestElemTransform(CPHShellHolder* object)
{
	CPhysicsElement* ph_elem = object->m_pPhysicsShell->NearestToPoint(vPosition);

	Fmatrix m;
	ph_elem->GetGlobalTransformDynamic(&m);

	return m;
}

void CPHMovementControl::PHReleaseObject( )
{
	if (m_capture)
	{
		m_capture->Release( );
	}
}

void CPHMovementControl::DestroyCharacter( )
{
	m_character->Destroy( );
	xr_delete(m_capture);
}

void CPHMovementControl::DeleteCharacterObject( )
{
	xr_delete(m_character);
	xr_delete(m_capture);
}

void CPHMovementControl::JumpV(const Fvector& jump_velocity)
{
	m_character->Enable( );
	m_character->Jump(jump_velocity);
}

void CPHMovementControl::Jump(const Fvector& end_point, float time)
{
	//vPosition
	Jump(smart_cast<CGameObject*>(m_character->PhysicsRefObject( ))->Position( ), end_point, time);
}

void CPHMovementControl::Jump(const Fvector& start_point, const Fvector& end_point, float time)
{
	Fvector velosity;
	velosity.sub(end_point, start_point);
	TransferenceToThrowVel(velosity, time, ph_world->Gravity( ));
	JumpV(velosity);
}

float CPHMovementControl::Jump(const Fvector& end_point)
{
	float time = JumpMinVelTime(end_point);
	Jump(smart_cast<CGameObject*>(m_character->PhysicsRefObject( ))->Position( ), end_point, time);
	return time;
}

void CPHMovementControl::GetJumpMinVelParam(Fvector& min_vel, float& time, JumpType& type, const Fvector& end_point)
{
	time = JumpMinVelTime(end_point);
	GetJumpParam(min_vel, type, end_point, time);
}

float CPHMovementControl::JumpMinVelTime(const Fvector& end_point)
{
	return ThrowMinVelTime(Fvector( ).sub(end_point, smart_cast<CGameObject*>(m_character->PhysicsRefObject( ))->Position( )), ph_world->Gravity( ));
}

void CPHMovementControl::GetJumpParam(Fvector& velocity, JumpType& type, const Fvector& end_point, float time)
{
	Fvector velosity; velosity.sub(smart_cast<CGameObject*>(m_character->PhysicsRefObject( ))->Position( ), end_point);
	TransferenceToThrowVel(velosity, time, ph_world->Gravity( ));
	if (velocity.y < 0.0f)
	{
		type = jtStrait;
		return;
	}

	float rise_time = velosity.y / ph_world->Gravity( );
	if (_abs(rise_time - time) < EPS_L)
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
	dVector3 size = { aabb.x2 - aabb.x1, aabb.y2 - aabb.y1, aabb.z2 - aabb.z1 };
	m_character->Create(size);
	m_character->SetMaterial(m_material);
	m_character->SetAirControlFactor(fAirControlParam);

#ifdef DEBUG
	if (ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject) && (!!pObject->cName( )) && stricmp(PH_DBG_ObjectTrack( ), *pObject->cName( )) == 0)
	{
		Msg("CPHMovementControl::CreateCharacter %s (Object Position) %f,%f,%f", PH_DBG_ObjectTrack( ), pObject->Position( ).x, pObject->Position( ).y, pObject->Position( ).z);
		Msg("CPHMovementControl::CreateCharacter %s (CPHMovementControl::vPosition) %f,%f,%f", PH_DBG_ObjectTrack( ), vPosition.x, vPosition.y, vPosition.z);
	}
#endif // def DEBUG

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
	if (m_character)
	{
		return smart_cast<CPHSynchronize*>(m_character);
	}
	else
	{
		return 0;
	}
}

void CPHMovementControl::Freeze( )
{
	if (m_character)
	{
		m_character->Freeze( );
	}
}

void CPHMovementControl::UnFreeze( )
{
	if (m_character)
	{
		m_character->UnFreeze( );
	}
}

void CPHMovementControl::ActivateBox(DWORD id, BOOL Check/*false*/)
{
	if (Check && (m_dwCurBox == id))
	{
		return;
	}

	m_dwCurBox = id;
	aabb.set(boxes[id]);
	if (!m_character || !m_character->b_exist)
	{
		return;
	}

	dVector3 size = { aabb.x2 - aabb.x1, aabb.y2 - aabb.y1, aabb.z2 - aabb.z1 };
	m_character->SetBox(size);
}

void CPHMovementControl::InterpolateBox(DWORD id, float k)
{
	if (m_dwCurBox == id)
	{
		return;
	}

	if (!m_character || !m_character->b_exist)
	{
		return;
	}

	dVector3 size = { aabb.x2 - aabb.x1, aabb.y2 - aabb.y1, aabb.z2 - aabb.z1 };
	dVector3 to_size = { boxes[id].x2 - boxes[id].x1, boxes[id].y2 - boxes[id].y1, boxes[id].z2 - boxes[id].z1 };
	dVectorInterpolate(size, to_size, k);
	m_character->SetBox(size);
}

void CPHMovementControl::ApplyHit(const Fvector& dir, const dReal P, ALife::EHitType hit_type)
{
	if (hit_type == ALife::eHitTypeExplosion || hit_type == ALife::eHitTypeWound)
	{
		ApplyImpulse(dir, P);
	}
}

void CPHMovementControl::SetFrictionFactor(float f)
{
	m_character->FrictionFactor( ) = f;
}

float CPHMovementControl::GetFrictionFactor( )
{
	return m_character->FrictionFactor( );
}

void CPHMovementControl::MulFrictionFactor(float f)
{
	m_character->FrictionFactor( ) *= f;
}

CElevatorState* CPHMovementControl::ElevatorState( )
{
	if (!m_character || !m_character->b_exist)
	{
		return nullptr;
	}

	return m_character->ElevatorState( );
}

struct STraceBorderQParams
{
	CPHMovementControl* m_movement;
	const Fvector& m_dir;
	STraceBorderQParams(CPHMovementControl* movement, const Fvector& dir) : m_dir(dir)
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
	STraceBorderQParams& p = *(STraceBorderQParams*) params;
	u16 mtl_idx = GAMEMTL_NONE_IDX;
	CDB::TRI* T = NULL;
	if (result.O)
	{
		return true;
	}
	else
	{	//получить треугольник и узнать его материал
		T = Level( ).ObjectSpace.GetStaticTris( ) + result.element;
		mtl_idx = T->material;
	}

	VERIFY(T);
	SGameMtl* mtl = GMLib.GetMaterialByIdx(mtl_idx);
	if (mtl->Flags.test(SGameMtl::flInjurious))
	{
		Fvector tri_norm;
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

void CPHMovementControl::TraceBorder(const Fvector& prev_position)
{
	const Fvector& from_pos = prev_position;
	const Fvector& to_position = vPosition;
	Fvector dir;
	dir.sub(to_position, from_pos);

	float sq_mag = dir.square_magnitude( );
	if (sq_mag == 0.0f)
	{
		return;
	}

	float mag = _sqrt(sq_mag);
	dir.mul(1.0f / mag);
	collide::ray_defs RD(from_pos, dir, mag, 0, collide::rqtStatic);
	VERIFY(!fis_zero(RD.dir.square_magnitude( )));

	STraceBorderQParams p(this, dir);
	storage.r_clear( );
	g_pGameLevel->ObjectSpace.RayQuery(storage, RD, BorderTraceCallback, &p, NULL, static_cast<CObject*>(m_character->PhysicsRefObject( )));
}

void CPHMovementControl::UpdateObjectBox(CPHCharacter* ach)
{
	if (!m_character || !m_character->b_exist)
	{
		return;
	}

	if (!ach || !ach->b_exist)
	{
		return;
	}

	Fvector cbox;
	PKinematics(pObject->Visual( ))->CalculateBones( );
	pObject->BoundingBox( ).getradius(cbox);
	const Fvector& pa = cast_fv(dBodyGetPosition(ach->get_body( )));
	const Fvector& p = cast_fv(dBodyGetPosition(m_character->get_body( )));
	Fvector2 poses_dir;
	poses_dir.set(p.x - pa.x, p.z - pa.z);
	float plane_dist = poses_dir.magnitude( );
	if (plane_dist > 2.0f)
	{
		return;
	}

	if (plane_dist > EPS_S)
	{
		poses_dir.mul(1.0f / plane_dist);
	}

	Fvector2 plane_cam;
	plane_cam.set(Device.vCameraDirection.x, Device.vCameraDirection.z);
	plane_cam.normalize_safe( );

	Fvector2 plane_i;
	plane_i.set(pObject->XFORM( ).i.x, pObject->XFORM( ).i.z);

	Fvector2 plane_k;
	plane_k.set(pObject->XFORM( ).k.x, pObject->XFORM( ).k.z);

	float R = _abs(poses_dir.dotproduct(plane_i) * cbox.x) + _abs(poses_dir.dotproduct(plane_k) * cbox.z);
	R *= poses_dir.dotproduct(plane_cam);
	Calculate(Fvector( ).set(0.0f, 0.0f, 0.0f), Fvector( ).set(1.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, false);
	m_character->SetObjectRadius(R);
	ach->ChooseRestrictionType(CPHCharacter::rtStalker, 1.0f, m_character);
	m_character->UpdateRestrictionType(ach);
}

void CPHMovementControl::SetPathDir(const Fvector& v)
{
	_vPathDir = v;

	if (_abs(_vPathDir.x) > 1000 || _abs(_vPathDir.y) > 1000 || _abs(_vPathDir.z) > 1000)
	{
		Log("_vPathDir", _vPathDir);
	}

	VERIFY2(_abs(_vPathDir.x) < 1000, " incorrect SetPathDir ");
}
