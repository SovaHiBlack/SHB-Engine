#pragma once

#include "alife_space.h"

class xrTime
{
	ALife::_TIME_ID		m_time;

public:
	xrTime( ) : m_time(0)
	{ }
	xrTime(const xrTime& other) : m_time(other.m_time)
	{ }
	xrTime(ALife::_TIME_ID t) : m_time(t)
	{ }

	bool	operator <		(const xrTime& other) const
	{
		return m_time < other.m_time;
	}
	bool	operator >		(const xrTime& other) const
	{
		return m_time > other.m_time;
	}
	bool	operator >=		(const xrTime& other) const
	{
		return m_time >= other.m_time;
	}
	bool	operator <=		(const xrTime& other) const
	{
		return m_time <= other.m_time;
	}
	bool	operator ==		(const xrTime& other) const
	{
		return m_time == other.m_time;
	}
	xrTime	operator +		(const xrTime& other)
	{
		return xrTime(m_time + other.m_time);
	}
	xrTime	operator -		(const xrTime& other)
	{
		return xrTime(m_time - other.m_time);
	}

	f32	diffSec(const xrTime& other);
	void	add(const xrTime& other);
	void	sub(const xrTime& other);

	void	add_script(xrTime* other)
	{
		add(*other);
	}
	void	sub_script(xrTime* other)
	{
		sub(*other);
	}
	f32	diffSec_script(xrTime* other)
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
extern xrTime get_time_struct( );
