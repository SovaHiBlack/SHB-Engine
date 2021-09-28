//	Module 		: xrServer_Object_Base.h
//	Description : Server base object

#pragma once

#include "xrServer_Objects_Abstract.h"
#include "object_interfaces.h"
#include "script_value_container.h"
#include "alife_space.h"
#include "..\ENGINE\ClientID.h"

class CNetPacket;
class xrClientData;
class CSE_ALifeGroupAbstract;
class CSE_ALifeSchedulable;
class CSE_ALifeInventoryItem;
class CSE_ALifeTraderAbstract;
class CSE_ALifeObject;
class CSE_ALifeDynamicObject;
class CSE_ALifeItemAmmo;
class CSE_ALifeItemWeapon;
class CSE_ALifeItemDetector;
class CSE_ALifeMonsterAbstract;
class CSE_ALifeHumanAbstract;
class CSE_ALifeAnomalousZone;
class CSE_ALifeTrader;
class CSE_ALifeCreatureAbstract;
class CSE_ALifeSmartZone;
class CSE_ALifeOnlineOfflineGroup;
class CSE_ALifeItemPDA;

#pragma warning(push)
#pragma warning(disable:4005)

SERVER_ENTITY_DECLARE_BEGIN(CPureServerObject, IPureServerObject)
virtual							~CPureServerObject( )
{ }
virtual void					load(IReader& tFileStream);
virtual void					save(IWriter& tMemoryStream);
virtual void					load(CNetPacket& tNetPacket);
virtual void					save(CNetPacket& tNetPacket);
};
add_to_type_list(CPureServerObject)
#define script_type_list save_type_list(CPureServerObject)

SERVER_ENTITY_DECLARE_BEGIN3(CSE_Abstract, ISE_Abstract, CPureServerObject, CScriptValueContainer)
public:
	enum ESpawnFlags
	{
		flSpawnEnabled = u32(1 << 0),
		flSpawnOnSurgeOnly = u32(1 << 1),
		flSpawnSingleItemOnly = u32(1 << 2),
		flSpawnIfDestroyedOnly = u32(1 << 3),
		flSpawnInfiniteCount = u32(1 << 4),
		flSpawnDestroyOnSpawn = u32(1 << 5)
	};

private:
	char* s_name_replace;

public:
	BOOL							net_Ready;
	BOOL							net_Processed;	// Internal flag for connectivity-graph

	U16								m_wVersion;
	U16								m_script_version;
	U16								RespawnTime;

	U16								ID;				// internal ID
	U16								ID_Parent;		// internal ParentID, 0xffff means no parent
	U16								ID_Phantom;		// internal PhantomID, 0xffff means no phantom
	xrClientData* owner;

	// spawn data
	CSharedString						s_name;
	U8								s_gameid;
	U8								s_RP;
	Flags16							s_flags;		// state flags
	xr_vector<U16>					children;

	// update data
	Fvector3							o_Position;
	Fvector3							o_Angle;
	CLASS_ID						m_tClassID;
	int								m_script_clsid;
	CSharedString						m_ini_string;
	CIniFile* m_ini_file;

	// for ALife control
	bool							m_bALifeControl;
	ALife::_SPAWN_ID				m_tSpawnID;

	// ALife spawn params
	// obsolete, just because we hope to uncomment all this stuff
	Flags32							m_spawn_flags;

	//client object custom data serialization
	xr_vector<U8>					client_data;
	virtual void					load(CNetPacket& tNetPacket);

	//////////////////////////////////////////////////////////////////////////

	CSE_Abstract(const char* caSection);
	virtual							~CSE_Abstract( );
	virtual void					OnEvent(CNetPacket& tNetPacket, U16 type, u32 time, ClientID sender)
	{ }
	virtual void					FillProps(const char* pref, PropItemsVec& items);
	virtual BOOL					Net_Relevant( )
	{
		return TRUE;
	}
//
	virtual void __stdcall	Spawn_Write(CNetPacket& tNetPacket, BOOL bLocal);
	virtual BOOL __stdcall	Spawn_Read(CNetPacket& tNetPacket);
	virtual void __stdcall	FillProp(const char* pref, PropItemsVec& items);
	virtual const char* __stdcall	name( ) const;
	virtual const char* __stdcall	name_replace( ) const;
	virtual void __stdcall	set_name(const char* s)
	{
		s_name = s;
	}
	virtual void __stdcall	set_name_replace(const char* s)
	{
		xr_free(s_name_replace);
		s_name_replace = xr_strdup(s);
	}
	virtual Fvector3& __stdcall	position( );
	virtual Fvector3& __stdcall	angle( );
	virtual Flags16& __stdcall	flags( );
	virtual CSE_Visual* __stdcall	visual( );
	virtual ISE_Shape* __stdcall	shape( );
	virtual CSE_Motion* __stdcall	motion( );
	virtual bool __stdcall	validate( );
	virtual void __stdcall	on_render(CDUInterface* du, ISE_AbstractLEOwner* owner, bool bSelected, const Fmatrix& parent, int priority, bool strictB2F)
	{ }
//

	inline const Fvector3& Position( ) const
	{
		return o_Position;
	}
// we need this to prevent virtual inheritance :-(
	virtual CSE_Abstract* base( );
	virtual const CSE_Abstract* base( ) const;
	virtual CSE_Abstract* init( );
	virtual bool					match_configuration( ) const
	{
		return true;
	}
// end of the virtual inheritance dependant code
	inline int						script_clsid( ) const
	{
		VERIFY(m_script_clsid >= 0);
		return (m_script_clsid);
	}
	CIniFile& spawn_ini( );

// for smart cast
	virtual CSE_ALifeGroupAbstract* cast_group_abstract( )
	{
		return nullptr;
	}
	virtual CSE_ALifeSchedulable* cast_schedulable( )
	{
		return nullptr;
	}
	virtual CSE_ALifeInventoryItem* cast_inventory_item( )
	{
		return nullptr;
	}
	virtual CSE_ALifeTraderAbstract* cast_trader_abstract( )
	{
		return nullptr;
	}

	virtual CSE_ALifeObject* cast_alife_object( )
	{
		return nullptr;
	}
	virtual CSE_ALifeDynamicObject* cast_alife_dynamic_object( )
	{
		return nullptr;
	}
	virtual CSE_ALifeItemAmmo* cast_item_ammo( )
	{
		return nullptr;
	}
	virtual CSE_ALifeItemWeapon* cast_item_weapon( )
	{
		return nullptr;
	}
	virtual CSE_ALifeItemDetector* cast_item_detector( )
	{
		return nullptr;
	}
	virtual CSE_ALifeMonsterAbstract* cast_monster_abstract( )
	{
		return nullptr;
	}
	virtual CSE_ALifeHumanAbstract* cast_human_abstract( )
	{
		return nullptr;
	}
	virtual CSE_ALifeAnomalousZone* cast_anomalous_zone( )
	{
		return nullptr;
	}
	virtual CSE_ALifeTrader* cast_trader( )
	{
		return nullptr;
	}

	virtual CSE_ALifeCreatureAbstract* cast_creature_abstract( )
	{
		return nullptr;
	}
	virtual CSE_ALifeSmartZone* cast_smart_zone( )
	{
		return nullptr;
	}
	virtual CSE_ALifeOnlineOfflineGroup* cast_online_offline_group( )
	{
		return nullptr;
	}
	virtual CSE_ALifeItemPDA* cast_item_pda( )
	{
		return nullptr;
	}
};

add_to_type_list(CSE_Abstract)
#define script_type_list save_type_list(CSE_Abstract)

#pragma warning(pop)
