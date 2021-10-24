//	Module 		: ALifeInteractionManager.cpp
//	Description : ALife interaction manager

#include "stdafx.h"

#include "ALifeInteractionManager.h"

CALifeInteractionManager::CALifeInteractionManager(CServer* server, const char* section) : CALifeCombatManager(server, section), CALifeCommunicationManager(server, section), CAlifeSimulatorBase(server, section)
{ }
