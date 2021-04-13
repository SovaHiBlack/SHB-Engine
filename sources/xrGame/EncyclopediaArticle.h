// структура, хранящая и загружающая статьи в энциклопедию
#pragma once

#include "EncyclopediaArticle_defs.h"
#include "XML_IdToIndex.h"
#include "shared_data.h"

#include "ui\UIStatic.h"//

struct SArticleData : CSharedResource
{
	shared_str name;
	shared_str group;

	//картинка
	CUIStatic image;

	//текст статьи
	xr_string	text;

	//секция ltx, откуда читать данные
//	shared_str ltx;

	// Тип статьи
	ARTICLE_DATA::EArticleType	articleType;
	shared_str					ui_template_name;
};

class CEncyclopediaArticle;

class CEncyclopediaArticle : public CSharedClass<SArticleData, shared_str, false>, public CXML_IdToIndex<CEncyclopediaArticle>
{
private:
	using inherited_shared = CSharedClass<SArticleData, shared_str, false>;
	using id_to_index = CXML_IdToIndex<CEncyclopediaArticle>;

	friend id_to_index;

public:
	CEncyclopediaArticle( );
	virtual				~CEncyclopediaArticle( );

	virtual void Load(shared_str str_id);

protected:
	shared_str			m_ArticleId;
	virtual void		load_shared(const char*);
	static void		InitXmlIdToIndex( );

public:
	const shared_str	Id( )
	{
		return m_ArticleId;
	}
	SArticleData* data( )
	{
		VERIFY(inherited_shared::get_sd( ));
		return inherited_shared::get_sd( );
	}
};
