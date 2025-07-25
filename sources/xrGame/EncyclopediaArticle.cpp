///////////////////////////////////////////////////////////////
// encyclopedia_article.cpp
// ���������, �������� � ����������� ������ � ������������
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EncyclopediaArticle.h"
//#include "ui/UIXml.h"
#include "ui/UIXmlInit.h"
#include "ui/UIInventoryUtilities.h"
#include "object_broker.h"

using namespace InventoryUtilities;

void ARTICLE_DATA::load(IReader& stream)
{
	load_data(receive_time, stream);
	load_data(article_id, stream);
	load_data(readed, stream);
	load_data(article_type, stream);
}

void ARTICLE_DATA::save(IWriter& stream)
{
	save_data(receive_time, stream);
	save_data(article_id, stream);
	save_data(readed, stream);
	save_data(article_type, stream);
}

CEncyclopediaArticle::CEncyclopediaArticle( )
{ }

CEncyclopediaArticle::~CEncyclopediaArticle( )
{
	if (data( )->image.GetParent( ))
	{
		data( )->image.GetParent( )->DetachChild(&(data( )->image));
	}
}

void CEncyclopediaArticle::Load(shared_str  id)
{
	m_ArticleId = id;
	inherited_shared::load_shared(m_ArticleId, NULL);
}

void CEncyclopediaArticle::load_shared(pcstr)
{
	const ITEM_DATA& item_data = *id_to_index::GetById(m_ArticleId);

	CUIXml* pXML = item_data._xml;
	pXML->SetLocalRoot(pXML->GetRoot( ));

	//�������� �� XML
	XML_NODE* pNode = pXML->NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
	THROW3(pNode, "encyclopedia article id=", *item_data.id);

	//�����
	data( )->text = pXML->Read(pNode, "text", 0, "");
	//���
	data( )->name = pXML->ReadAttrib(pNode, "name", "");
	//������
	data( )->group = pXML->ReadAttrib(pNode, "group", "");
	//������ ltx, ������ ������ ������
	pcstr ltx = pXML->Read(pNode, "ltx", 0, NULL);

	if (ltx)
	{
		data( )->image.SetShader(InventoryUtilities::GetEquipmentIconsShader( ));

		f32 x = f32(pSettings->r_u32(ltx, "inv_grid_x") * INV_GRID_WIDTH);
		f32 y = f32(pSettings->r_u32(ltx, "inv_grid_y") * INV_GRID_HEIGHT);
		f32 width = f32(pSettings->r_u32(ltx, "inv_grid_width") * INV_GRID_WIDTH);
		f32 height = f32(pSettings->r_u32(ltx, "inv_grid_height") * INV_GRID_HEIGHT);

		data( )->image.GetUIStaticItem( ).SetOriginalRect(x, y, width, height);
		data( )->image.ClipperOn( );
		data( )->image.TextureAvailable(true);
	}
	else
	{
		if (pXML->NavigateToNode(pNode, "texture", 0))
		{
			pXML->SetLocalRoot(pNode);
			CUIXmlInit::InitTexture(*pXML, "", 0, &data( )->image);
			pXML->SetLocalRoot(pXML->GetRoot( ));
		}
	}

	if (data( )->image.TextureAvailable( ))
	{
		fRect r = data( )->image.GetUIStaticItem( ).GetOriginalRect( );
		data( )->image.SetAutoDelete(false);

		const s32 minSize = 65;

		// ������� ������������� ���� ���� ���������� ���������� ������� ������
		if (r.width( ) < minSize)
		{
			f32 dx = minSize - r.width( );
			r.x2 += dx;
			data( )->image.SetTextureOffset(dx / 2, data( )->image.GetTextureOffeset( )[1]);
		}

		if (r.height( ) < minSize)
		{
			f32 dy = minSize - r.height( );
			r.y2 += dy;
			data( )->image.SetTextureOffset(data( )->image.GetTextureOffeset( )[0], dy / 2);
		}

		data( )->image.SetWndRect(0, 0, r.width( ), r.height( ));
	}

	//��� ������
	xr_string atricle_type = pXML->ReadAttrib(pNode, "article_type", "encyclopedia");
	if (0 == stricmp(atricle_type.c_str( ), "encyclopedia"))
	{
		data( )->articleType = ARTICLE_DATA::eEncyclopediaArticle;
	}
	else if (0 == stricmp(atricle_type.c_str( ), "journal"))
	{
		data( )->articleType = ARTICLE_DATA::eJournalArticle;
	}
	else if (0 == stricmp(atricle_type.c_str( ), "task"))
	{
		data( )->articleType = ARTICLE_DATA::eTaskArticle;
	}
	else if (0 == stricmp(atricle_type.c_str( ), "info"))
	{
		data( )->articleType = ARTICLE_DATA::eInfoArticle;
	}
	else
	{
		Msg("incorrect article type definition for [%s]", *item_data.id);
	}

	data( )->ui_template_name = pXML->ReadAttrib(pNode, "ui_template", "common");
}

void CEncyclopediaArticle::InitXmlIdToIndex( )
{
	if (!id_to_index::tag_name)
	{
		id_to_index::tag_name = "article";
	}

	if (!id_to_index::file_str)
	{
		id_to_index::file_str = pSettings->r_string("encyclopedia", "files");
	}
}
