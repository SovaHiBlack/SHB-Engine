// ========================================== SHB_Engine ==========================================
// Projekt		: Game
// Module		: UIXmlInit.cpp
// Author		: Anahoret
// Description	: 
// ========================================= SovaHiBlack� =========================================

#include "stdafx.h"
#pragma hdrstop

#include "../game_base_space.h"
#include "../hudmanager.h"
#include "../level.h"
#include "../StringTable.h"
#include "UI.h"
#include "UICustomItem.h"
#include "UIAnimatedStatic.h"
#include "UICheckButton.h"
#include "UIComboBox.h"
#include "UICustomSpin.h"
#include "UIDragDropListEx.h"
#include "UIEditBox.h"
#include "UIEditBoxEx.h"
#include "UIFrameWindow.h"
#include "UILabel.h"
#include "UIListBox.h"
#include "UIListWnd.h"
#include "UIMultiTextStatic.h"
#include "UIProgressBar.h"
#include "UIProgressShape.h"
#include "UIRadioButton.h"
#include "UIStatic.h"
#include "UITabControl.h"
#include "UITextBanner.h"
#include "UITextureMaster.h"
#include "UITrackBar.h"
#include "UIXmlInit.h"

extern s32 keyname_to_dik(pcstr);

#define SMALL_FONT_NAME			"small"
#define MEDIUM_FONT_NAME		"medium"
#define DI_FONT_NAME			"di"

#define ARIAL14_FONT_NAME		"arial14"
#define ARIAL21_FONT_NAME		"arial21"

#define GRAFFITI19_FONT_NAME	"graffiti19"
#define GRAFFITI22_FONT_NAME	"graffiti22"
#define GRAFFITI32_FONT_NAME	"graffiti32"
#define GRAFFITI40_FONT_NAME	"graffiti40"
#define GRAFFITI50_FONT_NAME	"graffiti50"
#define LETTERICA16_FONT_NAME	"letterica16"
#define LETTERICA18_FONT_NAME	"letterica18"
#define LETTERICA25_FONT_NAME	"letterica25"

//////////////////////////////////////////////////////////////////////////

pcstr		COLOR_DEFINITIONS = "color_defs.xml";
CUIXmlInit::ColorDefs* CUIXmlInit::m_pColorDefs = NULL;

//////////////////////////////////////////////////////////////////////////

CUIXmlInit::CUIXmlInit( )
{
	InitColorDefs( );
}

//////////////////////////////////////////////////////////////////////////

CUIXmlInit::~CUIXmlInit( )
{ }

//////////////////////////////////////////////////////////////////////////

fRect CUIXmlInit::GetFRect(CUIXml& xml_doc, pcstr path, s32 index)
{
	R_ASSERT3(xml_doc.NavigateToNode(path, index), "XML node not found", path);
	fRect rect;
	rect.set(0.0f, 0.0f, 0.0f, 0.0f);
	rect.x1 = xml_doc.ReadAttribFlt(path, index, "x");
	rect.y1 = xml_doc.ReadAttribFlt(path, index, "y");
	rect.x2 = rect.x1 + xml_doc.ReadAttribFlt(path, index, "width");
	rect.y2 = rect.y1 + xml_doc.ReadAttribFlt(path, index, "height");

	return rect;
}

bool CUIXmlInit::InitWindow(CUIXml& xml_doc, pcstr path, s32 index, CUIWindow* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path, index), "XML node not found", path);

	f32 x = xml_doc.ReadAttribFlt(path, index, "x");
	f32 y = xml_doc.ReadAttribFlt(path, index, "y");
	InitAlignment(xml_doc, path, index, x, y, pWnd);
	f32 width = xml_doc.ReadAttribFlt(path, index, "width");
	f32 height = xml_doc.ReadAttribFlt(path, index, "height");
	pWnd->Init(x, y, width, height);

	string512 buf;
	CGameFont* LocalFont = NULL;
	u32 cl;
	strconcat(sizeof(buf), buf, path, ":font");
	InitFont(xml_doc, buf, index, cl, LocalFont);
	if (LocalFont)
	{
		pWnd->SetFont(LocalFont);
	}

	strconcat(sizeof(buf), buf, path, ":window_name");
	if (xml_doc.NavigateToNode(buf, index))
	{
		pWnd->SetWindowName(xml_doc.Read(buf, index, NULL));
	}

	InitAutoStaticGroup(xml_doc, path, index, pWnd);
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitFrameWindow(CUIXml& xml_doc, pcstr path, s32 index, CUIFrameWindow* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path, index), "XML node not found", path);

	InitWindow(xml_doc, path, index, pWnd);
	InitTexture(xml_doc, path, index, pWnd);

	//���������������� ��������� ����
	string256 buf;
	strconcat(sizeof(buf), buf, path, ":title");
	if (xml_doc.NavigateToNode(buf, index))
	{
		InitStatic(xml_doc, buf, index, pWnd->UITitleText);
	}

	pWnd->BringToTop(pWnd->UITitleText);

	return true;
}

bool CUIXmlInit::InitOptionsItem(CUIXml& xml_doc, pcstr paht, s32 index, CUIOptionsItem* pWnd)
{
	string256 buf;
	strconcat(sizeof(buf), buf, paht, ":options_item");
	if (xml_doc.NavigateToNode(buf, index))
	{
		shared_str entry = xml_doc.ReadAttrib(buf, index, "entry");
		shared_str group = xml_doc.ReadAttrib(buf, index, "group");

		pWnd->Register(*entry, *group);
		return true;
	}
	else
	{
		return false;
	}
}

