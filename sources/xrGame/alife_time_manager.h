////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_time_manager.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife time manager class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_interfaces.h"
#include "alife_space.h"

class CALifeTimeManager {
private:
	ALife::_TIME_ID					m_game_time;
	f32							m_time_factor;
	f32							m_normal_time_factor;
	u32								m_start_time;

private:
	ALife::_TIME_ID					m_start_game_time;

public:
									CALifeTimeManager		(pcstr		section);
	virtual							~CALifeTimeManager		();
	virtual void					save					(IWriter	&memory_stream);
	virtual void					load					(IReader	&file_stream);
			void					init					(pcstr		section);
	IC		void					set_time_factor			(f32		time_factor);
	IC		ALife::_TIME_ID			game_time				() const;
	IC		f32					time_factor				() const;
	IC		f32					normal_time_factor		() const;
};

#include "alife_time_manager_inline.h"