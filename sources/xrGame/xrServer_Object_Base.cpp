////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Object_Base.cpp
//	Created 	: 19.09.2002
//  Modified 	: 16.07.2004
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server base object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects.h"
#include "xrMessages.h"
#include "game_base_space.h"
#include "script_value_container_impl.h"
#include "clsid_game.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#pragma warning(pop)

#include "object_factory.h"

#include "xrEProps.h"

IPropHelper& PHelper( )
{
	NODEFAULT;
#ifdef DEBUG
	return(*(IPropHelper*)0);
#	endif
}

pcstr script_section = "script";
pcstr current_version = "current_server_entity_version";

IC	u16	script_server_object_version( )
{
	static bool initialized = false;
	static u16 script_version = 0;
	if (!initialized)
	{
		initialized = true;
		if (!pSettings->section_exist(script_section) || !pSettings->line_exist(script_section, current_version))
			script_version = 0;
		script_version = pSettings->r_u16(script_section, current_version);
	}
	return						(script_version);
}

////////////////////////////////////////////////////////////////////////////
// CPureServerObject
////////////////////////////////////////////////////////////////////////////
void CPureServerObject::save(IWriter& tMemoryStream)
{ }

void CPureServerObject::load(IReader& tFileStream)
{ }

void CPureServerObject::load(CNetPacket& tNetPacket)
{ }

void CPureServerObject::save(CNetPacket& tNetPacket)
{ }

////////////////////////////////////////////////////////////////////////////
// CSE_Abstract
////////////////////////////////////////////////////////////////////////////
CSE_Abstract::CSE_Abstract(pcstr caSection)
{
	m_editor_flags.zero( );
	RespawnTime = 0;
	net_Ready = FALSE;
	ID = 0xffff;
	ID_Parent = 0xffff;
	ID_Phantom = 0xffff;
	owner = 0;
	s_gameid = 0;
	s_RP = 0xFE;			// Use supplied coords
	s_flags.assign(0);
	s_name = caSection;
	s_name_replace = 0;			//xr_strdup("");
	o_Angle.set(0.0f, 0.0f, 0.0f);
	o_Position.set(0.0f, 0.0f, 0.0f);
	m_bALifeControl = false;
	m_wVersion = 0;
	m_script_version = 0;
	m_tClassID = TEXT2CLSID(pSettings->r_string(caSection, "class"));

	m_spawn_flags.zero( );
	m_spawn_flags.set(flSpawnEnabled, TRUE);
	m_spawn_flags.set(flSpawnOnSurgeOnly, TRUE);
	m_spawn_flags.set(flSpawnSingleItemOnly, TRUE);
	m_spawn_flags.set(flSpawnIfDestroyedOnly, TRUE);
	m_spawn_flags.set(flSpawnInfiniteCount, TRUE);

	m_ini_file = 0;

	if (pSettings->line_exist(caSection, "custom_data"))
	{
		string_path file_name;
		FS.update_path(file_name, "$game_config$", pSettings->r_string(caSection, "custom_data"));
		if (!FS.exist(file_name))
		{
			Msg("! cannot open config file %s", file_name);
		}
		else
		{
			IReader* reader = FS.r_open(file_name);
			VERIFY(reader);
			{
				s32 size = reader->length( ) * sizeof(char);
				pstr temp = (pstr)_alloca(size + 1);
				CopyMemory(temp, reader->pointer( ), size);
				temp[size] = 0;
				m_ini_string = temp;
			}
			FS.r_close(reader);
		}
	}

	m_script_clsid = object_factory( ).script_clsid(m_tClassID);
}

CSE_Abstract::~CSE_Abstract( )
{
	xr_free(s_name_replace);
	xr_delete(m_ini_file);
}

CSE_Visual* CSE_Abstract::visual( )
{
	return						(0);
}

ISE_Shape* CSE_Abstract::shape( )
{
	return						(0);
}

CSE_Motion* CSE_Abstract::motion( )
{
	return						(0);
}

