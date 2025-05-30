#include "stdafx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
#include "PHJointDestroyInfo.h"
#include "ExtendedGeom.h"
#include "PHElement.h"
#include "PHJoint.h"
#include "PHShell.h"

const f32 hinge2_spring = 20000.0f;
const f32 hinge2_damping = 1000.0f;

IC dBodyID body_for_joint(CPhysicsElement* e)
{
	return (e->isFixed( ) ? 0 : e->get_body( ));
}

IC void SwapLimits(f32& lo, f32& hi)
{
	f32 t = -lo;
	lo = -hi;
	hi = t;
}

CPHJoint::~CPHJoint( )
{
	xr_delete(m_destroy_info);
	VERIFY(!bActive);
	axes.clear( );
	if (m_back_ref)
	{
		*m_back_ref = NULL;
	}
}

void CPHJoint::SetBackRef(CPhysicsJoint** j)
{
	R_ASSERT2(*j == static_cast<CPhysicsJoint*>(this), "wronng reference");
	m_back_ref = j;
}

void CPHJoint::CreateBall( )
{
	m_joint = dJointCreateBall(0, 0);
	fVector3 pos;
	fMatrix4x4 first_matrix;
	fMatrix4x4 second_matrix;
	CPHElement* first = (pFirst_element);
	CPHElement* second = (pSecond_element);

	VERIFY(first && second);
	first->GetGlobalTransformDynamic(&first_matrix);
	second->GetGlobalTransformDynamic(&second_matrix);
	pos.set(0.0f, 0.0f, 0.0f);
	switch (vs_anchor)
	{
		case vs_first:
		{
			first_matrix.transform_tiny(pos, anchor);
		}
		break;
		case vs_second:
		{
			second_matrix.transform_tiny(pos, anchor);
		}
		break;
		case vs_global:
		{
			pShell->mXFORM.transform_tiny(pos, anchor);
		}
		break;
		default:
		{
			NODEFAULT;
		}
	}

	dJointAttach(m_joint, body_for_joint(first), body_for_joint(second));
	dJointSetBallAnchor(m_joint, pos.x, pos.y, pos.z);
}

void CPHJoint::CreateHinge( )
{
	m_joint = dJointCreateHinge(0, 0);

	fVector3 pos;
	fMatrix4x4 first_matrix;
	fMatrix4x4 second_matrix;
	fVector3 axis;

	CPHElement* first = (pFirst_element);
	CPHElement* second = (pSecond_element);
	VERIFY(first && second);
	first->GetGlobalTransformDynamic(&first_matrix);
	second->GetGlobalTransformDynamic(&second_matrix);

	pos.set(0, 0, 0);
	switch (vs_anchor)
	{
		case vs_first:
		{
			first_matrix.transform_tiny(pos, anchor);
		}
		break;
		case vs_second:
		{
			second_matrix.transform_tiny(pos, anchor);
		}
		break;
		case vs_global:
		{
			pShell->mXFORM.transform_tiny(pos, anchor);
		}
		break;
		default:
		{
			NODEFAULT;
		}
	}

	axis.set(0.0f, 0.0f, 0.0f);

	first_matrix.invert( );

	fMatrix4x4 rotate;
	rotate.mul(first_matrix, second_matrix);

	f32 hi;
	f32 lo;
	CalcAxis(0, axis, lo, hi, first_matrix, second_matrix, rotate);
	dBodyID b1 = body_for_joint(first); if (!b1)axis.invert( );//SwapLimits(lo,hi);
	dJointAttach(m_joint, b1, body_for_joint(second));

	dJointSetHingeAnchor(m_joint, pos.x, pos.y, pos.z);
	dJointSetHingeAxis(m_joint, axis.x, axis.y, axis.z);

	dJointSetHingeParam(m_joint, dParamLoStop, lo);
	dJointSetHingeParam(m_joint, dParamHiStop, hi);
	if (axes[0].force > 0.0f)
	{
		dJointSetHingeParam(m_joint, dParamFMax, axes[0].force);
		dJointSetHingeParam(m_joint, dParamVel, axes[0].velocity);
	}

	dJointSetHingeParam(m_joint, dParamStopERP, axes[0].erp);
	dJointSetHingeParam(m_joint, dParamStopCFM, axes[0].cfm);

	dJointSetHingeParam(m_joint, dParamCFM, m_cfm);
}

void CPHJoint::CreateHinge2( )
{
	m_joint = dJointCreateHinge2(0, 0);

	fVector3 pos;
	fMatrix4x4 first_matrix;
	fMatrix4x4 second_matrix;
	fVector3 axis;
	CPHElement* first = (pFirst_element);
	CPHElement* second = (pSecond_element);
	VERIFY(first && second);
	first->GetGlobalTransformDynamic(&first_matrix);
	second->GetGlobalTransformDynamic(&second_matrix);
	pos.set(0.0f, 0.0f, 0.0f);
	switch (vs_anchor)
	{
		case vs_first:
		{
			first_matrix.transform_tiny(pos, anchor);
		}
		break;
		case vs_second:
		{
			second_matrix.transform_tiny(pos, anchor);
		}
		break;
		case vs_global:
		{
			pShell->mXFORM.transform_tiny(pos, anchor);
		}
		break;
		default:
		{
			NODEFAULT;
		}
	}
	//////////////////////////////////////

	dBodyID b1 = body_for_joint(first);
	dJointAttach(m_joint, b1, body_for_joint(second));
	dJointSetHinge2Anchor(m_joint, pos.x, pos.y, pos.z);

	/////////////////////////////////////////////

	fMatrix4x4 first_matrix_inv;
	first_matrix_inv.set(first_matrix);
	first_matrix_inv.invert( );
	fMatrix4x4 rotate;

	rotate.mul(first_matrix_inv, second_matrix);
	/////////////////////////////////////////////

	f32 lo;
	f32 hi;
	//////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	axis.set(0, 0, 0);
	CalcAxis(0, axis, lo, hi, first_matrix, second_matrix, rotate);
	if (!b1)axis.invert( );//SwapLimits(lo,hi);
	dJointSetHinge2Axis1(m_joint, axis.x, axis.y, axis.z);

	dJointSetHinge2Param(m_joint, dParamLoStop, lo);
	dJointSetHinge2Param(m_joint, dParamHiStop, hi);

	if (!(axes[0].force < 0.f))
	{
		dJointSetHinge2Param(m_joint, dParamFMax, axes[0].force);
		dJointSetHinge2Param(m_joint, dParamVel, axes[0].velocity);
	}

	CalcAxis(1, axis, lo, hi, first_matrix, second_matrix, rotate);

	dJointSetHinge2Axis2(m_joint, axis.x, axis.y, axis.z);

	dJointSetHinge2Param(m_joint, dParamLoStop2, lo);
	dJointSetHinge2Param(m_joint, dParamHiStop2, hi);
	if (!(axes[1].force < 0.f))
	{
		dJointSetHinge2Param(m_joint, dParamFMax2, axes[1].force);
		dJointSetHinge2Param(m_joint, dParamVel2, axes[1].velocity);
	}
	//////////////////////////////////////////////////////////////////

	dJointSetHinge2Param(m_joint, dParamSuspensionERP, m_erp);
	dJointSetHinge2Param(m_joint, dParamSuspensionCFM, m_cfm);
	//dJointSetHinge2Param(m_joint, dParamStopERP, 1.f);
	//dJointSetHinge2Param(m_joint, dParamStopCFM,0.f);
	dJointSetHinge2Param(m_joint, dParamStopERP, axes[0].erp);
	dJointSetHinge2Param(m_joint, dParamStopCFM, axes[0].cfm);
}

