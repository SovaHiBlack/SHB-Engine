// RelationRegistry_defs.h:	реестр для хранения данных об отношении персонажа к другим персонажам

#pragma once

#include "object_interfaces.h"

//структура, описывающая отношение одного персонажа к другому или к группировке
struct SRelation
{
	SRelation( );
	~SRelation( );
	CharacterGoodwill		Goodwill( ) const
	{
		return m_iGoodwill;
	}
	void					SetGoodwill(CharacterGoodwill new_goodwill)
	{
		m_iGoodwill = new_goodwill;
	}

private:
	//благосклонность
	CharacterGoodwill m_iGoodwill;
};

using PersonalRelationMap = xr_map<U16, SRelation>;
using CommunityRelationMap = xr_map<CharacterCommunityIndex, SRelation>;

//структура, существует для каждого персонажа в игре
struct SRelationData : public IPureSerializeObject<IReader, IWriter>
{
	virtual void clear( );

	virtual void load(IReader&);
	virtual void save(IWriter&);

	//личные отношения
	PersonalRelationMap personal;
	//отношения с группировками
	CommunityRelationMap communities;
};
