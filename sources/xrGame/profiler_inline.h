//	Module 		: Profiler_inline.h
//	Description : Profiler inline functions

#pragma once

inline SProfilePortion::SProfilePortion(const char* timer_id)
{
	if (!psAI_Flags.test(aiStats))
	{
		return;
	}

	if (!psDeviceFlags.test(rsStatistic))
	{
		return;
	}

	m_timer_id							= timer_id;
	m_time								= CPU::QPC();
}

inline	SProfilePortion::~SProfilePortion()
{
	if (!psAI_Flags.test(aiStats))
		return;

	if (!psDeviceFlags.test(rsStatistic))
		return;

	unsigned __int64									temp = CPU::QPC();
	m_time								= temp - m_time;
	profiler().add_profile_portion		(*this);
}

inline	CProfiler&	profiler				()
{
	return			(*g_profiler);
}

inline	SProfileStats::SProfileStats()
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
