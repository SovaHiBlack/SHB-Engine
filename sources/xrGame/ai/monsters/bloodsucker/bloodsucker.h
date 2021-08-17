#pragma once

#include "../BaseMonster/BaseMonster.h"
#include "../ai_monster_bones.h"
#include "../controlled_actor.h"
#include "../anim_triple.h"
#include "../../../script_export_space.h"
#include "BloodsuckerAlien.h"

class CBloodsucker : public CBaseMonster, public CControlledActor
{
	using inherited = CBaseMonster;
	
public:
	CBloodsucker();
	virtual					~CBloodsucker();

	virtual void			reinit					();
	virtual	void			reload					(const char* section);

	virtual void			UpdateCL				();
	virtual void			shedule_Update			(u32 dt);
	virtual void			Die						(CObject* who);
	virtual BOOL			net_Spawn				(CSE_Abstract* DC);
	virtual	void			Load					(const char* section);

	virtual	void			CheckSpecParams			(u32 spec_params);
	virtual bool			ability_invisibility	() {return true;}
	virtual bool			ability_pitch_correction() {return false;}
	virtual	void			post_fsm_update			();
	
	virtual bool			use_center_to_aim		() const {return true;}
	virtual bool			check_start_conditions	(ControlCom::EControlType);
	virtual void			HitEntity				(const CEntity *pEntity, float fDamage, float impulse, Fvector3& dir);
	
	//--------------------------------------------------------------------
	// Utils
	//--------------------------------------------------------------------
			void			move_actor_cam			();

	//--------------------------------------------------------------------
	// Bones
	//--------------------------------------------------------------------
private:
	static	void			BoneCallback			(CBoneInstance *B);
			void			vfAssignBones			();
			void			LookDirection			(Fvector3 to_dir, float bone_turn_speed);

	bonesManipulation		Bones;

	CBoneInstance			*bone_spine;
	CBoneInstance			*bone_head;

	//--------------------------------------------------------------------
	// Invisibility
	//--------------------------------------------------------------------
	SMotionVel				invisible_vel;
	const char* invisible_particle_name;

public:
			void			start_invisible_predator	();
			void			stop_invisible_predator		();

	//--------------------------------------------------------------------
	// Vampire
	//--------------------------------------------------------------------
	u32						m_vampire_min_delay;
	SAnimationTripleData	anim_triple_vampire;

	SPPInfo					pp_vampire_effector;

			
			void			ActivateVampireEffector	();
			inline		bool			WantVampire				() {return (fsimilar(m_vampire_want_value,1.f) == TRUE);}
			inline		void			SatisfyVampire			() {m_vampire_want_value = 0.f;}

private:
	float					m_vampire_want_value;
	float					m_vampire_want_speed;		// load from ltx
	float					m_vampire_wound;
	
			void			LoadVampirePPEffector	(const char* section);

	//--------------------------------------------------------------------
	// Alien
	//--------------------------------------------------------------------
public:
	CBloodsuckerAlien		m_alien_control;
	u32						m_time_lunge;

			void			set_alien_control		(bool val);

	//--------------------------------------------------------------------
	// Predator
	//--------------------------------------------------------------------
			CSharedString				m_visual_default;
	const char* m_visual_predator;
	bool					m_predator;
			
			void			predator_start			();
			void			predator_stop			();
			void			predator_freeze			();
			void			predator_unfreeze		();
	
	//--------------------------------------------------------------------
	// Sounds
	//--------------------------------------------------------------------
	enum EBloodsuckerSounds 
	{
		eAdditionalSounds		= MonsterSound::eMonsterSoundCustom,

		eVampireGrasp			= eAdditionalSounds | 0,
		eVampireSucking			= eAdditionalSounds | 1,
		eVampireHit				= eAdditionalSounds | 2,
		eVampireStartHunt		= eAdditionalSounds | 3,

		eGrowl					= eAdditionalSounds | 5,

		eChangeVisibility		= eAdditionalSounds | 6,
		
		eAlien					= eAdditionalSounds | 7
	};

	//--------------------------------------------------------------------
			void	set_manual_control	(bool value) {}
			void	manual_activate		();
			void	manual_deactivate	();

#ifdef DEBUG
	virtual CBaseMonster::SDebugInfo show_debug_info();

#ifdef _DEBUG
			void debug_on_key						(int key);
#endif

#endif

public:
	static void script_register(lua_State*);
};

add_to_type_list(CBloodsucker)
#undef script_type_list
#define script_type_list save_type_list(CBloodsucker)
