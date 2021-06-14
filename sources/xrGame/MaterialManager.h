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
	U16							m_my_material_idx;
	ref_sound					m_step_sound[4];
	CObject						*m_object;
	CPHMovementControl			*m_movement_control;

protected:
	U16							m_last_material_idx;

public:
								CMaterialManager		(CObject *object, CPHMovementControl *movement_control);
	virtual						~CMaterialManager		();
	virtual void				Load					(const char* section);
	virtual void				reinit					();
	virtual void				reload					(const char* section);
	virtual void				set_run_mode			(bool run_mode);
	virtual void				update					(float time_delta, float volume, float step_time, bool standing);
	inline		U16					last_material_idx		() const;
	inline		U16					self_material_idx		() const;
	inline		SGameMtlPair		*get_current_pair		();
};

#include "MaterialManager_inline.h"
