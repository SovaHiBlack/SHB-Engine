////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_abstract.h
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item abstract class
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ObjectFactory_space.h"

class CObjectItemAbstract {
protected:
	CLASS_ID									m_clsid;
	CSharedString									m_script_clsid;

public:
	inline											CObjectItemAbstract	(const CLASS_ID &clsid, const char* script_clsid);
	inline		const CLASS_ID						&clsid				() const;
	inline		CSharedString							script_clsid		() const;

	virtual ObjectFactory::CLIENT_BASE_CLASS	*client_object		() const = 0;
	virtual ObjectFactory::SERVER_BASE_CLASS	*server_object		(const char* section) const = 0;
};

#include "ObjectItemAbstract_inline.h"
