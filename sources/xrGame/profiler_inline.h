////////////////////////////////////////////////////////////////////////////
//	Module 		: Profiler_inline.h
//	Created 	: 23.07.2004
//  Modified 	: 23.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Profiler inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

inline	CProfilePortion::CProfilePortion	(const char* timer_id)
{
	if (!psAI_Flags.test(aiStats))
		return;

	if (!psDeviceFlags.test(rsStatistic))
		return;

	m_timer_id							= timer_id;
	m_time								= CPU::QPC();
}

inline	CProfilePortion::~CProfilePortion	()
{
	if (!psAI_Flags.test(aiStats))
		return;

	if (!psDeviceFlags.test(rsStatistic))
		return;

	U64									temp = CPU::QPC();
	m_time								= temp - m_time;
	profiler().add_profile_portion		(*this);
}

inline	CProfiler&	profiler				()
{
	return			(*g_profiler);
}

inline	CProfileStats::CProfileStats		()
{
	m_update_time	= 0;
	m_name			= CSharedString("");
	m_time			= 0.0f;
	m_min_time		= 0.0f;
	m_max_time		= 0.0f;
	m_total_time	= 0.0f;
	m_count			= 0;
	m_call_count	= 0;
}