void CPHJoint::CreateSlider( )
{
	fVector3 pos;
	fMatrix4x4 first_matrix;
	fMatrix4x4 second_matrix;
	fVector3 axis;
	CPHElement* first = (pFirst_element);
	CPHElement* second = (pSecond_element);

	VERIFY(first);
	first->GetGlobalTransformDynamic(&first_matrix);
	dBodyID body1 = body_for_joint(first);
	VERIFY(second);
	second->GetGlobalTransformDynamic(&second_matrix);
	dBodyID body2 = body_for_joint(second);

	pos.set(0.0f, 0.0f, 0.0f);
	switch (vs_anchor)
	{
		case vs_first:
		{
			first_matrix.transform_tiny(pos, anchor);
		}
		break;
		case vs_second:
		{
			second_matrix.transform_tiny(pos, anchor);
		}
		break;
		case vs_global:
		{
			pShell->mXFORM.transform_tiny(pos, anchor);
		}
		break;
		default:
		{
			NODEFAULT;
		}
	}
	//////////////////////////////////////

	m_joint = dJointCreateSlider(0, 0);
	dJointAttach(m_joint, body1, body2);
	if (body1)
	{
		axes[0].vs = vs_first;
		axes[1].vs = vs_first;
	}
	else if (body2)
	{
		axes[0].vs = vs_second;
		axes[1].vs = vs_second;
	}

	m_joint1 = dJointCreateAMotor(0, 0);
	dJointSetAMotorMode(m_joint1, dAMotorEuler);
	dJointSetAMotorNumAxes(m_joint1, 1);

	dJointAttach(m_joint1, body1, body2);

	/////////////////////////////////////////////
	fMatrix4x4 first_matrix_inv;
	first_matrix_inv.set(first_matrix);
	first_matrix_inv.invert( );
	fMatrix4x4 rotate;
	rotate.mul(first_matrix_inv, second_matrix);
	/////////////////////////////////////////////

	f32 lo;
	f32 hi;
	//////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	axis.set(0, 0, 0);
	//axis 0
	CalcAxis(0, axis, lo, hi, first_matrix, second_matrix, rotate);
	//if(body1)axis.invert();//SwapLimits(lo,hi);!!!

	dJointSetSliderAxis(m_joint, axis.x, axis.y, axis.z);

	dJointSetSliderParam(m_joint, dParamLoStop, lo);
	dJointSetSliderParam(m_joint, dParamHiStop, hi);

	if (!(axes[0].force < 0.f))
	{
		dJointSetSliderParam(m_joint, dParamFMax, axes[0].force);
		dJointSetSliderParam(m_joint, dParamVel, axes[0].velocity);
	}

	dJointSetSliderParam(m_joint, dParamStopERP, axes[0].erp);
	dJointSetSliderParam(m_joint, dParamStopCFM, axes[0].cfm);

	//axis 1

	CalcAxis(1, axis, lo, hi, first_matrix, second_matrix, rotate);
	if (!body1)axis.invert( );//SwapLimits(lo,hi);
	int rel = body1 ? 1 : 2;
	dJointSetAMotorAxis(m_joint1, 0, rel, axis.x, axis.y, axis.z);
	dJointSetAMotorParam(m_joint1, dParamLoStop, lo);
	dJointSetAMotorParam(m_joint1, dParamHiStop, hi);
	if (!(axes[1].force < 0.f))
	{
		dJointSetAMotorParam(m_joint1, dParamFMax, axes[1].force);
		dJointSetAMotorParam(m_joint1, dParamVel, axes[1].velocity);
	}

	dJointSetAMotorParam(m_joint1, dParamStopERP, axes[1].erp);
	dJointSetAMotorParam(m_joint1, dParamStopCFM, axes[1].cfm);

	/////////////////////////////////////////////////////////////////////
	///dJointSetAMotorParam(m_joint1,dParamFudgeFactor ,0.1f);
	//dJointSetAMotorParam(m_joint1,dParamFudgeFactor2 ,0.1f);
	//dJointSetAMotorParam(m_joint1,dParamFudgeFactor3 ,0.1f);
	/////////////////////////////////////////////////////////////////////////////
	dJointSetAMotorParam(m_joint1, dParamCFM, m_cfm);
	dJointSetSliderParam(m_joint, dParamCFM, m_cfm);
}

