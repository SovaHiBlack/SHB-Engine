////////////////////////////////////////////////////////////////////////////
//	Module 		: Profiler_inline.h
//	Description : Profiler inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC SProfilePortion::SProfilePortion(pcstr timer_id)
{
	if (!psAI_Flags.test(aiStats))
	{
		return;
	}

	if (!psDeviceFlags.test(rsStatistic))
	{
		return;
	}

	m_timer_id = timer_id;
	m_time = CPU::QPC( );
}

IC SProfilePortion::~SProfilePortion( )
{
	if (!psAI_Flags.test(aiStats))
	{
		return;
	}

	if (!psDeviceFlags.test(rsStatistic))
	{
		return;
	}

	u64 temp = CPU::QPC( );
	m_time = temp - m_time;
	profiler( ).add_profile_portion(*this);
}

IC CProfiler& profiler( )
{
	return *g_profiler;
}

IC SProfileStats::SProfileStats( )
{
	m_update_time = 0;
	m_name = shared_str("");
	m_time = 0.0f;
	m_min_time = 0.0f;
	m_max_time = 0.0f;
	m_total_time = 0.0f;
	m_count = 0;
	m_call_count = 0;
}
