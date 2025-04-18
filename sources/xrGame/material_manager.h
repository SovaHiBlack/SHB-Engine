////////////////////////////////////////////////////////////////////////////
//	Module 		: material_manager.h
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
	f32						m_time_to_step;
	u32							m_step_id;
	u16							m_my_material_idx;
	ref_sound					m_step_sound[4];
	CObject						*m_object;
	CPHMovementControl			*m_movement_control;

protected:
	u16							m_last_material_idx;

public:
								CMaterialManager		(CObject *object, CPHMovementControl *movement_control);
	virtual						~CMaterialManager		();
	virtual void				Load					(pcstr section);
	virtual void				reinit					();
	virtual void				reload					(pcstr section);
	virtual void				set_run_mode			(bool run_mode);
	virtual void				update					(f32 time_delta, f32 volume, f32 step_time, bool standing);
	IC		u16					last_material_idx		() const;
	IC		u16					self_material_idx		() const;
	IC		SGameMtlPair		*get_current_pair		();
};

#include "material_manager_inline.h"