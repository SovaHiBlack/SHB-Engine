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
	void					initialize					(unsigned short id);
	Locations&				Locations					( );
	CMapLocation*			AddMapLocation				(const CSharedString& spot_type, unsigned short id);
	CMapLocation*			AddRelationLocation			(CInventoryOwner* pInvOwner);
//.	CMapLocation*			AddUserLocation				(const CSharedString& spot_type, const CSharedString& level_name, Fvector3 position);
	void					RemoveMapLocation			(const CSharedString& spot_type, unsigned short id);
	unsigned short						HasMapLocation				(const CSharedString& spot_type, unsigned short id);
	void					RemoveMapLocationByObjectID	(unsigned short id); //call on destroy object
	void					RemoveMapLocation			(CMapLocation* ml);
	CMapLocation*			GetMapLocation				(const CSharedString& spot_type, unsigned short id);
	void					DisableAllPointers			( );

#ifdef DEBUG
	void					Dump						( );
#endif // def DEBUG

};
