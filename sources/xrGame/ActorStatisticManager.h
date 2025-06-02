#pragma once

#include "ActorStatistic_defs.h"

class CActorStatisticsWrapper;

class CActorStatisticManager
{
private:
	CActorStatisticsWrapper* m_actor_stats_wrapper;
	vStatSectionData& GetStorage( );

public:
	CActorStatisticManager( );
	~CActorStatisticManager( );
	SStatSectionData& GetSection(const shared_str& key);

	void					AddPoints(const shared_str& key, const shared_str& detail_key, const shared_str& str_value);
	void					AddPoints(const shared_str& key, const shared_str& detail_key, s32 cnt, s32 pts);
	s32						GetSectionPoints(const shared_str& key);
	const vStatSectionData& GetCStorage( );
};
