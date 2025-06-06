#pragma once
#include "object_interfaces.h"
#include "MapLocation_defs.h"

class CMapLocationWrapper;
class CInventoryOwner;
class CMapLocation;

class CMapManager
{
	CMapLocationWrapper* m_locations;

public:
	CMapManager( );
	~CMapManager( );
	void					Update( );
	void					initialize(u16 id);
	Locations& Locations( );
	CMapLocation* AddMapLocation(const shared_str& spot_type, u16 id);
	CMapLocation* AddRelationLocation(CInventoryOwner* pInvOwner);
	void					RemoveMapLocation(const shared_str& spot_type, u16 id);
	u16						HasMapLocation(const shared_str& spot_type, u16 id);
	void					RemoveMapLocationByObjectID(u16 id); //call on destroy object
	void					RemoveMapLocation(CMapLocation* ml);
	CMapLocation* GetMapLocation(const shared_str& spot_type, u16 id);
	void					DisableAllPointers( );

#ifdef DEBUG
	void					Dump( );
#endif

};
