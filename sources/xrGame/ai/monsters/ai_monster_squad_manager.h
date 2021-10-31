#pragma once

class CMonsterSquad;
class CEntity;

class CMonsterSquadManager {
	
	//------------------------------------------------------------------------
	// Monster classification: Team -> Level -> Squad
	// Note: Its names differ from global ones, which are: Team -> Squad -> Group
	//		 but nesting hierarchy logically means the same
	//		 Team->Level->Squad used only for private members and functions
	//------------------------------------------------------------------------

//	DEFINE_VECTOR(CMonsterSquad*, MONSTER_SQUAD_VEC, MONSTER_SQUAD_VEC_IT);
	using MONSTER_SQUAD_VEC = xr_vector<CMonsterSquad*>;
	using MONSTER_SQUAD_VEC_IT = MONSTER_SQUAD_VEC::iterator;
//	DEFINE_VECTOR(MONSTER_SQUAD_VEC, MONSTER_LEVEL_VEC,MONSTER_LEVEL_VEC_IT);
	using MONSTER_LEVEL_VEC = xr_vector<MONSTER_SQUAD_VEC>;
	using MONSTER_LEVEL_VEC_IT = MONSTER_LEVEL_VEC::iterator;
//	DEFINE_VECTOR(MONSTER_LEVEL_VEC, MONSTER_TEAM_VEC,MONSTER_TEAM_VEC_IT);
	using MONSTER_TEAM_VEC = xr_vector<MONSTER_LEVEL_VEC>;
	using MONSTER_TEAM_VEC_IT = MONSTER_TEAM_VEC::iterator;
	MONSTER_TEAM_VEC team;

public:
	CMonsterSquadManager	();
	~CMonsterSquadManager	();

	void			register_member			(unsigned char team_id, unsigned char squad_id, unsigned char group_id, CEntity *e);
	void			remove_member			(unsigned char team_id, unsigned char squad_id, unsigned char group_id, CEntity *e);

	CMonsterSquad	*get_squad				(unsigned char team_id, unsigned char squad_id, unsigned char group_id);
	CMonsterSquad	*get_squad				(const CEntity *entity);

	void			update					(CEntity *entity);

	void			remove_links			(CObject *O);
};

inline CMonsterSquadManager &monster_squad();
extern CMonsterSquadManager *g_monster_squad;

#include "ai_monster_squad_manager_inline.h"
