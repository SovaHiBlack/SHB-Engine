#include "stdafx.h"

#include "PHObject.h"
#include "Physics.h"
//#include "MathUtils.h"
//#include "PHValideValues.h"
#include "PHActivationShape.h"
#include "Extendedgeom.h"
#include "SpaceUtils.h"
//#include "MathUtils.h"
#include "GameMtlLib.h"
#include "Level.h"
#include "PHWorld.h"
#include "..\xrODE\util.h"

#ifdef DEBUG
#	include "PHDebug.h"
#endif // DEBUG

#include "PHDynamicData.h"
#include "PHSynchronize.h"
#include "phnetstate.h"

static f32 max_depth = 0.0f;
static f32 friction_factor = 0.0f;
static f32 cfm = 1.e-10f;
static f32 erp = 1.0f;

#ifdef DEBUG
#	define	CHECK_POS(pos, msg, br)			if (!valid_pos(pos, phBoundaries)) { Msg("pos:%f,%f,%f", pos.x, pos.y, pos.z); Msg(msg); VERIFY(!br); }
#else // def DEBUG
#	define	CHECK_POS(pos, msg, br)
#endif // def DEBUG

void ActivateTestDepthCallback(bool& do_colide, bool bo1, dContact& c, SGameMtl* material_1, SGameMtl* material_2)
{
	if (do_colide && !material_1->Flags.test(SGameMtl::flPassable) && !material_2->Flags.test(SGameMtl::flPassable))
	{
		f32& depth = c.geom.depth;
		f32 test_depth = depth;
		save_max(max_depth, test_depth);
		c.surface.mu *= friction_factor;

		c.surface.soft_cfm = cfm;
		c.surface.soft_erp = erp;
	}
}

void StaticEnvironment(bool& do_colide, bool bo1, dContact& c, SGameMtl* material_1, SGameMtl* material_2)
{
	dJointID contact_joint = dJointCreateContact(0, ContactGroup, &c);

	if (bo1)
	{
		((CPHActivationShape*)(retrieveGeomUserData(c.geom.g1)->callback_data))->DActiveIsland( )->ConnectJoint(contact_joint);
		dJointAttach(contact_joint, dGeomGetBody(c.geom.g1), 0);
	}
	else
	{
		((CPHActivationShape*)(retrieveGeomUserData(c.geom.g2)->callback_data))->DActiveIsland( )->ConnectJoint(contact_joint);
		dJointAttach(contact_joint, 0, dGeomGetBody(c.geom.g2));
	}

	do_colide = false;
}

void GetMaxDepthCallback(bool& do_colide, bool bo1, dContact& c, SGameMtl* material_1, SGameMtl* material_2)
{
	if (do_colide && !material_1->Flags.test(SGameMtl::flPassable) && !material_2->Flags.test(SGameMtl::flPassable))
	{
		f32& depth = c.geom.depth;
		f32 test_depth = depth;
		//save_max(max_depth,test_depth);
		max_depth += test_depth;
	}
	//do_colide=false;
}

void RestoreVelocityState(V_PH_WORLD_STATE& state)
{
	V_PH_WORLD_STATE::iterator i = state.begin( ), e = state.end( );
	for (; e != i; ++i)
	{
		CPHSynchronize& sync = *i->first;
		SPHNetState& old_s = i->second;
		SPHNetState new_s;
		sync.get_State(new_s);
		new_s.angular_vel.set(old_s.angular_vel);
		new_s.linear_vel.set(old_s.linear_vel);
		new_s.enabled = old_s.enabled;
		sync.set_State(new_s);
	}
}

CPHActivationShape::CPHActivationShape( )
{
	m_geom = NULL;
	m_body = NULL;
	m_flags.zero( );
	m_flags.set(flFixedRotation, TRUE);
}

CPHActivationShape::~CPHActivationShape( )
{
	VERIFY(!m_body && !m_geom);
}

