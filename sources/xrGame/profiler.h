////////////////////////////////////////////////////////////////////////////
//	Module 		: Profiler.h
//	Created 	: 23.07.2004
//  Modified 	: 23.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Profiler
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef DEBUG
#	define	USE_PROFILER
#endif // DEBUG

#ifdef USE_PROFILER
#	include "ai_debug.h"

#pragma pack(push,4)
struct CProfileResultPortion
{
	U64				m_time;
	const char* m_timer_id;
};
#pragma pack(pop)

struct CProfilePortion : public CProfileResultPortion {
	inline				CProfilePortion		(const char* timer_id);
	inline				~CProfilePortion	();
};

struct CProfileStats {
	u32				m_update_time;
	shared_str		m_name;
	float			m_time;
	float			m_min_time;
	float			m_max_time;
	float			m_total_time;
	u32				m_count;
	u32				m_call_count;

	inline				CProfileStats		();
};

class CProfiler {
private:
	struct pred_rstr {
		inline	bool operator()	(const shared_str &_1, const shared_str &_2) const
		{
			return	(xr_strcmp(*_1,*_2) < 0);
		}
	};
protected:
	typedef xr_vector<CProfileResultPortion>		PORTIONS;
	typedef xr_map<shared_str,CProfileStats,pred_rstr>	TIMERS;

protected:
	PORTIONS			m_portions;
	TIMERS				m_timers;
	bool				m_actual;
	xrCriticalSection	m_section;
	u32					m_call_count;

protected:
			void		setup_timer			(const char* timer_id, const U64& timer_time, const u32 &call_count);
	inline		void		convert_string		(const char* str, shared_str &out, u32 max_string_size);

public:
						CProfiler			();
						~CProfiler			();
			void		show_stats			(CGameFont *game_font, bool show);
			void		clear				();
			void		add_profile_portion	(const CProfileResultPortion &profile_portion);
};

extern 	CProfiler *g_profiler;
extern Flags32 psAI_Flags;

inline	CProfiler&	profiler();
		
#	define START_PROFILE(a) { CProfilePortion	__profile_portion__(a);
#	define STOP_PROFILE     }

#	include "Profiler_inline.h"

#else // DEBUG
#	define START_PROFILE(a) {
#	define STOP_PROFILE		}
#endif // DEBUG