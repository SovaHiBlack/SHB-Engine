//	Module 		: alife_communication_manager.cpp
//	Description : ALife communication manager

#include "stdafx.h"

#include "alife_interaction_manager.h"

CALifeInteractionManager::CALifeInteractionManager(CServer* server, const char* section) :
	CALifeCombatManager(server, section),
	CALifeCommunicationManager(server, section),
	CALifeSimulatorBase(server, section)
{ }
