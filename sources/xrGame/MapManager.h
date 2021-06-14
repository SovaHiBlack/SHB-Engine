#pragma once

#include "object_interfaces.h"
#include "MapLocation_defs.h"

class CMapLocationWrapper;
class CInventoryOwner;
class CMapLocation;

class CMapManager
{
	CMapLocationWrapper*								m_locations;

public:
							CMapManager					( );
							~CMapManager				( );
	void					Update						( );
	void					initialize					(U16 id);
	Locations&				Locations					( );
	CMapLocation*			AddMapLocation				(const shared_str& spot_type, U16 id);
	CMapLocation*			AddRelationLocation			(CInventoryOwner* pInvOwner);
//.	CMapLocation*			AddUserLocation				(const shared_str& spot_type, const shared_str& level_name, Fvector3 position);
	void					RemoveMapLocation			(const shared_str& spot_type, U16 id);
	U16						HasMapLocation				(const shared_str& spot_type, U16 id);
	void					RemoveMapLocationByObjectID	(U16 id); //call on destroy object
	void					RemoveMapLocation			(CMapLocation* ml);
	CMapLocation*			GetMapLocation				(const shared_str& spot_type, U16 id);
	void					DisableAllPointers			( );

#ifdef DEBUG
	void					Dump						( );
#endif // def DEBUG

};