#ifdef ODE_SLOW_SOLVER
#define FIX_BY_ONE_HINGE
#endif
void CPHJoint::CreateFullControl( )
{
	fVector3 pos;
	fMatrix4x4 first_matrix;
	fMatrix4x4 second_matrix;
	fVector3 axis;
	CPHElement* first = (pFirst_element);
	CPHElement* second = (pSecond_element);
	VERIFY(first);
	first->GetGlobalTransformDynamic(&first_matrix);
	dBodyID body1 = body_for_joint(first);
	VERIFY(second);
	second->GetGlobalTransformDynamic(&second_matrix);
	dBodyID body2 = body_for_joint(second);

	pos.set(0.0f, 0.0f, 0.0f);
	switch (vs_anchor)
	{
		case vs_first:
		{
			first_matrix.transform_tiny(pos, anchor);
		}
		break;
		case vs_second:
		{
			second_matrix.transform_tiny(pos, anchor);
		}
		break;
		case vs_global:
		{
			pShell->mXFORM.transform_tiny(pos, anchor);
		}
		break;
		default:
		{
			NODEFAULT;
		}
	}
	//////////////////////////////////////
	m_joint = dJointCreateBall(0, 0);
	dJointAttach(m_joint, body1, body2);
	dJointSetBallAnchor(m_joint, pos.x, pos.y, pos.z);

	m_joint1 = dJointCreateAMotor(0, 0);
	dJointSetAMotorMode(m_joint1, dAMotorEuler);
	dJointSetAMotorNumAxes(m_joint1, 3);

	dJointAttach(m_joint1, body1, body2);

	/////////////////////////////////////////////
	fMatrix4x4 first_matrix_inv;
	first_matrix_inv.set(first_matrix);
	first_matrix_inv.invert( );
	fMatrix4x4 rotate;
	rotate.mul(first_matrix_inv, second_matrix);
	/////////////////////////////////////////////

	f32 lo;
	f32 hi;
	//////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	axis.set(0, 0, 0);
	//axis 0
	CalcAxis(0, axis, lo, hi, first_matrix, second_matrix, rotate);
	if (!body1)axis.invert( );//SwapLimits(lo,hi);
	dJointSetAMotorAxis(m_joint1, 0, 1, axis.x, axis.y, axis.z);
	dJointSetAMotorParam(m_joint1, dParamLoStop, lo);
	dJointSetAMotorParam(m_joint1, dParamHiStop, hi);

	if (!(axes[0].force < 0.f))
	{
		dJointSetAMotorParam(m_joint1, dParamFMax, axes[0].force);
		dJointSetAMotorParam(m_joint1, dParamVel, axes[0].velocity);
	}

	//axis 1
	CalcAxis(1, axis, lo, hi, first_matrix, second_matrix, rotate);
	if (!body1)axis.invert( );//SwapLimits(lo,hi);
	dJointSetAMotorParam(m_joint1, dParamLoStop2, lo);
	dJointSetAMotorParam(m_joint1, dParamHiStop2, hi);
	if (!(axes[1].force < 0.f))
	{
		dJointSetAMotorParam(m_joint1, dParamFMax2, axes[1].force);
		dJointSetAMotorParam(m_joint1, dParamVel2, axes[1].velocity);
	}

	//axis 2
	CalcAxis(2, axis, lo, hi, first_matrix, second_matrix, rotate);
	if (!body1)axis.invert( );//SwapLimits(lo,hi);
	dJointSetAMotorAxis(m_joint1, 2, 2, axis.x, axis.y, axis.z);
	dJointSetAMotorParam(m_joint1, dParamLoStop3, lo);
	dJointSetAMotorParam(m_joint1, dParamHiStop3, hi);
	if (!(axes[2].force < 0.f))
	{
		dJointSetAMotorParam(m_joint1, dParamFMax3, axes[2].force);
		dJointSetAMotorParam(m_joint1, dParamVel3, axes[2].velocity);
	}

	dJointSetAMotorParam(m_joint1, dParamStopERP, axes[0].erp);
	dJointSetAMotorParam(m_joint1, dParamStopCFM, axes[0].cfm);

	dJointSetAMotorParam(m_joint1, dParamStopERP2, axes[1].erp);
	dJointSetAMotorParam(m_joint1, dParamStopCFM2, axes[1].cfm);

	dJointSetAMotorParam(m_joint1, dParamStopERP3, axes[2].erp);
	dJointSetAMotorParam(m_joint1, dParamStopCFM3, axes[2].cfm);
	/////////////////////////////////////////////////////////////////////
		///dJointSetAMotorParam(m_joint1,dParamFudgeFactor ,0.1f);
		//dJointSetAMotorParam(m_joint1,dParamFudgeFactor2 ,0.1f);
		//dJointSetAMotorParam(m_joint1,dParamFudgeFactor3 ,0.1f);
	/////////////////////////////////////////////////////////////////////////////
	dJointSetAMotorParam(m_joint1, dParamCFM, m_cfm);
	dJointSetAMotorParam(m_joint1, dParamCFM2, m_cfm);
	dJointSetAMotorParam(m_joint1, dParamCFM3, m_cfm);
}

void CPHJoint::SetAnchor(const f32 x, const f32 y, const f32 z)
{
	vs_anchor = vs_global;
	anchor.set(x, y, z);
}

void CPHJoint::SetAnchorVsFirstElement(const f32 x, const f32 y, const f32 z)
{
	vs_anchor = vs_first;
	anchor.set(x, y, z);
}

void CPHJoint::SetAnchorVsSecondElement(const f32 x, const f32 y, const f32 z)
{
	vs_anchor = vs_second;
	anchor.set(x, y, z);
}

void CPHJoint::SetAxisDir(const f32 x, const f32 y, const f32 z, const s32 axis_num)
{
	s32 ax = axis_num;
	LimitAxisNum(ax);
	if (-1 == ax)
	{
		return;
	}

	axes[ax].vs = vs_global;
	axes[ax].direction.set(x, y, z);
}

void CPHJoint::SetAxisDirVsFirstElement(const f32 x, const f32 y, const f32 z, const s32 axis_num)
{
	s32 ax = axis_num;
	LimitAxisNum(ax);
	if (-1 == ax)
	{
		return;
	}

	axes[ax].vs = vs_first;
	axes[ax].direction.set(x, y, z);
}

void CPHJoint::SetAxisDirVsSecondElement(const f32 x, const f32 y, const f32 z, const s32 axis_num)
{
	s32 ax = axis_num;
	LimitAxisNum(ax);
	if (-1 == ax)
	{
		return;
	}

	axes[ax].vs = vs_second;
	axes[ax].direction.set(x, y, z);
}

