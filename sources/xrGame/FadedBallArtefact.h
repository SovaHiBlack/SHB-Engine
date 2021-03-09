// FadedBallArtefact.h
// FadedBallArtefact - артефакт блеклый шар

#pragma once

#include "Artefact.h"

class CFadedBallArtefact : public CArtefact
{
private:
	using inherited = CArtefact;

public:
	CFadedBallArtefact( );
	virtual ~CFadedBallArtefact( );

	virtual void Load(const char* section);
};
