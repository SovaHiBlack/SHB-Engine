#include "stdafx.h"

CORE_API bool			g_bEnableStatGather = false;

CStatTimer::CStatTimer( )
{
	accum = 0;
	result = 0.0f;
	count = 0;
}

void CStatTimer::FrameStart( )
{
	accum = 0;
	count = 0;
}
void CStatTimer::FrameEnd( )
{
	f32 _time = 1000.0f * f32(f64(accum) / f64(CPU::qpc_freq));
	if (_time > result)
	{
		result = _time;
	}
	else
	{
		result = 0.99f * result + 0.01f * _time;
	}
}

CORE_API pauseMngr	g_pauseMngr;

pauseMngr::pauseMngr( ) : m_paused(FALSE)
{
	m_timers.reserve(3);
}

void pauseMngr::Pause(BOOL b)
{
	if (m_paused == b)
	{
		return;
	}

	xr_vector<CTimer_paused*>::iterator it = m_timers.begin( );
	for (; it != m_timers.end( ); ++it)
	{
		(*it)->Pause(b);
	}

	m_paused = b;
}

void pauseMngr::Register(CTimer_paused* t)
{
	m_timers.push_back(t);
}

void pauseMngr::UnRegister(CTimer_paused* t)
{
	xr_vector<CTimer_paused*>::iterator it = std::find(m_timers.begin( ), m_timers.end( ), t);
	if (it != m_timers.end( ))
	{
		m_timers.erase(it);
	}
}