CIniFile& CSE_Abstract::spawn_ini( )
{
	if (!m_ini_file)
#pragma warning(push)
#pragma warning(disable:4238)
		m_ini_file = xr_new<CIniFile>(
			&IReader(
				(pvoid)(*(m_ini_string)),
				m_ini_string.size( )
			),
			FS.get_path("$game_config$")->m_Path
		);
#pragma warning(pop)
	return						(*m_ini_file);
}

void CSE_Abstract::Spawn_Write(CNetPacket& tNetPacket, BOOL bLocal)
{
	// generic
	tNetPacket.w_begin(M_SPAWN);
	tNetPacket.w_stringZ(s_name);
	tNetPacket.w_stringZ(s_name_replace ? s_name_replace : "");
	tNetPacket.w_u8(s_gameid);
	tNetPacket.w_u8(s_RP);
	tNetPacket.w_vec3(o_Position);
	tNetPacket.w_vec3(o_Angle);
	tNetPacket.w_u16(RespawnTime);
	tNetPacket.w_u16(ID);
	tNetPacket.w_u16(ID_Parent);
	tNetPacket.w_u16(ID_Phantom);

	s_flags.set(M_SPAWN_VERSION, TRUE);
	if (bLocal)
		tNetPacket.w_u16(u16(s_flags.flags | M_SPAWN_OBJECT_LOCAL));
	else
		tNetPacket.w_u16(u16(s_flags.flags & ~(M_SPAWN_OBJECT_LOCAL | M_SPAWN_OBJECT_ASPLAYER)));

	tNetPacket.w_u16(SPAWN_VERSION);

	tNetPacket.w_u16(script_server_object_version( ));

	//client object custom data serialization SAVE
	u16 client_data_size = (u16)client_data.size( ); //�� ����� ���� ������ 256 ����
	tNetPacket.w_u16(client_data_size);
	//	Msg							("SERVER:saving:save:%d bytes:%d:%s",client_data_size,ID,s_name_replace ? s_name_replace : "");
	if (client_data_size > 0)
	{
		tNetPacket.w(&*client_data.begin( ), client_data_size);
	}

	tNetPacket.w(&m_tSpawnID, sizeof(m_tSpawnID));

	// write specific data
	u32	position = tNetPacket.w_tell( );
	tNetPacket.w_u16(0);
	STATE_Write(tNetPacket);
	u16 size = u16(tNetPacket.w_tell( ) - position);

	R_ASSERT3((m_tClassID == CLSID_SPECTATOR) || (size > sizeof(size)), "object isn't successfully saved, get your backup :(", name_replace( ));

	tNetPacket.w_seek(position, &size, sizeof(u16));
}