bool CUIXmlInit::InitStatic(CUIXml& xml_doc, pcstr path, s32 index, CUIStatic* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path, index), "XML node not found", path);

	InitWindow(xml_doc, path, index, pWnd);
	InitMultiText(xml_doc, path, index, pWnd);
	InitTexture(xml_doc, path, index, pWnd);
	InitTextureOffset(xml_doc, path, index, pWnd);

	s32 flag = xml_doc.ReadAttribInt(path, index, "heading", 0);
	pWnd->EnableHeading((flag) ? true : false);

	pcstr str_flag = xml_doc.ReadAttrib(path, index, "light_anim", "");
	s32 flag_cyclic = xml_doc.ReadAttribInt(path, index, "la_cyclic", 1);
	s32 flag_text = xml_doc.ReadAttribInt(path, index, "la_text", 1);
	s32 flag_texture = xml_doc.ReadAttribInt(path, index, "la_texture", 1);
	s32 flag_alpha = xml_doc.ReadAttribInt(path, index, "la_alpha", 0);

	pWnd->SetClrLightAnim(str_flag, (flag_cyclic) ? true : false, (flag_alpha) ? true : false, (flag_text) ? true : false, (flag_texture) ? true : false);

	str_flag = xml_doc.ReadAttrib(path, index, "xform_anim", "");
	flag_cyclic = xml_doc.ReadAttribInt(path, index, "xform_anim_cyclic", 1);

	pWnd->SetXformLightAnim(str_flag, (flag_cyclic) ? true : false);

	s32 flag_highlight_txt = xml_doc.ReadAttribInt(path, index, "highlight_text", 0);
	if (flag_highlight_txt)
	{
		pWnd->HighlightText(true);

		u32 hA = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "hA", 255));
		u32 hR = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "hR", 255));
		u32 hG = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "hG", 255));
		u32 hB = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "hB", 255));
		pWnd->SetHighlightColor(color_argb(hA, hR, hG, hB));
	}

	if (xml_doc.ReadAttribInt(path, index, "clipper", 0))
	{
		pWnd->ClipperOn( );
	}
	else
	{
		pWnd->ClipperOff( );
	}

	bool bComplexMode = xml_doc.ReadAttribInt(path, index, "complex_mode", 0) ? true : false;
	if (bComplexMode)
	{
		pWnd->SetTextComplexMode(bComplexMode);
	}

	return true;
}

bool CUIXmlInit::InitCheck(CUIXml& xml_doc, pcstr path, s32 index, CUICheckButton* pWnd)
{
	InitStatic(xml_doc, path, index, pWnd);
	InitOptionsItem(xml_doc, path, index, pWnd);

	return true;
}

bool CUIXmlInit::InitSpin(CUIXml& xml_doc, pcstr path, s32 index, CUICustomSpin* pWnd)
{
	InitWindow(xml_doc, path, index, pWnd);
	InitOptionsItem(xml_doc, path, index, pWnd);

	string256 foo;
	u32 color;
	strconcat(sizeof(foo), foo, path, ":text_color:e");
	if (xml_doc.NavigateToNode(foo, index))
	{
		color = GetColor(xml_doc, foo, index, 0x00);
		pWnd->SetTextColor(color);
	}

	strconcat(sizeof(foo), foo, path, ":text_color:d");
	if (xml_doc.NavigateToNode(foo, index))
	{
		color = GetColor(xml_doc, foo, index, 0x00);
		pWnd->SetTextColorD(color);
	}

	return true;
}

bool CUIXmlInit::InitText(CUIXml& xml_doc, pcstr path, s32 index, CUIStatic* pWnd)
{
	InitText(xml_doc, path, index, (IUITextControl*) pWnd);
	shared_str al = xml_doc.ReadAttrib(path, index, "vert_align", "");
	if (0 == xr_strcmp(al, "c"))
	{
		pWnd->SetVTextAlignment(valCenter);
	}
	else if (0 == xr_strcmp(al, "b"))
	{
		pWnd->SetVTextAlignment(valBotton);
	}
	else if (0 == xr_strcmp(al, "t"))
	{
		pWnd->SetVTextAlignment(valTop);
	}

	pWnd->SetTextComplexMode(xml_doc.ReadAttribInt(path, index, "complex_mode", 0) ? true : false);

	// Text coordinates
	f32 text_x = xml_doc.ReadAttribFlt(path, index, "x", 0);
	f32 text_y = xml_doc.ReadAttribFlt(path, index, "y", 0);
	pWnd->SetTextX(text_x);
	pWnd->SetTextY(text_y);

	return true;
}

bool CUIXmlInit::InitText(CUIXml& xml_doc, pcstr path, s32 index, IUITextControl* pWnd)
{
	u32 color;
	CGameFont* pTmpFont = NULL;
	InitFont(xml_doc, path, index, color, pTmpFont);
	pWnd->SetTextColor(color);
	if (pTmpFont)
	{
		pWnd->SetFont(pTmpFont);
	}

	// Load font alignment
	shared_str al = xml_doc.ReadAttrib(path, index, "align");
	if (0 == xr_strcmp(al, "c"))
	{
		pWnd->SetTextAlignment(CGameFont::alCenter);
	}
	else if (0 == xr_strcmp(al, "r"))
	{
		pWnd->SetTextAlignment(CGameFont::alRight);
	}
	else if (0 == xr_strcmp(al, "l"))
	{
		pWnd->SetTextAlignment(CGameFont::alLeft);
	}

	shared_str text = xml_doc.Read(path, index, NULL);
	CStringTable st;
	if (!!text)
	{
		pWnd->SetText(*st.translate(*text));
	}

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////
extern s32 keyname_to_dik(pcstr);

bool CUIXmlInit::Init3tButton(CUIXml& xml_doc, pcstr path, s32 index, CUI3tButton* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path, index), "XML node not found", path);

	InitWindow(xml_doc, path, index, pWnd);
	InitMultiText(xml_doc, path, index, pWnd);
	InitMultiTexture(xml_doc, path, index, pWnd);
	InitTextureOffset(xml_doc, path, index, pWnd);
	InitSound(xml_doc, path, index, pWnd);

	pcstr accel = xml_doc.ReadAttrib(path, index, "accel", NULL);
	if (accel)
	{
		s32 acc = keyname_to_dik(accel);
		pWnd->SetAccelerator(acc, 0);
	}

	accel = xml_doc.ReadAttrib(path, index, "accel_ext", NULL);
	if (accel)
	{
		s32 acc = keyname_to_dik(accel);
		pWnd->SetAccelerator(acc, 1);
	}

	f32 shadowOffsetX = xml_doc.ReadAttribFlt(path, index, "shadow_offset_x", 0);
	f32 shadowOffsetY = xml_doc.ReadAttribFlt(path, index, "shadow_offset_y", 0);

	pWnd->SetShadowOffset(fVector2( ).set(shadowOffsetX, shadowOffsetY));

	// init hint static
	string256 hint;
	strconcat(sizeof(hint), hint, path, ":hint");

	if (xml_doc.NavigateToNode(hint, index))
	{
		InitStatic(xml_doc, hint, index, &pWnd->m_hint);
	}

	s32 r = xml_doc.ReadAttribInt(path, index, "check_mode", -1);
	if (r != -1)
	{
		pWnd->SetCheckMode((r == 1) ? true : false);
	}

	pcstr text_hint = xml_doc.ReadAttrib(path, index, "hint", NULL);
	if (text_hint)
	{
		pWnd->m_hint_text = CStringTable( ).translate(text_hint);
	}

	return true;
}

