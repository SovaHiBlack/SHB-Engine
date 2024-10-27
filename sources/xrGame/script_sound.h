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
								CScriptSound		(pcstr caSoundName, ESoundTypes sound_type = SOUND_TYPE_NO_SOUND);
	virtual						~CScriptSound		();
	IC		u32					Length				();
	IC		void				Play				(CScriptGameObject *object);
	IC		void				Play				(CScriptGameObject *object, f32 delay);
			void				Play				(CScriptGameObject *object, f32 delay, int flags);
	IC		void				PlayAtPos			(CScriptGameObject *object, const fVector3& position);
	IC		void				PlayAtPos			(CScriptGameObject *object, const fVector3& position, f32 delay);
			void				PlayAtPos			(CScriptGameObject *object, const fVector3& position, f32 delay, int flags);
			void				PlayNoFeedback		(CScriptGameObject *object,	u32 flags/*!< Looping */, f32 delay/*!< Delay */, fVector3 pos, f32 vol);
	IC		void				Stop				();
	IC		void				StopDeffered		();
	IC		void				SetPosition			(const fVector3& position);
	IC		void				SetFrequency		(f32 frequency);
	IC		void				SetVolume			(f32 volume);
	IC		const CSound_params	*GetParams			();
	IC		void				SetParams			(CSound_params *sound_params);
			void				SetMinDistance		(const f32 fMinDistance);
	IC		void				SetMaxDistance		(const f32 fMaxDistance);
	fVector3				GetPosition			() const;
	IC		const f32			GetFrequency		() const;
	IC		const f32			GetMinDistance		() const;
	IC		const f32			GetMaxDistance		() const;
	IC		const f32			GetVolume			() const;
	IC		bool				IsPlaying			() const;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CScriptSound)
#undef script_type_list
#define script_type_list save_type_list(CScriptSound)

#include "script_sound_inline.h"