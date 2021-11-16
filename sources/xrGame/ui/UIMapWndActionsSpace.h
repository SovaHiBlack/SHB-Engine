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
		ePropDummy					= unsigned short(-1)
	};

	enum EWorldOperators
	{
		eOperatorMapResize,
		eOperatorMapMinimize,
		eOperatorMapIdle,
		eOperatorMapCenter,
		eWorldOperatorDummy			= unsigned short(-1)
	};
};