bool CUIXmlInit::InitSound(CUIXml& xml_doc, pcstr path, s32 index, CUI3tButton* pWnd)
{
	string256 sound_h;
	string256 sound_t;
	strconcat(sizeof(sound_h), sound_h, path, ":sound_h");
	strconcat(sizeof(sound_t), sound_t, path, ":sound_t");

	shared_str sound_h_result = xml_doc.Read(sound_h, index, "");
	shared_str sound_t_result = xml_doc.Read(sound_t, index, "");

	if (xr_strlen(sound_h_result) != 0)
	{
		pWnd->InitSoundH(*sound_h_result);
	}

	if (xr_strlen(sound_t_result) != 0)
	{
		pWnd->InitSoundT(*sound_t_result);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitButton(CUIXml& xml_doc, pcstr path, s32 index, CUIButton* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path, index), "XML node not found", path);

	InitStatic(xml_doc, path, index, pWnd);

	pcstr accel = xml_doc.ReadAttrib(path, index, "accel", NULL);
	if (accel)
	{
		s32 acc = keyname_to_dik(accel);
		pWnd->SetAccelerator(acc, 0);
	}

	accel = xml_doc.ReadAttrib(path, index, "accel_ext", NULL);
	if (accel)
	{
		s32 acc = keyname_to_dik(accel);
		pWnd->SetAccelerator(acc, 1);
	}

	f32 shadowOffsetX = xml_doc.ReadAttribFlt(path, index, "shadow_offset_x", 0);
	f32 shadowOffsetY = xml_doc.ReadAttribFlt(path, index, "shadow_offset_y", 0);

	f32 pushOffsetX = xml_doc.ReadAttribFlt(path, index, "push_off_x", 2);
	f32 pushOffsetY = xml_doc.ReadAttribFlt(path, index, "push_off_y", 3);

	pcstr text_hint = xml_doc.ReadAttrib(path, index, "hint", NULL);
	if (text_hint)
	{
		pWnd->m_hint_text = CStringTable( ).translate(text_hint);
	}

	pWnd->SetShadowOffset(fVector2( ).set(shadowOffsetX, shadowOffsetY));
	pWnd->SetPushOffset(fVector2( ).set(pushOffsetX, pushOffsetY));

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CUIXmlInit::InitDragDropListEx(CUIXml& xml_doc, pcstr path, s32 index, CUIDragDropListEx* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path, index), "XML node not found", path);

	f32 x = xml_doc.ReadAttribFlt(path, index, "x");
	f32 y = xml_doc.ReadAttribFlt(path, index, "y");
	f32 width = xml_doc.ReadAttribFlt(path, index, "width");
	f32 height = xml_doc.ReadAttribFlt(path, index, "height");

	InitAlignment(xml_doc, path, index, x, y, pWnd);

	pWnd->Init(x, y, width, height);

	iVector2 w_cell_sz;
	iVector2 w_cells;

	w_cell_sz.x = xml_doc.ReadAttribInt(path, index, "cell_width");
	w_cell_sz.y = xml_doc.ReadAttribInt(path, index, "cell_height");
	w_cells.y = xml_doc.ReadAttribInt(path, index, "rows_num");
	w_cells.x = xml_doc.ReadAttribInt(path, index, "cols_num");
	pWnd->SetCellSize(w_cell_sz);
	pWnd->SetStartCellsCapacity(w_cells);

	s32 tmp = xml_doc.ReadAttribInt(path, index, "unlimited", 0);
	pWnd->SetAutoGrow(tmp != 0);
	tmp = xml_doc.ReadAttribInt(path, index, "group_similar", 0);
	pWnd->SetGrouping(tmp != 0);
	tmp = xml_doc.ReadAttribInt(path, index, "custom_placement", 1);
	pWnd->SetCustomPlacement(tmp != 0);

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitListWnd(CUIXml& xml_doc, pcstr path, s32 index, CUIListWnd* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path, index), "XML node not found", path);

	f32 x = xml_doc.ReadAttribFlt(path, index, "x");
	f32 y = xml_doc.ReadAttribFlt(path, index, "y");

	InitAlignment(xml_doc, path, index, x, y, pWnd);

	f32 width = xml_doc.ReadAttribFlt(path, index, "width");
	f32 height = xml_doc.ReadAttribFlt(path, index, "height");
	f32 item_height = xml_doc.ReadAttribFlt(path, index, "item_height");
	s32 active_background = xml_doc.ReadAttribInt(path, index, "active_bg");

	// Init font from xml config file
	string256 buf;
	CGameFont* LocalFont = NULL;
	u32 cl;

	shared_str text_path = strconcat(sizeof(buf), buf, path, ":font");
	InitFont(xml_doc, *text_path, index, cl, LocalFont);
	if (LocalFont)
	{
		pWnd->SetFont(LocalFont);
		pWnd->SetTextColor(cl);
	}

	pWnd->SetScrollBarProfile(xml_doc.ReadAttrib(path, index, "scroll_profile", "default"));
	pWnd->Init(x, y, width, height, item_height);
	pWnd->EnableActiveBackground(!!active_background);

	if (xml_doc.ReadAttribInt(path, index, "always_show_scroll"))
	{
		pWnd->SetAlwaysShowScroll(true);
		pWnd->EnableAlwaysShowScroll(true);
		pWnd->EnableScrollBar(true);
	}

	if (xml_doc.ReadAttribInt(path, index, "always_hide_scroll"))
	{
		pWnd->SetAlwaysShowScroll(false);
		pWnd->EnableAlwaysShowScroll(true);
	}

	bool bVertFlip = (1 == xml_doc.ReadAttribInt(path, index, "flip_vert", 0));
	pWnd->SetVertFlip(bVertFlip);

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitProgressBar(CUIXml& xml_doc, pcstr path, s32 index, CUIProgressBar* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path, index), "XML node not found", path);

	string256 buf;

	f32 x = xml_doc.ReadAttribFlt(path, index, "x");
	f32 y = xml_doc.ReadAttribFlt(path, index, "y");

	InitAlignment(xml_doc, path, index, x, y, pWnd);

	f32 width = xml_doc.ReadAttribFlt(path, index, "width");
	f32 height = xml_doc.ReadAttribFlt(path, index, "height");
	bool is_horizontal = (xml_doc.ReadAttribInt(path, index, "horz") == 1);

	pWnd->Init(x, y, width, height, is_horizontal);

	f32 min = xml_doc.ReadAttribFlt(path, index, "min");
	f32 max = xml_doc.ReadAttribFlt(path, index, "max");
	f32 pos = xml_doc.ReadAttribFlt(path, index, "pos");

	pWnd->SetRange(min, max);
	pWnd->SetProgressPos(pos);
	pWnd->m_inertion = xml_doc.ReadAttribFlt(path, index, "inertion", 0.0f);
	// progress
	strconcat(sizeof(buf), buf, path, ":progress");

	if (!xml_doc.NavigateToNode(buf, index))
	{
		return false;
	}

	InitStatic(xml_doc, buf, index, &pWnd->m_UIProgressItem);

	pWnd->m_UIProgressItem.SetWndSize(pWnd->GetWndSize( ));

	// background
	strconcat(sizeof(buf), buf, path, ":background");

	if (xml_doc.NavigateToNode(buf, index))
	{
		InitStatic(xml_doc, buf, index, &pWnd->m_UIBackgroundItem);
		pWnd->m_bBackgroundPresent = true;
		pWnd->m_UIBackgroundItem.SetWndSize(pWnd->GetWndSize( ));
	}

	strconcat(sizeof(buf), buf, path, ":min_color");

	if (xml_doc.NavigateToNode(buf, index))
	{
		pWnd->m_bUseColor = true;

		u32 color = GetColor(xml_doc, buf, index, 0xff);
		pWnd->m_minColor.set(color);

		strconcat(sizeof(buf), buf, path, ":max_color");

		color = GetColor(xml_doc, buf, index, 0xff);
		pWnd->m_maxColor.set(color);
	}

	return true;
}

