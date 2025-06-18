#pragma once

#include "script_export_space.h"
#include "ui\UIXml.h"

class CUIWindow;
class CUIFrameWindow;
class CUIStatic;
class CUICheckButton;
class CUISpinNum;
class CUISpinText;
class CUISpinFlt;
class CUIComboBox;
class CUIButton;
class CUI3tButton;
class CUICheckButton;
class CUIListWnd;
class CUITabControl;
class CUIFrameLineWnd;
class CUILabel;
class CUIEditBox;
class CUITextBanner;
class CUIMultiTextStatic;
class CUIAnimatedStatic;
class CUIArtefactPanel;
class CUITrackBar;
class CUIMMShniaga;
class CUIScrollView;
class CUIProgressBar;

class CScriptXmlInit
{
public:
	DECLARE_SCRIPT_REGISTER_FUNCTION

						CScriptXmlInit		( );
						CScriptXmlInit		(const CScriptXmlInit& other);
	CScriptXmlInit&		operator =			(const CScriptXmlInit& other);

	void				ParseFile			(pcstr xml_file);
	void				ParseShTexInfo		(pcstr xml_file);
	void				InitWindow			(pcstr path, s32 index, CUIWindow* pWnd);

	CUIListWnd*			InitList			(pcstr path, CUIWindow* parent);
	CUIFrameWindow*		InitFrame			(pcstr path, CUIWindow* parent);
	CUIFrameLineWnd*	InitFrameLine		(pcstr path, CUIWindow* parent);
	CUILabel*			InitLabel			(pcstr path, CUIWindow* parent);
	CUIEditBox*			InitEditBox			(pcstr path, CUIWindow* parent);
	CUIStatic*			InitStatic			(pcstr path, CUIWindow* parent);
	CUIStatic*			InitAnimStatic		(pcstr path, CUIWindow* parent);
	CUICheckButton*		InitCheck			(pcstr path, CUIWindow* parent);
	CUISpinNum*			InitSpinNum			(pcstr path, CUIWindow* parent);
	CUISpinFlt*			InitSpinFlt			(pcstr path, CUIWindow* parent);
	CUISpinText*		InitSpinText		(pcstr path, CUIWindow* parent);
	CUIComboBox*		InitComboBox		(pcstr path, CUIWindow* parent);
	CUIButton*			InitButton			(pcstr path, CUIWindow* parent);
	CUI3tButton*		Init3tButton		(pcstr path, CUIWindow* parent);
	CUITabControl*		InitTab				(pcstr path, CUIWindow* parent);
	CUITrackBar*		InitTrackBar		(pcstr path, CUIWindow* parent);
	CUIMMShniaga*		InitMMShniaga		(pcstr path, CUIWindow* parent);
	CUIWindow*			InitKeyBinding		(pcstr path, CUIWindow* parent);
	CUIScrollView*		InitScrollView		(pcstr path, CUIWindow* parent);
	CUIProgressBar*		InitProgressBar		(pcstr path, CUIWindow* parent);
	void				InitAutoStaticGroup	(pcstr path, CUIWindow* pWnd);

protected:
	CUIXml				m_xml;
};

add_to_type_list(CScriptXmlInit)
#undef script_type_list
#define script_type_list save_type_list(CScriptXmlInit)