void CPHJoint::SetLimits(const f32 low, const f32 high, const s32 axis_num)
{
	if (!(pFirst_element && pSecond_element))
	{
		return;
	}

	s32 ax = axis_num;
	LimitAxisNum(ax);
	if (-1 == ax)
	{
		return;
	}

	fVector3 axis;
	switch (axes[ax].vs)
	{
		case vs_first:
		{
			pFirst_element->mXFORM.transform_dir(axis, axes[ax].direction);
		}
		break;
		case vs_second:
		{
			pSecond_element->mXFORM.transform_dir(axis, axes[ax].direction);
		}
		break;
		case vs_global:
		default:
		{
			axis.set(axes[ax].direction);
		}
	}

	axes[ax].low = low;
	axes[ax].high = high;
	fMatrix4x4 m1;
	fMatrix4x4 m2;
	m1.set(pFirst_element->mXFORM);
	m1.invert( );
	m2.mul(m1, pSecond_element->mXFORM);
	//m2.mul(pSecond_element->mXFORM,m1);

	f32 zer;
	//axis_angleB(m2,axis,zer);
	axis_angleA(m2, axes[ax].direction, zer);

	axes[ax].zero = zer;
	//m2.invert();
	//axes[ax].zero_transform.set(m2);
	if (bActive)
	{
		SetLimitsActive(axis_num);
	}
}

CPHJoint::CPHJoint(CPhysicsJoint::enumType type, CPhysicsElement* first, CPhysicsElement* second)
{
	pShell = NULL;
	m_bone_id = u16(-1);
	m_back_ref = NULL;
	m_destroy_info = NULL;
	pFirstGeom = NULL;
	pFirst_element = cast_PHElement(first);
	pSecond_element = cast_PHElement(second);
	m_joint = NULL;
	m_joint1 = NULL;
	eType = type;
	bActive = false;

#ifndef ODE_SLOW_SOLVER
	m_erp = world_erp;
	m_cfm = world_cfm;
#else
	m_erp = world_erp;
	m_cfm = world_cfm;
#endif

	SPHAxis axis;
	SPHAxis axis2;
	SPHAxis axis3;
	axis2.set_direction(1.0f, 0.0f, 0.0f);
	axis3.direction.crossproduct(axis.direction, axis3.direction);
	vs_anchor = vs_first;

	switch (eType)
	{
		case ball:
		{
		}
		break;
		case hinge:
		{
			axes.push_back(axis);
		}
		break;
		case hinge2:
		{
			axes.push_back(axis);
			axes.push_back(axis2);
		}
		break;
		case full_control:
		{
			axes.push_back(axis);
			axes.push_back(axis2);
			axes.push_back(axis3);
		}
		case slider:
		{
			axes.push_back(axis);
			axes.push_back(axis);
		}
	}
}

void CPHJoint::SetLimitsVsFirstElement(const f32 low, const f32 high, const s32 axis_num)
{ }

void CPHJoint::SetLimitsVsSecondElement(const f32 low, const f32 high, const s32 axis_num)
{ }

void CPHJoint::Create( )
{
	if (bActive)
	{
		return;
	}

	switch (eType)
	{
		case ball:
		{
			CreateBall( );
		}
		break;
		case hinge:
		{
			CreateHinge( );
		}
		break;
		case hinge2:
		{
			CreateHinge2( );
		}
		break;
		case full_control:
		{
			CreateFullControl( );
		}
		break;
		case slider:
		{
			CreateSlider( );
		}
		break;
	}

	if (m_destroy_info)
	{
		dJointSetFeedback(m_joint, m_destroy_info->JointFeedback( ));
		if (m_joint1)
		{
			dJointSetFeedback(m_joint1, m_destroy_info->JointFeedback( ));
		}
	}

	dJointSetData(m_joint, (pvoid)this);
	if (m_joint1)
	{
		dJointSetData(m_joint1, (pvoid)this);
	}

	bActive = true;
}

void CPHJoint::RunSimulation( )
{
	pShell->Island( ).AddJoint(m_joint);
	///dWorldAddJoint(phWorld,m_joint);
	if (m_joint1)
	{
		//dWorldAddJoint(phWorld,m_joint1);
		pShell->Island( ).AddJoint(m_joint1);
	}
}

void CPHJoint::Activate( )
{
	Create( );
	RunSimulation( );
}

void CPHJoint::Deactivate( )
{
	if (!bActive)
	{
		return;
	}

	switch (eType)
	{
		case ball:
		case hinge:
		case hinge2:
		{
			if (m_joint->world)
			{
				pShell->Island( ).RemoveJoint(m_joint);
			}

			dJointDestroy(m_joint);
		}
		break;
		case full_control:
		case slider:
		{
			if (m_joint->world)
			{
				pShell->Island( ).RemoveJoint(m_joint);
			}

			if (m_joint1->world)
			{
				pShell->Island( ).RemoveJoint(m_joint1);
			}

			dJointDestroy(m_joint);
			dJointDestroy(m_joint1);
			m_joint1 = NULL;
		}
		break;
	}

	m_joint = NULL;
	bActive = false;
}

void CPHJoint::ReattachFirstElement(CPHElement* new_element)
{
	Deactivate( );
	pFirst_element = (new_element);
	Activate( );
	//dJointAttach(m_joint,pFirst_element->get_body(),pSecond_element->get_body());
	//if(m_joint1)dJointAttach(m_joint1,pFirst_element->get_body(),pSecond_element->get_body());
}

void CPHJoint::SetForceAndVelocity(const f32 force, const f32 velocity, const s32 axis_num)
{
	if (pShell && pShell->isActive( ))
	{
		pShell->Enable( );
	}

	SetForce(force, axis_num);
	SetVelocity(velocity, axis_num);
}

void CPHJoint::GetMaxForceAndVelocity(f32& force, f32& velocity, s32 axis_num)
{
	force = axes[axis_num].force;
	velocity = axes[axis_num].velocity;
}