bool CUIXmlInit::InitProgressShape(CUIXml& xml_doc, pcstr path, s32 index, CUIProgressShape* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path, index), "XML node not found", path);

	InitWindow(xml_doc, path, index, pWnd);

	if (xml_doc.ReadAttribInt(path, index, "text"))
	{
		pWnd->SetTextVisible(true);
	}

	string256 _path;
	if (xml_doc.NavigateToNode(strconcat(sizeof(_path), _path, path, ":back"), index))
	{
		InitStatic(xml_doc, _path, index, pWnd->m_pBackground);
	}

	InitStatic(xml_doc, strconcat(sizeof(_path), _path, path, ":front"), index, pWnd->m_pTexture);

	pWnd->m_sectorCount = xml_doc.ReadAttribInt(path, index, "sector_count", 8);
	pWnd->m_bClockwise = xml_doc.ReadAttribInt(path, index, "clockwise") ? true : false;

	return true;
}

CUIXmlInit::StaticsVec CUIXmlInit::InitAutoStaticGroup(CUIXml& xml_doc, pcstr path, s32 index, CUIWindow* pParentWnd)
{
	s32 items_num = xml_doc.GetNodesNum(path, index, "auto_static");

	StaticsVec tmpVec;
	XML_NODE* _stored_root = xml_doc.GetLocalRoot( );
	xml_doc.SetLocalRoot(xml_doc.NavigateToNode(path, index));

	CUIStatic* pUIStatic = NULL;
	string64 sname;
	for (s32 i = 0; i < items_num; i++)
	{
		pUIStatic = xr_new<CUIStatic>( );
		InitStatic(xml_doc, "auto_static", i, pUIStatic);
		sprintf_s(sname, "auto_static_%d", i);
		pUIStatic->SetWindowName(sname);
		pUIStatic->SetAutoDelete(true);
		pParentWnd->AttachChild(pUIStatic);
		tmpVec.push_back(pUIStatic);
		pUIStatic = NULL;
	}

	xml_doc.SetLocalRoot(_stored_root);
	return tmpVec;
}

CUIXmlInit::StaticsVec CUIXmlInit::InitAutoStatic(CUIXml& xml_doc, pcstr tag_name, CUIWindow* pParentWnd)
{
	s32 items_num = xml_doc.GetNodesNum(xml_doc.GetRoot( ), tag_name);
	// tmp statics vector
	StaticsVec	tmpVec;

	CUIStatic* pUIStatic = NULL;
	for (s32 i = 0; i < items_num; i++)
	{
		pUIStatic = xr_new<CUIStatic>( );
		InitStatic(xml_doc, tag_name, i, pUIStatic);
		pUIStatic->SetAutoDelete(true);
		pParentWnd->AttachChild(pUIStatic);
		tmpVec.push_back(pUIStatic);
		pUIStatic = NULL;
	}

	return tmpVec;
}

bool CUIXmlInit::InitFont(CUIXml& xml_doc, pcstr path, s32 index, u32& color, CGameFont*& pFnt)
{
	color = GetColor(xml_doc, path, index, 0xff);

	shared_str font_name = xml_doc.ReadAttrib(path, index, "font", NULL);
	if (*font_name == NULL || xr_strlen(*font_name) < 1)
	{
		pFnt = NULL;
		return false;
	}

	if (*font_name)
	{
		if (!xr_strcmp(*font_name, SMALL_FONT_NAME))
		{
			pFnt = UI( )->Font( )->pFontSmall;
		}
		else if (!xr_strcmp(*font_name, MEDIUM_FONT_NAME))
		{
			pFnt = UI( )->Font( )->pFontMedium;
		}
		else if (!xr_strcmp(*font_name, DI_FONT_NAME))
		{
			pFnt = UI( )->Font( )->pFontDI;
		}
		else if (!xr_strcmp(*font_name, ARIAL14_FONT_NAME))
		{
			pFnt = UI( )->Font( )->pFontArial14;
		}
		else if (!xr_strcmp(*font_name, ARIAL21_FONT_NAME))
		{
			pFnt = UI( )->Font( )->pFontArial21;
		}
		else if (!xr_strcmp(*font_name, GRAFFITI19_FONT_NAME))
		{
			pFnt = UI( )->Font( )->pFontGraffiti19Russian;
		}
		else if (!xr_strcmp(*font_name, GRAFFITI22_FONT_NAME))
		{
			pFnt = UI( )->Font( )->pFontGraffiti22Russian;
		}
		else if (!xr_strcmp(*font_name, GRAFFITI32_FONT_NAME))
		{
			pFnt = UI( )->Font( )->pFontGraffiti32Russian;
		}
		else if (!xr_strcmp(*font_name, GRAFFITI40_FONT_NAME))
		{
			pFnt = UI( )->Font( )->pFontGraffiti40Russian;
		}
		else if (!xr_strcmp(*font_name, GRAFFITI50_FONT_NAME))
		{
			pFnt = UI( )->Font( )->pFontGraffiti50Russian;
		}
		else if (!xr_strcmp(*font_name, LETTERICA16_FONT_NAME))
		{
			pFnt = UI( )->Font( )->pFontLetterica16Russian;
		}
		else if (!xr_strcmp(*font_name, LETTERICA18_FONT_NAME))
		{
			pFnt = UI( )->Font( )->pFontLetterica18Russian;
		}
		else if (!xr_strcmp(*font_name, LETTERICA25_FONT_NAME))
		{
			pFnt = UI( )->Font( )->pFontLetterica25Russian;
		}
		else
		{
			R_ASSERT3(0, "unknown font", *font_name);
			pFnt = NULL;
		}
	}

	return true;
}

