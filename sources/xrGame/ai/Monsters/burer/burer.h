#pragma once
#include "../BaseMonster/base_monster.h"
#include "../telekinesis.h"
#include "../anim_triple.h"
#include "../scanning_ability.h"
#include "../../../script_export_space.h"

class CCharacterPhysicsSupport;
class CBurerFastGravi;

class CBurer :	public CBaseMonster,
				public CTelekinesis,
				public CScanningAbility<CBurer> {

	typedef		CBaseMonster				inherited;

private:
	xr_vector<CObject*>	m_nearest;

public:
	typedef		CScanningAbility<CBurer>	TScanner;


	static		bool	can_scan;

				u32		last_hit_frame;
				u32		time_last_scan;
	

	typedef		CTelekinesis				TTelekinesis;

	struct	GraviObject {
		bool		active;
		fVector3		cur_pos;
		fVector3		target_pos;
		fVector3		from_pos;

		u32			time_last_update;

		const CEntityAlive *enemy;
		
		GraviObject() {
			active = false;
			enemy = 0;
		}
		
		
		void		activate(const CEntityAlive *e, const fVector3& cp, const fVector3& tp) {
			active				= true;
			from_pos			= cp;
			cur_pos				= cp;
			target_pos			= tp;
			time_last_update	= Device.dwTimeGlobal;
			enemy				= e;
		}

		void		deactivate() {
			active = false;
		}

	} m_gravi_object;

	pcstr	 particle_gravi_wave;
	pcstr   particle_gravi_prepare;
	pcstr	 particle_tele_object;

	//////////////////////////////////////////////////////////////////////////
	// Sounds
	ref_sound	sound_gravi_wave;
	ref_sound	sound_scan;
	
	ref_sound	sound_tele_hold;
	ref_sound	sound_tele_throw;

	enum EBurerSounds {
		eAdditionalSounds			= MonsterSound::eMonsterSoundCustom,

		eMonsterSoundGraviAttack	= eAdditionalSounds | 0,
		eMonsterSoundTeleAttack		= eAdditionalSounds | 1,
	};	
	//////////////////////////////////////////////////////////////////////////

	u32		m_gravi_speed;
	u32		m_gravi_step;
	u32		m_gravi_time_to_hold;
	f32	m_gravi_radius;
	f32	m_gravi_impulse_to_objects;
	f32	m_gravi_impulse_to_enemy;
	f32	m_gravi_hit_power;

	u32		m_tele_max_handled_objects;
	u32		m_tele_time_to_hold;
	f32	m_tele_object_min_mass;
	f32	m_tele_object_max_mass;
	f32	m_tele_find_radius;

	bool	m_shield_active;
	pcstr	particle_fire_shield;

	CBurerFastGravi	*m_fast_gravi;

public:
					CBurer				();
	virtual			~CBurer				();

	virtual void	reinit				();
	virtual void	reload				(pcstr section);

	virtual void	Load				(pcstr section);

	virtual void	net_Destroy			();
	virtual void	net_Relcase			(CObject *O);
	virtual	void	shedule_Update		(u32 dt);
	virtual void	UpdateCL			();
	virtual	void	Hit					(SHit* pHDS);
	virtual void	Die					(CObject* who);
			void	ProcessTurn			();
	virtual void	CheckSpecParams		(u32 spec_params);

			void	UpdateGraviObject	();

			void	StartGraviPrepare	();
			void	StopGraviPrepare	();

			void	StartTeleObjectParticle(CGameObject *pO);
			void	StopTeleObjectParticle(CGameObject *pO);

			void	ActivateShield		() {m_shield_active = true;}
			void	DeactivateShield	() {m_shield_active = false;}

	virtual bool	ability_distant_feel() {return true;}

	virtual void	on_scanning			();
	virtual void	on_scan_success		();

public:
	SAnimationTripleData	anim_triple_gravi;
	SAnimationTripleData	anim_triple_tele;


#ifdef DEBUG
	virtual CBaseMonster::SDebugInfo show_debug_info();
#endif


	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CBurer)
#undef script_type_list
#define script_type_list save_type_list(CBurer)