void CPHJoint::SetForce(const f32 force, const s32 axis_num)
{
	s32 ax;
	ax = axis_num;
	LimitAxisNum(ax);

	if (ax == -1)
		switch (eType)
		{
			case ball:
			{
				return;
			}
			case hinge:
			{
				axes[0].force = force;
			}
			break;
			case hinge2:
			case slider:
			{
				axes[0].force = force;
				axes[1].force = force;
			}
			break;
			case full_control:
			{
				axes[0].force = force;
				axes[1].force = force;
				axes[2].force = force;
			}
			break;
		}
	else
	{
		axes[ax].force = force;
	}

	if (bActive)
	{
		SetForceActive(ax);
	}
}

void CPHJoint::SetForceActive(const s32 axis_num)
{
	switch (eType)
	{
		case hinge2:
		{
			switch (axis_num)
			{
				case -1:
				{
					dJointSetHinge2Param(m_joint, dParamFMax, axes[0].force);
					dJointSetHinge2Param(m_joint, dParamFMax2, axes[1].force);
				}
				case 0:
				{
					dJointSetHinge2Param(m_joint, dParamFMax, axes[0].force);
				}
				break;
				case 1:
				{
					dJointSetHinge2Param(m_joint, dParamFMax2, axes[1].force);
				}
				break;
			}
		}
		break;
		case slider:
		{
			switch (axis_num)
			{
				case -1:
				{
					dJointSetSliderParam(m_joint, dParamFMax, axes[0].force);
					dJointSetAMotorParam(m_joint1, dParamFMax, axes[1].force);
				}
				case 0:
				{
					dJointSetSliderParam(m_joint, dParamFMax, axes[0].force);
				}
				break;
				case 1:
				{
					dJointSetAMotorParam(m_joint1, dParamFMax, axes[1].force);
				}
				break;
			}
		}
		break;
		case ball:
		{
		}
		break;
		case hinge:
		{
			dJointSetHingeParam(m_joint, dParamFMax, axes[0].force);
		}
		break;
		case full_control:
		{
			switch (axis_num)
			{
				case -1:
				{
					dJointSetAMotorParam(m_joint1, dParamFMax, axes[0].force);
					dJointSetAMotorParam(m_joint1, dParamFMax2, axes[1].force);
					dJointSetAMotorParam(m_joint1, dParamFMax3, axes[2].force);
				}
				case 0:
				{
					dJointSetAMotorParam(m_joint1, dParamFMax, axes[0].force);
				}
				break;
				case 1:
				{
					dJointSetAMotorParam(m_joint1, dParamFMax2, axes[1].force);
				}
				break;
				case 2:
				{
					dJointSetAMotorParam(m_joint1, dParamFMax3, axes[2].force);
				}
				break;
			}
		}
		break;
	}
}

void CPHJoint::SetVelocity(const f32 velocity, const s32 axis_num)
{
	s32 ax;
	ax = axis_num;
	LimitAxisNum(ax);

	if (ax == -1)
	{
		switch (eType)
		{
			case ball:
			{
				return;
			}
			case hinge:
			{
				axes[0].velocity = velocity;
			}
			break;
			case hinge2:
			case slider:
			{
				axes[0].velocity = velocity;
				axes[1].velocity = velocity;
			}
			break;
			case full_control:
			{
				axes[0].velocity = velocity;
				axes[1].velocity = velocity;
				axes[2].velocity = velocity;
			}
			break;
		}
	}
	else
	{
		axes[ax].velocity = velocity;
	}

	if (bActive)
	{
		SetVelocityActive(ax);
	}
}

void CPHJoint::SetVelocityActive(const s32 axis_num)
{
	switch (eType)
	{
		case hinge2:
		{
			switch (axis_num)
			{
				case -1:
				{
					dJointSetHinge2Param(m_joint, dParamVel, axes[0].velocity);
					dJointSetHinge2Param(m_joint, dParamVel2, axes[1].velocity);
				}
				case 0:
				{
					dJointSetHinge2Param(m_joint, dParamVel, axes[0].velocity);
				}
				break;
				case 1:
				{
					dJointSetHinge2Param(m_joint, dParamVel2, axes[1].velocity);
				}
				break;
			}
		}
		break;
		case slider:
		{
			switch (axis_num)
			{
				case -1:
				{
					dJointSetSliderParam(m_joint, dParamVel, axes[0].velocity);
					dJointSetAMotorParam(m_joint1, dParamVel, axes[1].velocity);
				}
				case 0:
				{
					dJointSetSliderParam(m_joint, dParamVel, axes[0].velocity);
				}
				break;
				case 1:
				{
					dJointSetAMotorParam(m_joint1, dParamVel, axes[1].velocity);
				}
				break;
			}
		}
		break;
		case ball:
		{
		}
		break;
		case hinge:
		{
			dJointSetHingeParam(m_joint, dParamVel, axes[0].velocity);
		}
		break;
		case full_control:
		{
			switch (axis_num)
			{
				case -1:
				{
					dJointSetAMotorParam(m_joint1, dParamVel, axes[0].velocity);
					dJointSetAMotorParam(m_joint1, dParamVel2, axes[1].velocity);
					dJointSetAMotorParam(m_joint1, dParamVel3, axes[2].velocity);
				}
				case 0:
				{
					dJointSetAMotorParam(m_joint1, dParamVel, axes[0].velocity);
				}
				break;
				case 1:
				{
					dJointSetAMotorParam(m_joint1, dParamVel2, axes[1].velocity);
				}
				break;
				case 2:
				{
					dJointSetAMotorParam(m_joint1, dParamVel3, axes[2].velocity);
				}break;
			}
		}
		break;
	}
}