bool CUIXmlInit::InitTabControl(CUIXml& xml_doc, pcstr path, s32 index, CUITabControl* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path, index), "XML node not found", path);

	bool status = true;

	status &= InitWindow(xml_doc, path, index, pWnd);
	InitOptionsItem(xml_doc, path, index, pWnd);
	s32 tabsCount = xml_doc.GetNodesNum(path, index, "button");
	s32 radio = xml_doc.ReadAttribInt(path, index, "radio");

	XML_NODE* tab_node = xml_doc.NavigateToNode(path, index);
	xml_doc.SetLocalRoot(tab_node);

	CUITabButton* newButton;

	for (s32 i = 0; i < tabsCount; ++i)
	{
		newButton = radio ? xr_new<CUIRadioButton>( ) : xr_new<CUITabButton>( );
		status &= Init3tButton(xml_doc, "button", i, newButton);
		pWnd->AddItem(newButton);
	}

	xml_doc.SetLocalRoot(xml_doc.GetRoot( ));

	return status;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitFrameLine(CUIXml& xml_doc, pcstr path, s32 index, CUIFrameLineWnd* pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path, index), "XML node not found");

	string256 buf;

	f32 x = xml_doc.ReadAttribFlt(path, index, "x");
	f32 y = xml_doc.ReadAttribFlt(path, index, "y");

	InitAlignment(xml_doc, path, index, x, y, pWnd);

	f32 width = xml_doc.ReadAttribFlt(path, index, "width");
	f32 height = xml_doc.ReadAttribFlt(path, index, "height");
	bool vertical = !!xml_doc.ReadAttribInt(path, index, "vertical");

	strconcat(sizeof(buf), buf, path, ":texture");
	shared_str base_name = xml_doc.Read(buf, index, NULL);

	VERIFY(base_name);

	u32 color = GetColor(xml_doc, buf, index, 0xff);
	pWnd->SetColor(color);

	pWnd->Init(*base_name, x, y, width, height, !vertical);

	strconcat(sizeof(buf), buf, path, ":title");
	if (xml_doc.NavigateToNode(buf, index))
	{
		InitStatic(xml_doc, buf, index, &pWnd->UITitleText);
	}

	return true;
}

bool CUIXmlInit::InitLabel(CUIXml& xml_doc, pcstr path, s32 index, CUILabel* pWnd)
{
	InitFrameLine(xml_doc, path, index, pWnd);

	string256 buf;
	strconcat(sizeof(buf), buf, path, ":text");
	InitText(xml_doc, buf, index, (IUITextControl*) pWnd);

	f32 text_x = xml_doc.ReadAttribFlt(buf, index, "x", 0);
	f32 text_y = xml_doc.ReadAttribFlt(buf, index, "y", 0);

	if (text_x)
	{
		pWnd->SetTextPosX(text_x);
	}

	if (text_y)
	{
		pWnd->SetTextPosY(text_y);
	}

	return true;
}

bool CUIXmlInit::InitCustomEdit(CUIXml& xml_doc, pcstr path, s32 index, CUICustomEdit* pWnd)
{
	InitWindow(xml_doc, path, index, pWnd);

	string256 foo;
	u32 color;
	InitText(xml_doc, strconcat(sizeof(foo), foo, path, ":text"), index, (IUITextControl*) pWnd);

	strconcat(sizeof(foo), foo, path, ":text_color:e");
	if (xml_doc.NavigateToNode(foo, index))
	{
		color = GetColor(xml_doc, foo, index, 0x00);
		pWnd->SetTextColor(color);
	}

	strconcat(sizeof(foo), foo, path, ":text_color:d");
	if (xml_doc.NavigateToNode(foo, index))
	{
		color = GetColor(xml_doc, foo, index, 0x00);
		pWnd->SetTextColorD(color);
	}

	strconcat(sizeof(foo), foo, path, ":text_color:cursor");
	if (xml_doc.NavigateToNode(foo, index))
	{
		color = GetColor(xml_doc, foo, index, 0x00);
		pWnd->SetCursorColor(color);
	}

	if (xml_doc.ReadAttribInt(path, index, "db_click", 0))
	{
		pWnd->SetDbClickMode( );
	}

	if (xml_doc.ReadAttribInt(path, index, "numonly", 0))
	{
		pWnd->SetNumbersOnly(true);
		if (xml_doc.ReadAttribInt(path, index, "float", 0))
		{
			pWnd->SetFloatNumbers(true);
		}
	}

	if (xml_doc.ReadAttribInt(path, index, "password", 0))
	{
		pWnd->SetPasswordMode( );
	}

	f32 text_x = xml_doc.ReadAttribFlt(foo, index, "x", 0);
	f32 text_y = xml_doc.ReadAttribFlt(foo, index, "y", 0);

	if (text_x)
	{
		pWnd->SetTextPosX(text_x);
	}

	if (text_y)
	{
		pWnd->SetTextPosY(text_y);
	}

	s32 cnt = xml_doc.ReadAttribInt(path, index, "max_symb_count", 0);
	if (cnt != 0)
	{
		pWnd->SetMaxCharCount(cnt);
	}

	return true;
}

