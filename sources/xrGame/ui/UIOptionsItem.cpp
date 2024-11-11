#include "stdafx.h"
#include "UIOptionsItem.h"//
#include "UIOptionsManager.h"//
#include "../..\XR_3DA\Console.h"

CUIOptionsManager CUIOptionsItem::m_optionsManager;

CUIOptionsItem::~CUIOptionsItem()
{
	m_optionsManager.UnRegisterItem(this);
}

void CUIOptionsItem::Register(pcstr entry, pcstr group)
{
	m_optionsManager.RegisterItem	(this, group);
	m_entry							= entry;	
}

void CUIOptionsItem::SendMessage2Group(pcstr group, pcstr message)
{
	m_optionsManager.SendMessage2Group(group,message);
}

void CUIOptionsItem::OnMessage(pcstr message)
{
	// do nothing
}

pcstr CUIOptionsItem::GetOptStringValue()
{
	return Console->GetString(m_entry.c_str());
}

void CUIOptionsItem::SaveOptStringValue(pcstr val)
{
	xr_string command	= m_entry;
	command				+= " ";
	command				+= val;
	Console->Execute	(command.c_str());
}

void CUIOptionsItem::GetOptIntegerValue(int& val, int& min, int& max)
{
	Console->GetInteger(m_entry.c_str(), val, min, max);
}

void CUIOptionsItem::SaveOptIntegerValue(int val)
{
	string512			command;
	sprintf_s				(command, "%s %d", m_entry.c_str(), val);
	Console->Execute	(command);
}

void CUIOptionsItem::GetOptFloatValue(f32& val, f32& min, f32& max)
{
	Console->GetFloat(m_entry.c_str(), val, min, max);
}

void CUIOptionsItem::SaveOptFloatValue(f32 val)
{
	string512			command;
	sprintf_s				(command, "%s %f", m_entry.c_str(), val);
	Console->Execute	(command);
}

bool CUIOptionsItem::GetOptBoolValue()
{
	BOOL val;
	Console->GetBool(m_entry.c_str(), val);
	return val ? true : false;
}

void CUIOptionsItem::SaveOptBoolValue(bool val)
{
	string512			command;
	sprintf_s				(command, "%s %s", m_entry.c_str(), (val)?"on":"off");
	Console->Execute	(command);
}

pstr CUIOptionsItem::GetOptTokenValue()
{
	return Console->GetToken(m_entry.c_str());
}

xr_token* CUIOptionsItem::GetOptToken()
{
	return Console->GetXRToken(m_entry.c_str());
}

void CUIOptionsItem::SaveOptTokenValue(pcstr val){
	SaveOptStringValue(val);
}

void CUIOptionsItem::SaveValue(){
	if (	m_entry == "vid_mode"		||
			m_entry == "_preset"		||
			m_entry == "rs_fullscreen" 	||
			m_entry == "rs_fullscreen"	||
			m_entry == "r__supersample"	||
			m_entry == "rs_refresh_60hz"||
			m_entry == "rs_no_v_sync"	||
			m_entry == "texture_lod")
	m_optionsManager.DoVidRestart();

	if (/*m_entry == "snd_freq" ||*/ m_entry == "snd_efx")
		m_optionsManager.DoSndRestart();
}
