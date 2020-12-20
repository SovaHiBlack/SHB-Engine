#pragma once

#include "script_export_space.h"
#include "ui\xrUIXmlParser.h"//

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
	static void script_register(lua_State*);

						CScriptXmlInit		( );
						CScriptXmlInit		(const CScriptXmlInit& other);
	CScriptXmlInit&		operator =			(const CScriptXmlInit& other);

	void				ParseFile			(const char* xml_file);
	void				ParseShTexInfo		(const char* xml_file);
	void				InitWindow			(const char* path, int index, CUIWindow* pWnd);

	CUIListWnd*			InitList			(const char* path, CUIWindow* parent);
	CUIFrameWindow*		InitFrame			(const char* path, CUIWindow* parent);
	CUIFrameLineWnd*	InitFrameLine		(const char* path, CUIWindow* parent);
	CUILabel*			InitLabel			(const char* path, CUIWindow* parent);
	CUIEditBox*			InitEditBox			(const char* path, CUIWindow* parent);
	CUIStatic*			InitStatic			(const char* path, CUIWindow* parent);
	CUIStatic*			InitAnimStatic		(const char* path, CUIWindow* parent);
	CUICheckButton*		InitCheck			(const char* path, CUIWindow* parent);
	CUISpinNum*			InitSpinNum			(const char* path, CUIWindow* parent);
	CUISpinFlt*			InitSpinFlt			(const char* path, CUIWindow* parent);
	CUISpinText*		InitSpinText		(const char* path, CUIWindow* parent);
	CUIComboBox*		InitComboBox		(const char* path, CUIWindow* parent);
	CUIButton*			InitButton			(const char* path, CUIWindow* parent);
	CUI3tButton*		Init3tButton		(const char* path, CUIWindow* parent);
	CUITabControl*		InitTab				(const char* path, CUIWindow* parent);
	CUITrackBar*		InitTrackBar		(const char* path, CUIWindow* parent);
	CUIMMShniaga*		InitMMShniaga		(const char* path, CUIWindow* parent);
	CUIWindow*			InitKeyBinding		(const char* path, CUIWindow* parent);
	CUIScrollView*		InitScrollView		(const char* path, CUIWindow* parent);
	CUIProgressBar*		InitProgressBar		(const char* path, CUIWindow* parent);
	void				InitAutoStaticGroup	(const char* path, CUIWindow* pWnd);

protected:
	CUIXml				m_xml;
};

add_to_type_list(CScriptXmlInit)
#undef script_type_list
#define script_type_list save_type_list(CScriptXmlInit)