bool CUIXmlInit::InitEditBoxEx(CUIXml& xml_doc, pcstr path, s32 index, CUIEditBoxEx* pWnd)
{
	InitCustomEdit(xml_doc, path, index, pWnd);
	InitTexture(xml_doc, path, index, pWnd);
	return true;
}

bool CUIXmlInit::InitEditBox(CUIXml& xml_doc, pcstr path, s32 index, CUIEditBox* pWnd)
{
	InitCustomEdit(xml_doc, path, index, pWnd);
	InitTexture(xml_doc, path, index, pWnd);
	InitOptionsItem(xml_doc, path, index, pWnd);
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitTextBanner(CUIXml& xml_doc, pcstr path, s32 index, CUITextBanner* pBnr)
{
	R_ASSERT3(xml_doc.NavigateToNode(path, index), "XML node not found", path);

	xr_map<shared_str, CUITextBanner::TextBannerStyles> conformityTable;
	conformityTable["none"] = CUITextBanner::tbsNone;
	conformityTable["fade"] = CUITextBanner::tbsFade;
	conformityTable["flicker"] = CUITextBanner::tbsFlicker;

	s32 animationsCount = xml_doc.GetNodesNum(path, index, "animation");

	XML_NODE* tab_node = xml_doc.NavigateToNode(path, index);
	XML_NODE* old_node = xml_doc.GetLocalRoot( );
	xml_doc.SetLocalRoot(tab_node);

	shared_str a;

	for (s32 i = 0; i < animationsCount; ++i)
	{
		a = xml_doc.ReadAttrib("animation", i, "anim", "none");
		EffectParams* param = pBnr->SetStyleParams(conformityTable[a]);

		param->bCyclic = !!xml_doc.ReadAttribInt("animation", i, "cyclic", 1);
		param->bOn = !!xml_doc.ReadAttribInt("animation", i, "on", 1);
		param->fPeriod = static_cast<f32>(atof(xml_doc.ReadAttrib("animation", i, "period", "1")));
		param->iEffectStage = xml_doc.ReadAttribInt("animation", i, "stage", 0);
	}

	xml_doc.SetLocalRoot(old_node);

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitMultiTextStatic(CUIXml& xml_doc, pcstr path, s32 index, CUIMultiTextStatic* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path, index), "XML node not found", path);

	bool status = true;
	string128 buf;

	status &= InitStatic(xml_doc, path, index, pWnd);
	s32 phrasesCount = xml_doc.GetNodesNum(path, index, "phrase");

	strconcat(sizeof(buf), buf, path, ":phrase");
	XML_NODE* tab_node = xml_doc.NavigateToNode(path, index);
	xml_doc.SetLocalRoot(tab_node);

	CUIMultiTextStatic::SinglePhrase* p;
	u32	argb = 0;
	pcstr ph = "phrase";

	for (s32 i = 0; i < phrasesCount; ++i)
	{
		p = pWnd->AddPhrase( );

		status &= InitTextBanner(xml_doc, ph, i, &p->effect);
		p->outX = (xml_doc.ReadAttribFlt(ph, i, "x", 0));
		p->outY = (xml_doc.ReadAttribFlt(ph, i, "y", 0));
		p->maxWidth = xml_doc.ReadAttribFlt(ph, i, "width", -1);

		CGameFont* pFont;
		InitFont(xml_doc, ph, i, argb, pFont);
		p->effect.SetFont(pFont);
		p->effect.SetTextColor(argb);

		CStringTable st;

		p->str = st.translate(xml_doc.Read(ph, i, ""));
	}

	xml_doc.SetLocalRoot(xml_doc.GetRoot( ));

	return status;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitAnimatedStatic(CUIXml& xml_doc, pcstr path, s32 index, CUIAnimatedStatic* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path, index), "XML node not found", path);

	InitStatic(xml_doc, path, index, pWnd);

	f32 x = xml_doc.ReadAttribFlt(path, index, "x_offset", 0);
	f32 y = xml_doc.ReadAttribFlt(path, index, "y_offset", 0);
	u32 framesCount = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "frames", 0));
	u32 animDuration = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "duration", 0));
	u32 animCols = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "columns", 0));
	u32 frameWidth = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "frame_width", 0));
	u32 frameHeight = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "frame_height", 0));
	bool cyclic = !!xml_doc.ReadAttribInt(path, index, "cyclic", 0);
	bool play = !!xml_doc.ReadAttribInt(path, index, "autoplay", 0);

	pWnd->SetFrameDimentions(frameWidth, frameHeight);
	pWnd->SetFramesCount(framesCount);
	pWnd->m_bCyclic = cyclic;
	pWnd->SetAnimCols(animCols);
	pWnd->SetAnimationDuration(animDuration);
	pWnd->SetOffset(x, y);
	pWnd->SetAnimPos(0.0f);
	if (play)
	{
		pWnd->Play( );
	}

	return true;
}

bool CUIXmlInit::InitTexture(CUIXml& xml_doc, pcstr path, s32 index, IUIMultiTextureOwner* pWnd)
{
	string256 buf;
	shared_str texture;

	strconcat(sizeof(buf), buf, path, ":texture");
	if (xml_doc.NavigateToNode(buf))
	{
		texture = xml_doc.Read(buf, index, NULL);
	}

	if (!!texture)
	{
		pWnd->InitTexture(*texture);
		return true;
	}

	return false;
}

bool CUIXmlInit::InitTexture(CUIXml& xml_doc, pcstr path, s32 index, IUISingleTextureOwner* pWnd)
{
	string256 buf;
	InitTexture(xml_doc, path, index, (IUIMultiTextureOwner*) pWnd);
	strconcat(sizeof(buf), buf, path, ":texture");

	fRect rect;

	rect.x1 = xml_doc.ReadAttribFlt(buf, index, "x", 0.0f);
	rect.y1 = xml_doc.ReadAttribFlt(buf, index, "y", 0.0f);
	rect.x2 = rect.x1 + xml_doc.ReadAttribFlt(buf, index, "width", 0.0f);
	rect.y2 = rect.y1 + xml_doc.ReadAttribFlt(buf, index, "height", 0.0f);

	bool stretch_flag = xml_doc.ReadAttribInt(path, index, "stretch") ? true : false;
	pWnd->SetStretchTexture(stretch_flag);

	u32 color = GetColor(xml_doc, buf, index, 0xff);
	pWnd->SetTextureColor(color);

	if (rect.width( ) != 0 && rect.height( ) != 0)
	{
		pWnd->SetOriginalRect(rect);
	}

	return true;
}

