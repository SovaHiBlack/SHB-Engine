///////////////////////////////////////////////////////////////
// MercuryBall.h
// MercuryBall - переливающийся и колыхающийся шар
// перекатывается с места на место
///////////////////////////////////////////////////////////////

#pragma once
#include "Artefact.h"

class CMercuryBall : public CArtefact
{
private:
	typedef CArtefact inherited;

public:
	CMercuryBall( );
	virtual ~CMercuryBall( );

	virtual void Load(pcstr section);

protected:
	virtual void	UpdateCLChild( );

	//время последнего обновления поведения шара
	ALife::_TIME_ID m_timeLastUpdate;
	//время между апдейтами
	ALife::_TIME_ID m_timeToUpdate;

	//диапазон импульсов катания шара
	f32 m_fImpulseMin;
	f32 m_fImpulseMax;
};
