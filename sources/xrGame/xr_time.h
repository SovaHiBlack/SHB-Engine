#pragma once

#include "alife_space.h"

class ÑTime
{
	ALife::_TIME_ID		m_time;

public:
	ÑTime( ) : m_time(0)
	{ }
	ÑTime(const ÑTime& other) : m_time(other.m_time)
	{ }
	ÑTime(ALife::_TIME_ID t) : m_time(t)
	{ }

	bool	operator <		(const ÑTime& other) const
	{
		return m_time < other.m_time;
	}
	bool	operator >		(const ÑTime& other) const
	{
		return m_time > other.m_time;
	}
	bool	operator >=		(const ÑTime& other) const
	{
		return m_time >= other.m_time;
	}
	bool	operator <=		(const ÑTime& other) const
	{
		return m_time <= other.m_time;
	}
	bool	operator ==		(const ÑTime& other) const
	{
		return m_time == other.m_time;
	}
	ÑTime	operator +		(const ÑTime& other)
	{
		return ÑTime(m_time + other.m_time);
	}
	ÑTime	operator -		(const ÑTime& other)
	{
		return ÑTime(m_time - other.m_time);
	}

	f32	diffSec(const ÑTime& other);
	void	add(const ÑTime& other);
	void	sub(const ÑTime& other);

	void	add_script(ÑTime* other)
	{
		add(*other);
	}
	void	sub_script(ÑTime* other)
	{
		sub(*other);
	}
	f32	diffSec_script(ÑTime* other)
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
extern ÑTime get_time_struct( );
