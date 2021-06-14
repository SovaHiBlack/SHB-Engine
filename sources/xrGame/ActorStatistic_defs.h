#pragma once

#include "alife_abstract_registry.h"

struct SStatDetailBData : public IPureSerializeObject<IReader, IWriter>
{
	shared_str							key;
	int									int_count;
	int									int_points;
	shared_str							str_value;

	virtual void		save			(IWriter& stream);
	virtual void		load			(IReader& stream);
};

using vStatDetailData					= xr_vector<SStatDetailBData>;

struct SStatSectionData : public IPureSerializeObject<IReader, IWriter>
{
	shared_str							key;
	vStatDetailData						data;

	SStatDetailBData&	GetData			(const shared_str&);
	int					GetTotalPoints	( ) const;
	virtual void		save			(IWriter& stream);
	virtual void		load			(IReader& stream);
};

using vStatSectionData					= xr_vector<SStatSectionData>;
using CActorStatisticRegistry			= CALifeAbstractRegistry<U16, vStatSectionData>;
