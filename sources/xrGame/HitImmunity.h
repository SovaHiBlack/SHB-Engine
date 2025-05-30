// HitImmunity.h: класс для тех объектов, которые поддерживают
//				   коэффициенты иммунитета для разных типов хитов
//////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "HitImmunity_space.h"

class CHitImmunity
{
public:
	CHitImmunity();
	virtual ~CHitImmunity();

	virtual void LoadImmunities (pcstr section, CIniFile* ini);

	virtual f32 AffectHit		(f32 power, ALife::EHitType hit_type);

protected:
	//коэффициенты на которые домножается хит
	//при соответствующем типе воздействия
	//(для защитных костюмов и специфичных животных)
	HitImmunity::HitTypeSVec m_HitTypeK;
};