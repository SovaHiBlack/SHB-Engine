#pragma once

class CBaseMonster;

class CAnomalyDetector
{
	CBaseMonster*						m_object;

	float								m_radius;
	unsigned int						m_time_to_rememeber;

	bool								m_active;

	struct SAnomalyInfo
	{
		CObject*						object;
		unsigned int					time_registered;

		bool operator ==				(CObject* obj)
		{
			return (object == obj);
		}
	};

	struct remove_predicate
	{
		unsigned int					time_remember;
				remove_predicate		(unsigned int time) : time_remember(time)
		{ }

		inline bool operator ( )		(const SAnomalyInfo& info)
		{
			return (info.time_registered + time_remember < Device.dwTimeGlobal);
		}
	};

	using AnomalyInfoVec				= xr_vector<SAnomalyInfo>;
	using AnomalyInfoVec_it				= AnomalyInfoVec::iterator;
	AnomalyInfoVec						m_storage;

public:
				CAnomalyDetector		(CBaseMonster* monster);
	virtual		~CAnomalyDetector		( );

	void		load					(const char* section);
	void		reinit					( );

	void		update_schedule			( );
	void		on_contact				(CObject* obj);

	void		activate				( )
	{
		m_active = true;
	}
	void		deactivate				( )
	{
		m_active = false;
	}
};
