#pragma once

namespace UIMapWndActionsSpace
{
	enum EWorldProperties
	{
		ePropTargetMapShown,
		ePropMapMinimized,
		ePropMapResized,
		ePropMapIdle,
		ePropMapCentered,
		ePropDummy					= U16(-1)
	};

	enum EWorldOperators
	{
		eOperatorMapResize,
		eOperatorMapMinimize,
		eOperatorMapIdle,
		eOperatorMapCenter,
		eWorldOperatorDummy			= U16(-1)
	};
};
