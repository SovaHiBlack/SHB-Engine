//	Module 		: PurchaseList.h
//	Description : purchase list class

#pragma once

#include "associative_vector.h"

class CInventoryOwner;
class CGameObject;

class CPurchaseList
{
public:
	using DEFICITS = associative_vector<CSharedString, float>;

private:
	DEFICITS				m_deficits;

	void			process(const CGameObject& owner, const CSharedString& name, const u32& count, const float& probability);

public:
	void			process(CIniFile& ini_file, const char* section, CInventoryOwner& owner);

	inline void			deficit(const CSharedString& section, const float& deficit);
	inline float			deficit(const CSharedString& section) const;
	inline const DEFICITS& deficits( ) const;
};

#include "PurchaseList_inline.h"
