//	Module 		: server_entity_wrapper.cpp
//	Description : Server entity wrapper

#include "stdafx.h"

#include "server_entity_wrapper.h"
#include "xrServer_Objects.h"
#include "Messages.h"
#include "..\ENGINE\NetPacket.h"

struct ISE_Abstract;

CServerEntityWrapper::~CServerEntityWrapper	()
{
	F_entity_Destroy		(m_object);
}

void CServerEntityWrapper::save				(IWriter &stream)
{
	CNetPacket				net_packet;

	// Spawn
	stream.open_chunk		(0);

	m_object->Spawn_Write	(net_packet,TRUE);
	stream.w_u16			(U16(net_packet.B.count));
	stream.w				(net_packet.B.data,net_packet.B.count);
	
	stream.close_chunk		();

	// Update
	stream.open_chunk		(1);

	net_packet.w_begin		(M_UPDATE);
	m_object->UPDATE_Write	(net_packet);
	stream.w_u16			(U16(net_packet.B.count));
	stream.w				(net_packet.B.data,net_packet.B.count);

//	U16						ID;
//	net_packet.r_begin		(ID);
//	VERIFY					(ID==M_UPDATE);
//	m_object->UPDATE_Read	(net_packet);
	
	stream.close_chunk		();
}

void CServerEntityWrapper::load				(IReader &stream)
{
	CNetPacket				net_packet;
	U16						ID;
	IReader					*chunk;
	
	chunk					= stream.open_chunk(0);

	net_packet.B.count		= chunk->r_u16();
	chunk->r				(net_packet.B.data,net_packet.B.count);

	chunk->close			();

	net_packet.r_begin		(ID);
	R_ASSERT2				(M_SPAWN == ID,"Invalid packet ID (!= M_SPAWN)!");

	string64				s_name;
	net_packet.r_stringZ	(s_name);
	
	m_object				= F_entity_Create(s_name);

	R_ASSERT3				(m_object,"Can't create entity.",s_name);
	m_object->Spawn_Read	(net_packet);
	
	chunk					= stream.open_chunk(1);
	
	net_packet.B.count		= chunk->r_u16();
	chunk->r				(net_packet.B.data,net_packet.B.count);
	
	chunk->close			();

	net_packet.r_begin		(ID);
	R_ASSERT2				(M_UPDATE == ID,"Invalid packet ID (!= M_UPDATE)!");
	m_object->UPDATE_Read	(net_packet);
}

void CServerEntityWrapper::save_update		(IWriter &stream)
{
//	CNetPacket				net_packet;
//	net_packet.w_begin		(M_UPDATE);
//	m_object->save_update	(net_packet);
//	stream.w_u16			(U16(net_packet.B.count));
//	stream.w				(net_packet.B.data,net_packet.B.count);
}

void CServerEntityWrapper::load_update		(IReader &stream)
{
//	CNetPacket				net_packet;
//	U16						ID;
//
//	net_packet.B.count		= stream.r_u16();
//	stream.r				(net_packet.B.data,net_packet.B.count);
//
//	net_packet.r_begin		(ID);
//	R_ASSERT2				(M_UPDATE == ID,"Invalid packet ID (!= M_UPDATE)!");
//	m_object->load_update	(net_packet);
}
