////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_cast.cpp
//	Created 	: 17.09.2004
//  Modified 	: 17.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Smart dynamic cast
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifndef PURE_DYNAMIC_CAST
#	ifdef XRGAME_EXPORTS
#		include "../XR_3DA\BasicVisual.h"
#		include "alife_space.h"
#		include "Hit.h"
#		include "Actor.h"
#		include "custommonster.h"
#		include "ai/stalker/Stalker.h"
#		include "ui/UIWindow.h"
#		include "CustomZone.h"
#		include "Weapon.h"
#		include "CameraEffector.h"
#	endif
#	include "xrServer_Objects_ALife_Monsters.h"
#endif

#undef   SMART_CAST_H
#include "smart_cast.h"
