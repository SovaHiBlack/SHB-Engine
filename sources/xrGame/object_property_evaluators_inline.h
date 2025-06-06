////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluators_inline.h
//	Created 	: 12.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object property evaluators inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename _item_type>
IC	CObjectPropertyEvaluatorBase<_item_type>::CObjectPropertyEvaluatorBase(_item_type* item, CStalker* owner)
{
	m_object		= owner;
	m_item			= item;
}

template <typename _item_type>
IC	CStalker& CObjectPropertyEvaluatorBase<_item_type>::object( ) const
{
	VERIFY			(m_object);
	return			(*m_object);
}

IC	CStalker& CObjectPropertyEvaluatorNoItems::object( ) const
{
	VERIFY			(m_object);
	return			(*m_object);
}
