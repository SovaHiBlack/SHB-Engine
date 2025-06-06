////////////////////////////////////////////////////////////////////////////
//	Module 		: object_interfaces.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife interfaces
////////////////////////////////////////////////////////////////////////////

#pragma once

class CNetPacket;

class IPureDestroyableObject
{
public:
	virtual void					destroy( ) = 0;
};

template <typename _storage_type>
class IPureL�adableObject
{
public:
	virtual void					load(_storage_type& storage) = 0;
};

template <typename _storage_type>
class IPureSavableObject
{
public:
	virtual void					save(_storage_type& storage) = 0;
};

template <typename _storage_type_load, typename _storage_type_save>
class IPureSerializeObject : public IPureL�adableObject<_storage_type_load>, public IPureSavableObject<_storage_type_save>
{
public:
};

class IPureServerObject : public IPureSerializeObject<IReader, IWriter>
{
public:
	virtual void					STATE_Write(CNetPacket& tNetPacket) = 0;
	virtual void					STATE_Read(CNetPacket& tNetPacket, u16 size) = 0;
	virtual void					UPDATE_Write(CNetPacket& tNetPacket) = 0;
	virtual void					UPDATE_Read(CNetPacket& tNetPacket) = 0;
};

class IPureSchedulableObject
{
public:
	virtual void					update( ) = 0;
};
