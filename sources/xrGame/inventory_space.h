#pragma once

#define CMD_START			(1<<0)
#define CMD_STOP			(1<<1)

#define NO_ACTIVE_SLOT		0xffffffff
#define KNIFE_SLOT			0
#define PISTOL_SLOT			1
#define RIFLE_SLOT			2
#define GRENADE_SLOT		3
#define APPARATUS_SLOT		4
#define BOLT_SLOT			5
#define OUTFIT_SLOT			6
#define PDA_SLOT			7
#define DETECTOR_SLOT		8
#define TORCH_SLOT			9
#define ARTEFACT_SLOT		10
#define SLOTS_TOTAL			11

class CInventoryItem;
class CInventory;

using PIItem				= CInventoryItem*;
using TIItemContainer		= xr_vector<PIItem>;

enum EItemPlace
{
	eItemPlaceUndefined,
	eItemPlaceSlot,
	eItemPlaceBelt,
	eItemPlaceRuck
};

extern unsigned int			INV_STATE_LADDER;
extern unsigned int			INV_STATE_CAR;
extern unsigned int			INV_STATE_BLOCK_ALL;
extern unsigned int			INV_STATE_INV_WND;
extern unsigned int			INV_STATE_BUY_MENU;
