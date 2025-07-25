#include "stdafx.h"

#include "UIPdaListItem.h"//
#include "../Actor.h"
#include "UIInventoryUtilities.h"//
#include "../StringTable.h"

//#include "UIXml.h"
#include "UIXmlInit.h"

#include "../CharacterInfo.h"

#include "UIFrameWindow.h"
#include "..\InventoryOwner.h"
#include "UICharacterInfo.h"//
#include "UIStatic.h"

#define			PDA_CONTACT_CHAR		"pda_character.xml"

CUIPdaListItem::CUIPdaListItem( )
{
	UIMask = NULL;
	UIInfo = NULL;
}

CUIPdaListItem::~CUIPdaListItem( )
{ }

void CUIPdaListItem::Init(f32 x, f32 y, f32 width, f32 height)
{
	inherited::Init(x, y, width, height);

	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, PDA_CONTACT_CHAR);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit xml_init;
	UIInfo = xr_new<CUICharacterInfo>( );
	UIInfo->SetAutoDelete(true);
	AttachChild(UIInfo);
	UIInfo->Init(0.0f, 0.0f, width, height, PDA_CONTACT_CHAR);

	if (uiXml.NavigateToNode("mask_frame_window", 0))
	{
		UIMask = xr_new<CUIFrameWindow>( );
		UIMask->SetAutoDelete(true);
		xml_init.InitFrameWindow(uiXml, "mask_frame_window", 0, UIMask);
		UIInfo->UIIcon( ).SetMask(UIMask);
	}

	xml_init.InitAutoStaticGroup(uiXml, "pda_char_auto_statics", 0, this);
}

void CUIPdaListItem::InitCharacter(CInventoryOwner* pInvOwner)
{
	VERIFY(pInvOwner);
	UIInfo->InitCharacter(pInvOwner->object_id( ));
}
