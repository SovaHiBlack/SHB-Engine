#include "stdafx.h"

#include "ScriptXmlInit.h"
#include "ui\UIXmlInit.h"
#include "ui\UITextureMaster.h"
#include "ui\UICheckButton.h"
#include "ui\UISpinNum.h"
#include "ui\UISpinText.h"
#include "ui\UIComboBox.h"
#include "ui\UIListWnd.h"
#include "ui\UITabControl.h"
#include "ui\UIFrameWindow.h"
#include "ui\UILabel.h"
#include "ui\UIKeyBinding.h"//
#include "ui\UIEditBox.h"
#include "ui\UIAnimatedStatic.h"
#include "ui\UITrackBar.h"
#include "ui\UIMMShniaga.h"//
#include "ui\UIScrollView.h"
#include "ui\UIProgressBar.h"

using namespace luabind;

void				_attach_child						(CUIWindow* _child, CUIWindow* _parent)
{
	if (!_parent)
	{
		return;
	}

	CUIScrollView* _parent_scroll		= smart_cast<CUIScrollView*>(_parent);
	if (_parent_scroll)
	{
		_parent_scroll->AddWindow		(_child, true);
	}
	else
	{
		_parent->AttachChild			(_child);
	}
}

					CScriptXmlInit::CScriptXmlInit		( )
{ }

					CScriptXmlInit::CScriptXmlInit		(const CScriptXmlInit&)
{ }

CScriptXmlInit&		CScriptXmlInit::operator =			(const CScriptXmlInit& other)
{
	return								(*this);
}

void				CScriptXmlInit::ParseFile			(pcstr xml_file)
{
	m_xml.Init							(CONFIG_PATH, UI_PATH, xml_file);
}

void				CScriptXmlInit::InitWindow			(pcstr path, int index, CUIWindow* pWnd)
{
	CUIXmlInit::InitWindow				(m_xml, path, index, pWnd);
}

