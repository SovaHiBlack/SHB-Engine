////////////////////////////////////////////////////////////////////////////
//	Module 		: MaterialManager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Material manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamemtllib.h"

class CPHMovementControl;

class CMaterialManager {
private:
	bool						m_run_mode;
	float						m_time_to_step;
	u32							m_step_id;
	unsigned short							m_my_material_idx;
	ref_sound					m_step_sound[4];
	CObject						*m_object;
	CPHMovementControl			*m_movement_control;

protected:
	unsigned short							m_last_material_idx;

public:
								CMaterialManager		(CObject *object, CPHMovementControl *movement_control);
	virtual						~CMaterialManager		();
	virtual void				Load					(const char* section);
	virtual void				reinit					();
	virtual void				reload					(const char* section);
	virtual void				set_run_mode			(bool run_mode);
	virtual void				update					(float time_delta, float volume, float step_time, bool standing);
	inline		unsigned short					last_material_idx		() const;
	inline		unsigned short					self_material_idx		() const;
	inline		SGameMtlPair		*get_current_pair		();
};

#include "MaterialManager_inline.h"
