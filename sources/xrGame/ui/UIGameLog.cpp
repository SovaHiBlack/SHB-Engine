//=============================================================================
//  Filename:   UIGameLog.h
//  Multiplayer game log window
//=============================================================================
#include "stdafx.h"
#include "UIGameLog.h"
#include "UIXmlInit.h"
#include "UIColorAnimatorWrapper.h"//
#include "UIPdaMsgListItem.h"//
#include "UILines.h"

pcstr CHAT_LOG_ITEMS_ANIMATION = "ui_main_msgs_short";

CUIGameLog::CUIGameLog( )
{
	toDelList.reserve(30);
	kill_msg_height = 20.0f;
	txt_color = 0xff000000;
}

CUIGameLog::~CUIGameLog( )
{ }

CUIStatic* CUIGameLog::AddLogMessage(pcstr msg)
{
	CUIStatic* pItem = NULL;
	ADD_TEXT_TO_VIEW3(msg, pItem, this);
	pItem->SetTextComplexMode(true);
	pItem->SetFont(GetFont( ));
	pItem->SetTextColor(txt_color);
	pItem->SetClrAnimDelay(5000.0f);
	pItem->SetClrLightAnim(CHAT_LOG_ITEMS_ANIMATION, false, true, true, true);
	ForceUpdate( );
	return pItem;
}

// warning: initialization of item is incomplete!
// initialization of item's height, text static and icon still necessary
CUIPdaMsgListItem* CUIGameLog::AddPdaMessage(pcstr msg, f32 delay)
{
	CUIPdaMsgListItem* pItem = xr_new<CUIPdaMsgListItem>( );
	pItem->Init(0, 0, GetDesiredChildWidth( ), 10);	//fake height
	pItem->UIMsgText.SetTextST(msg);
	pItem->SetClrAnimDelay(delay);
	pItem->SetClrLightAnim(CHAT_LOG_ITEMS_ANIMATION, false, true, true, true);
	AddWindow(pItem, true);

	return pItem;
}

u32 CUIGameLog::GetTextColor( )
{
	return txt_color;
}

void CUIGameLog::SetTextAtrib(CGameFont* pFont, u32 color)
{
	SetFont(pFont);
	txt_color = color;
}

void CUIGameLog::Update( )
{
	CUIScrollView::Update( );
	toDelList.clear( );

	// REMOVE ITEMS WITH COMPLETED ANIMATION
	WINDOW_LIST_it end_it = m_pad->GetChildWndList( ).end( );
	WINDOW_LIST_it begin_it = m_pad->GetChildWndList( ).begin( );

	for (WINDOW_LIST_it it = begin_it; it != end_it; ++it)
	{
		CUIStatic* pItem = smart_cast<CUIStatic*>(*it);
		VERIFY(pItem);
		pItem->Update( );

		if (pItem->IsClrAnimStoped( ))
		{
			toDelList.push_back(pItem);
		}
	}

	// Delete elements
	{
		xr_vector<CUIWindow*>::iterator it;
		for (it = toDelList.begin( ); it != toDelList.end( ); it++)
		{
			RemoveWindow(*it);
		}
	}

	// REMOVE INVISIBLE AND PART VISIBLE ITEMS
	if (m_flags.test(eNeedRecalc))
	{
		RecalcSize( );
	}

	toDelList.clear( );
}
