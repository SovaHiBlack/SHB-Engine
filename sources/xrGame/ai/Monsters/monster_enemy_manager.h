#pragma once
#include "ai_monster_defs.h"

class CBaseMonster;

class CMonsterEnemyManager {
	CBaseMonster		*monster;

	const CEntityAlive	*enemy;
	fVector3				position;
	u32					vertex;
	u32					time_last_seen;

	flags32				flags;
	bool				forced;

	bool				expediency;

	const CEntityAlive *prev_enemy;
	fVector3				prev_enemy_position;

	bool				enemy_see_me;

	EDangerType			danger_type;

	// node, where monster saw enemy last time
	u32					my_vertex_enemy_last_seen;
	// node, of enemy (its always valid unlike vertex)
	u32					enemy_vertex_enemy_last_seen;

	u32					m_time_updated;
	u32					m_time_start_see_enemy;
	
public:
						CMonsterEnemyManager		(); 
						~CMonsterEnemyManager		();
	void				init_external				(CBaseMonster *M);
	void				reinit						();

	void				update						();

	void				force_enemy					(const CEntityAlive *enemy);
	void				unforce_enemy				();

	const CEntityAlive *get_enemy					() {return enemy;}
	EDangerType			get_danger_type				() {return danger_type;}
	const fVector3&		get_enemy_position			() {return position;}
	u32					get_enemy_vertex			() {return vertex;}
	TTime				get_enemy_time_last_seen	() {return time_last_seen;}

	flags32&			get_flags					() {return flags;}
	
	bool				see_enemy_now				();
	bool				enemy_see_me_now			();

	// ������� ���������� ������
	u32					get_enemies_count			();

	void				add_enemy					(const CEntityAlive *);
	bool				is_faced					(const CEntityAlive *object0, const CEntityAlive *object1);
	
	bool				is_enemy					(const CEntityAlive *obj);

	// �������� ����� � ������������ � ������ � monster
	void				transfer_enemy				(CBaseMonster *friend_monster);

	u32					get_my_vertex_enemy_last_seen		() {return my_vertex_enemy_last_seen;}
	u32					get_enemy_vertex_enemy_last_seen	() {return enemy_vertex_enemy_last_seen;}

	u32					see_enemy_duration			();
};
