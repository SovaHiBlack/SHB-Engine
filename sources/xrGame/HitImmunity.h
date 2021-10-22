// hit_immunity.h: класс для тех объектов, которые поддерживают коэффициенты иммунитета для разных типов хитов

#pragma once

#include "ALife_space.h"
#include "HitImmunity_space.h"

class CHitImmunity
{
public:
	CHitImmunity( );
	virtual ~CHitImmunity( );

	virtual void LoadImmunities(const char* section, CConfigurationFile* ini);

	virtual float AffectHit(float power, ALife::EHitType hit_type);

protected:
	//коэффициенты на которые домножается хит при соответствующем типе воздействия (для защитных костюмов и специфичных животных)
	HitImmunity::HitTypeSVec m_HitTypeK;
};
