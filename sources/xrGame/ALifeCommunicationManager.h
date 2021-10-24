//	Module 		: ALifeCommunicationManager.h
//	Description : ALife communication manager

#pragma once

#include "alife_simulator_base.h"

class CSE_ALifeSchedulable;
class CSE_ALifeHumanAbstract;
class CSE_ALifeTrader;
class CSE_ALifeTraderAbstract;

#define FAST_OWNERSHIP

class CALifeCommunicationManager : public virtual CAlifeSimulatorBase
{
public:
	CALifeCommunicationManager(CServer* server, const char* section);
};
