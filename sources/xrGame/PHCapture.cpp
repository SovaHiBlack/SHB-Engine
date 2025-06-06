#include "stdafx.h"
#include "phcharacter.h"
#include "Physics.h"
#include "ExtendedGeom.h"
#include "PHCapture.h"
#include "EntityAlive.h"
#include "PHMovementControl.h"
#include "..\XR_3DA\skeletoncustom.h"
#include "characterphysicssupport.h"

void CPHCapture::CreateBody( )
{
	m_body = dBodyCreate(0);
	m_island.AddBody(m_body);
	dMass m;
	dMassSetSphere(&m, 1.f, 1000000.f);
	dMassAdjust(&m, 100000.f);
	dBodySetMass(m_body, &m);
	dBodySetGravityMode(m_body, 0);
}

CPHCapture::~CPHCapture( )
{
	Deactivate( );
}

void CPHCapture::PhDataUpdate(dReal /**step/**/)
{
	if (b_failed) return;
	switch (e_state)
	{
		case cstPulling:  PullingUpdate( );
			break;
		case cstCaptured: CapturedUpdate( );
			break;
		case cstReleased: ReleasedUpdate( );
			break;
		default: NODEFAULT;
	}
}

void CPHCapture::PhTune(dReal /**step/**/)
{
	if (b_failed) return;
	//if(!m_taget_object->PPhysicsShell())	{
	//	b_failed=true;
	//	return;			//. hack
	//}
	bool act_capturer = m_character->CPHObject::is_active( );
	bool act_taget = m_taget_object->PPhysicsShell( )->isEnabled( );
	b_disabled = !act_capturer && !act_taget;
	if (act_capturer)
	{
		m_taget_element->Enable( );
	}
	if (act_taget)
	{
		m_character->Enable( );
	}
	switch (e_state)
	{
		case cstPulling:;
			break;
		case cstCaptured:
		{
			if (b_disabled) dBodyDisable(m_body);
			else
			{
				m_character->Island( ).Merge(&m_island);
				m_taget_element->PhysicsShell( )->PIsland( )->Merge(&m_island);
			}
		}
		break;
		case cstReleased:;
			break;
		default: NODEFAULT;
	}
}

