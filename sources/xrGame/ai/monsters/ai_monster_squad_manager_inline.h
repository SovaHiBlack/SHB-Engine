#pragma once

inline CMonsterSquadManager &monster_squad()
{
	if (!g_monster_squad)
	{
		g_monster_squad = xr_new<CMonsterSquadManager>( );
	}

	return *g_monster_squad;
}
