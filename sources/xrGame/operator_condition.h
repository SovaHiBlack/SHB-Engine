////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_condition.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator condition
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _condition_type,
	typename _value_type
>
class COperatorConditionAbstract {
public:
	typedef _condition_type _condition_type;
	typedef _value_type		_value_type;

protected:
	typedef COperatorConditionAbstract<_condition_type,_value_type>	COperatorCondition;

protected:
	_condition_type		m_condition;
	u32					m_hash;
	_value_type			m_value;

public:
	inline								COperatorConditionAbstract	(const _condition_type condition, const _value_type value);
	inline		const _condition_type	&condition					() const;
	inline		const _value_type		&value						() const;
	inline		const u32				&hash_value					() const;
	inline		bool					operator<					(const COperatorCondition &condition) const;
	inline		bool					operator==					(const COperatorCondition &condition) const;
};

#include "operator_condition_inline.h"