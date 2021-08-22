//	Module 		: xrServer_Factory.cpp
//	Description : Server objects factory

#include "stdafx.h"

#include "ObjectFactory.h"

CSE_Abstract* F_entity_Create(const char* section)
{
	return object_factory( ).server_object(pSettings->r_clsid(section, "class"), section);
}
