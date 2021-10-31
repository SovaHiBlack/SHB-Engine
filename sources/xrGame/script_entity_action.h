////////////////////////////////////////////////////////////////////////////
//	Module 		: script_entity_action.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script entity action class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"
#include "script_movement_action.h"
#include "script_watch_action.h"
#include "ScriptAnimationAction.h"
#include "script_sound_action.h"
#include "script_particle_action.h"
#include "script_object_action.h"
#include "ScriptActionCondition.h"
#include "script_monster_action.h"

class CScriptEntityAction {
public:
	CScriptMovementAction	m_tMovementAction;
	CScriptWatchAction		m_tWatchAction;
	CScriptAnimationAction	m_tAnimationAction;
	CScriptSoundAction		m_tSoundAction;
	CScriptParticleAction	m_tParticleAction;
	CScriptObjectAction		m_tObjectAction;
	CScriptActionCondition	m_tActionCondition;
	CScriptMonsterAction	m_tMonsterAction;
	void					*m_user_data;
	bool					m_started;

public:
	inline				CScriptEntityAction					();
	inline				CScriptEntityAction					(const CScriptEntityAction *entity_action);
	virtual			~CScriptEntityAction				();
	template<typename T>
	inline		void	SetAction							(const T &t, T &tt);
	inline		void	SetAction							(CScriptMovementAction &tMovementAction);
	inline		void	SetAction							(CScriptWatchAction &tWatchAction);
	inline		void	SetAction							(CScriptAnimationAction &tAnimationAction);
	inline		void	SetAction							(CScriptSoundAction &tSoundAction);
	inline		void	SetAction							(CScriptParticleAction &tParticleAction);
	inline		void	SetAction							(CScriptObjectAction &tObjectAction);
	inline		void	SetAction							(CScriptActionCondition &tActionCondition);
	inline		void	SetAction							(CScriptMonsterAction &tMonsterAction);
	inline		void	SetAction							(void *user_data);
	inline		bool	CheckIfActionCompleted				(const CScriptAbstractAction &tAbstractAction) const;
	inline		bool	CheckIfMovementCompleted			() const;
	inline		bool	CheckIfWatchCompleted				() const;
	inline		bool	CheckIfAnimationCompleted			() const;
	inline		bool	CheckIfSoundCompleted				() const;
	inline		bool	CheckIfParticleCompleted			() const;
	inline		bool	CheckIfObjectCompleted				() const;
	inline		bool	CheckIfMonsterActionCompleted		() const;
	inline		bool	CheckIfTimeOver						();
	inline		bool	CheckIfActionCompleted				();
	inline		void	initialize							();

public:
	inline		const CScriptMovementAction		&move		();
	inline		const CScriptWatchAction		&look		();
	inline		const CScriptAnimationAction	&anim		();
	inline		const CScriptParticleAction		&particle	();
	inline		const CScriptObjectAction		&object		();
	inline		const CScriptActionCondition	&cond		();
	inline		void							*data		();

public:
	static void script_register(lua_State*);
};

add_to_type_list(CScriptEntityAction)
#undef script_type_list
#define script_type_list save_type_list(CScriptEntityAction)

#include "script_entity_action_inline.h"
