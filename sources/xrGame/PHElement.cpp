#include "stdafx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
#include "PHFracture.h"
#include "PHContactBodyEffector.h"
#include "MathUtils.h"
#include "PhysicsShellHolder.h"
#include "GameObject_space.h"
//#include "..\XR_3DA\skeletoncustom.h"
#include "..\XR_3DA\skeletonanimated.h"
#include "..\xrODE\util.h"
#ifdef DEBUG
#include	"PHDebug.h"
#endif

///////////////////////////////////////////////////////////////
#pragma warning(disable:4995)
#pragma warning(disable:4267)
#include "..\xrODE\collision_kernel.h"
#pragma warning(default:4267)
#pragma warning(default:4995)
///////////////////////////////////////////////////////////////////

#include "ExtendedGeom.h"

#include "PHShell.h"
#include "PHElement.h"
#include "PHElementInline.h"

extern CPHWorld* ph_world;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////Implementation for CPhysicsElement
CPHElement::CPHElement( )																															//aux
{
	m_w_limit = default_w_limit;
	m_l_limit = default_l_limit;
	m_l_scale = default_l_scale;
	m_w_scale = default_w_scale;

	m_body = NULL;
	m_flags.set(flActive, FALSE);
	m_flags.set(flActivating, FALSE);
	m_parent_element = NULL;
	m_shell = NULL;

	k_w = default_k_w;
	k_l = default_k_l;//1.8f;
	m_fratures_holder = NULL;
	m_flags.assign(0);
	mXFORM.identity( );
	m_mass.setZero( );
	m_mass_center.set(0, 0, 0);
	m_volume = 0.f;
}

void CPHElement::add_Box(const fObb& V)
{
	CPHGeometryOwner::add_Box(V);
}

void CPHElement::add_Sphere(const fSphere& V)
{
	CPHGeometryOwner::add_Sphere(V);
}

void CPHElement::add_Cylinder(const fCylinder& V)
{
	CPHGeometryOwner::add_Cylinder(V);
}

void CPHElement::build( )
{
	m_body = dBodyCreate(0);//phWorld
	//m_saved_contacts=dJointGroupCreate (0);
	//b_contacts_saved=false;
	dBodyDisable(m_body);
	//dBodySetFiniteRotationMode(m_body,1);
	//dBodySetFiniteRotationAxis(m_body,0,0,0);
	VERIFY2(dMass_valide(&m_mass), "Element has bad mass");
	if (m_geoms.empty( ))
	{
		Fix( );
	}
	else
	{
		VERIFY2(m_mass.mass > 0.0f, "Element has bad mass");
		dBodySetMass(m_body, &m_mass);
	}

	VERIFY_BOUNDARIES2(m_mass_center, phBoundaries, PhysicsRefObject( ), "m_mass_center")

		dBodySetPosition(m_body, m_mass_center.x, m_mass_center.y, m_mass_center.z);

	CPHDisablingTranslational::Reinit( );
	///////////////////////////////////////////////////////////////////////////////////////
	CPHGeometryOwner::build( );
	set_body(m_body);
}

void CPHElement::RunSimulation( )
{
	//if(push_untill)
		//push_untill+=Device.dwTimeGlobal;

	if (m_group)
		dSpaceAdd(m_shell->dSpace( ), (dGeomID)m_group);
	else
		if (!m_geoms.empty( ))(*m_geoms.begin( ))->add_to_space(m_shell->dSpace( ));
	if (!m_body->world)
	{
		//dWorldAddBody(phWorld, m_body);
		m_shell->Island( ).AddBody(m_body);
	}
	dBodyEnable(m_body);
}

void CPHElement::destroy( )
{
	//dJointGroupDestroy(m_saved_contacts);
	CPHGeometryOwner::destroy( );
	if (m_body)//&&m_body->world
	{
		if (m_body->world)m_shell->Island( ).RemoveBody(m_body);
		dBodyDestroy(m_body);
		m_body = NULL;
	}

	if (m_group)
	{
		dGeomDestroy(m_group);
		m_group = NULL;
	}
}

void CPHElement::calculate_it_data(const fVector3& mc, f32 mas)
{
	f32 density = mas / m_volume;
	calculate_it_data_use_density(mc, density);
}

static f32 static_dencity;
void CPHElement::calc_it_fract_data_use_density(const fVector3& mc, f32 density)
{
	m_mass_center.set(mc);
	dMassSetZero(&m_mass);
	static_dencity = density;
	recursive_mass_summ(0, m_fratures_holder->m_fractures.begin( ));
}

dMass CPHElement::recursive_mass_summ(u16 start_geom, FRACTURE_I cur_fracture)
{
	dMass end_mass;
	dMassSetZero(&end_mass);
	GEOM_I i_geom = m_geoms.begin( ) + start_geom, e = m_geoms.begin( ) + cur_fracture->m_start_geom_num;
	for (; i_geom != e; ++i_geom)(*i_geom)->add_self_mass(end_mass, m_mass_center, static_dencity);
	dMassAdd(&m_mass, &end_mass);
	start_geom = cur_fracture->m_start_geom_num;
	++cur_fracture;
	if (m_fratures_holder->m_fractures.end( ) != cur_fracture)
		cur_fracture->SetMassParts(m_mass, recursive_mass_summ(start_geom, cur_fracture));
	return end_mass;
}

void CPHElement::setDensity(f32 M)
{
	calculate_it_data_use_density(get_mc_data( ), M);
}

void CPHElement::setMass(f32 M)
{
	calculate_it_data(get_mc_data( ), M);
}

void CPHElement::setDensityMC(f32 M, const fVector3& mass_center)
{
	m_mass_center.set(mass_center);
	calc_volume_data( );
	calculate_it_data_use_density(mass_center, M);
}

void CPHElement::setMassMC(f32 M, const fVector3& mass_center)
{
	m_mass_center.set(mass_center);
	calc_volume_data( );
	calculate_it_data(mass_center, M);
}

void CPHElement::Start( )
{
	build( );
	RunSimulation( );
}

void CPHElement::Deactivate( )
{
	VERIFY(isActive( ));

	destroy( );
	m_flags.set(flActive, FALSE);
	m_flags.set(flActivating, FALSE);
	CKinematics* K = m_shell->PKinematics( );
	if (K)
	{
		K->LL_GetBoneInstance(m_SelfID).reset_callback( );
	}
}

void CPHElement::SetTransform(const fMatrix4x4& m0)
{
	VERIFY2(_valid(m0), "invalid_form_in_set_transform");
	fVector3 mc;
	CPHGeometryOwner::get_mc_vs_transform(mc, m0);
	VERIFY_BOUNDARIES2(mc, phBoundaries, PhysicsRefObject( ), "mass	center	in set transform");
	dBodySetPosition(m_body, mc.x, mc.y, mc.z);
	fMatrix3x3 m33;
	m33.set(m0);
	dMatrix3 R;
	PHDynamicData::FMX33toDMX(m33, R);
	dBodySetRotation(m_body, R);
	CPHDisablingFull::Reinit( );

	VERIFY2(dBodyGetPosition(m_body), "not valide safe position");
	VERIFY2(dBodyGetLinearVel(m_body), "not valide safe velocity");
	m_flags.set(flUpdate, TRUE);
	m_shell->spatial_move( );
}

