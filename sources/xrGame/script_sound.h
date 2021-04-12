////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound.h
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"
#include "ai_sounds.h"

class CScriptGameObject;

class CScriptSound {
	mutable ref_sound			m_sound;
	shared_str					m_caSoundToPlay;

	friend class CScriptSoundAction;

public:
								CScriptSound		(const char* caSoundName, ESoundTypes sound_type = SOUND_TYPE_NO_SOUND);
	virtual						~CScriptSound		();
	inline		u32					Length				();
	inline		void				Play				(CScriptGameObject *object);
	inline		void				Play				(CScriptGameObject *object, float delay);
			void				Play				(CScriptGameObject *object, float delay, int flags);
	inline		void				PlayAtPos			(CScriptGameObject *object, const Fvector3& position);
	inline		void				PlayAtPos			(CScriptGameObject *object, const Fvector3& position, float delay);
			void				PlayAtPos			(CScriptGameObject *object, const Fvector3& position, float delay, int flags);
			void				PlayNoFeedback		(CScriptGameObject *object,	u32 flags/*!< Looping */, float delay/*!< Delay */, Fvector3 pos, float vol);
	inline		void				Stop				();
	inline		void				StopDeffered		();
	inline		void				SetPosition			(const Fvector3& position);
	inline		void				SetFrequency		(float frequency);
	inline		void				SetVolume			(float volume);
	inline		const CSound_params	*GetParams			();
	inline		void				SetParams			(CSound_params *sound_params);
			void				SetMinDistance		(const float fMinDistance);
	inline		void				SetMaxDistance		(const float fMaxDistance);
	Fvector3				GetPosition			() const;
	inline		const float			GetFrequency		() const;
	inline		const float			GetMinDistance		() const;
	inline		const float			GetMaxDistance		() const;
	inline		const float			GetVolume			() const;
	inline		bool				IsPlaying			() const;

public:
	static void script_register(lua_State*);
};

add_to_type_list(CScriptSound)
#undef script_type_list
#define script_type_list save_type_list(CScriptSound)

#include "script_sound_inline.h"
