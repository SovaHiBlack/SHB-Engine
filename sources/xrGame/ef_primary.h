//	Module		: ef_primary.h
//	Description	: Primary function classes

#pragma once

#include "BaseEvaluationFunction.h"

class CDistanceFunction : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CDistanceFunction(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 3.0f;
		m_fMaxResultValue = 20.0f;
		strcat(m_caName, "Distance");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CDistanceFunction( )
	{ }

	virtual float ffGetValue( );

	virtual unsigned int	dwfGetDiscreteValue(unsigned int dwDiscretizationValue)
	{
		float fTemp = ffGetValue( );
		if (fTemp <= m_fMinResultValue)
		{
			return 0;
		}
		else if (fTemp >= m_fMaxResultValue)
		{
			return(dwDiscretizationValue - 1);
		}
		else
		{
			return 1;
		}
	}
};

class CPersonalHealthFunction : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CPersonalHealthFunction(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName, "PersonalHealth");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual float ffGetValue( );
};

class CPersonalMoraleFunction : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CPersonalMoraleFunction(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName, "PersonalMorale");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CPersonalMoraleFunction( )
	{ }

	virtual float ffGetValue( );
};

class CPersonalCreatureTypeFunction : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CPersonalCreatureTypeFunction(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 1.0;
		m_fMaxResultValue = 21.0;
		strcat(m_caName, "PersonalCreatureType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CPersonalCreatureTypeFunction( )
	{ }

	virtual float ffGetValue( );
};

class CPersonalWeaponTypeFunction : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CPersonalWeaponTypeFunction(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 1.0;
		m_fMaxResultValue = 12.0;
		strcat(m_caName, "PersonalWeaponType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CPersonalWeaponTypeFunction( )
	{ }

	unsigned int dwfGetWeaponType( );
	float ffGetTheBestWeapon( );
	virtual float ffGetValue( );
};

class CPersonalAccuracyFunction : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CPersonalAccuracyFunction(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName, "PersonalAccuracy");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CPersonalAccuracyFunction( )
	{ }

	virtual float ffGetValue( );
};

class CPersonalIntelligenceFunction : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CPersonalIntelligenceFunction(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName, "PersonalIntelligence");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CPersonalIntelligenceFunction( )
	{ }

	virtual float ffGetValue( );
};

class CPersonalRelationFunction : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CPersonalRelationFunction(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName, "PersonalRelation");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CPersonalRelationFunction( )
	{ }

	virtual float ffGetValue( );
};

class CPersonalGreedFunction : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CPersonalGreedFunction(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName, "PersonalGreed");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CPersonalGreedFunction( )
	{ }

	virtual float ffGetValue( );
};

class CPersonalAggressivenessFunction : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CPersonalAggressivenessFunction(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName, "PersonalAggressiveness");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CPersonalAggressivenessFunction( )
	{ }

	virtual float ffGetValue( );
};

class CEnemyMoraleFunction : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CEnemyMoraleFunction(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName, "EnemyMorale");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CEnemyMoraleFunction( )
	{ }

	virtual float ffGetValue( );
};

class CEnemyEquipmentCostFunction : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CEnemyEquipmentCostFunction(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 12.0;
		strcat(m_caName, "EnemyEquipmentCost");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CEnemyEquipmentCostFunction( )
	{ }

	virtual float ffGetValue( );
};

class CEnemyRukzakWeightFunction : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CEnemyRukzakWeightFunction(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 1.0;
		m_fMaxResultValue = 12.0;
		strcat(m_caName, "EnemyRukzakWeight");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CEnemyRukzakWeightFunction( )
	{ }

	virtual float ffGetValue( );
};

class CEnemyAnomalityFunction : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CEnemyAnomalityFunction(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 1.0;
		m_fMaxResultValue = 12.0;
		strcat(m_caName, "EnemyAnomality");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CEnemyAnomalityFunction( )
	{ }

	virtual float ffGetValue( );
};

class CGraphPointType0 : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CGraphPointType0(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName, "GraphPointType0");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CGraphPointType0( )
	{ }

	virtual float ffGetValue( );
};

class CPersonalEyeRange : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CPersonalEyeRange(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName, "PersonalEyeRange");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CPersonalEyeRange( )
	{ }

	virtual float ffGetValue( );
};

class CPersonalMaxHealth : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CPersonalMaxHealth(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 1000.0;
		strcat(m_caName, "PersonalMaxHealth");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CPersonalMaxHealth( )
	{ }

	virtual float	ffGetValue( );
	virtual unsigned int		dwfGetDiscreteValue(u32 dwDiscretizationValue = 1);
};

class CEquipmentType : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CEquipmentType(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 1.0;
		m_fMaxResultValue = 5.0;
		strcat(m_caName, "EquipmentType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CEquipmentType( )
	{ }

	virtual float	ffGetValue( );
};

class CItemDeterioration : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CItemDeterioration(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName, "ItemDeterioration");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CItemDeterioration( )
	{ }

	virtual float	ffGetValue( );
};

class CEquipmentPreference : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CEquipmentPreference(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 1.0;
		m_fMaxResultValue = 3.0;
		strcat(m_caName, "EquipmentPreference");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CEquipmentPreference( )
	{ }

	virtual float	ffGetValue( );
};

class CMainWeaponType : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CMainWeaponType(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 1.0;
		m_fMaxResultValue = 4.0;
		strcat(m_caName, "MainWeaponType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CMainWeaponType( )
	{ }

	virtual float	ffGetValue( );
};

class CMainWeaponPreference : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CMainWeaponPreference(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 1.0;
		m_fMaxResultValue = 3.0;
		strcat(m_caName, "MainWeaponPreference");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CMainWeaponPreference( )
	{ }

	virtual float	ffGetValue( );
};

class CItemValue : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CItemValue(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 100.0;
		m_fMaxResultValue = 2000.0;
		strcat(m_caName, "ItemValue");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CItemValue( )
	{ }

	virtual float	ffGetValue( );
};

class CWeaponAmmoCount : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CWeaponAmmoCount(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 10.0;
		strcat(m_caName, "WeaponAmmoCount");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CWeaponAmmoCount( )
	{ }

	virtual float	ffGetValue( );
	virtual unsigned int		dwfGetDiscreteValue(unsigned int dwDiscretizationValue = 1);
};

class CEnemyAnomalyType : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CEnemyAnomalyType(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 7.0;
		strcat(m_caName, "EnemyAnomalyType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CEnemyAnomalyType( )
	{ }

	virtual float	ffGetValue( );
};

class CDetectorType : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CDetectorType(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 2.0;
		strcat(m_caName, "DetectorType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CDetectorType( )
	{ }

	virtual float	ffGetValue( );
};

class CEnemyDistanceToGraphPoint : public CBaseEvaluationFunction
{
public:
	using inherited = CBaseEvaluationFunction;

	CEnemyDistanceToGraphPoint(CEvaluationFunctionStorage* storage) : CBaseEvaluationFunction(storage)
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 4.0;
		strcat(m_caName, "EnemyDistanceToGraphPoint");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initialized", m_caName);
	}

	virtual ~CEnemyDistanceToGraphPoint( )
	{ }

	virtual float	ffGetValue( );
};
