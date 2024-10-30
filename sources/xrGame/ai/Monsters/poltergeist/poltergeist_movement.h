#pragma once

#include "../control_path_builder.h"

class CPoltergeist;
class CCustomMonster;

class CPoltergeisMovementManager : public CControlPathBuilder {
	typedef CControlPathBuilder inherited;

	CPoltergeist	*m_monster;

public:
					CPoltergeisMovementManager	(CPoltergeist *monster) : inherited((CCustomMonster*)monster), m_monster(monster) {}
	virtual			~CPoltergeisMovementManager	(){}

	virtual	void	move_along_path				(CPHMovementControl *movement_control, fVector3& dest_position, f32 time_delta);

	fVector3	CalculateRealPosition		();
};