BOOL CSE_Abstract::Spawn_Read(CNetPacket& tNetPacket)
{
	u16							dummy16;
	// generic
	tNetPacket.r_begin(dummy16);
	R_ASSERT(M_SPAWN == dummy16);
	tNetPacket.r_stringZ(s_name);

	string256					temp;
	tNetPacket.r_stringZ(temp);
	set_name_replace(temp);
	tNetPacket.r_u8(s_gameid);
	tNetPacket.r_u8(s_RP);
	tNetPacket.r_vec3(o_Position);
	tNetPacket.r_vec3(o_Angle);
	tNetPacket.r_u16(RespawnTime);
	tNetPacket.r_u16(ID);
	tNetPacket.r_u16(ID_Parent);
	tNetPacket.r_u16(ID_Phantom);

	tNetPacket.r_u16(s_flags.flags);

	// dangerous!!!!!!!!!
	if (s_flags.is(M_SPAWN_VERSION))
		tNetPacket.r_u16(m_wVersion);

	if (0 == m_wVersion)
	{
		tNetPacket.r_pos -= sizeof(u16);
		m_wVersion = 0;
		return					FALSE;
	}

	if (m_wVersion > 69)
		m_script_version = tNetPacket.r_u16( );

	// read specific data

	//client object custom data serialization LOAD
	if (m_wVersion > 70)
	{
		u16 client_data_size = (m_wVersion > 93) ? tNetPacket.r_u16( ) : tNetPacket.r_u8( ); //�� ����� ���� ������ 256 ����
		if (client_data_size > 0)
		{
			//			Msg					("SERVER:loading:load:%d bytes:%d:%s",client_data_size,ID,s_name_replace ? s_name_replace : "");
			client_data.resize(client_data_size);
			tNetPacket.r(&*client_data.begin( ), client_data_size);
		}
		else
			client_data.clear( );
	}
	else
		client_data.clear( );

	if (m_wVersion > 79)
		tNetPacket.r(&m_tSpawnID, sizeof(m_tSpawnID));

	if (m_wVersion < 112)
	{
		if (m_wVersion > 82)
			tNetPacket.r_float( );//m_spawn_probability);

		if (m_wVersion > 83)
		{
			tNetPacket.r_u32( );//m_spawn_flags.assign(tNetPacket.r_u32());
			xr_string				temp;
			tNetPacket.r_stringZ(temp);//tNetPacket.r_stringZ(m_spawn_control);
			tNetPacket.r_u32( );//m_max_spawn_count);
			// this stuff we do not need even in case of uncomment
			tNetPacket.r_u32( );//m_spawn_count);
			tNetPacket.r_u64( );//m_last_spawn_time);
		}

		if (m_wVersion > 84)
		{
			tNetPacket.r_u64( );//m_min_spawn_interval);
			tNetPacket.r_u64( );//m_max_spawn_interval);
		}
	}

	u16							size;
	tNetPacket.r_u16(size);	// size
	R_ASSERT3((m_tClassID == CLSID_SPECTATOR) || (size > sizeof(size)), "cannot read object, which is not successfully saved :(", name_replace( ));
	STATE_Read(tNetPacket, size);
	return						TRUE;
}

void	CSE_Abstract::load(CNetPacket& tNetPacket)
{
	CPureServerObject::load(tNetPacket);
	u16 client_data_size = (m_wVersion > 93) ? tNetPacket.r_u16( ) : tNetPacket.r_u8( ); //�� ����� ���� ������ 256 ����
	if (client_data_size > 0)
	{
#ifdef DEBUG
		//		Msg						("SERVER:loading:load:%d bytes:%d:%s",client_data_size,ID,s_name_replace ? s_name_replace : "");
#endif // DEBUG
		client_data.resize(client_data_size);
		tNetPacket.r(&*client_data.begin( ), client_data_size);
	}
	else
	{
#ifdef DEBUG
		if (!client_data.empty( ))
			Msg("CSE_Abstract::load: client_data is cleared for [%d][%s]", ID, name_replace( ));
#endif // DEBUG
		client_data.clear( );
	}
}

CSE_Abstract* CSE_Abstract::base( )
{
	return						(this);
}

const CSE_Abstract* CSE_Abstract::base( ) const
{
	return						(this);
}

CSE_Abstract* CSE_Abstract::init( )
{
	return						(this);
}

pcstr		CSE_Abstract::name( ) const
{
	return	(*s_name);
}

pcstr		CSE_Abstract::name_replace( ) const
{
	return	(s_name_replace);
}

fVector3& CSE_Abstract::position( )
{
	return	(o_Position);
}

fVector3& CSE_Abstract::angle( )
{
	return	(o_Angle);
}

flags16& CSE_Abstract::flags( )
{
	return	(s_flags);
}

xr_token game_types[ ] = {
	{ "any game",		GAME_ANY		},
	{ "single",			GAME_SINGLE		},
	{ 0,				0				}
};

void CSE_Abstract::FillProps(pcstr pref, PropItemVec& items)
{ }

void CSE_Abstract::FillProp(pcstr pref, PropItemVec& items)
{
	FillProps(pref, items);
}

bool CSE_Abstract::validate( )
{
	return						(true);
}