void CPHCapture::PullingUpdate( )
{
	if (!m_taget_element->isActive( ) || Device.dwTimeGlobal - m_time_start > m_capture_time)
	{
		Release( );
		return;
	}

	fVector3 dir;
	fVector3 capture_bone_position;
	CObject* object = smart_cast<CObject*>(m_character->PhysicsRefObject( ));
	capture_bone_position.set(m_capture_bone->mTransform.c);
	object->XFORM( ).transform_tiny(capture_bone_position);
	m_taget_element->GetGlobalPositionDynamic(&dir);
	dir.sub(capture_bone_position, dir);
	f32 dist = dir.magnitude( );
	if (dist > m_pull_distance)
	{
		Release( );
		return;
	}

	dir.mul(1.f / dist);
	if (dist < m_capture_distance)
	{
		m_back_force = 0.0f;

		m_joint = dJointCreateBall(0, 0);
		m_island.AddJoint(m_joint);
		m_ajoint = dJointCreateAMotor(0, 0);
		m_island.AddJoint(m_ajoint);
		dJointSetAMotorMode(m_ajoint, dAMotorEuler);
		dJointSetAMotorNumAxes(m_ajoint, 3);

		CreateBody( );
		dBodySetPosition(m_body, capture_bone_position.x, capture_bone_position.y, capture_bone_position.z);
		dJointAttach(m_joint, m_body, m_taget_element->get_body( ));
		dJointAttach(m_ajoint, m_body, m_taget_element->get_body( ));
		dJointSetFeedback(m_joint, &m_joint_feedback);
		dJointSetFeedback(m_ajoint, &m_joint_feedback);
		dJointSetBallAnchor(m_joint, capture_bone_position.x, capture_bone_position.y, capture_bone_position.z);
		dJointSetAMotorAxis(m_ajoint, 0, 1, dir.x, dir.y, dir.z);

		if (dir.x > EPSILON_5)
		{
			if (dir.y > EPSILON_5)
			{
				f32 mag = dir.x * dir.x + dir.y * dir.y;
				dJointSetAMotorAxis(m_ajoint, 2, 2, -dir.y / mag, dir.x / mag, 0.f);
			}
			else if (dir.z > EPSILON_5)
			{
				f32 mag = dir.x * dir.x + dir.z * dir.z;
				dJointSetAMotorAxis(m_ajoint, 2, 2, -dir.z / mag, 0.f, dir.x / mag);
			}
			else
			{
				dJointSetAMotorAxis(m_ajoint, 2, 2, 1.f, 0.f, 0.f);
			}
		}
		else
		{
			if (dir.y > EPSILON_5)
			{
				if (dir.z > EPSILON_5)
				{
					f32 mag = dir.y * dir.y + dir.z * dir.z;
					dJointSetAMotorAxis(m_ajoint, 2, 2, 0.f, -dir.z / mag, dir.y / mag);
				}
				else
				{
					dJointSetAMotorAxis(m_ajoint, 2, 2, 0.f, 1.f, 0.f);
				}
			}
			else
			{
				dJointSetAMotorAxis(m_ajoint, 2, 2, 0.f, 0.f, 1.f);
			}
		}
		//f32 hi=-M_PI/2.f,lo=-hi;
		//dJointSetAMotorParam(m_ajoint,dParamLoStop ,lo);
		//dJointSetAMotorParam(m_ajoint,dParamHiStop ,hi);	
		//dJointSetAMotorParam(m_ajoint,dParamLoStop2 ,lo);
		//dJointSetAMotorParam(m_ajoint,dParamHiStop2 ,hi);	
		//dJointSetAMotorParam(m_ajoint,dParamLoStop3 ,lo);
		//dJointSetAMotorParam(m_ajoint,dParamHiStop3 ,hi);

		dJointSetAMotorParam(m_ajoint, dParamFMax, m_capture_force * 0.2f);
		dJointSetAMotorParam(m_ajoint, dParamVel, 0.f);

		dJointSetAMotorParam(m_ajoint, dParamFMax2, m_capture_force * 0.2f);
		dJointSetAMotorParam(m_ajoint, dParamVel2, 0.f);

		dJointSetAMotorParam(m_ajoint, dParamFMax3, m_capture_force * 0.2f);
		dJointSetAMotorParam(m_ajoint, dParamVel3, 0.f);

///////////////////////////////////
		f32 sf = 0.1f, df = 10.f;

		f32 erp = ERP(world_spring * sf, world_damping * df);
		f32 cfm = CFM(world_spring * sf, world_damping * df);
		dJointSetAMotorParam(m_ajoint, dParamStopERP, erp);
		dJointSetAMotorParam(m_ajoint, dParamStopCFM, cfm);

		dJointSetAMotorParam(m_ajoint, dParamStopERP2, erp);
		dJointSetAMotorParam(m_ajoint, dParamStopCFM2, cfm);

		dJointSetAMotorParam(m_ajoint, dParamStopERP3, erp);
		dJointSetAMotorParam(m_ajoint, dParamStopCFM3, cfm);
		/////////////////////////////////////////////////////////////////////
		///dJointSetAMotorParam(m_joint1,dParamFudgeFactor ,0.1f);
		//dJointSetAMotorParam(m_joint1,dParamFudgeFactor2 ,0.1f);
		//dJointSetAMotorParam(m_joint1,dParamFudgeFactor3 ,0.1f);
		/////////////////////////////////////////////////////////////////////////////
		sf = 0.1f, df = 10.f;
		erp = ERP(world_spring * sf, world_damping * df);
		cfm = CFM(world_spring * sf, world_damping * df);
		dJointSetAMotorParam(m_ajoint, dParamCFM, cfm);
		dJointSetAMotorParam(m_ajoint, dParamCFM2, cfm);
		dJointSetAMotorParam(m_ajoint, dParamCFM3, cfm);

///////////////////////////

		//dJointSetAMotorParam(m_ajoint,dParamLoStop ,0.f);
		//dJointSetAMotorParam(m_ajoint,dParamHiStop ,0.f);	

		m_taget_element->set_DynamicLimits( );
		//m_taget_object->PPhysicsShell()->set_JointResistance()
		e_state = cstCaptured;
		return;
	}
	m_taget_element->applyForce(dir, m_pull_force);
}