void CPHElement::getQuaternion(fQuaternion& quaternion)
{
	if (!isActive( )) return;
	const f32* q = dBodyGetQuaternion(m_body);
	quaternion.set(-q[0], q[1], q[2], q[3]);
	VERIFY(_valid(quaternion));
}

void CPHElement::setQuaternion(const fQuaternion& quaternion)
{
	VERIFY(_valid(quaternion));
	if (!isActive( ))
	{
		return;
	}

	dQuaternion q = {-quaternion.w,quaternion.x,quaternion.y,quaternion.z};
	dBodySetQuaternion(m_body, q);
	CPHDisablingRotational::Reinit( );
	m_flags.set(flUpdate, TRUE);
	m_shell->spatial_move( );
}

void CPHElement::GetGlobalPositionDynamic(fVector3* v)
{
	if (!isActive( ))
	{
		return;
	}

	v->set((*(fVector3*)dBodyGetPosition(m_body)));
	VERIFY(_valid(*v));
}

void CPHElement::SetGlobalPositionDynamic(const fVector3& position)
{
	if (!isActive( ))
	{
		return;
	}

	VERIFY(_valid(position));
	VERIFY_BOUNDARIES2(position, phBoundaries, PhysicsRefObject( ), "SetGlobalPosition argument ");
	dBodySetPosition(m_body, position.x, position.y, position.z);
	CPHDisablingTranslational::Reinit( );
	m_flags.set(flUpdate, TRUE);
	m_shell->spatial_move( );
}

void CPHElement::TransformPosition(const fMatrix4x4& form)
{
	if (!isActive( ))return;
	VERIFY(_valid(form));
	R_ASSERT2(m_body, "body is not created");
	fMatrix4x4 bm;
	PHDynamicData::DMXPStoFMX(dBodyGetRotation(m_body), dBodyGetPosition(m_body), bm);
	fMatrix4x4 new_bm;
	new_bm.mul(form, bm);
	dMatrix3 dBM;
	PHDynamicData::FMXtoDMX(new_bm, dBM);
	dBodySetRotation(m_body, dBM);
	VERIFY_BOUNDARIES2(new_bm.c, phBoundaries, PhysicsRefObject( ), "TransformPosition dest pos")
		dBodySetPosition(m_body, new_bm.c.x, new_bm.c.y, new_bm.c.z);
	CPHDisablingFull::Reinit( );
	m_body_interpolation.ResetPositions( );
	m_body_interpolation.ResetRotations( );
	m_flags.set(flUpdate, TRUE);
	m_shell->spatial_move( );
}
CPHElement::~CPHElement( )
{
	VERIFY(!isActive( ));
	DeleteFracturesHolder( );
}

void CPHElement::Activate(const fMatrix4x4& transform, const fVector3& lin_vel, const fVector3& ang_vel, bool disable)
{
	VERIFY(!isActive( ));
	mXFORM.set(transform);
	Start( );
	SetTransform(transform);

	dBodySetLinearVel(m_body, lin_vel.x, lin_vel.y, lin_vel.z);

	dBodySetAngularVel(m_body, ang_vel.x, ang_vel.y, ang_vel.z);
	VERIFY(dBodyStateValide(m_body));
	//	dVectorSet(m_safe_position,dBodyGetPosition(m_body));
	//	dQuaternionSet(m_safe_quaternion,dBodyGetQuaternion(m_body));
	//	dVectorSet(m_safe_velocity,dBodyGetLinearVel(m_body));

	m_body_interpolation.SetBody(m_body);

	if (disable) dBodyDisable(m_body);
	m_flags.set(flActive, TRUE);
	m_flags.set(flActivating, TRUE);
	CKinematics* K = m_shell->PKinematics( );
	if (K)
	{
		K->LL_GetBoneInstance(m_SelfID).set_callback(bctPhysics, m_shell->GetBonesCallback( ), static_cast<CPhysicsElement*>(this));
	}
}
void CPHElement::Activate(const fMatrix4x4& m0, f32 dt01, const fMatrix4x4& m2, bool disable)
{
	fVector3 lvel;
	fVector3 avel;
	lvel.set(m2.c.x - m0.c.x, m2.c.y - m0.c.y, m2.c.z - m0.c.z);
	avel.set(0.0f, 0.0f, 0.0f);
	Activate(m0, lvel, avel, disable);
}

void CPHElement::Activate(bool disable)
{
	fVector3 lvel;
	fVector3 avel;
	lvel.set(0.0f, 0.0f, 0.0f);
	avel.set(0.0f, 0.0f, 0.0f);
	Activate(mXFORM, lvel, avel, disable);
}

void CPHElement::Activate(const fMatrix4x4& start_from, bool disable)
{
	VERIFY(_valid(start_from));
	fMatrix4x4 globe;
	globe.mul_43(start_from, mXFORM);

	fVector3 lvel;
	fVector3 avel;
	lvel.set(0.0f, 0.0f, 0.0f);
	avel.set(0.0f, 0.0f, 0.0f);
	Activate(globe, lvel, avel, disable);
}

void CPHElement::Update( )
{
	if (!isActive( ))
	{
		return;
	}

	if (m_flags.test(flActivating))
	{
		m_flags.set(flActivating, FALSE);
	}

	if (!dBodyIsEnabled(m_body) && !m_flags.test(flUpdate)/*!bUpdate*/)
	{
		return;
	}

	InterpolateGlobalTransform(&mXFORM);
	VERIFY2(_valid(mXFORM), "invalid position in update");
}

