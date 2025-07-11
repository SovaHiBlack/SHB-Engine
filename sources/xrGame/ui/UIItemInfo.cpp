#include "stdafx.h"

#include "UIItemInfo.h"//
#include "uistatic.h"
#include "UIXmlInit.h"

#include "UIListWnd.h"
#include "UIProgressBar.h"
#include "UIScrollView.h"

#include "../StringTable.h"
#include "../InventoryItem.h"
#include "UIInventoryUtilities.h"//
#include "../PhysicsShellHolder.h"
#include "UIWpnParams.h"//
#include "UIArtefactParams.h"//

CUIItemInfo::CUIItemInfo( )
{
	UIItemImageSize.set(0.0f, 0.0f);
	UICondProgresBar = NULL;
	UICondition = NULL;
	UICost = NULL;
	UIWeight = NULL;
	UIItemImage = NULL;
	UIDesc = NULL;
	UIWpnParams = NULL;
	UIArtefactParams = NULL;
	UIName = NULL;
	m_pInvItem = NULL;
	m_b_force_drawing = false;
}

CUIItemInfo::~CUIItemInfo( )
{
	xr_delete(UIWpnParams);
	xr_delete(UIArtefactParams);
}

void CUIItemInfo::Init(pcstr xml_name)
{
	CUIXml						uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, xml_name);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit					xml_init;

	if (uiXml.NavigateToNode("main_frame", 0))
	{
		fRect wnd_rect;
		wnd_rect.x1 = uiXml.ReadAttribFlt("main_frame", 0, "x", 0);
		wnd_rect.y1 = uiXml.ReadAttribFlt("main_frame", 0, "y", 0);

		wnd_rect.x2 = uiXml.ReadAttribFlt("main_frame", 0, "width", 0);
		wnd_rect.y2 = uiXml.ReadAttribFlt("main_frame", 0, "height", 0);

		inherited::Init(wnd_rect.x1, wnd_rect.y1, wnd_rect.x2, wnd_rect.y2);
	}

	if (uiXml.NavigateToNode("static_name", 0))
	{
		UIName = xr_new<CUIStatic>( );
		AttachChild(UIName);
		UIName->SetAutoDelete(true);
		xml_init.InitStatic(uiXml, "static_name", 0, UIName);
	}

	if (uiXml.NavigateToNode("static_weight", 0))
	{
		UIWeight = xr_new<CUIStatic>( );
		AttachChild(UIWeight);
		UIWeight->SetAutoDelete(true);
		xml_init.InitStatic(uiXml, "static_weight", 0, UIWeight);
	}

	if (uiXml.NavigateToNode("static_cost", 0))
	{
		UICost = xr_new<CUIStatic>( );
		AttachChild(UICost);
		UICost->SetAutoDelete(true);
		xml_init.InitStatic(uiXml, "static_cost", 0, UICost);
	}

	if (uiXml.NavigateToNode("static_condition", 0))
	{
		UICondition = xr_new<CUIStatic>( );
		AttachChild(UICondition);
		UICondition->SetAutoDelete(true);
		xml_init.InitStatic(uiXml, "static_condition", 0, UICondition);
	}

	if (uiXml.NavigateToNode("condition_progress", 0))
	{
		UICondProgresBar = xr_new<CUIProgressBar>( );
		AttachChild(UICondProgresBar);
		UICondProgresBar->SetAutoDelete(true);
		xml_init.InitProgressBar(uiXml, "condition_progress", 0, UICondProgresBar);
	}

	if (uiXml.NavigateToNode("descr_list", 0))
	{
		UIWpnParams = xr_new<CUIWpnParams>( );
		UIArtefactParams = xr_new<CUIArtefactParams>( );
		UIWpnParams->InitFromXml(uiXml);
		UIArtefactParams->InitFromXml(uiXml);
		UIDesc = xr_new<CUIScrollView>( );
		AttachChild(UIDesc);
		UIDesc->SetAutoDelete(true);
		m_desc_info.bShowDescrText = !!uiXml.ReadAttribInt("descr_list", 0, "only_text_info", 1);
		xml_init.InitScrollView(uiXml, "descr_list", 0, UIDesc);
		xml_init.InitFont(uiXml, "descr_list:font", 0, m_desc_info.uDescClr, m_desc_info.pDescFont);
	}

	if (uiXml.NavigateToNode("image_static", 0))
	{
		UIItemImage = xr_new<CUIStatic>( );
		AttachChild(UIItemImage);
		UIItemImage->SetAutoDelete(true);
		xml_init.InitStatic(uiXml, "image_static", 0, UIItemImage);
		UIItemImage->TextureAvailable(true);

		UIItemImage->TextureOff( );
		UIItemImage->ClipperOn( );
		UIItemImageSize.set(UIItemImage->GetWidth( ), UIItemImage->GetHeight( ));
	}

	xml_init.InitAutoStaticGroup(uiXml, "auto", 0, this);
}