void CPHCapture::CapturedUpdate( )
{
	m_island.Unmerge( );
	if (m_character->CPHObject::is_active( ))
	{
		m_taget_element->Enable( );
	}

	if (!m_taget_element->isActive( ) || dDOT(m_joint_feedback.f2, m_joint_feedback.f2) > m_capture_force * m_capture_force)
	{
		Release( );
		return;
	}

	f32 mag = dSqrt(dDOT(m_joint_feedback.f1, m_joint_feedback.f1));

	//m_back_force=m_back_force*0.999f+ ((mag<m_capture_force/5.f) ? mag : (m_capture_force/5.f))*0.001f;
	//
	if (b_character_feedback && mag > m_capture_force / 2.2f)
	{
		f32 f = mag / (m_capture_force / 15.f);
		m_character->ApplyForce(m_joint_feedback.f1[0] / f, m_joint_feedback.f1[1] / f, m_joint_feedback.f1[2] / f);
	}

	fVector3 capture_bone_position;
	CObject* object = smart_cast<CObject*>(m_character->PhysicsRefObject( ));
	capture_bone_position.set(m_capture_bone->mTransform.c);
	object->XFORM( ).transform_tiny(capture_bone_position);
	dBodySetPosition(m_body, capture_bone_position.x, capture_bone_position.y, capture_bone_position.z);
}

void CPHCapture::ReleasedUpdate( )
{
	if (b_disabled) return;
	if (!b_collide)
	{
		b_failed = true;
		m_taget_element->Enable( );
	}
	b_collide = false;
}

void CPHCapture::ReleaseInCallBack( )
{
	//	if(!b_failed) return;
	b_collide = true;
}

void CPHCapture::object_contactCallbackFun(bool& do_colide, bool bo1, dContact& c, SGameMtl* /*material_1*/, SGameMtl* /*material_2*/)
{
	dxGeomUserData* l_pUD1 = NULL;
	dxGeomUserData* l_pUD2 = NULL;
	l_pUD1 = retrieveGeomUserData(c.geom.g1);
	l_pUD2 = retrieveGeomUserData(c.geom.g2);

	if (!l_pUD1) return;
	if (!l_pUD2) return;

	CEntityAlive* capturer = smart_cast<CEntityAlive*>(l_pUD1->ph_ref_object);
	if (capturer)
	{
		CPHCapture* capture = capturer->character_physics_support( )->movement( )->PHCapture( );
		if (capture)
		{
			if (capture->m_taget_element->PhysicsRefObject( ) == l_pUD2->ph_ref_object)
			{
				do_colide = false;
				capture->m_taget_element->Enable( );
				if (capture->e_state == CPHCapture::cstReleased) capture->ReleaseInCallBack( );
			}

		}
	}

	capturer = smart_cast<CEntityAlive*>(l_pUD2->ph_ref_object);
	if (capturer)
	{
		CPHCapture* capture = capturer->character_physics_support( )->movement( )->PHCapture( );
		if (capture)
		{
			if (capture->m_taget_element->PhysicsRefObject( ) == l_pUD1->ph_ref_object)
			{
				do_colide = false;
				capture->m_taget_element->Enable( );
				if (capture->e_state == CPHCapture::cstReleased) capture->ReleaseInCallBack( );
			}

		}
	}
}

void CPHCapture::net_Relcase(CObject* O)
{
	if (static_cast<CObject*>(m_taget_object) == O)
	{
		Deactivate( );
	}
}
