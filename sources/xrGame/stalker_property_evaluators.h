////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_property_evaluators.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker property evaluators classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "wrapper_abstract.h"
#include "property_evaluator_const.h"
#include "property_evaluator_member.h"
#include "danger_object.h"

class CStalker;

typedef CWrapperAbstract2<CStalker, CPropertyEvaluator>		CStalkerPropertyEvaluator;
typedef CWrapperAbstract2<CStalker, CPropertyEvaluatorConst>	CStalkerPropertyEvaluatorConst;
typedef CWrapperAbstract2<CStalker, CPropertyEvaluatorMember>	CStalkerPropertyEvaluatorMember;

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorALife
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorALife : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorALife(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorAlive
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorAlive : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorAlive(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItems
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorItems : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorItems(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemies
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorEnemies : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;
protected:
	u32					m_time_to_wait;
	const bool* m_dont_wait;

public:
	CStalkerPropertyEvaluatorEnemies(CStalker* object = 0, pcstr evaluator_name = "", u32 time_to_wait = 0, const bool* dont_wait = 0);
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorSeeEnemy
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorSeeEnemy : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorSeeEnemy(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemySeeMe
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorEnemySeeMe : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorEnemySeeMe(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItemToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorItemToKill : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorItemToKill(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItemCanKill
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorItemCanKill : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorItemCanKill(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorFoundItemToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorFoundItemToKill : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorFoundItemToKill(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorFoundAmmo
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorFoundAmmo : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorFoundAmmo(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorReadyToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorReadyToKill : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorReadyToKill(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorReadyToDetour
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorReadyToDetour : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorReadyToDetour(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorAnomaly
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorAnomaly : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorAnomaly(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorInsideAnomaly
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorInsideAnomaly : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorInsideAnomaly(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorPanic
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorPanic : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorPanic(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorSmartTerrainTask
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorSmartTerrainTask : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorSmartTerrainTask(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemyReached
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorEnemyReached : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorEnemyReached(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorPlayerOnThePath
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorPlayerOnThePath : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorPlayerOnThePath(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemyCriticallyWounded
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorEnemyCriticallyWounded : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorEnemyCriticallyWounded(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};
