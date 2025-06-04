////////////////////////////////////////////////////////////////////////////
//	Module 		: PurchaseList.h
//	Description : purchase list class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "associative_vector.h"

class CInventoryOwner;
class CGameObject;

class CPurchaseList
{
public:
	typedef associative_vector<shared_str, f32>	DEFICITS;

private:
	DEFICITS				m_deficits;

private:
	void			process(const CGameObject& owner, const shared_str& name, const u32& count, const f32& probability);

public:
	void			process(CIniFile& ini_file, pcstr section, CInventoryOwner& owner);

public:
	IC		void			deficit(const shared_str& section, const f32& deficit);
	IC		f32			deficit(const shared_str& section) const;
	IC		const DEFICITS& deficits( ) const;
};

#include "PurchaseList_inline.h"