void CPHElement::PhTune(dReal step)
{
	if (!isActive( ))
	{
		return;
	}

	CPHContactBodyEffector* contact_effector = (CPHContactBodyEffector*)dBodyGetData(m_body);
	if (contact_effector)
	{
		contact_effector->Apply( );
	}

	VERIFY_BOUNDARIES2(cast_fv(dBodyGetPosition(m_body)), phBoundaries, PhysicsRefObject( ), "PhTune body position");
}
void CPHElement::PhDataUpdate(dReal step)
{
	if (!isActive( ))
	{
		return;
	}

	///////////////skip for disabled elements////////////////////////////////////////////////////////////
	//VERIFY_BOUNDARIES2(cast_fv(dBodyGetPosition(m_body)),phBoundaries,PhysicsRefObject(),"PhDataUpdate begin, body position");
#ifdef DEBUG
	if (ph_dbg_draw_mask.test(phDbgDrawMassCenters))
	{
		DBG_DrawPoint(cast_fv(dBodyGetPosition(m_body)), 0.03f, D3DCOLOR_XRGB(255, 0, 0));
	}
#endif

	m_flags.set(flEnabledOnStep, !!dBodyIsEnabled(m_body));
	if (!m_flags.test(flEnabledOnStep))
	{
		return;
	}

	//////////////////////////////////base pointers/////////////////////////////////////////////////
	const dReal* linear_velocity = dBodyGetLinearVel(m_body);
	const dReal* angular_velocity = dBodyGetAngularVel(m_body);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////scale velocity///////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	VERIFY(dV_valid(linear_velocity));
#ifdef DEBUG
	if (!dV_valid(angular_velocity))
	{
		Msg("angular vel %f,%f,%f", angular_velocity[0], angular_velocity[1], angular_velocity[2]);
		Msg("linear vel %f,%f,%f", linear_velocity[0], linear_velocity[1], linear_velocity[2]);
		Msg("position  %f,%f,%f", dBodyGetPosition(m_body)[0], dBodyGetPosition(m_body)[1], dBodyGetPosition(m_body)[2]);
		Msg("quaternion  %f,%f,%f,%f", dBodyGetQuaternion(m_body)[0], dBodyGetQuaternion(m_body)[1], dBodyGetQuaternion(m_body)[2], dBodyGetQuaternion(m_body)[3]);
		Msg("matrix");
		Msg("x  %f,%f,%f", dBodyGetRotation(m_body)[0], dBodyGetRotation(m_body)[4], dBodyGetRotation(m_body)[8]);
		Msg("y  %f,%f,%f", dBodyGetRotation(m_body)[1], dBodyGetRotation(m_body)[5], dBodyGetRotation(m_body)[9]);
		Msg("z  %f,%f,%f", dBodyGetRotation(m_body)[2], dBodyGetRotation(m_body)[6], dBodyGetRotation(m_body)[10]);
		CPhysicsShellHolder* ph = PhysicsRefObject( );
		Msg("name visual %s", *ph->cNameVisual( ));
		Msg("name obj %s", ph->Name( ));
		Msg("name section %s", *ph->cNameSect( ));
		VERIFY2(0, "bad angular velocity");
	}
#endif
	VERIFY(!fis_zero(m_l_scale));
	VERIFY(!fis_zero(m_w_scale));
	dBodySetLinearVel(
		m_body,
		linear_velocity[0] / m_l_scale,
		linear_velocity[1] / m_l_scale,
		linear_velocity[2] / m_l_scale
	);
	dBodySetAngularVel(
		m_body,
		angular_velocity[0] / m_w_scale,
		angular_velocity[1] / m_w_scale,
		angular_velocity[2] / m_w_scale
	);


	///////////////////scale changes values directly so get base values after it/////////////////////////
	/////////////////////////////base values////////////////////////////////////////////////////////////
	dReal linear_velocity_smag = dDOT(linear_velocity, linear_velocity);
	dReal linear_velocity_mag = _sqrt(linear_velocity_smag);

	dReal angular_velocity_smag = dDOT(angular_velocity, angular_velocity);
	dReal angular_velocity_mag = _sqrt(angular_velocity_smag);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////limit velocity & secure /////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////limit linear vel////////////////////////////////////////////////////////////////////////////////////////

	VERIFY(dV_valid(linear_velocity));
	if (linear_velocity_mag > m_l_limit)
	{
		CutVelocity(m_l_limit, m_w_limit);
		VERIFY_BOUNDARIES2(cast_fv(dBodyGetPosition(m_body)), phBoundaries, PhysicsRefObject( ), "PhDataUpdate end, body position");
		linear_velocity_smag = dDOT(linear_velocity, linear_velocity);
		linear_velocity_mag = _sqrt(linear_velocity_smag);
		angular_velocity_smag = dDOT(angular_velocity, angular_velocity);
		angular_velocity_mag = _sqrt(angular_velocity_smag);
	}
	////////////////secure position///////////////////////////////////////////////////////////////////////////////////
	const dReal* position = dBodyGetPosition(m_body);
	VERIFY(dV_valid(position));
	/////////////////limit & secure angular vel///////////////////////////////////////////////////////////////////////////////
	VERIFY(dV_valid(angular_velocity));

	if (angular_velocity_mag > m_w_limit)
	{
		CutVelocity(m_l_limit, m_w_limit);
		angular_velocity_smag = dDOT(angular_velocity, angular_velocity);
		angular_velocity_mag = _sqrt(angular_velocity_smag);
		linear_velocity_smag = dDOT(linear_velocity, linear_velocity);
		linear_velocity_mag = _sqrt(linear_velocity_smag);
	}

	////////////////secure rotation////////////////////////////////////////////////////////////////////////////////////////
	{

		VERIFY(dQ_valid(dBodyGetQuaternion(m_body)));


	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////disable///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (dBodyIsEnabled(m_body)) Disabling( );
	if (!dBodyIsEnabled(m_body)) return;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////air resistance/////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (!fis_zero(k_w))
		dBodyAddTorque(
		m_body,
		-angular_velocity[0] * k_w,
		-angular_velocity[1] * k_w,
		-angular_velocity[2] * k_w
		);

	dMass mass;
	dBodyGetMass(m_body, &mass);
	dReal l_air = linear_velocity_mag * k_l;//force/velocity !!!
	if (l_air > mass.mass / fixed_step) l_air = mass.mass / fixed_step;//validate

	if (!fis_zero(l_air))
		dBodyAddForce(
		m_body,
		-linear_velocity[0] * l_air,
		-linear_velocity[1] * l_air,
		-linear_velocity[2] * l_air
		);
	VERIFY(dBodyStateValide(m_body));
	VERIFY2(dV_valid(dBodyGetPosition(m_body)), "invalid body position");
	VERIFY2(dV_valid(dBodyGetQuaternion(m_body)), "invalid body rotation");

	VERIFY_BOUNDARIES2(cast_fv(dBodyGetPosition(m_body)), phBoundaries, PhysicsRefObject( ), "PhDataUpdate end, body position");
	UpdateInterpolation( );
}

void CPHElement::Enable( )
{
	if (!isActive( )) return;
	m_shell->EnableObject(0);
	if (dBodyIsEnabled(m_body)) return;
	dBodyEnable(m_body);
}

void CPHElement::Disable( )
{

	//	return;
	if (!isActive( ) || !dBodyIsEnabled(m_body)) return;
	FillInterpolation( );

	dBodyDisable(m_body);
}

void CPHElement::ReEnable( )
{

	//dJointGroupEmpty(m_saved_contacts);
	//b_contacts_saved=false;

}

void	CPHElement::Freeze( )
{
	if (!m_body) return;

	m_flags.set(flWasEnabledBeforeFreeze, !!dBodyIsEnabled(m_body));
	dBodyDisable(m_body);
}
void	CPHElement::UnFreeze( )
{
	if (!m_body)
	{
		return;
	}

	if (m_flags.test(flWasEnabledBeforeFreeze))
	{
		dBodyEnable(m_body);
	}
}
void	CPHElement::applyImpulseVsMC(const fVector3& pos, const fVector3& dir, f32 val)
{
	if (!isActive( ) || m_flags.test(flFixed)) return;
	if (!dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	/////////////////////////////////////////////////////////////////////////
	fVector3 impulse;
	val /= fixed_step;
	impulse.set(dir);
	impulse.mul(val);
	dBodyAddForceAtRelPos(m_body, impulse.x, impulse.y, impulse.z, pos.x, pos.y, pos.z);
	BodyCutForce(m_body, m_l_limit, m_w_limit);
	////////////////////////////////////////////////////////////////////////
}
void	CPHElement::applyImpulseVsGF(const fVector3& pos, const fVector3& dir, f32 val)
{
	VERIFY(_valid(pos) && _valid(dir) && _valid(val));
	if (!isActive( ) || m_flags.test(flFixed)) return;
	if (!dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	/////////////////////////////////////////////////////////////////////////
	fVector3 impulse;
	val /= fixed_step;
	impulse.set(dir);
	impulse.mul(val);
	dBodyAddForceAtPos(m_body, impulse.x, impulse.y, impulse.z, pos.x, pos.y, pos.z);
	BodyCutForce(m_body, m_l_limit, m_w_limit);
	VERIFY(dBodyStateValide(m_body));
	////////////////////////////////////////////////////////////////////////
}
void	CPHElement::applyImpulseTrace(const fVector3& pos, const fVector3& dir, f32 val, u16 id)
{

	VERIFY(_valid(pos) && _valid(dir) && _valid(val));
	if (!isActive( ) || m_flags.test(flFixed)) return;
	fVector3 body_pos;
	if (id != BI_NONE)
	{
		if (id == m_SelfID)
		{
			body_pos.sub(pos, m_mass_center);
		}
		else
		{
			CKinematics* K = m_shell->PKinematics( );
			if (K)
			{
				fMatrix4x4 m;
				m.set(K->LL_GetTransform(m_SelfID));
				m.invert( ); m.mulB_43(K->LL_GetTransform(id));
				m.transform(body_pos, pos);
				body_pos.sub(m_mass_center);
			}
			else
			{
				body_pos.set(0.f, 0.f, 0.f);
			}
		}
	}
	else
	{
		body_pos.set(0.f, 0.f, 0.f);
	}

#ifdef DEBUG
	if (ph_dbg_draw_mask.test(phHitApplicationPoints))
	{
		DBG_OpenCashedDraw( );
		fVector3 dbg_position;
		dbg_position.set(body_pos);
		dMULTIPLY0_331(cast_fp(dbg_position), dBodyGetRotation(m_body), cast_fp(body_pos));
		dbg_position.add(cast_fv(dBodyGetPosition(m_body)));
		DBG_DrawPoint(dbg_position, 0.01f, D3DCOLOR_XRGB(255, 255, 255));
		DBG_DrawLine(cast_fv(dBodyGetPosition(m_body)), dbg_position, D3DCOLOR_XRGB(255, 255, 255));
		DBG_DrawLine(dbg_position, fVector3( ).add(dbg_position, fVector3( ).mul(dir, 0.4f)), D3DCOLOR_XRGB(255, 0, 255));
		DBG_ClosedCashedDraw(10000);
	}
#endif	

	applyImpulseVsMC(body_pos, dir, val);
	if (m_fratures_holder)
	{
		///impulse.add(*((fVector3*)dBodyGetPosition(m_body)));
		fVector3 impulse;
		impulse.set(dir);
		impulse.mul(val / fixed_step);
		m_fratures_holder->AddImpact(impulse, body_pos, m_shell->BoneIdToRootGeom(id));
	}
}
void CPHElement::applyImpact(const SPHImpact& I)
{
	fVector3 pos;
	pos.add(I.point, m_mass_center);
	fVector3 dir;
	dir.set(I.force);
	f32 val = I.force.magnitude( );

	if (!fis_zero(val) && GeomByBoneID(I.geom))
	{
		dir.mul(1.f / val);
		applyImpulseTrace(pos, dir, val, I.geom);
	}
}
void CPHElement::InterpolateGlobalTransform(fMatrix4x4* m)
{
	if (!m_flags.test(flUpdate))
	{
		GetGlobalTransformDynamic(m);
		VERIFY(_valid(*m));
		return;
	}

	m_body_interpolation.InterpolateRotation(*m);
	m_body_interpolation.InterpolatePosition(m->c);
	MulB43InverceLocalForm(*m);
	m_flags.set(flUpdate, FALSE);
	VERIFY(_valid(*m));
}
void CPHElement::GetGlobalTransformDynamic(fMatrix4x4* m)
{
	PHDynamicData::DMXPStoFMX(dBodyGetRotation(m_body), dBodyGetPosition(m_body), *m);
	MulB43InverceLocalForm(*m);
	VERIFY(_valid(*m));
}

void CPHElement::InterpolateGlobalPosition(fVector3* v)
{
	m_body_interpolation.InterpolatePosition(*v);
	VERIFY(_valid(*v));
}

void CPHElement::build(bool disable)
{
	if (isActive( )) return;
	m_flags.set(flActive, TRUE);
	m_flags.set(flActivating, TRUE);
	build( );

	{
		SetTransform(mXFORM);
	}

	m_body_interpolation.SetBody(m_body);
	//previous_f[0]=dInfinity;
	if (disable) dBodyDisable(m_body);
}

void CPHElement::RunSimulation(const fMatrix4x4& start_from)
{
	RunSimulation( );

	{
		fMatrix4x4 globe;
		globe.mul(start_from, mXFORM);
		SetTransform(globe);
	}
}

void CPHElement::StataticRootBonesCallBack(CBoneInstance* B)
{
	fMatrix4x4 parent;
	VERIFY2(isActive( ), "the element is not active");
	VERIFY(_valid(m_shell->mXFORM));
	//VERIFY2(fsimilar(DET(B->mTransform),1.f,DET_CHECK_EPS),"Bones callback resive 0 matrix");
	VERIFY_RMATRIX(B->mTransform);
	VERIFY(valid_pos(B->mTransform.c, phBoundaries));
	if (m_flags.test(flActivating))
	{
		//if(!dBodyIsEnabled(m_body))
		//	dBodyEnable(m_body);
		VERIFY(!ph_world->Processing( ));
		VERIFY(_valid(B->mTransform));
		VERIFY(!m_shell->dSpace( )->lock_count);
		mXFORM.set(B->mTransform);
		//m_start_time=Device.fTimeGlobal;
		fMatrix4x4 global_transform;
		//if(m_parent_element)
		global_transform.mul_43(m_shell->mXFORM, mXFORM);
		SetTransform(global_transform);

		FillInterpolation( );
		//bActivating=false;
		m_flags.set(flActivating, FALSE);
		if (!m_parent_element)
		{
			m_shell->m_object_in_root.set(mXFORM);
			m_shell->m_object_in_root.invert( );
			m_shell->SetNotActivating( );
		}
		B->Callback_overwrite = TRUE;
		//VERIFY2(fsimilar(DET(B->mTransform),1.f,DET_CHECK_EPS),"Bones callback returns 0 matrix");
		VERIFY_RMATRIX(B->mTransform);
		VERIFY(valid_pos(B->mTransform.c, phBoundaries));
		//return;
	}



	//VERIFY2(fsimilar(DET(B->mTransform),1.f,DET_CHECK_EPS),"Bones callback returns 0 matrix");
	VERIFY_RMATRIX(B->mTransform);
	VERIFY(valid_pos(B->mTransform.c, phBoundaries));
	//if( !m_shell->is_active() && !m_flags.test(flUpdate)/*!bUpdate*/ ) return;

	{
		//InterpolateGlobalTransform(&mXFORM);
		parent.invert(m_shell->mXFORM);
		B->mTransform.mul_43(parent, mXFORM);
	}
	//VERIFY2(fsimilar(DET(B->mTransform),1.f,DET_CHECK_EPS),"Bones callback returns 0 matrix");
	VERIFY_RMATRIX(B->mTransform);
	VERIFY(valid_pos(B->mTransform.c, phBoundaries));
	VERIFY2(_valid(B->mTransform), "Bones callback returns bad matrix");
	//else
	//{

	//	InterpolateGlobalTransform(&m_shell->mXFORM);
	//	mXFORM.identity();
	//	B->mTransform.set(mXFORM);
	//parent.set(B->mTransform);
	//parent.invert();
	//m_shell->mXFORM.mulB(parent);

	//}


}

void CPHElement::BoneGlPos(fMatrix4x4& m, CBoneInstance* B)
{
	VERIFY(m_shell);
	m.mul_43(m_shell->mXFORM, B->mTransform);
}

void CPHElement::GetAnimBonePos(fMatrix4x4& bp)
{
	VERIFY(m_shell->PKinematics( ));
	CKinematicsAnimated* ak = m_shell->PKinematics( )->dcast_PKinematicsAnimated( );
	VERIFY(ak);
	CBoneInstance* BI = &ak->LL_GetBoneInstance(m_SelfID);
	if (!BI->Callback)//.
	{
		bp.set(BI->mTransform);
		return;
	}

	ak->Bone_GetAnimPos(bp, m_SelfID, u8(-1), true);
}

IC bool put_in_range(fVector3& v, f32 range)
{
	VERIFY(range > EPSILON_7);
	f32 sq_mag = v.square_magnitude( );
	if (sq_mag > range * range)
	{
		f32 mag = _sqrt(sq_mag);
		v.mul(range / mag);
		return true;
	}

	return false;
}

bool CPHElement::AnimToVel(f32 dt, f32 l_limit, f32 a_limit)
{
	VERIFY(m_shell);
	VERIFY(m_shell->PKinematics( ));
	//CBoneInstance *BI = &m_shell->PKinematics()->LL_GetBoneInstance(m_SelfID);
//
//	fMatrix4x4 bp;BoneGlPos(bp,BI);
//
	CPhysicsShellHolder* ph = PhysicsRefObject( );
	VERIFY(ph);
	fMatrix4x4 bpl;
	GetAnimBonePos(bpl);
	fMatrix4x4 bp;
	bp.mul_43(ph->XFORM( ), bpl);
	//BoneGlPos(bp,BI);

	fMatrix4x4 cp;
	GetGlobalTransformDynamic(&cp);

	cp.invert( );
	fMatrix4x4 diff;
	diff.mul_43(cp, bp);
	if (dt < EPSILON_7)
		dt = EPSILON_7;
	fVector3 mc1;
	CPHGeometryOwner::get_mc_vs_transform(mc1, bp);
	fVector3 mc0 = cast_fv(dBodyGetPosition(m_body));
	//fVector3 mc1;diff.transform_tiny(mc1,mc0);
	fVector3	lv;
	lv.mul(fVector3( ).sub(mc1, mc0), (1.0f / dt));
	fVector3 aw;
	aw.set((diff._32 - diff._23) / 2.0f / dt, (diff._13 - diff._31) / 2.0f / dt, (diff._21 - diff._12) / 2.0f / dt);

	bool ret = aw.square_magnitude( ) < a_limit * a_limit && lv.square_magnitude( ) < l_limit * l_limit;

	put_in_range(lv, m_l_limit);
	put_in_range(aw, m_w_limit);

	VERIFY(_valid(lv));
	VERIFY(_valid(aw));

	dBodySetLinearVel(m_body, lv.x, lv.y, lv.z);
	dBodySetAngularVel(m_body, aw.x, aw.y, aw.z);
	//set_LinearVel(lv);
	//set_AngularVel(aw);
	return ret;
}

void CPHElement::ToBonePos(CBoneInstance* B)
{
	VERIFY2(!ph_world->Processing( ), *PhysicsRefObject( )->cNameSect( ));
	VERIFY(_valid(B->mTransform));
	VERIFY(!m_shell->dSpace( )->lock_count);

	mXFORM.set(B->mTransform);

	fMatrix4x4 global_transform;
	BoneGlPos(global_transform, B);
	SetTransform(global_transform);
	FillInterpolation( );
}

void	CPHElement::SetBoneCallbackOverwrite(bool v)
{
	VERIFY(m_shell);
	VERIFY(m_shell->PKinematics( ));
	m_shell->PKinematics( )->LL_GetBoneInstance(m_SelfID).Callback_overwrite = v;
}
void CPHElement::BonesCallBack(CBoneInstance* B)
{
	fMatrix4x4 parent;
	VERIFY(isActive( ));
	VERIFY(_valid(m_shell->mXFORM));
	//VERIFY2(fsimilar(DET(B->mTransform),1.f,DET_CHECK_EPS),"Bones callback receive 0 matrix");
	VERIFY_RMATRIX(B->mTransform);
	VERIFY_BOUNDARIES2(B->mTransform.c, phBoundaries, PhysicsRefObject( ), "BonesCallBack incoming bone position");
	if (m_flags.test(flActivating))
	{
		ToBonePos(B);
		m_flags.set(flActivating, FALSE);
		if (!m_parent_element)
		{
			m_shell->m_object_in_root.set(mXFORM);
			m_shell->m_object_in_root.invert( );
			m_shell->SetNotActivating( );
		}
		B->Callback_overwrite = TRUE;
		//VERIFY2(fsimilar(DET(B->mTransform),1.f,DET_CHECK_EPS),"Bones callback returns 0 matrix");
		VERIFY_RMATRIX(B->mTransform);
		VERIFY(valid_pos(B->mTransform.c, phBoundaries));
		return;
	}

	VERIFY_RMATRIX(B->mTransform);
	VERIFY(valid_pos(B->mTransform.c, phBoundaries));


	{

		parent.invert(m_shell->mXFORM);
		B->mTransform.mul_43(parent, mXFORM);
	}
	//VERIFY2(fsimilar(DET(B->mTransform),1.f,DET_CHECK_EPS),"Bones callback returns 0 matrix");
	VERIFY_RMATRIX(B->mTransform);
	VERIFY(valid_pos(B->mTransform.c, phBoundaries));
	VERIFY2(_valid(B->mTransform), "Bones callback returns bad matrix");
	//else
	//{

	//	InterpolateGlobalTransform(&m_shell->mXFORM);
	//	mXFORM.identity();
	//	B->mTransform.set(mXFORM);
	//parent.set(B->mTransform);
	//parent.invert();
	//m_shell->mXFORM.mulB(parent);

	//}


}

void CPHElement::set_PhysicsRefObject(CPhysicsShellHolder* ref_object)
{
	CPHGeometryOwner::set_PhysicsRefObject(ref_object);
}


void CPHElement::set_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	CPHGeometryOwner::set_ObjectContactCallback(callback);
}
void CPHElement::add_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	CPHGeometryOwner::add_ObjectContactCallback(callback);
}
void CPHElement::remove_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	CPHGeometryOwner::remove_ObjectContactCallback(callback);
}
ObjectContactCallbackFun* CPHElement::get_ObjectContactCallback( )
{
	return CPHGeometryOwner::get_ObjectContactCallback( );
}
void CPHElement::set_CallbackData(void* cd)
{
	CPHGeometryOwner::set_CallbackData(cd);
}
void* CPHElement::get_CallbackData( )
{
	return	CPHGeometryOwner::get_CallbackData( );
}
void CPHElement::set_ContactCallback(ContactCallbackFun* callback)
{
	//push_untill=0;
	CPHGeometryOwner::set_ContactCallback(callback);
}

