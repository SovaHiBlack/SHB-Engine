#include "stdafx.h"
#include "UIActorInfo.h"
#include "UIXmlInit.h"
#include "UIPdaAux.h"//
#include "../Level.h"
#include "../Actor.h"

#include "UIInventoryUtilities.h"//
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIAnimatedStatic.h"
#include "UIScrollView.h"
#include "UICharacterInfo.h"//
#include "UI3tButton.h"
#include "../ActorStatisticManager.h"
#include "../CharacterCommunity.h"
#include "../CharacterReputation.h"
#include "../RelationRegistry.h"
#include "../StringTable.h"

#define ACTOR_STATISTIC_XML							"actor_statistic.xml"
#define ACTOR_CHARACTER_XML							"pda_dialog_character.xml"

CUIActorInfoWnd::CUIActorInfoWnd( )
{ }

void CUIActorInfoWnd::Init( )
{
	CUIXml											uiXml;
	CUIXmlInit										xml_init;
	uiXml.Init										(CONFIG_PATH, UI_PATH, ACTOR_STATISTIC_XML);

	xml_init.InitWindow								(uiXml, "main_wnd", 0, this);

	// ������������ ����������
	UICharIconFrame									= xr_new<CUIFrameWindow>( );
	UICharIconFrame->SetAutoDelete					(true);
	xml_init.InitFrameWindow						(uiXml, "chicon_frame_window", 0, UICharIconFrame);
	AttachChild										(UICharIconFrame);

	UICharIconHeader								= xr_new<CUIFrameLineWnd>( );
	UICharIconHeader->SetAutoDelete					(true);
	xml_init.InitFrameLine							(uiXml, "chicon_frame_line", 0, UICharIconHeader);
	UICharIconFrame->AttachChild					(UICharIconHeader);

	UIAnimatedIcon									= xr_new<CUIAnimatedStatic>( );
	UIAnimatedIcon->SetAutoDelete					(true);
	xml_init.InitAnimatedStatic						(uiXml, "a_static", 0, UIAnimatedIcon);
	UICharIconHeader->AttachChild					(UIAnimatedIcon);

	UIInfoFrame										= xr_new<CUIFrameWindow>( );
	UIInfoFrame->SetAutoDelete						(true);
	xml_init.InitFrameWindow						(uiXml, "info_frame_window", 0, UIInfoFrame);
	AttachChild										(UIInfoFrame);

	UIInfoHeader									= xr_new<CUIFrameLineWnd>( );
	UIInfoHeader->SetAutoDelete						(true);
	xml_init.InitFrameLine							(uiXml, "info_frame_line", 0, UIInfoHeader);
	UIInfoFrame->AttachChild						(UIInfoHeader);

	UIDetailList									= xr_new<CUIScrollView>( );
	UIDetailList->SetAutoDelete						(true);
	UIInfoFrame->AttachChild						(UIDetailList);
	xml_init.InitScrollView							(uiXml, "detail_list", 0, UIDetailList);

	UIMasterList									= xr_new<CUIScrollView>( );
	UIMasterList->SetAutoDelete						(true);
	UICharIconFrame->AttachChild					(UIMasterList);
	xml_init.InitScrollView							(uiXml, "master_list", 0, UIMasterList);

	UICharacterWindow								= xr_new<CUIWindow>( );
	UICharacterWindow->SetAutoDelete				(true);
	UICharIconFrame->AttachChild					(UICharacterWindow);
	xml_init.InitWindow								(uiXml, "character_info", 0, UICharacterWindow);

	UICharacterInfo									= xr_new<CUICharacterInfo>( );
	UICharacterInfo->SetAutoDelete					(true);
	UICharacterWindow->AttachChild					(UICharacterInfo);
	UICharacterInfo->Init							(0.0f, 0.0f, UICharacterWindow->GetWidth( ), UICharacterWindow->GetHeight( ), ACTOR_CHARACTER_XML);

	//�������� ��������������� ����������
	xml_init.InitAutoStatic							(uiXml, "right_auto_static", UICharIconFrame);
	xml_init.InitAutoStatic							(uiXml, "left_auto_static", UIInfoFrame);
}

void CUIActorInfoWnd::Show(bool status)
{
	inherited::Show									(status);
	if (!status)
	{
		return;
	}

	UICharacterInfo->InitCharacter					(Actor( )->ID( ));
	UICharIconHeader->UITitleText.SetText			(Actor( )->Name( ));
	FillPointsInfo									( );
}

