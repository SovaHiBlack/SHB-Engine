#pragma once

class	CTimer_paused;

class CORE_API				pauseMngr
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
	unsigned __int64			qwStartTime;
	unsigned __int64			qwPausedTime;
	unsigned __int64			qwPauseAccum;
	BOOL		bPause;

public:
	CTimerBase( ) : qwStartTime(0), qwPausedTime(0), qwPauseAccum(0), bPause(FALSE)
	{ }
	__forceinline void	Start( )
	{
		if (bPause)
		{
			return;
		}

		qwStartTime = CPU::QPC( ) - qwPauseAccum;
	}
	__forceinline unsigned __int64		GetElapsed_ticks( )const
	{
		if (bPause)
		{
			return qwPausedTime;
		}
		else
		{
			return CPU::QPC( ) - qwStartTime - CPU::qpc_overhead - qwPauseAccum;
		}
	}
	inline unsigned int		GetElapsed_ms( ) const
	{
		return unsigned int(GetElapsed_ticks( ) * unsigned __int64(1000) / CPU::qpc_freq);
	}
	inline float	GetElapsed_sec( ) const
	{
		FPU::m64r( );
		float _result = float(double(GetElapsed_ticks( )) / double(CPU::qpc_freq));
		FPU::m24r( );

		return _result;
	}
	inline void	Dump( ) const
	{
		Msg("* Elapsed time (sec): %f", GetElapsed_sec( ));
	}
};

class CORE_API CTimer : public CTimerBase
{
private:
	using inherited = CTimerBase;

	float				m_time_factor;
	unsigned __int64					m_real_ticks;
	unsigned __int64					m_ticks;

	inline unsigned __int64				GetElapsed_ticks(const unsigned __int64& current_ticks) const
	{
		unsigned __int64				delta = current_ticks - m_real_ticks;
		double			delta_d = (double) delta;
		double			time_factor_d = time_factor( );
		double			time = delta_d * time_factor_d + 0.5;
		unsigned __int64				result = (unsigned __int64) time;
		return			(m_ticks + result);
	}

public:
	inline					CTimer( ) : m_time_factor(1.0f), m_real_ticks(0), m_ticks(0)
	{ }

	__forceinline void			Start( )
	{
		if (bPause)
		{
			return;
		}

		inherited::Start( );

		m_real_ticks = 0;
		m_ticks = 0;
	}

	inline const float& time_factor( ) const
	{
		return m_time_factor;
	}

	inline void			time_factor(const float& time_factor)
	{
		unsigned __int64 current = inherited::GetElapsed_ticks( );
		m_ticks = GetElapsed_ticks(current);
		m_real_ticks = current;
		m_time_factor = time_factor;
	}

	inline unsigned __int64				GetElapsed_ticks( ) const
	{
		FPU::m64r( );
		unsigned __int64 result = GetElapsed_ticks(inherited::GetElapsed_ticks( ));
		FPU::m24r( );

		return result;
	}

	unsigned int				GetElapsed_ms( ) const
	{
		return			(unsigned int(GetElapsed_ticks( ) * unsigned __int64(1000) / CPU::qpc_freq));
	}

	float			GetElapsed_sec( ) const
	{
		FPU::m64r( );
		float result = float(double(GetElapsed_ticks( )) / double(CPU::qpc_freq));
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
	unsigned __int64											save_clock;

public:
	CTimer_paused_ex( )
	{ }
	virtual					~CTimer_paused_ex( )
	{ }
	inline BOOL				Paused( ) const
	{
		return bPause;
	}
	inline void				Pause(BOOL b)
	{
		if (bPause == b)
		{
			return;
		}

		unsigned __int64 _current = CPU::QPC( ) - CPU::qpc_overhead;
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
	virtual					~CTimer_paused( )
	{
		g_pauseMngr.UnRegister(this);
	}
};

extern CORE_API BOOL							g_bEnableStatGather;
class CORE_API CStatTimer
{
public:
	CTimer										T;
	unsigned __int64											accum;
	float										result;
	unsigned int											count;

	CStatTimer( );
	void					FrameStart( );
	void					FrameEnd( );

	__forceinline void		Begin( )
	{
		if (!g_bEnableStatGather)
		{
			return;
		}

		count++;
		T.Start( );
	}
	__forceinline void		End( )
	{
		if (!g_bEnableStatGather)
		{
			return;
		}

		accum += T.GetElapsed_ticks( );
	}

	__forceinline unsigned __int64		GetElapsed_ticks( ) const
	{
		return accum;
	}

	inline unsigned int				GetElapsed_ms( ) const
	{
		return unsigned int(GetElapsed_ticks( ) * unsigned __int64(1000) / CPU::qpc_freq);
	}
	inline float			GetElapsed_sec( ) const
	{
		FPU::m64r( );
		float _result = float(double(GetElapsed_ticks( )) / double(CPU::qpc_freq));
		FPU::m24r( );

		return _result;
	}
};
