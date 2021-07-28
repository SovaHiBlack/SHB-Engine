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
	U64			qwStartTime;
	U64			qwPausedTime;
	U64			qwPauseAccum;
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
	__forceinline U64		GetElapsed_ticks( )const
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
	inline U32		GetElapsed_ms( ) const
	{
		return U32(GetElapsed_ticks( ) * U64(1000) / CPU::qpc_freq);
	}
	inline F32	GetElapsed_sec( ) const
	{
		FPU::m64r( );
		F32 _result = F32(F64(GetElapsed_ticks( )) / F64(CPU::qpc_freq));
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

	F32				m_time_factor;
	U64					m_real_ticks;
	U64					m_ticks;

	inline U64				GetElapsed_ticks(const U64& current_ticks) const
	{
		U64				delta = current_ticks - m_real_ticks;
		F64			delta_d = (F64) delta;
		F64			time_factor_d = time_factor( );
		F64			time = delta_d * time_factor_d + 0.5;
		U64				result = (U64) time;
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

	inline const F32& time_factor( ) const
	{
		return m_time_factor;
	}

	inline void			time_factor(const F32& time_factor)
	{
		U64 current = inherited::GetElapsed_ticks( );
		m_ticks = GetElapsed_ticks(current);
		m_real_ticks = current;
		m_time_factor = time_factor;
	}

	inline U64				GetElapsed_ticks( ) const
	{
		FPU::m64r( );
		U64 result = GetElapsed_ticks(inherited::GetElapsed_ticks( ));
		FPU::m24r( );

		return result;
	}

	U32				GetElapsed_ms( ) const
	{
		return			(U32(GetElapsed_ticks( ) * U64(1000) / CPU::qpc_freq));
	}

	F32			GetElapsed_sec( ) const
	{
		FPU::m64r( );
		F32 result = F32(F64(GetElapsed_ticks( )) / F64(CPU::qpc_freq));
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
	U64											save_clock;

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

		U64 _current = CPU::QPC( ) - CPU::qpc_overhead;
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
	U64											accum;
	F32										result;
	U32											count;

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

	__forceinline U64		GetElapsed_ticks( ) const
	{
		return accum;
	}

	inline U32				GetElapsed_ms( ) const
	{
		return U32(GetElapsed_ticks( ) * U64(1000) / CPU::qpc_freq);
	}
	inline F32			GetElapsed_sec( ) const
	{
		FPU::m64r( );
		F32 _result = F32(F64(GetElapsed_ticks( )) / F64(CPU::qpc_freq));
		FPU::m24r( );

		return _result;
	}
};