void CPHActivationShape::Create(const fVector3 start_pos, const fVector3 start_size, CPhysicsShellHolder* ref_obj, EType _type, u16 flags)
{
	VERIFY(ref_obj);
	m_body = dBodyCreate(0);
	dMass m;
	dMassSetSphere(&m, 1.0f, 100000.0f);
	dMassAdjust(&m, 1.0f);
	dBodySetMass(m_body, &m);
	switch (_type)
	{
		case etBox:
		{
			m_geom = dCreateBox(0, start_size.x, start_size.y, start_size.z);
			break;
		}
		case etSphere:
		{
			m_geom = dCreateSphere(0, start_size.x);
			break;
		}
	};

	dGeomCreateUserData(m_geom);
	dGeomUserDataSetObjectContactCallback(m_geom, ActivateTestDepthCallback);
	dGeomUserDataSetPhysicsRefObject(m_geom, ref_obj);
	dGeomSetBody(m_geom, m_body);
	dBodySetPosition(m_body, start_pos.x, start_pos.y, start_pos.z);
	Island( ).AddBody(m_body);
	dBodyEnable(m_body);
	m_safe_state.create(m_body);
	spatial_register( );
	m_flags.set(flags, TRUE);
}

void CPHActivationShape::Destroy( )
{
	VERIFY(m_geom && m_body);
	spatial_unregister( );
	CPHObject::deactivate( );
	dGeomDestroyUserData(m_geom);
	dGeomDestroy(m_geom);
	m_geom = NULL;
	dBodyDestroy(m_body);
	m_body = NULL;
}

bool CPHActivationShape::Activate(const fVector3 need_size, u16 steps, f32 max_displacement, f32 max_rotation, bool un_freeze_later)
{

#ifdef DEBUG
	if (ph_dbg_draw_mask.test(phDbgDrawDeathActivationBox))
	{
		DBG_OpenCashedDraw( );
		fMatrix4x4 M;
		PHDynamicData::DMXPStoFMX(dBodyGetRotation(m_body), dBodyGetPosition(m_body), M);
		fVector3 v;
		dGeomBoxGetLengths(m_geom, cast_fp(v));
		v.mul(0.5f);
		DBG_DrawOBB(M, v, D3DCOLOR_XRGB(0, 255, 0));
	}
#endif // def DEBUG

	VERIFY(m_geom && m_body);
	CPHObject::activate( );
	ph_world->Freeze( );
	UnFreeze( );
	max_depth = 0.0f;

	dGeomUserDataSetObjectContactCallback(m_geom, GetMaxDepthCallback);
	//ph_world->Step();
	ph_world->StepTouch( );
	u16 num_it = 15;
	f32 fnum_it = f32(num_it);
	f32 fnum_steps = f32(steps);
	f32 fnum_steps_r = 1.0f / fnum_steps;
	f32 resolve_depth = 0.01f;
	f32 max_vel = max_depth / fnum_it * fnum_steps_r / fixed_step;
	f32 limit_l_vel = _max(_max(need_size.x, need_size.y), need_size.z) / fnum_it * fnum_steps_r / fixed_step;
	if (limit_l_vel > default_l_limit)
	{
		limit_l_vel = default_l_limit;
	}

	if (max_vel > limit_l_vel)
	{
		max_vel = limit_l_vel;
	}

	f32 max_a_vel = max_rotation / fnum_it * fnum_steps_r / fixed_step;
	if (max_a_vel > default_w_limit)
	{
		max_a_vel = default_w_limit;
	}

	//ph_world->CutVelocity(0.f,0.f);
	dGeomUserDataSetCallbackData(m_geom, this);
	dGeomUserDataSetObjectContactCallback(m_geom, ActivateTestDepthCallback);
	if (m_flags.test(flStaticEnvironment))
	{
		dGeomUserDataAddObjectContactCallback(m_geom, StaticEnvironment);
	}

	max_depth = 0.0f;

	fVector3 from_size;
	fVector3 step_size;
	fVector3 size;
	dGeomBoxGetLengths(m_geom, cast_fp(from_size));
	step_size.sub(need_size, from_size);
	step_size.mul(fnum_steps_r);
	size.set(from_size);
	bool ret = false;
	V_PH_WORLD_STATE temp_state;
	ph_world->GetState(temp_state);
	for (s32 m = 0; steps > m; ++m)
	{
		//f32 param =fnum_steps_r*(1+m);
		//InterpolateBox(id,param);
		size.add(step_size);
		dGeomBoxSetLengths(m_geom, size.x, size.y, size.z);
		u16 attempts = 10;
		do
		{
			ret = false;
			for (s32 i = 0; num_it > i; ++i)
			{
				max_depth = 0.f;
				ph_world->Step( );
				CHECK_POS(Position( ), "pos after ph_world->Step()", false);
				ph_world->CutVelocity(max_vel, max_a_vel);
				CHECK_POS(Position( ), "pos after CutVelocity", true);
				//if(m==0&&i==0)ph_world->GetState(temp_state);
				if (max_depth < resolve_depth)
				{
					ret = true;
					break;
				}
			}

			attempts--;
		}
		while (!ret && attempts > 0);

#ifdef DEBUG
		Msg("correction attempts %d", 10 - attempts);
#endif // def DEBUG

	}
	RestoreVelocityState(temp_state);
	CHECK_POS(Position( ), "pos after RestoreVelocityState(temp_state);", true);
	if (!un_freeze_later)
	{
		ph_world->UnFreeze( );
	}

#ifdef DEBUG
	if (ph_dbg_draw_mask.test(phDbgDrawDeathActivationBox))
	{
		DBG_OpenCashedDraw( );
		fMatrix4x4 M;
		PHDynamicData::DMXPStoFMX(dBodyGetRotation(m_body), dBodyGetPosition(m_body), M);
		fVector3 v;
		v.set(need_size);
		v.mul(0.5f);
		DBG_DrawOBB(M, v, D3DCOLOR_XRGB(0, 255, 255));
		DBG_ClosedCashedDraw(30000);
	}
#endif // def DEBUG

	return ret;
}

