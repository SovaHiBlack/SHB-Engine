#pragma once

#include "alife_space.h"

class �Time
{
	ALife::_TIME_ID		m_time;

public:
	�Time( ) : m_time(0)
	{ }
	�Time(const �Time& other) : m_time(other.m_time)
	{ }
	�Time(ALife::_TIME_ID t) : m_time(t)
	{ }

	bool	operator <		(const �Time& other) const
	{
		return m_time < other.m_time;
	}
	bool	operator >		(const �Time& other) const
	{
		return m_time > other.m_time;
	}
	bool	operator >=		(const �Time& other) const
	{
		return m_time >= other.m_time;
	}
	bool	operator <=		(const �Time& other) const
	{
		return m_time <= other.m_time;
	}
	bool	operator ==		(const �Time& other) const
	{
		return m_time == other.m_time;
	}
	�Time	operator +		(const �Time& other)
	{
		return �Time(m_time + other.m_time);
	}
	�Time	operator -		(const �Time& other)
	{
		return �Time(m_time - other.m_time);
	}

	f32	diffSec(const �Time& other);
	void	add(const �Time& other);
	void	sub(const �Time& other);

	void	add_script(�Time* other)
	{
		add(*other);
	}
	void	sub_script(�Time* other)
	{
		sub(*other);
	}
	f32	diffSec_script(�Time* other)
	{
		return diffSec(*other);
	}

	void	setHMS(s32 h, s32 m, s32 s);
	void	setHMSms(s32 h, s32 m, s32 s, s32 ms);
	void	set(s32 y, s32 mo, s32 d, s32 h, s32 mi, s32 s, s32 ms);
	void	get(u32& y, u32& mo, u32& d, u32& h, u32& mi, u32& s, u32& ms);

	pcstr	dateToString(s32 mode);
	pcstr	timeToString(s32 mode);
};

extern u32 get_time( );
extern �Time get_time_struct( );