void CPHJoint::SetLimitsActive(s32 axis_num)
{
	switch (eType)
	{
		case hinge2:
		{
			switch (axis_num)
			{
				case -1:
				case 0:
				case 1:
				{
					dJointSetHinge2Param(m_joint, dParamLoStop, axes[0].low);
					dJointSetHinge2Param(m_joint, dParamHiStop, axes[0].high);
				}
				break;
			}
		}
		break;
		case slider:
		{
			switch (axis_num)
			{
				case -1:
				{
					dJointSetSliderParam(m_joint, dParamLoStop, axes[0].low);
					dJointSetSliderParam(m_joint, dParamHiStop, axes[0].high);
					dJointSetAMotorParam(m_joint1, dParamLoStop, axes[1].low);
					dJointSetAMotorParam(m_joint1, dParamHiStop, axes[1].high);
				}
				case 0:
				{
					dJointSetSliderParam(m_joint, dParamLoStop, axes[0].low);
					dJointSetSliderParam(m_joint, dParamHiStop, axes[0].high);
				}
				break;
				case 1:
				{
					dJointSetAMotorParam(m_joint1, dParamLoStop, axes[1].low);
					dJointSetAMotorParam(m_joint1, dParamHiStop, axes[1].high);
				}
				break;
			}
		}
		break;
		case ball:
		{
		}
		break;
		case hinge:
		{
			dJointSetHingeParam(m_joint, dParamLoStop, axes[0].low);
			dJointSetHingeParam(m_joint, dParamHiStop, axes[0].high);
		}
		break;
		case full_control:
		{
			switch (axis_num)
			{
				case -1:
				{
					dJointSetAMotorParam(m_joint1, dParamLoStop, axes[0].low);
					dJointSetAMotorParam(m_joint1, dParamLoStop, axes[0].low);
					dJointSetAMotorParam(m_joint1, dParamLoStop2, axes[1].low);
					dJointSetAMotorParam(m_joint1, dParamHiStop2, axes[1].high);
					dJointSetAMotorParam(m_joint1, dParamHiStop3, axes[2].high);
					dJointSetAMotorParam(m_joint1, dParamHiStop3, axes[2].high);
				}
				case 0:
				{
					dJointSetAMotorParam(m_joint1, dParamLoStop, axes[0].low);
					dJointSetAMotorParam(m_joint1, dParamHiStop, axes[0].high);
				}
				break;
				case 1:
				{
					dJointSetAMotorParam(m_joint1, dParamLoStop2, axes[1].low);
					dJointSetAMotorParam(m_joint1, dParamHiStop2, axes[1].high);
				}
				break;
				case 2:
				{
					dJointSetAMotorParam(m_joint1, dParamLoStop3, axes[2].low);
					dJointSetAMotorParam(m_joint1, dParamHiStop3, axes[2].high);
				}
				break;
			}
		}
		break;
	}
}

f32 CPHJoint::GetAxisAngle(s32 axis_num)
{
	f32 ret = dInfinity;
	switch (eType)
	{
		case hinge2:
		{
			ret = dJointGetHinge2Angle1(m_joint);
		}
		break;
		case ball:
		{
			ret = dInfinity;
		}
		break;
		case hinge:
		{
			ret = dJointGetHingeAngle(m_joint);
		}
		break;
		case full_control:
		{
			ret = dJointGetAMotorAngle(m_joint1, axis_num);
		}
		break;
		case slider:
		{
			switch (axis_num)
			{
				case 0:
				{
					ret = dJointGetSliderPosition(m_joint);
				}
				break;
				case 1:
				{
					ret = dJointGetAMotorAngle(m_joint1, 0);
				}
				break;
			}
		}
		break;
	}

	return ret;// body_for_joint(pFirst_element) ? ret : -
}

void CPHJoint::LimitAxisNum(s32& axis_num)
{
	if (axis_num < -1)
	{
		axis_num = -1;
		return;
	}

	switch (eType)
	{
		case ball:
		{
			axis_num = -1;
		}
		break;
		case hinge:
		{
			axis_num = 0;
		}
		break;
		case slider:
		case hinge2:
		{
			axis_num = axis_num > 1 ? 1 : axis_num;
		}
		break;
		case full_control:
		{
			axis_num = axis_num > 2 ? 2 : axis_num;
		}
		break;
	}
}

void CPHJoint::SetAxis(const SPHAxis& axis, const s32 axis_num)
{
	s32 ax = axis_num;
	LimitAxisNum(ax);
	if (ax == -1)
	{
		switch (eType)
		{
			case ball:
			{
			}
			break;
			case hinge:
			{
				axes[0] = axis;
			}
			break;
			case hinge2:
			case slider:
			{
				axes[0] = axis;
				axes[1] = axis;
			}
			break;
			case full_control:
			{
				axes[0] = axis;
				axes[1] = axis;
				axes[2] = axis;
			}
			break;
		}
	}
	else
	{
		axes[ax] = axis;
	}
}

void CPHJoint::SetAxisSDfactors(f32 spring_factor, f32 damping_factor, s32 axis_num)
{
	s32 ax = axis_num;
	LimitAxisNum(ax);
	if (ax == -1)
	{
		switch (eType)
		{
			case ball:
			{
			}
			break;
			case hinge:
			{
				axes[0].set_sd_factors(spring_factor, damping_factor, eType);
			}
			break;
			case hinge2:
			case slider:
			{
				axes[0].set_sd_factors(spring_factor, damping_factor, eType);
				axes[1].set_sd_factors(spring_factor, damping_factor, eType);
			}
			break;
			case full_control:
			{
				axes[0].set_sd_factors(spring_factor, damping_factor, eType);
				axes[1].set_sd_factors(spring_factor, damping_factor, eType);
				axes[2].set_sd_factors(spring_factor, damping_factor, eType);
			}
			break;
		}

		if (bActive)
		{
			SetLimitsSDfactorsActive( );
		}
	}
	else
	{
		axes[ax].set_sd_factors(spring_factor, damping_factor, eType);
		if (bActive)
		{
			SetAxisSDfactorsActive(ax);
		}
	}
}

void CPHJoint::SetJointSDfactors(f32 spring_factor, f32 damping_factor)
{
	switch (eType)
	{
		case hinge2:
		{
			m_cfm = CFM(hinge2_spring * spring_factor, hinge2_damping * damping_factor);
			m_erp = ERP(hinge2_spring * spring_factor, hinge2_damping * damping_factor);
		}
		break;
		case ball:
		case hinge:
		case full_control:
		case slider:
		{
			m_erp = ERP(world_spring * spring_factor, world_damping * damping_factor);
			m_cfm = CFM(world_spring * spring_factor, world_damping * damping_factor);
		}
		break;
	}

	if (bActive)
	{
		SetJointSDfactorsActive( );
	}
}

