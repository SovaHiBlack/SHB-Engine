#pragma once

#include "control_combase.h"

struct SControlCriticalWoundData : public ControlCom::IComData
{
	const char* animation;
};

class CControlCriticalWound : public CControl_ComCustom<SControlCriticalWoundData>
{
	using inherited = CControl_ComCustom<SControlCriticalWoundData>;

public:
	virtual void	on_event(ControlCom::EEventType, ControlCom::IEventData*);
	virtual void	activate( );
	virtual void	on_release( );
	virtual bool	check_start_conditions( );
};