const fVector3& CPHActivationShape::Position( )
{
	return cast_fv(dBodyGetPosition(m_body));
}

void CPHActivationShape::Size(fVector3& size)
{
	dGeomBoxGetLengths(m_geom, cast_fp(size));
}

void CPHActivationShape::PhDataUpdate(dReal step)
{
	m_safe_state.new_state(m_body);
}

void CPHActivationShape::PhTune(dReal step)
{ }

dGeomID CPHActivationShape::dSpacedGeom( )
{
	return m_geom;
}

void CPHActivationShape::get_spatial_params( )
{
	spatialParsFromDGeom(m_geom, spatial.sphere.P, AABB, spatial.sphere.R);
}

void CPHActivationShape::InitContact(dContact* c, bool& do_collide, u16, u16)
{ }

void CPHActivationShape::CutVelocity(f32 l_limit, f32 a_limit)
{
	dVector3 limitedl;
	dVector3 diffl;
	if (dVectorLimit(dBodyGetLinearVel(m_body), l_limit, limitedl))
	{
		dVectorSub(diffl, limitedl, dBodyGetLinearVel(m_body));
		dBodySetLinearVel(m_body, diffl[0], diffl[1], diffl[2]);
		dBodySetAngularVel(m_body, 0.0f, 0.0f, 0.0f);
		dxStepBody(m_body, fixed_step);
		dBodySetLinearVel(m_body, limitedl[0], limitedl[1], limitedl[2]);
	}
}

void CPHActivationShape::set_rotation(const fMatrix4x4& sof)
{
	dMatrix3 rot;
	PHDynamicData::FMXtoDMX(sof, rot);
	dBodySetRotation(ODEBody( ), rot);
}
