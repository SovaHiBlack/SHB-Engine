// структура, хранящая и загружающая статьи в энциклопедию
#pragma once

#include "EncyclopediaArticle_defs.h"
#include "XML_IdToIndex.h"
#include "shared_data.h"

#include "ui\UIStatic.h"//

struct SArticleData : CSharedResource
{
	CSharedString name;
	CSharedString group;

	//картинка
	CUIStatic image;

	//текст статьи
	xr_string	text;

	//секция ltx, откуда читать данные
//	CSharedString ltx;

	// Тип статьи
	ARTICLE_DATA::EArticleType	articleType;
	CSharedString					ui_template_name;
};

class CEncyclopediaArticle;

class CEncyclopediaArticle : public CSharedClass<SArticleData, CSharedString, false>, public CXML_IdToIndex<CEncyclopediaArticle>
{
private:
	using inherited_shared = CSharedClass<SArticleData, CSharedString, false>;
	using id_to_index = CXML_IdToIndex<CEncyclopediaArticle>;

	friend id_to_index;

public:
	CEncyclopediaArticle( );
	virtual				~CEncyclopediaArticle( );

	virtual void Load(CSharedString str_id);

protected:
	CSharedString			m_ArticleId;
	virtual void		load_shared(const char*);
	static void		InitXmlIdToIndex( );

public:
	const CSharedString	Id( )
	{
		return m_ArticleId;
	}
	SArticleData* data( )
	{
		VERIFY(inherited_shared::get_sd( ));
		return inherited_shared::get_sd( );
	}
};
