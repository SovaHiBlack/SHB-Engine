#include "stdafx.h"
#include "xr_time.h"
#include "ui/UIInventoryUtilities.h"//
#include "Level.h"
#include "date_time.h"
#include "AISpace.h"
#include "alife_simulator.h"
#include "alife_time_manager.h"

#define sec2ms		1000
#define min2ms		60*sec2ms
#define hour2ms		60*min2ms
#define day2ms		24*hour2ms

ALife::_TIME_ID __game_time( )
{
	return (ai( ).get_alife( ) ? ai( ).alife( ).time( ).game_time( ) : Level( ).GetGameTime( ));
}

u32 get_time( )
{
	return u32(__game_time( ) & u32(-1));
}

ÑTime get_time_struct( )
{
	return ÑTime(__game_time( ));
}

pcstr ÑTime::dateToString(s32 mode)
{
	return *InventoryUtilities::GetDateAsString(m_time, (InventoryUtilities::EDatePrecision)mode);
}
pcstr ÑTime::timeToString(s32 mode)
{
	return *InventoryUtilities::GetTimeAsString(m_time, (InventoryUtilities::ETimePrecision)mode);
}

void ÑTime::add(const ÑTime& other)
{
	m_time += other.m_time;
}
void ÑTime::sub(const ÑTime& other)
{
	if (*this > other)
		m_time -= other.m_time;
	else
		m_time = 0;
}

void ÑTime::setHMS(s32 h, s32 m, s32 s)
{
	m_time = 0;
	m_time += generate_time(1, 1, 1, h, m, s);
}

void ÑTime::setHMSms(s32 h, s32 m, s32 s, s32 ms)
{
	m_time = 0;
	m_time += generate_time(1, 1, 1, h, m, s, ms);
}

void ÑTime::set(s32 y, s32 mo, s32 d, s32 h, s32 mi, s32 s, s32 ms)
{
	m_time = 0;
	m_time += generate_time(y, mo, d, h, mi, s, ms);
}

void ÑTime::get(u32& y, u32& mo, u32& d, u32& h, u32& mi, u32& s, u32& ms)
{
	split_time(m_time, y, mo, d, h, mi, s, ms);
}

f32 ÑTime::diffSec(const ÑTime& other)
{
	if (*this > other)
		return (m_time - other.m_time) / (f32)sec2ms;
	return ((other.m_time - m_time) / (f32)sec2ms) * (-1.0f);
}
