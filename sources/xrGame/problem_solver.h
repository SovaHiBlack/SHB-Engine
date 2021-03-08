////////////////////////////////////////////////////////////////////////////
//	Module 		: problem_solver.h
//	Created 	: 24.02.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Problem solver
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "associative_vector.h"

template <
	typename _operator_condition,
	typename _condition_state,
	typename _operator,
	typename _condition_evaluator,
	typename _operator_id_type,
	bool	 _reverse_search = false,
	typename _operator_ptr = _operator*,
	typename _condition_evaluator_ptr = _condition_evaluator*
>
class CProblemSolver {
public:
	enum {
		reverse_search = _reverse_search,
	};

public:
	typedef _operator_condition								COperatorCondition;
	typedef _operator										COperator;
	typedef _condition_state								CState;
	typedef _condition_evaluator							CConditionEvaluator;
	typedef _operator_ptr									_operator_ptr;
	typedef _condition_evaluator_ptr						_condition_evaluator_ptr;
	typedef typename _operator_condition::_condition_type	_condition_type;
	typedef typename _operator_condition::_value_type		_value_type;
	typedef typename _operator::_edge_value_type			_edge_value_type;
	typedef CState											_index_type;
	typedef _operator_id_type								_edge_type;

	struct SOperator {
		_operator_id_type	m_operator_id;
		_operator_ptr		m_operator;

		inline					SOperator(const _operator_id_type &operator_id, _operator_ptr _operator) :
								m_operator_id(operator_id),
								m_operator(_operator)
		{
		}

		bool				operator<(const _operator_id_type &operator_id) const
		{
			return			(m_operator_id < operator_id);
		}

		_operator_ptr		get_operator() const
		{
			return			(m_operator);
		}
	};
	typedef xr_vector<SOperator>											OPERATOR_VECTOR;
	typedef typename OPERATOR_VECTOR::const_iterator						const_iterator;
	typedef associative_vector<_condition_type,_condition_evaluator_ptr>	EVALUATORS;

protected:
	OPERATOR_VECTOR				m_operators;
	EVALUATORS					m_evaluators;
	xr_vector<_edge_type>		m_solution;
	CState						m_target_state;
	mutable CState				m_current_state;
	mutable CState				m_temp;
	mutable bool				m_applied;
	bool						m_actuality;
	bool						m_solution_changed;
	bool						m_failed;

private:
	template <bool>
	struct _boolType {};

	template <bool>
	inline		bool						is_goal_reached_impl	(const _index_type	&vertex_index) const {return is_goal_reached_impl(vertex_index);}
	template <>
	inline		bool						is_goal_reached_impl<true>	(const _index_type	&vertex_index) const {return is_goal_reached_impl(vertex_index,true);}

	inline		bool						is_goal_reached_impl	(const _index_type	&vertex_index) const;
	inline		bool						is_goal_reached_impl	(const _index_type	&vertex_index, bool) const;
	
	template <bool _bVal>
	inline		_edge_value_type			estimate_edge_weight_impl(const _index_type	&vertex_index) const { return estimate_edge_weight_impl_helper(vertex_index, _boolType<_bVal>()); };

	inline		_edge_value_type			estimate_edge_weight_impl_helper(const _index_type	&vertex_index, _boolType<false>) const { return estimate_edge_weight_impl(vertex_index); }
	inline		_edge_value_type			estimate_edge_weight_impl_helper(const _index_type	&vertex_index, _boolType<true>) const { return estimate_edge_weight_impl(vertex_index, true); }

	inline		_edge_value_type			estimate_edge_weight_impl(const _index_type	&vertex_index) const;
	inline		_edge_value_type			estimate_edge_weight_impl(const _index_type	&vertex_index, bool) const;

protected:
#ifdef DEBUG
	inline		void						validate_properties		(const CState &conditions) const;
#endif


public:
	// common interface
	inline									CProblemSolver			();
	virtual								~CProblemSolver			();
			void						init					();
	virtual void						setup					();
	inline		bool						actual					() const;

	// graph interface
	inline		_edge_value_type			get_edge_weight			(const _index_type	&vertex_index0,	const _index_type &vertex_index1,	const const_iterator	&i) const;
	inline		bool						is_accessible			(const _index_type	&vertex_index) const;
	inline		const _index_type			&value					(const _index_type	&vertex_index,	const_iterator		&i,				bool					reverse_search) const;
	inline		void						begin					(const _index_type	&vertex_index,	const_iterator		&b,				const_iterator			&e) const;
	inline		bool						is_goal_reached			(const _index_type	&vertex_index) const;
	inline		_edge_value_type			estimate_edge_weight	(const _index_type	&vertex_index) const;

	// operator interface
	inline		virtual void				add_operator			(const _edge_type	&operator_id,	_operator_ptr _operator);
	inline		void						remove_operator			(const _edge_type	&operator_id);
	inline		_operator_ptr				get_operator			(const _operator_id_type &operator_id);
	inline		const OPERATOR_VECTOR		&operators				() const;

	// state interface
	inline		void						set_target_state		(const CState		&state);
	inline		const CState				&current_state			() const;
	inline		const CState				&target_state			() const;

	// evaluator interface
	inline		virtual void				add_evaluator			(const _condition_type &condition_id, _condition_evaluator_ptr evaluator);
	inline		void						remove_evaluator		(const _condition_type &condition_id);
	inline		_condition_evaluator_ptr	evaluator				(const _condition_type &condition_id) const;
	inline		const EVALUATORS			&evaluators				() const;
	inline		void						evaluate_condition		(typename xr_vector<COperatorCondition>::const_iterator &I, typename xr_vector<COperatorCondition>::const_iterator &E, const _condition_type &condition_id) const;

	// solver interface
	inline		void						solve					();
	inline		const xr_vector<_edge_type>	&solution				() const;
	virtual	void						clear					();
};

#include "ai_space.h"

#include "GraphEngine.h"
#include "object_broker.h"

#include "problem_solver_inline.h"