void CPHJoint::SetJointSDfactorsActive( )
{
	switch (eType)
	{
		case hinge2:
		{
			dJointSetHinge2Param(m_joint, dParamSuspensionERP, m_erp);
			dJointSetHinge2Param(m_joint, dParamSuspensionCFM, m_cfm);
		}
		break;
		case ball:
		{
		}
		break;
		case hinge:
		{
			dJointSetHingeParam(m_joint, dParamCFM, m_cfm);
		}
		break;
		case full_control:
		{
			dJointSetAMotorParam(m_joint1, dParamCFM, m_cfm);
			dJointSetAMotorParam(m_joint1, dParamCFM2, m_cfm);
			dJointSetAMotorParam(m_joint1, dParamCFM3, m_cfm);
		}
		break;
		case slider:
		{
			dJointSetSliderParam(m_joint, dParamCFM, m_cfm);
		}
		break;
	}
}

void CPHJoint::SetLimitsSDfactorsActive( )
{
	switch (eType)
	{
		case hinge2:
		{
			dJointSetHinge2Param(m_joint, dParamStopERP, axes[0].erp);
			dJointSetHinge2Param(m_joint, dParamStopCFM, axes[0].cfm);
		}
		break;
		case ball:
		{
		}
		break;
		case hinge:
		{
			dJointSetHingeParam(m_joint, dParamStopERP, axes[0].erp);
			dJointSetHingeParam(m_joint, dParamStopCFM, axes[0].cfm);
		}
		break;
		case full_control:
		{
			dJointSetAMotorParam(m_joint1, dParamStopERP, axes[0].erp);
			dJointSetAMotorParam(m_joint1, dParamStopCFM, axes[0].cfm);
			dJointSetAMotorParam(m_joint1, dParamStopERP2, axes[1].erp);
			dJointSetAMotorParam(m_joint1, dParamStopCFM2, axes[1].cfm);
			dJointSetAMotorParam(m_joint1, dParamStopERP3, axes[2].erp);
			dJointSetAMotorParam(m_joint1, dParamStopCFM3, axes[2].cfm);
		}
		break;
		case slider:
		{
			dJointSetSliderParam(m_joint, dParamStopERP, axes[0].erp);
			dJointSetSliderParam(m_joint, dParamStopCFM, axes[0].cfm);
			dJointSetAMotorParam(m_joint1, dParamStopERP, axes[1].erp);
			dJointSetAMotorParam(m_joint1, dParamStopCFM, axes[1].cfm);
		}
		break;
	}
}

void CPHJoint::SetAxisSDfactorsActive(s32 axis_num)
{
	LimitAxisNum(axis_num);

	switch (eType)
	{
		case hinge2:
		{
			dJointSetHinge2Param(m_joint, dParamStopERP, axes[0].erp);
			dJointSetHinge2Param(m_joint, dParamStopCFM, axes[0].cfm);
		}
		break;
		case ball:
		{
		}
		break;
		case hinge:
		{
			dJointSetHingeParam(m_joint, dParamStopERP, axes[0].erp);
			dJointSetHingeParam(m_joint, dParamStopCFM, axes[0].cfm);
		}
		break;
		case full_control:
		{
			switch (axis_num)
			{
				case 0:
				{
					dJointSetAMotorParam(m_joint1, dParamStopERP, axes[axis_num].erp);
					dJointSetAMotorParam(m_joint1, dParamStopCFM, axes[0].cfm);
				}
				break;
				case 1:
				{
					dJointSetAMotorParam(m_joint1, dParamStopERP2, axes[1].erp);
					dJointSetAMotorParam(m_joint1, dParamStopCFM2, axes[1].cfm);
				}
				break;
				case 2:
				{
					dJointSetAMotorParam(m_joint1, dParamStopERP3, axes[2].erp);
					dJointSetAMotorParam(m_joint1, dParamStopCFM3, axes[2].cfm);
				}
				break;
			}
		}
		break;
		case slider:
		{
			switch (axis_num)
			{
				case 0:
				{
					dJointSetSliderParam(m_joint, dParamStopERP, axes[0].erp);
					dJointSetSliderParam(m_joint, dParamStopCFM, axes[0].cfm);
				}
				case 1:
				{
					dJointSetAMotorParam(m_joint1, dParamStopERP, axes[1].erp);
					dJointSetAMotorParam(m_joint1, dParamStopCFM, axes[1].cfm);
				}
			}
		}
		break;
	}
}

void CPHJoint::GetJointSDfactors(f32& spring_factor, f32& damping_factor)
{
	spring_factor = SPRING(m_cfm, m_erp);
	damping_factor = DAMPING(m_cfm, m_erp);
	if (eType == hinge2)
	{
		spring_factor /= hinge2_spring;
		damping_factor /= hinge2_damping;
	}
	else
	{
		spring_factor /= world_spring;
		damping_factor /= world_damping;
	}
}

void CPHJoint::GetAxisSDfactors(f32& spring_factor, f32& damping_factor, s32 axis_num)
{
	LimitAxisNum(axis_num);
	spring_factor = SPRING(axes[axis_num].cfm, axes[axis_num].erp) / world_spring;
	damping_factor = DAMPING(axes[axis_num].cfm, axes[axis_num].erp) / world_damping;
}

u16 CPHJoint::GetAxesNumber( )
{
	return u16(axes.size( ));
}

void CPHJoint::CalcAxis(int ax_num, fVector3& axis, f32& lo, f32& hi, const fMatrix4x4& first_matrix, const fMatrix4x4& second_matrix, const fMatrix4x4& rotate)
{
	switch (axes[ax_num].vs)
	{
		case vs_first:
		{
			first_matrix.transform_dir(axis, axes[ax_num].direction);
		}
		break;
		case vs_second:
		{
			second_matrix.transform_dir(axis, axes[ax_num].direction);
		}
		break;
		case vs_global:
		{
			pShell->mXFORM.transform_dir(axis, axes[ax_num].direction);
		}
		break;
		default:
		{
			NODEFAULT;
		}
	}

	lo = axes[ax_num].low;
	hi = axes[ax_num].high;
	if (lo < -M_PI)
	{
		hi -= (lo + M_PI);
		lo = -M_PI;
	}

	if (lo > 0.0f)
	{
		hi -= lo;
		lo = 0.0f;
	}

	if (hi > M_PI)
	{
		lo -= (hi - M_PI);
		hi = M_PI;
	}

	if (hi < 0.0f)
	{
		lo -= hi;
		hi = 0.0f;
	}
}

