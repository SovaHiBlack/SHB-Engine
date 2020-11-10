///////////////////////////////////////////////////////////////
// MercuryBallArtefact.h	Ртутный шар
// MercuryBallArtefact - переливающийся и колыхающийся шар
// перекатывается с места на место
///////////////////////////////////////////////////////////////

#pragma once

#include "Artefact.h"

class CMercuryBallArtefact : public CArtefact
{
private:
	typedef CArtefact inherited;

public:
	CMercuryBallArtefact( );
	virtual ~CMercuryBallArtefact( );

	virtual void Load(const char* section);

protected:
	virtual void	UpdateCLChild( );

	//время последнего обновления поведения шара
	ALife::_TIME_ID m_timeLastUpdate;
	//время между апдейтами
	ALife::_TIME_ID m_timeToUpdate;

	//диапазон импульсов катания шара
	float m_fImpulseMin;
	float m_fImpulseMax;
};
