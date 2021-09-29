//	Module		: BaseEvaluationFunction.h
//	Description	: Base evaluation function class

#pragma once

#ifdef EVALUATION_FUNCTION_INITIALIZED_LOG
#	define OUT_MESSAGE(s1, s2)					Msg(s1, s2);
#else // def EVALUATION_FUNCTION_INITIALIZED_LOG
#	define OUT_MESSAGE(s1, s2)
#endif // def EVALUATION_FUNCTION_INITIALIZED_LOG

class CEntityAlive;
class CSE_ALifeObject;
class CEvaluationFunctionStorage;

class CBaseEvaluationFunction
{
protected:
	float										m_fMinResultValue;
	float										m_fMaxResultValue;
	char										m_caName[260];
	CEvaluationFunctionStorage* m_storage;

public:
	CBaseEvaluationFunction(CEvaluationFunctionStorage* storage)
	{
		m_storage = storage;
		VERIFY(m_storage);
		m_caName[0] = 0;
	}
	virtual				~CBaseEvaluationFunction( )
	{ }

	inline CEvaluationFunctionStorage& ef_storage( ) const
	{
		VERIFY(m_storage);
		return *m_storage;
	}

	virtual float		ffGetValue( ) = 0;

	virtual unsigned int			dwfGetDiscreteValue(unsigned int dwDiscretizationValue = 2)
	{
		float fTemp = ffGetValue( );
		if (fTemp <= m_fMinResultValue)
		{
			return 0;
		}
		else if (fTemp >= m_fMaxResultValue)
		{
			return (dwDiscretizationValue - 1);
		}
		else
		{
			return (iFloor((fTemp - m_fMinResultValue) / (m_fMaxResultValue - m_fMinResultValue) * float(dwDiscretizationValue - 1) + 0.5f));
		}
	}

	inline float			ffGetMaxResultValue( )
	{
		return m_fMaxResultValue;
	}
	inline float			ffGetMinResultValue( )
	{
		return m_fMinResultValue;
	}

	inline const char* Name( )
	{
		return m_caName;
	}

	inline CLASS_ID			clsid_member( ) const;
	inline CLASS_ID			clsid_enemy( ) const;
	inline CLASS_ID			clsid_member_item( ) const;
	inline CLASS_ID			clsid_enemy_item( ) const;
};
