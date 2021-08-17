#pragma once

#include "ActorStatistic_defs.h"

class CActorStatisticsWrapper;

class CActorStatisticMgr
{
private:
	CActorStatisticsWrapper*							m_actor_stats_wrapper;
	vStatSectionData&			GetStorage				( );

public:
								CActorStatisticMgr		( );
								~CActorStatisticMgr		( );
	SStatSectionData&			GetSection				(const CSharedString& key);

	void						AddPoints				(const CSharedString& key, const CSharedString& detail_key, const CSharedString& str_value);
	void						AddPoints				(const CSharedString& key, const CSharedString& detail_key, int cnt, int pts);
	int							GetSectionPoints		(const CSharedString& key);
	const vStatSectionData&		GetCStorage				( );
};
