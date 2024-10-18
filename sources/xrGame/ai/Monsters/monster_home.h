#pragma once

class CBaseMonster;
class CPatrolPath;

class CMonsterHome {
	CBaseMonster		*m_object;
	const CPatrolPath	*m_path;

	f32				m_radius_min;
	f32				m_radius_max;

	bool				m_aggressive;

public:
				CMonsterHome		(CBaseMonster *obj) {m_object = obj;}

		void	load				(pcstr line);
		void	setup				(pcstr path_name, f32 min_radius, f32 max_radius, bool aggressive = false);
		void	remove_home			();

		u32		get_place			();
		u32		get_place_in_cover	();
		bool	at_home				();
		bool	at_home				(const Fvector &pos);
	IC	bool	has_home			() {return (m_path != 0);}
	IC	bool	is_aggressive		() {return m_aggressive;}
};
