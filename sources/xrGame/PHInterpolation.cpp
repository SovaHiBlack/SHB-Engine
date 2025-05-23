#include "stdafx.h"
#include "PHInterpolation.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "MathUtils.h"

extern CPHWorld* ph_world;


CPHInterpolation::CPHInterpolation( )
{
	m_body = NULL;

	//	fTimeDelta=0.f;
}

void CPHInterpolation::SetBody(dBodyID body)
{
	if (!body) return;
	m_body = body;
	qPositions.fill_in(*((fVector3*)dBodyGetPosition(m_body)));
	const dReal* dQ = dBodyGetQuaternion(m_body);
	fQuaternion fQ;
	fQ.set(-dQ[0], dQ[1], dQ[2], dQ[3]);
	qRotations.fill_in(fQ);
}

void CPHInterpolation::UpdatePositions( )
{
	VERIFY(m_body);
	///VERIFY2(dV_valid(dBodyGetPosition(m_body)),"invalid body position in update interpolation");
	qPositions.push_back(*((fVector3*)dBodyGetPosition(m_body)));
}

void CPHInterpolation::UpdateRotations( )
{
	VERIFY(m_body);
	//VERIFY2(dM_valid(dBodyGetRotation(m_body)),"invalid body rotation in update interpolation");
	const dReal* dQ = dBodyGetQuaternion(m_body);
	fQuaternion fQ;
	fQ.set(-dQ[0], dQ[1], dQ[2], dQ[3]);
	qRotations.push_back(fQ);
}

void CPHInterpolation::InterpolatePosition(fVector3& pos)
{
	pos.lerp(qPositions[0], qPositions[1], ph_world->m_frame_time / fixed_step);
}

void CPHInterpolation::InterpolateRotation(fMatrix4x4& rot)
{
	f32	t = ph_world->m_frame_time / fixed_step;
	VERIFY(t >= 0.0f && t <= 1.0f);

	fQuaternion q;
	q.slerp(qRotations[0], qRotations[1], t);
	rot.rotation(q);
}

void CPHInterpolation::ResetPositions( )
{
	VERIFY2(dBodyStateValide(m_body), "Invalid body state");
	qPositions.fill_in(*((fVector3*)dBodyGetPosition(m_body)));
}

void CPHInterpolation::ResetRotations( )
{
	VERIFY2(dBodyStateValide(m_body), "Invalid body state");
	const dReal* dQ = dBodyGetQuaternion(m_body);
	fQuaternion fQ;
	fQ.set(-dQ[0], dQ[1], dQ[2], dQ[3]);
	qRotations.fill_in(fQ);
}

void CPHInterpolation::GetRotation(fQuaternion& q, u16 num)
{
	if (!m_body)	return;
	q.set(qRotations[num]);
}

void CPHInterpolation::GetPosition(fVector3& p, u16 num)
{
	if (!m_body)	return;
	p.set(qPositions[num]);
}
void CPHInterpolation::SetPosition(const fVector3& p, u16 num)
{
	if (!m_body)	return;
	qPositions[num].set(p);
}

void CPHInterpolation::SetRotation(const fQuaternion& q, u16 num)
{
	if (!m_body)	return;
	qRotations[num] = q;
}