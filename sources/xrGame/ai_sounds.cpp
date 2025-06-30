#include "stdafx.h"

#include "ai_sounds.h"

xr_token anomaly_type_token[ ] =
{
	{ "undefined",				sg_Undefined				},
	{ "Item picking up",		eST_ITEM_PICKING_UP			},
	{ "Item dropping",			eST_ITEM_DROPPING			},
	{ "Item hiding",			eST_ITEM_HIDING				},
	{ "Item taking",			eST_ITEM_TAKING				},
	{ "Item using",				eST_ITEM_USING				},
	{ "Weapon shooting",		eST_WEAPON_SHOOTING			},
	{ "Weapon empty clicking",	eST_WEAPON_EMPTY_CLICKING	},
	{ "Weapon bullet hit",		eST_WEAPON_BULLET_HIT		},
	{ "Weapon recharging",		eST_WEAPON_RECHARGING		},
	{ "NPC dying",				eST_MONSTER_DYING			},
	{ "NPC injuring",			eST_MONSTER_INJURING		},
	{ "NPC step",				eST_MONSTER_STEP			},
	{ "NPC talking",			eST_MONSTER_TALKING			},
	{ "NPC attacking",			eST_MONSTER_ATTACKING		},
	{ "NPC eating",				eST_MONSTER_EATING			},
	{ "Anomaly idle",			eST_ANOMALY_IDLE			},
	{ "Object breaking",		eST_WORLD_OBJECT_BREAKING	},
	{ "Object colliding",		eST_WORLD_OBJECT_COLLIDING	},
	{ "Object exploding",		eST_WORLD_OBJECT_EXPLODING	},
	{ "World ambient",			eST_WORLD_AMBIENT			},
	{ 0,						0							}
};
