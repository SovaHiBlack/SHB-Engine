#pragma once

class CTimer_paused;

class CORE_API pauseMngr
{
	xr_vector<CTimer_paused*>	m_timers;
	BOOL						m_paused;

public:
	pauseMngr( );
	BOOL	Paused( )
	{
		return m_paused;
	}
	void	Pause(BOOL b);
	void	Register(CTimer_paused* t);
	void	UnRegister(CTimer_paused* t);
};

extern CORE_API pauseMngr		g_pauseMngr;

class CORE_API CTimerBase
{
protected:
	u64			qwStartTime;
	u64			qwPausedTime;
	u64			qwPauseAccum;
	BOOL		bPause;

public:
	CTimerBase( ) : qwStartTime(0), qwPausedTime(0), qwPauseAccum(0), bPause(FALSE)
	{ }
	ICF void	Start( )
	{
		if (bPause)
		{
			return;
		}

		qwStartTime = CPU::QPC( ) - qwPauseAccum;
	}
	ICF u64		GetElapsed_ticks( )const
	{
		if (bPause)
		{
			return qwPausedTime;
		}
		else
		{
			return (CPU::QPC( ) - qwStartTime - CPU::qpc_overhead - qwPauseAccum);
		}
	}
	IC u32		GetElapsed_ms( )const
	{
		return u32(GetElapsed_ticks( ) * u64(1000) / CPU::qpc_freq);
	}
	IC f32		GetElapsed_sec( )const
	{

		FPU::m64r( );

		f32		_result = f32(double(GetElapsed_ticks( )) / double(CPU::qpc_freq));

		FPU::m24r( );

		return _result;
	}
	IC void	Dump( ) const
	{
		Msg("* Elapsed time (sec): %f", GetElapsed_sec( ));
	}
};

class CORE_API CTimer : public CTimerBase
{
private:
	typedef CTimerBase					inherited;

private:
	f32					m_time_factor;
	u64					m_real_ticks;
	u64					m_ticks;

private:
	IC	u64				GetElapsed_ticks(const u64& current_ticks) const
	{
		u64				delta = current_ticks - m_real_ticks;
		f64			delta_d = (f64)delta;
		f64			time_factor_d = time_factor( );
		f64			time = delta_d * time_factor_d + 0.5;
		u64				result = (u64)time;
		return (m_ticks + result);
	}

public:
	IC					CTimer( ) : m_time_factor(1.0f), m_real_ticks(0), m_ticks(0)
	{ }

	ICF	void			Start( )
	{
		if (bPause)
		{
			return;
		}

		inherited::Start( );

		m_real_ticks = 0;
		m_ticks = 0;
	}

	IC	const f32& time_factor( ) const
	{
		return m_time_factor;
	}

	IC	void			time_factor(const f32& time_factor)
	{
		u64				current = inherited::GetElapsed_ticks( );
		m_ticks = GetElapsed_ticks(current);
		m_real_ticks = current;
		m_time_factor = time_factor;
	}

	IC	u64				GetElapsed_ticks( ) const
	{
		FPU::m64r( );

		u64				result = GetElapsed_ticks(inherited::GetElapsed_ticks( ));

		FPU::m24r( );

		return result;
	}

	u32				GetElapsed_ms( ) const
	{
		return			(u32(GetElapsed_ticks( ) * u64(1000) / CPU::qpc_freq));
	}

	f32			GetElapsed_sec( ) const
	{
		FPU::m64r( );

		f32			result = f32(double(GetElapsed_ticks( )) / double(CPU::qpc_freq));

		FPU::m24r( );

		return result;
	}

	void			Dump( ) const
	{
		Msg("* Elapsed time (sec): %f", GetElapsed_sec( ));
	}
};

class CORE_API CTimer_paused_ex : public CTimer
{
	u64							save_clock;
public:
	CTimer_paused_ex( )
	{ }
	virtual ~CTimer_paused_ex( )
	{ }
	IC BOOL		Paused( )const
	{
		return bPause;
	}
	IC void		Pause(BOOL b)
	{
		if (bPause == b)
		{
			return;
		}

		u64		_current = CPU::QPC( ) - CPU::qpc_overhead;
		if (b)
		{
			save_clock = _current;
			qwPausedTime = CTimerBase::GetElapsed_ticks( );
		}
		else
		{
			qwPauseAccum += _current - save_clock;
		}

		bPause = b;
	}
};

class CORE_API CTimer_paused : public CTimer_paused_ex
{
public:
	CTimer_paused( )
	{
		g_pauseMngr.Register(this);
	}
	virtual ~CTimer_paused( )
	{
		g_pauseMngr.UnRegister(this);
	}
};

extern CORE_API bool			g_bEnableStatGather;
class CORE_API CStatTimer
{
public:
	CTimer		T;
	u64			accum;
	f32			result;
	u32			count;

public:
	CStatTimer( );
	void		FrameStart( );
	void		FrameEnd( );

	ICF void	Begin( )
	{
		if (!g_bEnableStatGather)
		{
			return;
		}

		count++;
		T.Start( );
	}
	ICF void	End( )
	{
		if (!g_bEnableStatGather)
		{
			return;
		}

		accum += T.GetElapsed_ticks( );
	}

	ICF u64		GetElapsed_ticks( ) const
	{
		return accum;
	}

	IC u32		GetElapsed_ms( ) const
	{
		return u32(GetElapsed_ticks( ) * u64(1000) / CPU::qpc_freq);
	}
	IC f32	GetElapsed_sec( ) const
	{
		FPU::m64r( );

		f32 _result = f32(double(GetElapsed_ticks( )) / double(CPU::qpc_freq));

		FPU::m24r( );

		return _result;
	}
};
