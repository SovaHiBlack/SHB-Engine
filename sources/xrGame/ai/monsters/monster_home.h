#pragma once

class CBaseMonster;
class CPatrolPath;

class CMonsterHome {
	CBaseMonster		*m_object;
	const CPatrolPath	*m_path;

	float				m_radius_min;
	float				m_radius_max;

	bool				m_aggressive;

public:
				CMonsterHome		(CBaseMonster *obj) {m_object = obj;}

		void	load				(const char* line);
		void	setup				(const char* path_name, float min_radius, float max_radius, bool aggressive = false);
		void	remove_home			();

		u32		get_place			();
		u32		get_place_in_cover	();
		bool	at_home				();
		bool	at_home				(const Fvector &pos);
		inline	bool	has_home			() {return (m_path != 0);}
		inline	bool	is_aggressive		() {return m_aggressive;}
};