bool CUIXmlInit::InitTextureOffset(CUIXml& xml_doc, pcstr path, s32 index, CUIStatic* pWnd)
{
	string256 textureOffset;
	if (0 == xr_strcmp(path, ""))
	{
		strcpy(textureOffset, "texture_offset");
	}
	else
	{
		strconcat(sizeof(textureOffset), textureOffset, path, ":texture_offset");
	}

	f32 x = xml_doc.ReadAttribFlt(textureOffset, index, "x");
	f32 y = xml_doc.ReadAttribFlt(textureOffset, index, "y");

	pWnd->SetTextureOffset(x, y);

	return true;
}

bool CUIXmlInit::InitMultiTexture(CUIXml& xml_doc, pcstr path, s32 index, CUI3tButton* pWnd)
{
	string256 buff;
	bool success = false;

	strconcat(sizeof(buff), buff, path, ":texture");
	shared_str texture = xml_doc.Read(buff, index, NULL);

	if (texture.size( ) > 0)
	{
		pWnd->InitTexture(*texture);
		return true;
	}

	strconcat(sizeof(buff), buff, path, ":texture_e");
	texture = xml_doc.Read(buff, index, NULL);
	if (texture.size( ))
	{
		pWnd->m_background.CreateE( )->InitTexture(*texture);
		success = true;
	}

	strconcat(sizeof(buff), buff, path, ":texture_t");
	texture = xml_doc.Read(buff, index, NULL);
	if (texture.size( ))
	{
		pWnd->m_background.CreateT( )->InitTexture(*texture);
		success = true;
	}

	strconcat(sizeof(buff), buff, path, ":texture_d");
	texture = xml_doc.Read(buff, index, NULL);
	if (texture.size( ))
	{
		pWnd->m_background.CreateD( )->InitTexture(*texture);
		success = true;
	}

	strconcat(sizeof(buff), buff, path, ":texture_h");
	texture = xml_doc.Read(buff, index, NULL);
	if (texture.size( ))
	{
		pWnd->m_background.CreateH( )->InitTexture(*texture);
		success = true;
	}

	if (success)
	{
		pWnd->TextureOn( );
	}

	return success;
}

