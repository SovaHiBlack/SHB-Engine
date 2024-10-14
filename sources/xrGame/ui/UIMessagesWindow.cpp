// File:		UIMessagesWindow.cpp
// Description:	Window with MP chat and Game Log ( with PDA messages in single and Kill Messages in MP)
// Created:		22.04.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#include "stdafx.h"

#include "UIMessagesWindow.h"//
#include "../level.h"
#include "UIGameLog.h"
#include "xrUIXmlParser.h"
#include "UIXmlInit.h"
#include "UIPdaMsgListItem.h"//
#include "UIColorAnimatorWrapper.h"//
#include "../InfoPortion.h"
#include "../string_table.h"

CUIMessagesWindow::CUIMessagesWindow(){
	m_pGameLog = NULL;
	Init(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);
}

CUIMessagesWindow::~CUIMessagesWindow(){
	
}

void CUIMessagesWindow::AddLogMessage(const shared_str& msg){
	m_pGameLog->AddLogMessage(*msg);
}

void CUIMessagesWindow::Init(f32 x, f32 y, f32 width, f32 height){

	CUIXml		 uiXml;

	uiXml.Init(CONFIG_PATH, UI_PATH, "messages_window.xml");

	m_pGameLog = xr_new<CUIGameLog>();
	m_pGameLog->SetAutoDelete(true);
	m_pGameLog->Show(true);
	AttachChild(m_pGameLog);
	CUIXmlInit::InitScrollView(uiXml, "sp_log_list", 0, m_pGameLog);
}

void CUIMessagesWindow::AddIconedPdaMessage(pcstr textureName, Frect originalRect, pcstr message, int iDelay){
	
	CUIPdaMsgListItem *pItem			= m_pGameLog->AddPdaMessage(message, f32(iDelay));
	pItem->SetTextComplexMode			(true);
	pItem->UIIcon.InitTexture			(textureName);
	pItem->UIIcon.SetOriginalRect		(originalRect.left, originalRect.top, originalRect.right, originalRect.bottom);
	pItem->UIMsgText.SetWndPos			(pItem->UIIcon.GetWidth(), pItem->UIMsgText.GetWndPos().y);
	pItem->UIMsgText.AdjustHeightToText	();

	if (pItem->UIIcon.GetHeight() > pItem->UIMsgText.GetHeight())
		pItem->SetHeight(pItem->UIIcon.GetHeight());
	else
		pItem->SetHeight(pItem->UIMsgText.GetHeight());
	m_pGameLog->SendMessage(pItem,CHILD_CHANGED_SIZE);
}

void CUIMessagesWindow::Update()
{
	CUIWindow::Update();
}