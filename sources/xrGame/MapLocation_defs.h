#pragma once

#include "alife_abstract_registry.h"

class CMapLocation;

struct SLocationKey : public IPureSerializeObject<IReader, IWriter>, public IPureDestroyableObject
{
	CSharedString		spot_type;
	U16				object_id;
	CMapLocation* location;
	bool			actual;
	SLocationKey(CSharedString s, U16 id) :spot_type(s), object_id(id), location(NULL), actual(true)
	{ }
	SLocationKey( ) : spot_type(NULL), object_id(0), location(NULL), actual(true)
	{ }

	bool operator < (const SLocationKey& key)const
	{	//move non-actual to tail
		return  actual > key.actual;
	}

	virtual void save(IWriter& stream);
	virtual void load(IReader& stream);
	virtual void destroy( );
};

//DEFINE_VECTOR(SLocationKey, Locations, Locations_it);
using Locations = xr_vector<SLocationKey>;
using Locations_it = Locations::iterator;

struct CMapLocationRegistry : public CALifeAbstractRegistry<U16, Locations>
{
	virtual void save(IWriter& stream);
};
