////////////////////////////////////////////////////////////////////////////
//	Module 		: ObjectItemClientServer.h
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item client and server class
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ObjectFactory_space.h"
#include "ObjectItemAbstract.h"
#include "ObjectFactory.h"

template <typename _client_type, typename _server_type>
class CObjectItemClientServer : public CObjectItemAbstract {
protected:
	typedef CObjectItemAbstract	inherited;
	typedef _client_type		CLIENT_TYPE;
	typedef _server_type		SERVER_TYPE;

public:
	IC											CObjectItemClientServer		(const CLASS_ID &clsid, const char* script_clsid);

	virtual ObjectFactory::CLIENT_BASE_CLASS	*client_object				() const;
	virtual ObjectFactory::SERVER_BASE_CLASS	*server_object				(const char* section) const;
};

	template <typename _client_type_single, typename _client_type_mp, typename _server_type_single, typename _server_type_mp>
	class CObjectItemClientServerSingleMp : public CObjectItemAbstract {
	typedef CObjectItemAbstract	inherited;
	public:
		IC											CObjectItemClientServerSingleMp	(const CLASS_ID &clsid, const char* script_clsid);
		virtual ObjectFactory::CLIENT_BASE_CLASS	*client_object					() const;
		virtual ObjectFactory::SERVER_BASE_CLASS	*server_object					(const char* section) const;
	};

#include "ObjectItemClientServer_inline.h"
