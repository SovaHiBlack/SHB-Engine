////////////////////////////////////////////////////////////////////////////
//	Module 		: Profiler.h
//	Description : Profiler
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef XRGAME_EXPORTS
#	ifdef DEBUG
#		define	USE_PROFILER
#	endif // DEBUG
#endif // XRGAME_EXPORTS

#ifdef USE_PROFILER
#	include "ai_debug.h"

#pragma pack(push,4)
struct SProfileResultPortion
{
	u64				m_time;
	pcstr			m_timer_id;
};
#pragma pack(pop)

struct SProfilePortion : public SProfileResultPortion
{
	IC				SProfilePortion(pcstr timer_id);
	IC				~SProfilePortion( );
};

struct SProfileStats
{
	u32				m_update_time;
	shared_str		m_name;
	f32			m_time;
	f32			m_min_time;
	f32			m_max_time;
	f32			m_total_time;
	u32				m_count;
	u32				m_call_count;

	IC				SProfileStats( );
};

class CProfiler
{
private:
	struct pred_rstr
	{
		IC	bool operator()	(const shared_str& _1, const shared_str& _2) const
		{
			return	(xr_strcmp(*_1, *_2) < 0);
		}
	};

protected:
	typedef xr_vector<SProfileResultPortion>		PORTIONS;
	typedef xr_map<shared_str, SProfileStats, pred_rstr>	TIMERS;

protected:
	PORTIONS			m_portions;
	TIMERS				m_timers;
	bool				m_actual;
	xrCriticalSection	m_section;
	u32					m_call_count;

protected:
	void		setup_timer(pcstr timer_id, const u64& timer_time, const u32& call_count);
	IC		void		convert_string(pcstr str, shared_str& out, u32 max_string_size);

public:
	CProfiler( );
	~CProfiler( );
	void		show_stats(CGameFont* game_font, bool show);
	void		clear( );
	void		add_profile_portion(const SProfileResultPortion& profile_portion);
};

extern 	CProfiler* g_profiler;
extern flags32 psAI_Flags;

IC	CProfiler& profiler( );

#	define START_PROFILE(a) { SProfilePortion	__profile_portion__(a);
#	define STOP_PROFILE     }

#	include "Profiler_inline.h"

#else // USE_PROFILER
#	define START_PROFILE(a) {
#	define STOP_PROFILE		}
#endif // USE_PROFILER