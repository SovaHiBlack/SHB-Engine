////////////////////////////////////////////////////////////////////////////
//	Module 		: property_storage.h
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Property storage class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GraphEngine_space.h"
#include "script_export_space.h"

class CPropertyStorage {
public:
	typedef GraphEngine::_solver_condition_type	_condition_type;
	typedef GraphEngine::_solver_value_type		_value_type;
	typedef GraphEngine::CSolverConditionValue		CConditionValue;
	typedef GraphEngine::CSolverConditionStorage	CConditionStorage;

public:
	CConditionStorage			m_storage;

public:
	inline		void				clear			();
	inline		void				set_property	(const _condition_type &condition_id, const _value_type &value);
	inline		const _value_type	&property		(const _condition_type &condition_id) const;
public:
	static void script_register(lua_State*);
};

add_to_type_list(CPropertyStorage)
#undef script_type_list
#define script_type_list save_type_list(CPropertyStorage)

#include "property_storage_inline.h"
