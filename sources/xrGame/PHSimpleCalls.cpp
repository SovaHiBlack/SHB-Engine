#include	"stdafx.h"
#include	"physicsshell.h"
#include	"phsimplecalls.h"
#include	"phobject.h"
#include	"PHWorld.h"

extern		CPHWorld* ph_world;

CPHCallOnStepCondition::CPHCallOnStepCondition( )
{
	if (ph_world)
	{
		set_step(ph_world->m_steps_num);
	}
	else
	{
		set_step(0);
	}
}

IC bool CPHCallOnStepCondition::time_out( ) const
{
	return ph_world->m_steps_num > m_step;
}

bool CPHCallOnStepCondition::is_true( )
{
	return time_out( );
}

bool CPHCallOnStepCondition::obsolete( ) const
{
	return time_out( );
}

void CPHCallOnStepCondition::set_steps_interval(u64 steps)
{
	set_step(ph_world->m_steps_num + steps);
}

void CPHCallOnStepCondition::set_time_interval(f32 time)
{
	set_steps_interval(iCeil(time / fixed_step));
}

void CPHCallOnStepCondition::set_time_interval(u32 time)
{
	set_time_interval(f32(time) / 1000.0f);
}

void CPHCallOnStepCondition::set_global_time(f32 time)
{
	f32 time_interval = Device.fTimeGlobal - time;
	if (time_interval < 0.0f)
	{
		set_step(ph_world->m_steps_num);
	}

	set_time_interval(time_interval);
}

void CPHCallOnStepCondition::set_global_time(u32 time)
{
	set_global_time(f32(time) / 1000.0f);
}

CPHShellBasedAction::CPHShellBasedAction(CPhysicsShell* shell)
{
	VERIFY(shell && shell->isActive( ));
	m_shell = shell;
}

bool CPHShellBasedAction::obsolete( ) const
{
	return {!m_shell || !m_shell->isActive( )};
}

CPHConstForceAction::CPHConstForceAction(CPhysicsShell* shell, const fVector3& force) : CPHShellBasedAction(shell)
{
	m_force.set(force);
}

void CPHConstForceAction::run( )
{
	m_shell->applyForce(m_force.x, m_force.y, m_force.z);
}

CPHReqComparerHasShell::CPHReqComparerHasShell(CPhysicsShell* shell)
{
	VERIFY(shell);
	m_shell = shell;
}
