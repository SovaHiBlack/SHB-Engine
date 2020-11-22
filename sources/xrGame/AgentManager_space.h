//	Module		: AgentManager_space.h
//	Description	: Agent manager space

#pragma once

namespace AgentManager
{
	enum EProperties
	{
		ePropertyOrders = u32(0),
		ePropertyItem,
		ePropertyEnemy,
		ePropertyDanger,

		ePropertyScript,
		ePropertyDummy = u32(-1)
	};

	enum EOperators
	{
		eOperatorNoOrders = u32(0),
		eOperatorGatherItem,
		eOperatorKillEnemy,
		eOperatorReactOnDanger,

		eOperatorScript,
		eOperatorDummy = u32(-1)
	};
};