void CUIActorInfoWnd::FillPointsInfo( )
{
	CUIXml											uiXml;
	uiXml.Init										(CONFIG_PATH, UI_PATH, ACTOR_STATISTIC_XML);

	UIMasterList->Clear								( );

	s32 items_num									= uiXml.GetNodesNum("actor_stats_wnd", 0, "master_part");
	uiXml.SetLocalRoot								(uiXml.NavigateToNode("actor_stats_wnd", 0));
	string64										buff;

	for (s32 i = 0; i < items_num; ++i)
	{
		CUIActorStaticticHeader* itm				= xr_new<CUIActorStaticticHeader>(this);
		itm->Init									(&uiXml, "master_part", i);

		if (itm->m_id != "foo")
		{
			if (itm->m_id == "reputation")
			{
				itm->m_text2->SetTextST				(InventoryUtilities::GetReputationAsText(Actor( )->Reputation( )));
				itm->m_text2->SetTextColor			(InventoryUtilities::GetReputationColor(Actor( )->Reputation( )));
			}
			else
			{
				s32 _totl							= Actor( )->StatisticMgr( ).GetSectionPoints(itm->m_id);
				if (_totl == -1)
				{
					itm->m_text2->SetTextST			("");
				}
				else
				{
					sprintf_s						(buff, "%d", _totl);
					itm->m_text2->SetTextST			(buff);
				}
			}
		}

		UIMasterList->AddWindow						(itm, true);
	}

	UIMasterList->SetSelected						(UIMasterList->GetItem(1));
}

void CUIActorInfoWnd::FillMasterPart(CUIXml* xml, const shared_str& key_name)
{
	CUIActorStaticticHeader* itm					= xr_new<CUIActorStaticticHeader>(this);
	string128										buff;
	strconcat										(sizeof(buff), buff, "actor_stats_wnd:master_part_", key_name.c_str( ));
	itm->Init										(xml, buff, 0);

	if (key_name != "foo")
	{
		if (key_name == "reputation")
		{
			itm->m_text2->SetTextST					(InventoryUtilities::GetReputationAsText(Actor( )->Reputation( )));
			itm->m_text2->SetTextColor				(InventoryUtilities::GetReputationColor(Actor( )->Reputation( )));
		}
		else
		{
			s32 _totl								= Actor( )->StatisticMgr( ).GetSectionPoints(key_name);
			if (_totl == -1)
			{
				itm->m_text2->SetTextST				("");
			}
			else
			{
				sprintf_s							(buff, "%d", _totl);
				itm->m_text2->SetTextST				(buff);
			}
		}
	}

	UIMasterList->AddWindow							(itm, true);
}

void CUIActorInfoWnd::FillPointsDetail(const shared_str& id)
{
	UIDetailList->Clear								( );
	CUIXml											uiXml;
	uiXml.Init										(CONFIG_PATH, UI_PATH, ACTOR_STATISTIC_XML);
	uiXml.SetLocalRoot								(uiXml.NavigateToNode("actor_stats_wnd", 0));

	string512										path;
	sprintf_s										(path, "detail_part_%s", id.c_str( ));

	XML_NODE* n										= uiXml.NavigateToNode(path, 0);
	if (!n)
	{
		sprintf_s									(path, "detail_part_def");
	}

	if (id == "reputation")
	{
		UIInfoHeader->GetTitleStatic( )->SetTextST	("st_detail_list_for_community_relations");
		FillReputationDetails						(&uiXml, path);
		return;
	}

	string256										str;
	sprintf_s										(str, "st_detail_list_for_%s", id.c_str( ));
	UIInfoHeader->GetTitleStatic( )->SetTextST		(str);

	SStatSectionData& section						= Actor( )->StatisticMgr( ).GetSection(id);
	vStatDetailData::const_iterator it				= section.data.begin( );
	vStatDetailData::const_iterator it_e			= section.data.end( );

	s32 _cntr										= 0;
	string64										buff;
	for (; it != it_e; ++it, ++_cntr)
	{
		CUIActorStaticticDetail* itm				= xr_new<CUIActorStaticticDetail>( );
		itm->Init									(&uiXml, path, 0);

		sprintf_s									(buff, "%d.", _cntr);
		itm->m_text0->SetText						(buff);

		itm->m_text1->SetTextST						(*CStringTable( ).translate((*it).key));
		itm->m_text1->AdjustHeightToText			( );

		if (0 == (*it).str_value.size( ))
		{
			sprintf_s								(buff, "x%d", (*it).int_count);
			itm->m_text2->SetTextST					(buff);

			sprintf_s								(buff, "%d", (*it).int_points);
			itm->m_text3->SetTextST					(buff);
		}
		else
		{
			itm->m_text2->SetTextST					((*it).str_value.c_str( ));
			itm->m_text3->SetTextST					("");
		}

		fVector2 sz									= itm->GetWndSize( );
		f32											_height;
		_height										= _max(sz.y, itm->m_text1->GetWndPos( ).y + itm->m_text1->GetWndSize( ).y + 3);
		sz.y										= _height;
		itm->SetWndSize								(sz);
		UIDetailList->AddWindow						(itm, true);
	}
}

void CUIActorInfoWnd::Reset( )
{
	inherited::Reset								( );
}

