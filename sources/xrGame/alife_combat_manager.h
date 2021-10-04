//	Module 		: alife_combat_manager.h
//	Description : ALife combat manager

#pragma once

#include "alife_simulator_base.h"

class CALifeCombatManager : public virtual CAlifeSimulatorBase, CRandom
{
public:
			CALifeCombatManager(CServer* server, const char* section);

	void	kill_entity(CSE_ALifeMonsterAbstract* l_tpALifeMonsterAbstract, const GameGraph::_GRAPH_ID& l_tGraphID, CSE_ALifeSchedulable* schedulable);
};
