#include "stdafx.h"
#include "PHIsland.h"
#include "physics.h"

void CPHIsland::Step(dReal step)
{
	if (!m_flags.is_active( ))
	{
		return;
	}

	if (m_flags.is_exact_integration_prefeared( ) && nj < max_joint_allowed_for_exeact_integration)
	{
		dWorldStep(DWorld( ), fixed_step);
	}
	else
	{
		dWorldQuickStep(DWorld( ), fixed_step);
	}
}

void CPHIsland::Enable( )
{
	if (!m_flags.is_active( ))
	{
		return;
	}

	for (dxBody* body = DWorld( )->firstbody; body; body = (dxBody*)body->next)
	{
		body->flags &= ~dxBodyDisabled;
	}
}

void CPHIsland::Repair( )
{
	if (!m_flags.is_active( ))
	{
		return;
	}

	dBodyID body;
	for (body = firstbody; body; body = (dxBody*)body->next)
	{
		if (!dV_valid(dBodyGetAngularVel(body)))
		{
			dBodySetAngularVel(body, 0.0f, 0.0f, 0.0f);
		}

		if (!dV_valid(dBodyGetLinearVel(body)))
		{
			dBodySetLinearVel(body, 0.0f, 0.0f, 0.0f);
		}

		if (!dV_valid(dBodyGetPosition(body)))
		{
			dBodySetPosition(body, 0.0f, 0.0f, 0.0f);
		}

		if (!dQ_valid(dBodyGetQuaternion(body)))
		{
			dQuaternion q = {1.0f, 0.0f, 0.0f, 0.0f};
			dBodySetQuaternion(body, q);
		}
	}
}
