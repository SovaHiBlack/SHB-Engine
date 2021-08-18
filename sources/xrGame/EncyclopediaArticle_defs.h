#pragma	once

#include "alife_space.h"
#include "object_interfaces.h"

struct ARTICLE_DATA : public IPureSerializeObject<IReader, IWriter>
{
	enum EArticleType
	{
		eEncyclopediaArticle,
		eJournalArticle,
		eTaskArticle,
		eInfoArticle
	};

	ARTICLE_DATA( )
		: article_id(NULL),
		receive_time(0),
		readed(false),
		article_type(eEncyclopediaArticle)
	{ }

	ARTICLE_DATA(CSharedString id, ALife::_TIME_ID time, EArticleType articleType)
		: article_id(id),
		receive_time(time),
		readed(false),
		article_type(articleType)
	{ }

	virtual void load(IReader& stream);
	virtual void save(IWriter&);

	ALife::_TIME_ID			receive_time;
	CSharedString				article_id;
	bool					readed;

	EArticleType			article_type;
};

using ARTICLE_ID_VECTOR = xr_vector<CSharedString>;
using ARTICLE_VECTOR = xr_vector<ARTICLE_DATA>;

class FindArticleByIDPred
{
public:
	FindArticleByIDPred(CSharedString id)
	{
		object_id = id;
	}
	bool operator() (const ARTICLE_DATA& item)
	{
		if (item.article_id == object_id)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

private:
	CSharedString object_id;
};