void CPHElement::SetMaterial(u16 m)
{
	CPHGeometryOwner::SetMaterial(m);
}

dMass* CPHElement::getMassTensor( )																						//aux
{
	return &m_mass;
}

void	CPHElement::setInertia(const dMass& M)
{
	m_mass = M;
	if (!isActive( ) || m_flags.test(flFixed))return;
	dBodySetMass(m_body, &M);
}

void	CPHElement::addInertia(const dMass& M)
{
	dMassAdd(&m_mass, &M);
	if (!isActive( ))return;
	dBodySetMass(m_body, &m_mass);
}
void CPHElement::get_LinearVel(fVector3& velocity)
{
	if (!isActive( ) || !dBodyIsEnabled(m_body))
	{
		velocity.set(0, 0, 0);
		return;
	}
	dVectorSet((dReal*)&velocity, dBodyGetLinearVel(m_body));
}
void CPHElement::get_AngularVel(fVector3& velocity)
{
	if (!isActive( ) || !dBodyIsEnabled(m_body))
	{
		velocity.set(0, 0, 0);
		return;
	}
	dVectorSet((dReal*)&velocity, dBodyGetAngularVel(m_body));
}

void CPHElement::set_LinearVel(const fVector3& velocity)
{
	if (!isActive( ) || m_flags.test(flFixed)) return;
	VERIFY2(_valid(velocity), "not valid arqument velocity");
	fVector3 vel = velocity;

#ifdef DEBUG
	if (velocity.magnitude( ) > m_l_limit)
		Msg(" CPHElement::set_LinearVel set velocity magnitude is too large %f", velocity.magnitude( ));
#endif

	put_in_range(vel, m_l_limit);
	dBodySetLinearVel(m_body, vel.x, vel.y, vel.z);
}

