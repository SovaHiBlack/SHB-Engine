#pragma once

#include "PHCommander.h"
#include "PHReqComparer.h"
#include "alife_space.h"
#include "script_export_space.h"

class CPhysicsShell;

class CPHCallOnStepCondition : public CPHCondition
{
	u64					m_step;

public:
	CPHCallOnStepCondition( );
	virtual				bool				obsolete( ) const;
	virtual				bool				is_true( );
	IC						void				set_step(u64 step)
	{
		m_step = step;
	}
	void				set_steps_interval(u64 steps);
	void				set_time_interval(u32 time);
	void				set_time_interval(f32 time);
	void				set_global_time(f32 time);
	void				set_global_time(u32 time);

private:
	IC						bool				time_out( ) const;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPHCallOnStepCondition)
#undef script_type_list
#define script_type_list save_type_list(CPHCallOnStepCondition)

class CPHExpireOnStepCondition : public CPHCallOnStepCondition
{
public:
	virtual				bool				is_true( )
	{
		return true;
	}
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPHExpireOnStepCondition)
#undef script_type_list
#define script_type_list save_type_list( CPHExpireOnStepCondition)

class CPHShellBasedAction : public CPHAction
{
protected:
	CPhysicsShell* m_shell;

public:
	CPHShellBasedAction(CPhysicsShell* shell);

	virtual				bool				compare(const CPhysicsShell* shl) const
	{
		return shl == m_shell;
	}
	virtual				bool				obsolete( ) const;
};

class CPHConstForceAction : public CPHShellBasedAction
{
	fVector3				m_force;

public:
	CPHConstForceAction(CPhysicsShell* shell, const fVector3& force);
	virtual				void				run( );

	virtual				bool				compare(const CPHReqComparerV* v) const
	{
		return v->compare(this);
	}
	virtual				bool				compare(const CPhysicsShell* shl) const
	{
		return CPHShellBasedAction::compare(shl);
	}
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPHConstForceAction)
#undef script_type_list
#define script_type_list save_type_list( CPHConstForceAction)

class CPHReqComparerHasShell : public CPHReqComparerV
{
	CPhysicsShell* m_shell;

public:
	CPHReqComparerHasShell(CPhysicsShell* shl);
	virtual				bool				compare(const CPHConstForceAction* v) const
	{
		return v->compare(m_shell);
	}
};
