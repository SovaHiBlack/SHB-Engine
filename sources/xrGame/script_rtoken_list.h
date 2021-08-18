////////////////////////////////////////////////////////////////////////////
//	Module 		: script_rtoken_list.h
//	Created 	: 16.07.2004
//  Modified 	: 16.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Script rtoken list class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"

class CScriptRTokenList {
public:
	typedef xr_vector<CSharedString> RTOKEN_LIST;

protected:
	RTOKEN_LIST			m_values;

public:
	inline		void		add		(const char* value);
	inline		void		remove	(u32 index);
	inline		const char* get		(u32 index);
	inline		u32			size	();
	inline		void		clear	();
	inline		RTOKEN_LIST	&tokens	();

	static void script_register(lua_State*);
};

add_to_type_list(CScriptRTokenList)
#undef script_type_list
#define script_type_list save_type_list(CScriptRTokenList)

#include "script_rtoken_list_inline.h"