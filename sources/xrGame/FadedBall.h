///////////////////////////////////////////////////////////////
// FadedBall.h
// FadedBall - �������� ������� ���
///////////////////////////////////////////////////////////////

#pragma once
#include "Artefact.h"

class CFadedBall : public CArtefact 
{
private:
	typedef CArtefact inherited;
public:
	CFadedBall(void);
	virtual ~CFadedBall(void);

	virtual void Load				(pcstr section);

protected:
};