CUIFrameWindow*		CScriptXmlInit::InitFrame			(pcstr path, CUIWindow* parent)
{
	CUIFrameWindow* pWnd				= xr_new<CUIFrameWindow>( );
	CUIXmlInit::InitFrameWindow			(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

CUIFrameLineWnd*	CScriptXmlInit::InitFrameLine		(pcstr path, CUIWindow* parent)
{
	CUIFrameLineWnd* pWnd				= xr_new<CUIFrameLineWnd>( );
	CUIXmlInit::InitFrameLine			(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

CUILabel*			CScriptXmlInit::InitLabel			(pcstr path, CUIWindow* parent)
{
	CUILabel* pWnd						= xr_new<CUILabel>( );
	CUIXmlInit::InitLabel				(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

CUIEditBox*			CScriptXmlInit::InitEditBox			(pcstr path, CUIWindow* parent)
{
	CUIEditBox* pWnd					= xr_new<CUIEditBox>( );
	CUIXmlInit::InitEditBox				(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

CUIStatic*			CScriptXmlInit::InitStatic			(pcstr path, CUIWindow* parent)
{
	CUIStatic* pWnd						= xr_new<CUIStatic>( );
	CUIXmlInit::InitStatic				(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

void				CScriptXmlInit::InitAutoStaticGroup	(pcstr path, CUIWindow* pWnd)
{
	CUIXmlInit::InitAutoStaticGroup		(m_xml, path, 0, pWnd);
}

CUIStatic*			CScriptXmlInit::InitAnimStatic		(pcstr path, CUIWindow* parent)
{
	CUIAnimatedStatic* pWnd				= xr_new<CUIAnimatedStatic>( );
	CUIXmlInit::InitAnimatedStatic		(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

CUIScrollView*		CScriptXmlInit::InitScrollView		(pcstr path, CUIWindow* parent)
{
	CUIScrollView* pWnd					= xr_new<CUIScrollView>( );
	CUIXmlInit::InitScrollView			(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

CUICheckButton*		CScriptXmlInit::InitCheck			(pcstr path, CUIWindow* parent)
{
	CUICheckButton* pWnd				= xr_new<CUICheckButton>( );
	CUIXmlInit::InitCheck				(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

CUISpinNum*			CScriptXmlInit::InitSpinNum			(pcstr path, CUIWindow* parent)
{
	CUISpinNum* pWnd					= xr_new<CUISpinNum>( );
	CUIXmlInit::InitSpin				(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);
;
	return								pWnd;
}

CUISpinFlt*			CScriptXmlInit::InitSpinFlt			(pcstr path, CUIWindow* parent)
{
	CUISpinFlt* pWnd					= xr_new<CUISpinFlt>( );
	CUIXmlInit::InitSpin				(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

CUISpinText*		CScriptXmlInit::InitSpinText		(pcstr path, CUIWindow* parent)
{
	CUISpinText* pWnd					= xr_new<CUISpinText>( );
	CUIXmlInit::InitSpin				(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	if (parent)
	{
		parent->AttachChild				(pWnd);
	}

	return								pWnd;
}

CUIComboBox*		CScriptXmlInit::InitComboBox		(pcstr path, CUIWindow* parent)
{
	CUIComboBox* pWnd					= xr_new<CUIComboBox>( );
	CUIXmlInit::InitComboBox			(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

CUIButton*			CScriptXmlInit::InitButton			(pcstr path, CUIWindow* parent)
{
	CUIButton* pWnd						= xr_new<CUIButton>( );
	CUIXmlInit::InitButton				(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

CUI3tButton*		CScriptXmlInit::Init3tButton		(pcstr path, CUIWindow* parent)
{
	CUI3tButton* pWnd					= xr_new<CUI3tButton>( );
	CUIXmlInit::Init3tButton			(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

CUIListWnd*			CScriptXmlInit::InitList			(pcstr path, CUIWindow* parent)
{
	CUIListWnd* pWnd					= xr_new<CUIListWnd>( );
	CUIXmlInit::InitListWnd				(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

CUITabControl*		CScriptXmlInit::InitTab				(pcstr path, CUIWindow* parent)
{
	CUITabControl* pWnd					= xr_new<CUITabControl>( );
	CUIXmlInit::InitTabControl			(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

void				CScriptXmlInit::ParseShTexInfo		(pcstr xml_file)
{
	CUITextureMaster::ParseShTexInfo	(xml_file);
}

CUIMMShniaga*		CScriptXmlInit::InitMMShniaga		(pcstr path, CUIWindow* parent)
{
	CUIMMShniaga* pWnd					= xr_new<CUIMMShniaga>( );
	pWnd->Init							(m_xml, path);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

CUIWindow*			CScriptXmlInit::InitKeyBinding		(pcstr path, CUIWindow* parent)
{
	CUIKeyBinding* pWnd					= xr_new<CUIKeyBinding>( );
	pWnd->InitFromXml					(m_xml, path);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

CUITrackBar*		CScriptXmlInit::InitTrackBar		(pcstr path, CUIWindow* parent)
{
	CUITrackBar* pWnd					= xr_new<CUITrackBar>( );
	CUIXmlInit::InitTrackBar			(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

CUIProgressBar*		CScriptXmlInit::InitProgressBar		(pcstr path, CUIWindow* parent)
{
	CUIProgressBar* pWnd				= xr_new<CUIProgressBar>( );
	CUIXmlInit::InitProgressBar			(m_xml, path, 0, pWnd);
	pWnd->SetAutoDelete					(true);
	_attach_child						(pWnd, parent);

	return								pWnd;
}

#pragma optimize("s", on)
void				CScriptXmlInit::script_register		(lua_State* L)
{
	module(L)
	[
		class_<CScriptXmlInit>("CScriptXmlInit")
		.def(constructor<>( ))
		.def("ParseFile",			&CScriptXmlInit::ParseFile)
		.def("ParseShTexInfo",		&CScriptXmlInit::ParseShTexInfo)
		.def("InitWindow",			&CScriptXmlInit::InitWindow)
		.def("InitFrame",			&CScriptXmlInit::InitFrame)
		.def("InitFrameLine",		&CScriptXmlInit::InitFrameLine)
		.def("InitLabel",			&CScriptXmlInit::InitLabel)
		.def("InitEditBox",			&CScriptXmlInit::InitEditBox)
		.def("InitStatic",			&CScriptXmlInit::InitStatic)
		.def("InitAnimStatic",		&CScriptXmlInit::InitAnimStatic)
		.def("InitCheck",			&CScriptXmlInit::InitCheck)
		.def("InitSpinNum",			&CScriptXmlInit::InitSpinNum)
		.def("InitSpinFlt",			&CScriptXmlInit::InitSpinFlt)
		.def("InitSpinText",		&CScriptXmlInit::InitSpinText)
		.def("InitComboBox",		&CScriptXmlInit::InitComboBox)
		.def("InitButton",			&CScriptXmlInit::InitButton)
		.def("Init3tButton",		&CScriptXmlInit::Init3tButton)
		.def("InitList",			&CScriptXmlInit::InitList)
		.def("InitTab",				&CScriptXmlInit::InitTab)
		.def("InitTrackBar",		&CScriptXmlInit::InitTrackBar)
		.def("InitKeyBinding",		&CScriptXmlInit::InitKeyBinding)
		.def("InitMMShniaga",		&CScriptXmlInit::InitMMShniaga)
		.def("InitScrollView",		&CScriptXmlInit::InitScrollView)
		.def("InitAutoStaticGroup",	&CScriptXmlInit::InitAutoStaticGroup)
		.def("InitProgressBar",		&CScriptXmlInit::InitProgressBar)
	];
}
