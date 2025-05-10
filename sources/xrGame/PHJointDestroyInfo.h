#pragma once

#include "ode_include.h"

class CPHJointDestroyInfo
{
	friend class CPHShellSplitterHolder;
	friend class CPHShell;
	dJointFeedback			m_joint_feedback;
	f32						m_sq_break_force;
	f32						m_sq_break_torque;
	u16						m_end_element;
	u16						m_end_joint;
	bool					m_breaked;

public:
	CPHJointDestroyInfo(f32 break_force, f32 break_torque);
	IC dJointFeedback* JointFeedback( )
	{
		return &m_joint_feedback;
	}

	IC bool				Breaked( )
	{
		return m_breaked;
	}
	bool				Update( );
};