void CPHJoint::CalcAxis(s32 ax_num, fVector3& axis, f32& lo, f32& hi, const fMatrix4x4& first_matrix, const fMatrix4x4& second_matrix)
{
	switch (axes[ax_num].vs)
	{
		case vs_first:
		{
			first_matrix.transform_dir(axis, axes[ax_num].direction);
		}
		break;
		case vs_second:
		{
			second_matrix.transform_dir(axis, axes[ax_num].direction);
		}
		break;
		case vs_global:
		{
			pShell->mXFORM.transform_dir(axis, axes[ax_num].direction);
		}
		break;
		default:
		{
			NODEFAULT;
		}
	}

	fMatrix4x4 inv_first_matrix;
	inv_first_matrix.set(first_matrix);
	inv_first_matrix.invert( );

	fMatrix4x4 rotate;
	rotate.mul(inv_first_matrix, second_matrix);

	f32 shift_angle;
	axis_angleA(rotate, axes[ax_num].direction, shift_angle);

	shift_angle -= axes[ax_num].zero;

	if (shift_angle > M_PI)
	{
		shift_angle -= 2.0f * M_PI;
	}

	if (shift_angle < -M_PI)
	{
		shift_angle += 2.0f * M_PI;
	}

	lo = axes[ax_num].low;//+shift_angle;
	hi = axes[ax_num].high;//+shift_angle;
	if (lo < -M_PI)
	{
		hi -= (lo + M_PI);
		lo = -M_PI;
	}

	if (lo > 0.0f)
	{
		hi -= lo;
		lo = 0.0f;
	}

	if (hi > M_PI)
	{
		lo -= (hi - M_PI);
		hi = M_PI;
	}

	if (hi < 0.0f)
	{
		lo -= hi;
		hi = 0.0f;
	}
}

void CPHJoint::GetLimits(f32& lo_limit, f32& hi_limit, s32 axis_num)
{
	LimitAxisNum(axis_num);
	if (body_for_joint(pFirst_element))
	{
		lo_limit = axes[axis_num].low;
		hi_limit = axes[axis_num].high;
	}
	else
	{
		lo_limit = -axes[axis_num].high;
		hi_limit = -axes[axis_num].low;
	}
}

void CPHJoint::GetAxisDir(s32 num, fVector3& axis, eVs& vs)
{
	LimitAxisNum(num);
	vs = axes[num].vs;
	axis.set(axes[num].direction);
}

void CPHJoint::GetAxisDirDynamic(s32 num, fVector3& axis)
{
	LimitAxisNum(num);
	dVector3 result;
	switch (eType)
	{
		case ball:
		{
			return;
		}
		case hinge:
		{
			dJointGetHingeAxis(m_joint, result);
		}
		break;
		case hinge2:
		{
			if (num)
			{
				dJointGetHinge2Axis2(m_joint, result);
			}
			else
			{
				dJointGetHinge2Axis1(m_joint, result);
			}
		}
		break;
		case full_control:
		{
			dJointGetAMotorAxis(m_joint1, num, result);
		}
		break;
		case slider:
		{
			dJointGetSliderAxis(m_joint, result);
		}
	}

	axis.set(result[0], result[1], result[2]);
}

void CPHJoint::GetAnchorDynamic(fVector3& anchor)
{
	dVector3 result;
	switch (eType)
	{
		case hinge:
		{
			dJointGetHingeAnchor(m_joint, result);
		}
		break;
		case hinge2:
		{
			dJointGetHingeAnchor(m_joint, result);
		}
		break;
		case ball:
		case full_control:
		{
			dJointGetBallAnchor(m_joint, result);
		}
		break;
		case slider:
		{
			R_ASSERT2(false, "position of slider joint is undefinite");
		}
	}

	anchor.set(result[0], result[1], result[2]);
}

CPHJoint::SPHAxis::SPHAxis( )
{
	high = dInfinity;
	low = -dInfinity;
	zero = 0.0f;
	//erp=ERP(world_spring/5.f,world_damping*5.f);
	//cfm=CFM(world_spring/5.f,world_damping*5.f);

#ifndef ODE_SLOW_SOLVER
	erp = world_erp;
	cfm = world_cfm;
#else
	erp = 0.3f;
	cfm = 0.000001f;
#endif

	direction.set(0.0f, 0.0f, 1.0f);
	vs = vs_first;
	force = 0.0f;
	velocity = 0.0f;
}

void CPHJoint::SPHAxis::set_sd_factors(f32 sf, f32 df, enumType jt)
{
	switch (jt)
	{
		case hinge2:

#ifndef ODE_SLOW_SOLVER
		{
			cfm = 0.0f;
			erp = 1.0f;
		}
		break;
#endif

		case ball:
		case hinge:
		case full_control:
		case slider:
		{
			erp = ERP(world_spring * sf, world_damping * df);
			cfm = CFM(world_spring * sf, world_damping * df);
		}
		break;
	}
}

CPhysicsElement* CPHJoint::PFirst_element( )
{
	return cast_PhysicsElement(pFirst_element);
}

CPhysicsElement* CPHJoint::PSecond_element( )
{
	return cast_PhysicsElement(pSecond_element);
}

void CPHJoint::SetBreakable(f32 force, f32 torque)
{
	if (!m_destroy_info)
	{
		m_destroy_info = xr_new<CPHJointDestroyInfo>(force, torque);
	}
}

void CPHJoint::SetShell(CPHShell* p)
{
	if (!m_joint || !pShell)
	{
		pShell = p;
		return;
	}

	if (pShell != p)
	{
		pShell->Island( ).RemoveJoint(m_joint);
		p->Island( ).AddJoint(m_joint);
		if (m_joint1)
		{
			pShell->Island( ).RemoveJoint(m_joint1);
			p->Island( ).AddJoint(m_joint1);
		}

		pShell = p;
	}
}

void CPHJoint::ClearDestroyInfo( )
{
	xr_delete(m_destroy_info);
}