void CPHElement::set_AngularVel(const fVector3& velocity)
{
	VERIFY(_valid(velocity));
	if (!isActive( ) || m_flags.test(flFixed)) return;

	fVector3 vel = velocity;

#ifdef DEBUG
	if (velocity.magnitude( ) > m_w_limit)
		Msg("CPHElement::set_AngularVel set velocity magnitude is too large %f", velocity.magnitude( ));
#endif

	put_in_range(vel, m_w_limit);
	dBodySetAngularVel(m_body, vel.x, vel.y, vel.z);
}

void	CPHElement::getForce(fVector3& force)
{
	if (!isActive( )) return;
	force.set(*(fVector3*)dBodyGetForce(m_body));
	VERIFY(dBodyStateValide(m_body));
}
void	CPHElement::getTorque(fVector3& torque)
{
	if (!isActive( )) return;
	torque.set(*(fVector3*)dBodyGetTorque(m_body));
	VERIFY(dBodyStateValide(m_body));
}
void	CPHElement::setForce(const fVector3& force)
{
	if (!isActive( ) || m_flags.test(flFixed)) return;
	if (!dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	m_shell->EnableObject(0);
	dBodySetForce(m_body, force.x, force.y, force.z);
	BodyCutForce(m_body, m_l_limit, m_w_limit);
	VERIFY(dBodyStateValide(m_body));
}
void	CPHElement::setTorque(const fVector3& torque)
{
	if (!isActive( ) || m_flags.test(flFixed)) return;
	if (!dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	m_shell->EnableObject(0);
	dBodySetTorque(m_body, torque.x, torque.y, torque.z);
	BodyCutForce(m_body, m_l_limit, m_w_limit);
	VERIFY(dBodyStateValide(m_body));
}

void	CPHElement::applyForce(const fVector3& dir, f32 val)															//aux
{
	applyForce(dir.x * val, dir.y * val, dir.z * val);
}
void	CPHElement::applyForce(f32 x, f32 y, f32 z)																//called anywhere ph state influent
{
	VERIFY(_valid(x) && _valid(y) && _valid(z));
	if (!isActive( ))return;//hack??
	if (m_flags.test(flFixed)) return;
	if (!dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	m_shell->EnableObject(0);
	dBodyAddForce(m_body, x, y, z);
	BodyCutForce(m_body, m_l_limit, m_w_limit);
	VERIFY(dBodyStateValide(m_body));
}

void	CPHElement::applyImpulse(const fVector3& dir, f32 val)//aux
{
	applyForce(dir.x * val / fixed_step, dir.y * val / fixed_step, dir.z * val / fixed_step);
}

void CPHElement::add_Shape(const SBoneShape& shape, const fMatrix4x4& offset)
{
	CPHGeometryOwner::add_Shape(shape, offset);
}

void CPHElement::add_Shape(const SBoneShape& shape)
{
	CPHGeometryOwner::add_Shape(shape);
}

#pragma todo(remake it using Geometry functions)

void CPHElement::add_Mass(const SBoneShape& shape, const fMatrix4x4& offset, const fVector3& mass_center, f32 mass, CPHFracture* fracture)
{
	dMass m;
	dMatrix3 DMatx;
	switch (shape.type)
	{
		case SBoneShape::stBox:
		{
			dMassSetBox(&m, 1.f, shape.box.m_halfsize.x * 2.f, shape.box.m_halfsize.y * 2.f, shape.box.m_halfsize.z * 2.f);
			dMassAdjust(&m, mass);
			fMatrix4x4 box_transform;
			shape.box.xform_get(box_transform);
			PHDynamicData::FMX33toDMX(shape.box.m_rotate, DMatx);
			dMassRotate(&m, DMatx);
			dMassTranslate(&m, shape.box.m_translate.x - mass_center.x, shape.box.m_translate.y - mass_center.y, shape.box.m_translate.z - mass_center.z);
			break;
		}
		case SBoneShape::stSphere:
		{
			shape.sphere;
			dMassSetSphere(&m, 1.f, shape.sphere.R);
			dMassAdjust(&m, mass);
			dMassTranslate(&m, shape.sphere.P.x - mass_center.x, shape.sphere.P.y - mass_center.y, shape.sphere.P.z - mass_center.z);
			break;
		}
		case SBoneShape::stCylinder:
		{
			const fVector3& pos = shape.cylinder.m_center;
			fVector3 l;
			l.sub(pos, mass_center);
			dMassSetCylinder(&m, 1.f, 2, shape.cylinder.m_radius, shape.cylinder.m_height);
			dMassAdjust(&m, mass);
			dMatrix3 DMatx;
			fMatrix3x3 m33;
			m33.j.set(shape.cylinder.m_direction);
			fVector3::generate_orthonormal_basis(m33.j, m33.k, m33.i);
			PHDynamicData::FMX33toDMX(m33, DMatx);
			dMassRotate(&m, DMatx);
			dMassTranslate(&m, l.x, l.y, l.z);
			break;
		}
		case SBoneShape::stNone:
			break;
		default: NODEFAULT;
	}

	PHDynamicData::FMXtoDMX(offset, DMatx);
	dMassRotate(&m, DMatx);

	fVector3 mc;
	offset.transform_tiny(mc, mass_center);
	//calculate _new mass_center 
	//new_mc=(m_mass_center*m_mass.mass+mc*mass)/(mass+m_mass.mass)
	fVector3 tmp1;
	tmp1.set(m_mass_center);
	tmp1.mul(m_mass.mass);

	fVector3 tmp2;
	tmp2.set(mc);
	tmp2.mul(mass);

	fVector3 new_mc;
	new_mc.add(tmp1, tmp2);
	new_mc.mul(1.f / (mass + m_mass.mass));
	mc.sub(new_mc);
	dMassTranslate(&m, mc.x, mc.y, mc.z);
	m_mass_center.sub(new_mc);
	dMassTranslate(&m_mass, m_mass_center.x, m_mass_center.y, m_mass_center.z);
	if (m_fratures_holder)
	{
		m_fratures_holder->DistributeAdditionalMass(u16(m_geoms.size( ) - 1), m);
	}

	if (fracture)
	{
		fracture->MassAddToSecond(m);
	}

	R_ASSERT2(dMass_valide(&m), "bad bone mass params");
	dMassAdd(&m_mass, &m);
	R_ASSERT2(dMass_valide(&m), "bad result mass params");
	m_mass_center.set(new_mc);
}

void CPHElement::set_BoxMass(const fObb& box, f32 mass)
{
	dMassSetZero(&m_mass);
	m_mass_center.set(box.m_translate);
	const fVector3& hside = box.m_halfsize;
	dMassSetBox(&m_mass, 1, hside.x * 2.f, hside.y * 2.f, hside.z * 2.f);
	dMassAdjust(&m_mass, mass);
	dMatrix3 DMatx;
	PHDynamicData::FMX33toDMX(box.m_rotate, DMatx);
	dMassRotate(&m_mass, DMatx);
}

void CPHElement::calculate_it_data_use_density(const fVector3& mc, f32 density)
{
	dMassSetZero(&m_mass);
	GEOM_I i_geom = m_geoms.begin( ), e = m_geoms.end( );
	for (; i_geom != e; ++i_geom)(*i_geom)->add_self_mass(m_mass, mc, density);
	VERIFY2(dMass_valide(&m_mass), "non valide mass obtained!");
}

f32 CPHElement::getRadius( )
{
	return CPHGeometryOwner::getRadius( );
}

void CPHElement::set_DynamicLimits(f32 l_limit, f32 w_limit)
{
	m_l_limit = l_limit;
	m_w_limit = w_limit;
}

void CPHElement::set_DynamicScales(f32 l_scale/* =default_l_scale */, f32 w_scale/* =default_w_scale */)
{
	m_l_scale = l_scale;
	m_w_scale = w_scale;
}

void	CPHElement::set_DisableParams(const SAllDDOParams& params)
{
	CPHDisablingFull::set_DisableParams(params);
}

void CPHElement::get_Extensions(const fVector3& axis, f32 center_prg, f32& lo_ext, f32& hi_ext)
{
	CPHGeometryOwner::get_Extensions(axis, center_prg, lo_ext, hi_ext);
}

const fVector3& CPHElement::mass_Center( )
{
	VERIFY(dBodyStateValide(m_body));
	return *((const fVector3*)dBodyGetPosition(m_body));
}

CPhysicsShell* CPHElement::PhysicsShell( )
{
	return smart_cast<CPhysicsShell*>(m_shell);
}

CPHShell* CPHElement::PHShell( )
{
	return (m_shell);
}
void	CPHElement::SetShell(CPHShell* p)
{
	if (!m_body || !m_shell)
	{
		m_shell = p;
		return;
	}

	if (m_shell != p)
	{
		m_shell->Island( ).RemoveBody(m_body);
		p->Island( ).AddBody(m_body);
		m_shell = p;
	}
}
void CPHElement::PassEndGeoms(u16 from, u16 to, CPHElement* dest)
{
	GEOM_I i_from = m_geoms.begin( ) + from, e = m_geoms.begin( ) + to;
	u16 shift = to - from;
	GEOM_I i = i_from;
	for (; i != e; ++i)
	{
		(*i)->remove_from_space(m_group);
		//(*i)->add_to_space(dest->m_group);
		//(*i)->set_body(dest->m_body);
		(*i)->set_body(0);
		u16& element_pos = (*i)->element_position( );
		element_pos = element_pos - shift;
	}
	GEOM_I last = m_geoms.end( );
	for (; i != last; ++i)
	{
		u16& element_pos = (*i)->element_position( );
		element_pos = element_pos - shift;
	}

	dest->m_geoms.insert(dest->m_geoms.end( ), i_from, e);
	dest->b_builded = true;
	m_geoms.erase(i_from, e);
}
void CPHElement::SplitProcess(ELEMENT_PAIR_VECTOR& new_elements)
{
	m_fratures_holder->SplitProcess(this, new_elements);
	if (!m_fratures_holder->m_fractures.size( )) xr_delete(m_fratures_holder);
}
void CPHElement::DeleteFracturesHolder( )
{
	xr_delete(m_fratures_holder);
}

void CPHElement::CreateSimulBase( )
{
	m_body = dBodyCreate(0);
	m_shell->Island( ).AddBody(m_body);
	//m_saved_contacts=dJointGroupCreate (0);
	//b_contacts_saved=false;
	dBodyDisable(m_body);
	CPHGeometryOwner::CreateSimulBase( );
}
void CPHElement::ReAdjustMassPositions(const fMatrix4x4& shift_pivot, f32 density)
{
	GEOM_I i = m_geoms.begin( ), e = m_geoms.end( );
	for (; i != e; ++i)
	{
		(*i)->move_local_basis(shift_pivot);
	}

	if (m_shell->PKinematics( ))
	{
		f32 mass;
		get_mc_kinematics(m_shell->PKinematics( ), m_mass_center, mass);
		calculate_it_data(m_mass_center, mass);
	}
	else
	{
		setDensity(density);
	}

	dBodySetMass(m_body, &m_mass);
	//m_inverse_local_transform.identity();
	//m_inverse_local_transform.c.set(m_mass_center);
	//m_inverse_local_transform.invert();
	//dBodySetPosition(m_body,m_mass_center.x,m_mass_center.y,m_mass_center.z);
}
void CPHElement::ResetMass(f32 density)
{
	fVector3 tmp;
	fVector3 shift_mc;

	tmp.set(m_mass_center);

	setDensity(density);
	dBodySetMass(m_body, &m_mass);

	shift_mc.sub(m_mass_center, tmp);
	tmp.set(*(fVector3*)dBodyGetPosition(m_body));
	tmp.add(shift_mc);

	//bActivating = true;
	m_flags.set(flActivating, TRUE);

	CPHGeometryOwner::setPosition(m_mass_center);
}
void CPHElement::ReInitDynamics(const fMatrix4x4& shift_pivot, f32 density)
{
	VERIFY(_valid(shift_pivot) && _valid(density));
	ReAdjustMassPositions(shift_pivot, density);
	GEOM_I i = m_geoms.begin( ), e = m_geoms.end( );
	for (; i != e; ++i)
	{
		(*i)->set_position(m_mass_center);
		(*i)->set_body(m_body);
		//if(object_contact_callback)geom.set_obj_contact_cb(object_contact_callback);
		//if(m_phys_ref_object) geom.set_ref_object(m_phys_ref_object);
		if (m_group)
		{
			(*i)->add_to_space((dSpaceID)m_group);
		}
	}
}

void CPHElement::PresetActive( )
{
	if (isActive( )) return;

	CBoneInstance& B = m_shell->PKinematics( )->LL_GetBoneInstance(m_SelfID);
	mXFORM.set(B.mTransform);
	//m_start_time=Device.fTimeGlobal;
	fMatrix4x4 global_transform;
	global_transform.mul_43(m_shell->mXFORM, mXFORM);
	SetTransform(global_transform);

	if (!m_parent_element)
	{
		m_shell->m_object_in_root.set(mXFORM);
		m_shell->m_object_in_root.invert( );

	}
	//dVectorSet(m_safe_position,dBodyGetPosition(m_body));
	//dQuaternionSet(m_safe_quaternion,dBodyGetQuaternion(m_body));
	//dVectorSet(m_safe_velocity,dBodyGetLinearVel(m_body));

	//////////////////////////////////////////////////////////////
	//initializing values for disabling//////////////////////////
	//////////////////////////////////////////////////////////////
	VERIFY(dBodyStateValide(m_body));
	m_body_interpolation.SetBody(m_body);
	FillInterpolation( );
	//bActive=true;
	m_flags.set(flActive, TRUE);
	RunSimulation( );
	VERIFY(dBodyStateValide(m_body));
}

bool CPHElement::isBreakable( )
{
	return !!m_fratures_holder;
}
u16	CPHElement::setGeomFracturable(CPHFracture& fracture)
{
	if (!m_fratures_holder) m_fratures_holder = xr_new<CPHFracturesHolder>( );
	return m_fratures_holder->AddFracture(fracture);
}

CPHFracture& CPHElement::Fracture(u16 num)
{
	R_ASSERT2(m_fratures_holder, "no fractures!");
	return m_fratures_holder->Fracture(num);
}
u16	CPHElement::numberOfGeoms( )
{
	return CPHGeometryOwner::numberOfGeoms( );
}

void CPHElement::cv2bone_Xfrom(const fQuaternion& q, const fVector3& pos, fMatrix4x4& xform)
{
	VERIFY2(_valid(q) && _valid(pos), "cv2bone_Xfrom receive wrong data");
	xform.rotation(q);
	xform.c.set(pos);
	//xform.mulB(m_inverse_local_transform);
	MulB43InverceLocalForm(xform);
	VERIFY2(_valid(xform), "cv2bone_Xfrom returns wrong data");
}
void CPHElement::cv2obj_Xfrom(const fQuaternion& q, const fVector3& pos, fMatrix4x4& xform)
{
	cv2bone_Xfrom(q, pos, xform);
	xform.mulB_43(m_shell->m_object_in_root);
	VERIFY2(_valid(xform), "cv2obj_Xfrom returns wrong data");
}

void CPHElement::set_ApplyByGravity(bool flag)
{
	if (!isActive( ) || m_flags.test(flFixed)) return;
	dBodySetGravityMode(m_body, flag);
}
bool CPHElement::get_ApplyByGravity( )
{
	return (!!dBodyGetGravityMode(m_body));
}

void	CPHElement::Fix( )
{
	m_flags.set(flFixed, TRUE);
	FixBody(m_body);
}
void	CPHElement::ReleaseFixed( )
{
	if (!isFixed( ))	return;
	m_flags.set(flFixed, FALSE);
	if (!isActive( ))return;
	dBodySetMass(m_body, &m_mass);
}
void CPHElement::applyGravityAccel(const fVector3& accel)
{
	VERIFY(_valid(accel));
	if (m_flags.test(flFixed)) return;
	if (!dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	m_shell->EnableObject(0);
	fVector3 val;
	val.set(accel);
	val.mul(m_mass.mass);
	//ApplyGravityAccel(m_body,(const dReal*)(&accel));
	applyForce(val.x, val.y, val.z);
}

void CPHElement::CutVelocity(f32 l_limit, f32 a_limit)
{
	if (!isActive( ))return;
	VERIFY(_valid(l_limit) && _valid(a_limit));
	dVector3 limitedl, limiteda, diffl, diffa;
	bool blimitl = dVectorLimit(dBodyGetLinearVel(m_body), l_limit, limitedl);
	bool blimita = dVectorLimit(dBodyGetAngularVel(m_body), a_limit, limiteda);

	if (blimitl || blimita)
	{
		dVectorSub(diffl, limitedl, dBodyGetLinearVel(m_body));
		dVectorSub(diffa, limiteda, dBodyGetAngularVel(m_body));
		dBodySetLinearVel(m_body, diffl[0], diffl[1], diffl[2]);
		dBodySetAngularVel(m_body, diffa[0], diffa[1], diffa[2]);
		dxStepBody(m_body, fixed_step);
		dBodySetLinearVel(m_body, limitedl[0], limitedl[1], limitedl[2]);
		dBodySetAngularVel(m_body, limiteda[0], limiteda[1], limiteda[2]);
	}
}
void CPHElement::ClearDestroyInfo( )
{
	xr_delete(m_fratures_holder);
}
