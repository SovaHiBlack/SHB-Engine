//	Module 		: ScriptEntity.h
//	Description : Script entity class

#pragma once

#include "ScriptEntity_space.h"
#include "..\ENGINE\skeletonanimated.h"

class CSE_Abstract;
class CGameObject;
class CScriptEntityAction;
class CEntity;
class CScriptGameObject;
class CCustomMonster;

using namespace ScriptEntity;

class CScriptEntity
{
public:
	struct CSavedSound
	{
		CScriptGameObject* m_lua_game_object;
		int					m_sound_type;
		Fvector3				m_position;
		float				m_sound_power;

		inline				CSavedSound(
			CScriptGameObject* lua_game_object,
			int				sound_type,
			const Fvector3& position,
			float			sound_power
		) :
			m_lua_game_object(lua_game_object),
			m_sound_type(sound_type),
			m_position(position),
			m_sound_power(sound_power)
		{ }
	};

protected:
	using ACTIONS = xr_deque<CScriptEntityAction*>;

private:
	CGameObject* m_object;
	CCustomMonster* m_monster;
	bool						m_initialized;

	bool						m_can_capture;

protected:
	ACTIONS						m_tpActionQueue;
	bool						m_bScriptControl;
	CSharedString					m_caScriptName;
	MotionID					m_tpNextAnimation;
	bool						m_use_animation_movement_controller;
	CScriptEntityAction* m_tpCurrentEntityAction;

public:
	MotionID					m_tpScriptAnimation;

protected:
	ref_sound* m_current_sound;
	xr_vector<CSavedSound>		m_saved_sounds;

public:
	CScriptEntity( );
	virtual						~CScriptEntity( );
	void				init( );
	virtual void				reinit( );
	virtual BOOL				net_Spawn(CSE_Abstract* DC);
	virtual void				net_Destroy( );
	virtual void				shedule_Update(u32				DT);
	virtual void				UpdateCL( );
	virtual CScriptEntity* cast_script_entity( )
	{
		return this;
	}
	virtual DLL_Pure* _construct( );

	const Fmatrix		GetUpdatedMatrix(CSharedString caBoneName, const Fvector3& tPositionOffset, const Fvector3& tAngleOffset);
	void				vfUpdateParticles( );
	void				vfUpdateSounds( );
	virtual void				vfFinishAction(CScriptEntityAction* tpEntityAction);
	virtual void				SetScriptControl(const bool			bScriptControl, CSharedString	caSciptName);
	virtual bool				GetScriptControl( ) const;
	virtual const char* GetScriptControlName( ) const;
	virtual bool				CheckObjectVisibility(const CGameObject* tpObject);
	virtual bool				CheckTypeVisibility(const char* section_name);
	virtual bool				CheckIfCompleted( ) const
	{
		return false;
	}
	virtual CScriptEntityAction* GetCurrentAction( );
	virtual void				AddAction(const CScriptEntityAction* tpEntityAction, bool bHighPriority = false);
	virtual void				ProcessScripts( );
	virtual void				ResetScriptData(void* P = 0);
	virtual void				ClearActionQueue( );
	virtual bool				bfAssignMovement(CScriptEntityAction* tpEntityAction);
	virtual bool				bfAssignWatch(CScriptEntityAction* tpEntityAction);
	virtual bool				bfAssignAnimation(CScriptEntityAction* tpEntityAction);
	virtual bool				bfAssignSound(CScriptEntityAction* tpEntityAction);
	virtual bool				bfAssignParticles(CScriptEntityAction* tpEntityAction);
	virtual bool				bfAssignObject(CScriptEntityAction* tpEntityAction);
	virtual bool				bfAssignMonsterAction(CScriptEntityAction* tpEntityAction);

	virtual void				sound_callback(const CObject* object, int sound_type, const Fvector3& position, float sound_power);

	virtual const char* GetPatrolPathName( );
	bool				bfScriptAnimation( );
	u32					GetActionCount( ) const;
	const CScriptEntityAction* GetActionByIndex(u32 action_index) const;

	virtual CEntity* GetCurrentEnemy( );
	virtual CEntity* GetCurrentCorpse( );
	virtual int					get_enemy_strength( );
	void				process_sound_callbacks( );

	void				set_script_capture(bool val = true)
	{
		m_can_capture = val;
	}
	bool				can_script_capture( )
	{
		return m_can_capture;
	}

	inline		CGameObject& object( ) const;
};

#include "ScriptEntity_inline.h"
