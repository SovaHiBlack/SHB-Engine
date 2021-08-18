////////////////////////////////////////////////////////////////////////////
//	Module 		: purchase_list.h
//	Created 	: 12.01.2006
//  Modified 	: 12.01.2006
//	Author		: Dmitriy Iassenev
//	Description : purchase list class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "associative_vector.h"

class CInventoryOwner;
class CGameObject;

class CPurchaseList {
public:
	typedef associative_vector<CSharedString,float>	DEFICITS;

private:
	DEFICITS				m_deficits;

			void			process			(const CGameObject &owner, const CSharedString& name, const u32 &count, const float &probability);

public:
			void			process			(CIniFile&ini_file, const char* section, CInventoryOwner &owner);

	inline		void			deficit			(const CSharedString& section, const float &deficit);
	inline		float			deficit			(const CSharedString& section) const;
	inline		const DEFICITS	&deficits		() const;
};

#include "purchase_list_inline.h"
