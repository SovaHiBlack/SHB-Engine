////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_property_evaluators.h
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger property evaluators classes
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
// CStalkerPropertyEvaluatorDangers
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangers : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorDangers(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerUnknown
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerUnknown : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorDangerUnknown(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerInDirection
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerInDirection : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorDangerInDirection(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerWithGrenade
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerWithGrenade : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorDangerWithGrenade(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerBySound
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerBySound : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorDangerBySound(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerUnknownCoverActual
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerUnknownCoverActual : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

private:
	fVector3				m_cover_selection_position;

public:
	CStalkerPropertyEvaluatorDangerUnknownCoverActual(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorDangerGrenadeExploded
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorDangerGrenadeExploded : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorDangerGrenadeExploded(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorGrenadeToExplode
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorGrenadeToExplode : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorGrenadeToExplode(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemyWounded
//////////////////////////////////////////////////////////////////////////

class CStalkerPropertyEvaluatorEnemyWounded : public CStalkerPropertyEvaluator
{
protected:
	typedef CStalkerPropertyEvaluator inherited;

public:
	CStalkerPropertyEvaluatorEnemyWounded(CStalker* object = 0, pcstr evaluator_name = "");
	virtual _value_type	evaluate( );
};