bool CUIXmlInit::InitMultiText(CUIXml& xml_doc, pcstr path, s32 index, CUIStatic* pWnd)
{
	string256 buf;
	InitText(xml_doc, strconcat(sizeof(buf), buf, path, ":text"), index, pWnd);
	u32 color;

	strconcat(sizeof(buf), buf, path, ":text_color:e");
	if (xml_doc.NavigateToNode(buf, index))
	{
		color = GetColor(xml_doc, buf, index, 0x00);
		pWnd->SetTextColor(color, CUIStatic::E);
	}

	strconcat(sizeof(buf), buf, path, ":text_color:d");
	if (xml_doc.NavigateToNode(buf, index))
	{
		color = GetColor(xml_doc, buf, index, 0x00);
		pWnd->SetTextColor(color, CUIStatic::D);
	}

	strconcat(sizeof(buf), buf, path, ":text_color:t");
	if (xml_doc.NavigateToNode(buf, index))
	{
		color = GetColor(xml_doc, buf, index, 0x00);
		pWnd->SetTextColor(color, CUIStatic::T);
	}

	strconcat(sizeof(buf), buf, path, ":text_color:h");
	if (xml_doc.NavigateToNode(buf, index))
	{
		color = GetColor(xml_doc, buf, index, 0x00);
		pWnd->SetTextColor(color, CUIStatic::H);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

f32 CUIXmlInit::ApplyAlignX(f32 coord, u32 align)
{
	return coord;
}

//////////////////////////////////////////////////////////////////////////

f32 CUIXmlInit::ApplyAlignY(f32 coord, u32 align)
{
	return coord;
}

//////////////////////////////////////////////////////////////////////////

void CUIXmlInit::ApplyAlign(f32& x, f32& y, u32 align)
{
	x = ApplyAlignX(x, align);
	y = ApplyAlignY(y, align);
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitAlignment(CUIXml& xml_doc, pcstr path, s32 index, f32& x, f32& y, CUIWindow* pWnd)
{
	xr_string wnd_alignment = xml_doc.ReadAttrib(path, index, "alignment", "");

	if (strchr(wnd_alignment.c_str( ), 'c'))
	{
		pWnd->SetAlignment(waCenter);
	}

	// Alignment: right: "r", bottom: "b". Top, left - useless
	shared_str	alignStr = xml_doc.ReadAttrib(path, index, "align", "");

	bool result = false;

	if (strchr(*alignStr, 'r'))
	{
		x = ApplyAlignX(x, alRight);
		result = true;
	}

	if (strchr(*alignStr, 'b'))
	{
		y = ApplyAlignY(y, alBottom);
		result = true;
	}

	if (strchr(*alignStr, 'c'))
	{
		ApplyAlign(x, y, alCenter);
		result = true;
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////

void CUIXmlInit::InitColorDefs( )
{
	if (NULL != m_pColorDefs)
	{
		return;
	}

	m_pColorDefs = xr_new<ColorDefs>( );

	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, COLOR_DEFINITIONS);
	R_ASSERT3(xml_result, "xml file not found", COLOR_DEFINITIONS);

	s32 num = uiXml.GetNodesNum("colors", 0, "color");

	shared_str name;
	s32 r;
	s32 b;
	s32 g;
	s32 a;

	for (s32 i = 0; i < num; ++i)
	{
		name = uiXml.ReadAttrib("color", i, "name", "");
		r = uiXml.ReadAttribInt("color", i, "r", 0);
		g = uiXml.ReadAttribInt("color", i, "g", 0);
		b = uiXml.ReadAttribInt("color", i, "b", 0);
		a = uiXml.ReadAttribInt("color", i, "a", 255);

		(*m_pColorDefs)[name] = color_argb(a, r, g, b);
	}
}

bool CUIXmlInit::InitScrollView(CUIXml& xml_doc, pcstr path, s32 index, CUIScrollView* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path, index), "XML node not found", path);

	InitWindow(xml_doc, path, index, pWnd);
	pWnd->SetRightIndention(xml_doc.ReadAttribFlt(path, index, "right_ident", 0.0f));
	pWnd->SetLeftIndention(xml_doc.ReadAttribFlt(path, index, "left_ident", 0.0f));
	pWnd->SetUpIndention(xml_doc.ReadAttribFlt(path, index, "top_indent", 0.0f));
	pWnd->SetDownIndention(xml_doc.ReadAttribFlt(path, index, "bottom_indent", 0.0f));

	f32 vi = xml_doc.ReadAttribFlt(path, index, "vert_interval", 0.0f);
	pWnd->m_vertInterval = (vi);

	bool bInverseDir = (1 == xml_doc.ReadAttribInt(path, index, "inverse_dir", 0));
	pWnd->m_flags.set(CUIScrollView::eInverseDir, bInverseDir);

	pWnd->SetScrollBarProfile(xml_doc.ReadAttrib(path, index, "scroll_profile", "default"));

	pWnd->Init( );

	bool bVertFlip = (1 == xml_doc.ReadAttribInt(path, index, "flip_vert", 0));
	pWnd->SetVertFlip(bVertFlip);

	bool b = (1 == xml_doc.ReadAttribInt(path, index, "always_show_scroll", 1));

	pWnd->SetFixedScrollBar(b);

	b = (1 == xml_doc.ReadAttribInt(path, index, "can_select", 0));

	pWnd->m_flags.set(CUIScrollView::eItemsSelectabe, b);

	/////////////////////////////////////////////////////////////////////
	s32 tabsCount = xml_doc.GetNodesNum(path, index, "text");

	XML_NODE* _stored_root = xml_doc.GetLocalRoot( );
	xml_doc.SetLocalRoot(xml_doc.NavigateToNode(path, index));

	CUIStatic* newStatic;

	for (s32 i = 0; i < tabsCount; ++i)
	{
		newStatic = xr_new<CUIStatic>( );
		InitText(xml_doc, "text", i, newStatic);
		newStatic->SetTextComplexMode(true);
		newStatic->SetWidth(pWnd->GetDesiredChildWidth( ));
		newStatic->AdjustHeightToText( );
		pWnd->AddWindow(newStatic, true);
	}

	xml_doc.SetLocalRoot(_stored_root);
	return true;
}

bool CUIXmlInit::InitListBox(CUIXml& xml_doc, pcstr path, s32 index, CUIListBox* pWnd)
{
	InitScrollView(xml_doc, path, index, pWnd);

	string512 _path;

	u32 t_color;
	CGameFont* pFnt;
	strconcat(sizeof(_path), _path, path, ":font");
	InitFont(xml_doc, _path, index, t_color, pFnt);

	pWnd->SetTextColor(t_color);
	pWnd->SetFont(pFnt);

	strconcat(sizeof(_path), _path, path, ":font_s");
	t_color = GetColor(xml_doc, _path, index, 0x00);
	pWnd->SetTextColorS(t_color);

	// Load font alignment
	shared_str al = xml_doc.ReadAttrib(_path, index, "align");
	if (0 == xr_strcmp(al, "c"))
	{
		pWnd->SetTextAlignment(CGameFont::alCenter);
	}
	else if (0 == xr_strcmp(al, "r"))
	{
		pWnd->SetTextAlignment(CGameFont::alRight);
	}
	else if (0 == xr_strcmp(al, "l"))
	{
		pWnd->SetTextAlignment(CGameFont::alLeft);
	}

	return true;
}

bool CUIXmlInit::InitTrackBar(CUIXml& xml_doc, pcstr path, s32 index, CUITrackBar* pWnd)
{
	InitWindow(xml_doc, path, 0, pWnd);

	s32 is_integer = xml_doc.ReadAttribInt(path, index, "is_integer", 0);
	pWnd->SetType(!is_integer);
	InitOptionsItem(xml_doc, path, 0, pWnd);

	s32 invert = xml_doc.ReadAttribInt(path, index, "invert", 0);
	pWnd->SetInvert(!!invert);
	f32 step = xml_doc.ReadAttribFlt(path, index, "step", 0.1f);
	pWnd->SetStep(step);

	return true;
}

bool CUIXmlInit::InitComboBox(CUIXml& xml_doc, pcstr path, s32 index, CUIComboBox* pWnd)
{
	u32 color;
	CGameFont* pFont;

	pWnd->SetListLength(xml_doc.ReadAttribInt(path, index, "list_length", 4));

	InitWindow(xml_doc, path, index, pWnd);
	InitOptionsItem(xml_doc, path, index, pWnd);

	bool b = (1 == xml_doc.ReadAttribInt(path, index, "always_show_scroll", 1));

	pWnd->m_list.SetFixedScrollBar(b);

	string512 _path;
	strconcat(sizeof(_path), _path, path, ":list_font");
	InitFont(xml_doc, _path, index, color, pFont);
	pWnd->SetFont(pFont);
	pWnd->m_list.SetFont(pFont);
	pWnd->m_list.SetTextColor(color);
	strconcat(sizeof(_path), _path, path, ":list_font_s");
	InitFont(xml_doc, _path, index, color, pFont);
	pWnd->m_list.SetTextColorS(color);

	strconcat(sizeof(_path), _path, path, ":text_color:e");
	if (xml_doc.NavigateToNode(_path, index))
	{
		color = GetColor(xml_doc, _path, index, 0x00);
		pWnd->SetTextColor(color);
	}

	strconcat(sizeof(_path), _path, path, ":text_color:d");
	if (xml_doc.NavigateToNode(_path, index))
	{
		color = GetColor(xml_doc, _path, index, 0x00);
		pWnd->SetTextColorD(color);
	}

	return true;
}

u32 CUIXmlInit::GetColor(CUIXml& xml_doc, pcstr path, s32 index, u32 def_clr)
{
	pcstr clr_def = xml_doc.ReadAttrib(path, index, "color", NULL);
	if (clr_def)
	{
		VERIFY(GetColorDefs( )->find(clr_def) != GetColorDefs( )->end( ));
		return 	(*m_pColorDefs)[clr_def];
	}
	else
	{
		s32 r = xml_doc.ReadAttribInt(path, index, "r", def_clr);
		s32 g = xml_doc.ReadAttribInt(path, index, "g", def_clr);
		s32 b = xml_doc.ReadAttribInt(path, index, "b", def_clr);
		s32 a = xml_doc.ReadAttribInt(path, index, "a", 0xff);
		return color_argb(a, r, g, b);
	}
}
