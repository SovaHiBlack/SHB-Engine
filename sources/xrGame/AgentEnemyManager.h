//	Module 		: AgentEnemyManager.h
//	Description : Agent enemy manager

#pragma once

#include "MemberEnemy.h"

class CAgentManager;
class CMemberOrder;
class CEntityAlive;
class CStalker;

class CAgentEnemyManager
{
public:
	using ENEMIES = xr_vector<CMemberEnemy>;
	using squad_mask_type = MemorySpace::squad_mask_type;
	using WOUNDED = std::pair<ALife::_OBJECT_ID, bool>;
	using WOUNDED_ENEMY = std::pair<const CEntityAlive*, WOUNDED>;
	using WOUNDED_ENEMIES = xr_vector<WOUNDED_ENEMY>;

private:
	CAgentManager*									m_object;
	ENEMIES											m_enemies;
	WOUNDED_ENEMIES									m_wounded;
	bool											m_only_wounded_left;
	bool											m_is_any_wounded;

protected:
	template <typename T>
	inline void				setup_mask				(xr_vector<T>& objects, CMemberEnemy& enemy, const squad_mask_type& non_combat_members);

	inline void				setup_mask				(CMemberEnemy& enemy, const squad_mask_type& non_combat_members);
	void					fill_enemies			( );
	void					compute_enemy_danger	( );
	void					assign_enemies			( );
	void					permutate_enemies		( );
	void					assign_wounded			( );
	void					assign_enemy_masks		( );
	float					evaluate				(const CEntityAlive* object0, const CEntityAlive* object1) const;
	void					exchange_enemies		(CMemberOrder& member0, CMemberOrder& member1);
	inline CAgentManager&	object					( ) const;

public:
	inline					CAgentEnemyManager		(CAgentManager* object);
	void					update					( );
	void					distribute_enemies		( );
	inline ENEMIES&			enemies					( );
	void					remove_links			(CObject* object);

private:
	void					wounded_processor		(const CEntityAlive* object, const ALife::_OBJECT_ID& wounded_processor_id);

public:
	ALife::_OBJECT_ID		wounded_processor		(const CEntityAlive* object);
	void					wounded_processed		(const CEntityAlive* object, bool value);
	bool					wounded_processed		(const CEntityAlive* object) const;
	bool					assigned_wounded		(const CEntityAlive* wounded, const CStalker* member);
	bool					useful_enemy			(const CEntityAlive* enemy, const CStalker* member) const;
};

#include "AgentEnemyManager_inline.h"
