///////////////////////////////////////////////////////////////
// FadedBallArtefact.h
// FadedBallArtefact - �������� ������� ���
///////////////////////////////////////////////////////////////

#pragma once
#include "Artefact.h"

class CFadedBallArtefact : public CArtefact
{
private:
	typedef CArtefact inherited;

public:
	CFadedBallArtefact( );
	virtual ~CFadedBallArtefact( );

	virtual void Load(pcstr section);
};
