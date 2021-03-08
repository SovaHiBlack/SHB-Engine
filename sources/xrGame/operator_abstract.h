////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_abstract.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator abstract
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "condition_state.h"

template <
	typename _world_property,
	typename _edge_value_type
>
class COperatorAbstract {
public:
	typedef _edge_value_type					_edge_value_type;

protected:
	typedef CConditionState<_world_property>	inherited;
	typedef inherited							CSConditionState;
	typedef _world_property						COperatorCondition;

protected:
	CSConditionState				m_conditions;
	CSConditionState				m_effects;
	bool							*m_actuality;
	mutable bool					m_weight_actual;
	mutable _edge_value_type		m_min_weight;

protected:
	inline		void					actual				(bool value);

public:
	inline								COperatorAbstract	();
	inline								COperatorAbstract	(const CSConditionState &conditions, const CSConditionState &effects);
	virtual							~COperatorAbstract	();
	virtual	void					Load				(const char* section);
	virtual void					setup				(bool *actuality);
	inline		const CSConditionState	&conditions			() const;
	inline		const CSConditionState	&effects			() const;
	inline		void					add_condition		(const COperatorCondition &condition);
	inline		void					add_effect			(const COperatorCondition &effect);
	inline		void					remove_condition	(const typename COperatorCondition::_condition_type &condition);
	inline		void					remove_effect		(const typename COperatorCondition::_condition_type &effect);
	inline		_edge_value_type		min_weight			() const;
	
	template <typename T>
	inline		bool					applicable			(const CSConditionState &condition, const CSConditionState &start, const CSConditionState &self_condition, T &problem_solver) const;
	
	virtual bool					applicable_reverse	(const CSConditionState &condition, const CSConditionState &start, const CSConditionState &self_condition) const;
	
	template <typename T>
	inline		const CSConditionState	&apply				(const CSConditionState &condition, const CSConditionState &self_condition, CSConditionState &result, CSConditionState &current, T &problem_solver) const;
	virtual const CSConditionState	&apply				(const CSConditionState &condition, const CSConditionState &self_condition, CSConditionState &result) const;
	
	virtual bool					apply_reverse		(const CSConditionState &condition, const CSConditionState &start, CSConditionState &result, const CSConditionState &self_condition) const;
	virtual _edge_value_type		weight				(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

#include "operator_abstract_inline.h"