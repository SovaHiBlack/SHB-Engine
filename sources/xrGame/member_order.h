////////////////////////////////////////////////////////////////////////////
//	Module 		: member_order.h
//	Created 	: 26.05.2004
//  Modified 	: 26.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Member order
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AgentManager.h"
#include "stalker_decision_space.h"
#include "GraphEngine_space.h"
#include "condition_state.h"

class CStalker;
class CCoverPoint;
class CExplosive;
class CGameObject;

class CMemberOrder {
public:
	struct CMemberDeathReaction {
		CStalker				*m_member;
		u32						m_time;
		bool					m_processing;

		inline			CMemberDeathReaction()
		{
			clear				();
		}

		inline	void	clear				()
		{
			m_member			= 0;
			m_time				= 0;
			m_processing		= false;
		}
	};

	struct CGrenadeReaction {
		const CExplosive		*m_grenade;
		const CGameObject		*m_game_object;
		u32						m_time;
		bool					m_processing;

		inline			CGrenadeReaction	()
		{
			clear				();
		}

		inline	void	clear				()
		{
			m_grenade			= 0;
			m_game_object		= 0;
			m_time				= 0;
			m_processing		= false;
		}
	};

protected:
	CStalker					*m_object;
	mutable const CCoverPoint	*m_cover;
	bool						m_initialized;
	float						m_probability;
	xr_vector<u32>				m_enemies;
	bool						m_processed;
	u32							m_selected_enemy;
	CMemberDeathReaction		m_member_death_reaction;
	CGrenadeReaction			m_grenade_reaction;
	bool						m_detour;

public:
	inline							CMemberOrder			(CStalker *object);
	inline		bool				initialized				() const;
	inline		CStalker			&object					() const;
	inline		float				probability				() const;
	inline		bool				processed				() const;
	inline		u32					selected_enemy			() const;
	inline		const CCoverPoint	*cover					() const;
	inline		CMemberDeathReaction&member_death_reaction	();
	inline		CGrenadeReaction	&grenade_reaction		();
	inline		xr_vector<u32>		&enemies				();
	inline		void				cover					(const CCoverPoint *object_cover) const;
	inline		void				probability				(float probability);
	inline		void				processed				(bool processed);
	inline		void				selected_enemy			(u32 selected_enemy);
	inline		bool				detour					() const;
	inline		void				detour					(const bool &value);
};

#include "member_order_inline.h"