void CUIItemInfo::Init(f32 x, f32 y, f32 width, f32 height, pcstr xml_name)
{
	inherited::Init(x, y, width, height);
	Init(xml_name);
}

void CUIItemInfo::InitItem(CInventoryItem* pInvItem)
{
	m_pInvItem = pInvItem;
	if (!m_pInvItem)
	{
		return;
	}

	string256				str;
	if (UIName)
	{
		UIName->SetText(pInvItem->Name( ));
	}

	if (UIWeight)
	{
		sprintf_s(str, "%3.2f kg", pInvItem->Weight( ));
		UIWeight->SetText(str);
	}

	if (UICost)
	{
		sprintf_s(str, "%d RU", pInvItem->Cost( ));
		UICost->SetText(str);
	}

	if (UICondProgresBar)
	{
		f32 cond = pInvItem->GetConditionToShow( );
		UICondProgresBar->Show(true);
		UICondProgresBar->SetProgressPos(cond * 100.0f + 1.0f - EPSILON_5);
	}

	if (UIDesc)
	{
		UIDesc->Clear( );
		VERIFY(0 == UIDesc->GetSize( ));
		TryAddWpnInfo(pInvItem->object( ).cNameSect( ));
		TryAddArtefactInfo(pInvItem->object( ).cNameSect( ));
		if (m_desc_info.bShowDescrText)
		{
			CUIStatic* pItem = xr_new<CUIStatic>( );
			pItem->SetTextColor(m_desc_info.uDescClr);
			pItem->SetFont(m_desc_info.pDescFont);
			pItem->SetWidth(UIDesc->GetDesiredChildWidth( ));
			pItem->SetTextComplexMode(true);
			pItem->SetText(*pInvItem->ItemDescription( ));
			pItem->AdjustHeightToText( );
			UIDesc->AddWindow(pItem, true);
		}

		UIDesc->ScrollToBegin( );
	}

	if (UIItemImage)
	{
		// Загружаем картинку
		UIItemImage->SetShader(InventoryUtilities::GetEquipmentIconsShader( ));

		s32 iGridWidth = pInvItem->GetGridWidth( );
		s32 iGridHeight = pInvItem->GetGridHeight( );
		s32 iXPos = pInvItem->GetXPos( );
		s32 iYPos = pInvItem->GetYPos( );

		UIItemImage->GetUIStaticItem( ).SetOriginalRect(f32(iXPos * INV_GRID_WIDTH), f32(iYPos * INV_GRID_HEIGHT), f32(iGridWidth * INV_GRID_WIDTH), f32(iGridHeight * INV_GRID_HEIGHT));
		UIItemImage->TextureOn( );
		UIItemImage->ClipperOn( );
		UIItemImage->SetStretchTexture(true);
		fRect v_r = { 0.0f, 0.0f, f32(iGridWidth * INV_GRID_WIDTH), f32(iGridHeight * INV_GRID_HEIGHT) };
		if (UI( )->is_16_9_mode( ))
		{
			v_r.x2 /= 1.328f;
		}

		UIItemImage->GetUIStaticItem( ).SetRect(v_r);
		UIItemImage->SetWidth(_min(v_r.width( ), UIItemImageSize.x));
		UIItemImage->SetHeight(_min(v_r.height( ), UIItemImageSize.y));
	}
}

void CUIItemInfo::TryAddWpnInfo(const shared_str& wpn_section)
{
	if (UIWpnParams->Check(wpn_section))
	{
		UIWpnParams->SetInfo(wpn_section);
		UIDesc->AddWindow(UIWpnParams, false);
	}
}

void CUIItemInfo::TryAddArtefactInfo(const shared_str& af_section)
{
	if (UIArtefactParams->Check(af_section))
	{
		UIArtefactParams->SetInfo(af_section);
		UIDesc->AddWindow(UIArtefactParams, false);
	}
}

void CUIItemInfo::Draw( )
{
	if (m_pInvItem || m_b_force_drawing)
	{
		inherited::Draw( );
	}
}