void CUIActorInfoWnd::FillReputationDetails(CUIXml* xml, pcstr path)
{
	XML_NODE* _list_node							= xml->NavigateToNode("relation_communities_list", 0);
	s32 cnt											= xml->GetNodesNum("relation_communities_list", 0, "r");

	CCharacterCommunity								comm;

	CCharacterReputation							rep_actor;
	CCharacterReputation							rep_neutral;
	rep_actor.set									(Actor( )->Reputation( ));
	rep_neutral.set									(NEUTAL_REPUTATION);

	CHARACTER_GOODWILL d_neutral					= CCharacterReputation::relation(rep_actor.index( ), rep_neutral.index( ));

	string64										buff;
	for (s32 i = 0; i < cnt; ++i)
	{
		CUIActorStaticticDetail* itm				= xr_new<CUIActorStaticticDetail>( );
		itm->Init									(xml, path, 0);
		comm.set									(xml->Read(_list_node, "r", i, "unknown_community"));
		itm->m_text1->SetTextST						(*(comm.id( )));

		CHARACTER_GOODWILL gw						= SRelationRegistry( ).GetCommunityGoodwill(comm.index( ), Actor( )->ID( ));
		gw											+= CCharacterCommunity::relation(Actor( )->Community( ), comm.index( ));
		gw											+= d_neutral;

		itm->m_text2->SetTextST						(InventoryUtilities::GetGoodwillAsText(gw));
		itm->m_text2->SetTextColor					(InventoryUtilities::GetGoodwillColor(gw));

		sprintf_s									(buff, "%d", gw);
		itm->m_text3->SetTextST						(buff);

		UIDetailList->AddWindow						(itm, true);
	}
}

CUIActorStaticticHeader::CUIActorStaticticHeader(CUIActorInfoWnd* w) :m_actorInfoWnd(w)
{ }

void CUIActorStaticticHeader::Init(CUIXml* xml, pcstr path, s32 idx_in_xml)
{
	XML_NODE* _stored_root							= xml->GetLocalRoot( );

	CUIXmlInit										xml_init;
	xml_init.InitWindow								(*xml, path, idx_in_xml, this);

	xml->SetLocalRoot								(xml->NavigateToNode(path, idx_in_xml));

	m_text1											= xr_new<CUIStatic>( );
	m_text1->SetAutoDelete							(true);
	AttachChild										(m_text1);
	xml_init.InitStatic								(*xml, "text_1", 0, m_text1);

	m_text2											= xr_new<CUIStatic>( );
	m_text2->SetAutoDelete							(true);
	AttachChild										(m_text2);
	xml_init.InitStatic								(*xml, "text_2", 0, m_text2);

	xml_init.InitAutoStaticGroup					(*xml, "auto", 0, this);

	m_id											= xml->ReadAttrib(xml->GetLocalRoot( ), "id", NULL);

	m_stored_alpha									= color_get_A(m_text1->GetTextColor( ));
	xml->SetLocalRoot								(_stored_root);
}

bool CUIActorStaticticHeader::OnMouseDown(s32 mouse_btn)
{
	if (mouse_btn == MOUSE_1 && m_id != "total")
	{
		m_actorInfoWnd->MasterList( ).SetSelected	(this);
		return true;
	}
	else
	{
		return true;
	}
}

void CUIActorStaticticHeader::SetSelected(bool b)
{
	CUISelectable::SetSelected						(b);
	m_text1->SetTextColor							(subst_alpha(m_text1->GetTextColor( ), b ? 255 : m_stored_alpha));
	if (b)
	{
		m_actorInfoWnd->FillPointsDetail			(m_id);
	}
}

void CUIActorStaticticDetail::Init(CUIXml* xml, pcstr path, s32 idx)
{
	XML_NODE* _stored_root							= xml->GetLocalRoot( );

	CUIXmlInit										xml_init;
	xml_init.InitWindow								(*xml, path, idx, this);

	xml->SetLocalRoot								(xml->NavigateToNode(path, idx));

	m_text0											= xr_new<CUIStatic>( );
	m_text0->SetAutoDelete							(true);
	AttachChild										(m_text0);
	xml_init.InitStatic								(*xml, "text_0", 0, m_text0);

	m_text1											= xr_new<CUIStatic>( );
	m_text1->SetAutoDelete							(true);
	AttachChild										(m_text1);
	xml_init.InitStatic								(*xml, "text_1", 0, m_text1);

	m_text2											= xr_new<CUIStatic>( );
	m_text2->SetAutoDelete							(true);
	AttachChild										(m_text2);
	xml_init.InitStatic								(*xml, "text_2", 0, m_text2);

	m_text3											= xr_new<CUIStatic>( );
	m_text3->SetAutoDelete							(true);
	AttachChild										(m_text3);
	xml_init.InitStatic								(*xml, "text_3", 0, m_text3);

	xml_init.InitAutoStaticGroup					(*xml, "auto", 0, this);

	xml->SetLocalRoot								(_stored_root);
}
