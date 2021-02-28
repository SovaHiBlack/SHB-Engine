// Antirad - таблетки выводящие радиацию

#pragma once

#include "EatableItemObject.h"

class CAntirad : public CEatableItemObject
{
private:
	using inherited = CEatableItemObject;

public:
					CAntirad			( );
	virtual			~CAntirad			( );
	virtual	BOOL	net_Spawn			(CSE_Abstract* DC);
	virtual	void	Load				(const char* section);
	virtual	void	net_Destroy			( );
	virtual	void	shedule_Update		(unsigned int dt);
	virtual	void	UpdateCL			( );
	virtual	void	renderable_Render	( );
	virtual	void	OnH_A_Chield		( );
	virtual	void	OnH_B_Independent	(bool just_before_destroy);
};
