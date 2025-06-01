////////////////////////////////////////////////////////////////////////////
//	Module 		: enemy_manager.h
//	Created 	: 30.12.2003
//  Modified 	: 30.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Enemy manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_manager.h"
#include "EntityAlive.h"
#include "custommonster.h"
#include "script_callback_ex.h"

class CStalker;

class CEnemyManager : public CObjectManager<const CEntityAlive> {
public:
	typedef CObjectManager<const CEntityAlive>	inherited;
	typedef OBJECTS								ENEMIES;
	typedef CScriptCallbackEx<bool>				USEFULE_CALLBACK;

private:
	CCustomMonster				*m_object;
	CStalker* m_stalker;
	f32							m_ignore_monster_threshold;
	f32							m_max_ignore_distance;
	mutable bool				m_ready_to_save;
	u32							m_last_enemy_time;
	const CEntityAlive			*m_last_enemy;
	USEFULE_CALLBACK			m_useful_callback;
	bool						m_enable_enemy_change;

private:
	u32							m_last_enemy_change;

private:
	IC		bool				enemy_inertia		(const CEntityAlive *previous_enemy) const;
			bool				need_update			(const bool &only_wounded) const;
			void				process_wounded		(bool &only_wounded);
			bool				change_from_wounded	(const CEntityAlive *current, const CEntityAlive *previous) const;
			void				remove_wounded		();
			void				try_change_enemy	();

protected:
			void				on_enemy_change		(const CEntityAlive *previous_enemy);
			bool				expedient			(const CEntityAlive *object) const;

public:
								CEnemyManager		(CCustomMonster *object);
	virtual void				reload				(pcstr section);
	virtual bool				useful				(const CEntityAlive *object) const;
	virtual bool				is_useful			(const CEntityAlive *object) const;
	virtual	f32					evaluate			(const CEntityAlive *object) const;
	virtual	f32					do_evaluate			(const CEntityAlive *object) const;
	virtual void				update				();
	virtual void				set_ready_to_save	();
	IC		u32					last_enemy_time		() const;
	IC		const CEntityAlive	*last_enemy			() const;
	IC		USEFULE_CALLBACK	&useful_callback	();
			void				remove_links		(CObject *object);

public:
			void				ignore_monster_threshold			(const f32& ignore_monster_threshold);
			void				restore_ignore_monster_threshold	();
			f32					ignore_monster_threshold			() const;
			void				max_ignore_monster_distance			(const f32& max_ignore_monster_distance);
			void				restore_max_ignore_monster_distance	();
			f32					max_ignore_monster_distance			() const;

public:
			void				wounded				(const CEntityAlive *wounded_enemy);
	IC		const CEntityAlive	*wounded			() const;

public:
	IC		void				enable_enemy_change	(const bool &value);
	IC		bool				enable_enemy_change	() const;
};

#include "enemy_manager_inline.h"