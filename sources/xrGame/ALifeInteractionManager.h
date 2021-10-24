//	Module 		: ALifeInteractionManager.h
//	Description : ALife interaction manager

#pragma once

#include "Server_space.h"
#include "ALifeCombatManager.h"
#include "ALifeCommunicationManager.h"

class CALifeInteractionManager : public CALifeCombatManager, public CALifeCommunicationManager
{
public:
	CALifeInteractionManager(CServer* server, const char* section);
};
