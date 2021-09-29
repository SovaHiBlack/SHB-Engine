//	Module 		: ef_storage_inline.h
//	Description : Evaluation functions storage class inline functions

#pragma once

inline void CEvaluationFunctionStorage::alife_evaluation(bool value)
{
	if (value)
	{
		non_alife( ).clear( );
	}
	else
	{
		alife( ).clear( );
	}
}

inline CNonALifeParams& CEvaluationFunctionStorage::non_alife( )
{
	return m_non_alife_params;
}

inline CALifeParams& CEvaluationFunctionStorage::alife( )
{
	return m_alife_params;
}
