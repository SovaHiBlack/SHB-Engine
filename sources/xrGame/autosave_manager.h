////////////////////////////////////////////////////////////////////////////
//	Module 		: autosave_manager.h
//	Created 	: 04.11.2004
//  Modified 	: 04.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Autosave manager
////////////////////////////////////////////////////////////////////////////

#pragma once

class CAutosaveManager : public ISheduled {
private:
	typedef ISheduled inherited;

private:
	u32				m_autosave_interval;
	u32				m_last_autosave_time;
	u32				m_delay_autosave_interval;
	u32				m_not_ready_count;

public:
						CAutosaveManager		();
	virtual				~CAutosaveManager		();
	virtual	CSharedString	shedule_Name			() const		{ return CSharedString("autosave_manager"); }
	virtual	void		shedule_Update			(u32 dt);
	virtual float		shedule_Scale			();
	virtual bool		shedule_Needed			()				{return true;};

public:
	inline		u32		autosave_interval		() const;
	inline		u32		last_autosave_time		() const;
	inline		u32		not_ready_count			() const;
	inline		void	inc_not_ready			();
	inline		void	dec_not_ready			();
	inline		void	update_autosave_time	();
	inline		void	delay_autosave			();
	inline		bool	ready_for_autosave		();
};

#include "autosave_manager_inline.h"