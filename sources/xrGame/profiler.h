//	Module 		: Profiler.h
//	Description : Profiler

#pragma once

#ifdef DEBUG
#	define USE_PROFILER
#endif // DEBUG

#ifdef USE_PROFILER
#	include "ai_debug.h"

#pragma pack(push,4)
struct SProfileResultPortion
{
	unsigned __int64				m_time;
	const char*						m_timer_id;
};
#pragma pack(pop)

struct SProfilePortion : public SProfileResultPortion
{
	inline				SProfilePortion(const char* timer_id);
	inline				~SProfilePortion();
};

struct SProfileStats
{
	u32				m_update_time;
	CSharedString		m_name;
	float			m_time;
	float			m_min_time;
	float			m_max_time;
	float			m_total_time;
	u32				m_count;
	u32				m_call_count;

	inline				SProfileStats();
};

class CProfiler {
private:
	struct pred_rstr {
		inline	bool operator()	(const CSharedString& _1, const CSharedString& _2) const
		{
			return	(xr_strcmp(*_1,*_2) < 0);
		}
	};

protected:
	using ProfileResultPortionsVec = xr_vector<SProfileResultPortion>;
	typedef xr_map<CSharedString, SProfileStats,pred_rstr>	TIMERS;

	ProfileResultPortionsVec			m_portions;
	TIMERS				m_timers;
	bool				m_actual;
	xrCriticalSection	m_section;
	u32					m_call_count;

			void		setup_timer			(const char* timer_id, const U64& timer_time, const u32 &call_count);
	inline void		convert_string		(const char* str, CSharedString& out, u32 max_string_size);

public:
						CProfiler			();
						~CProfiler			();
			void		show_stats			(CGameFont *game_font, bool show);
			void		clear				();
			void		add_profile_portion	(const SProfileResultPortion& profile_portion);
};

extern 	CProfiler *g_profiler;
extern Flags32 psAI_Flags;

inline	CProfiler&	profiler();
		
#	define START_PROFILE(a) { SProfilePortion	__profile_portion__(a);
#	define STOP_PROFILE     }

#	include "Profiler_inline.h"

#else // DEBUG
#	define START_PROFILE(a) {
#	define STOP_PROFILE		}
#endif // DEBUG