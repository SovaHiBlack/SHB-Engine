#pragma once
#include "../BaseMonster/base_monster.h"
#include "../telekinesis.h"
#include "../energy_holder.h"
#include "../../../script_export_space.h"

class CPhysicsShellHolder;
class CStateManagerPoltergeist;
class CPoltergeisMovementManager;
class CPolterSpecialAbility;

//////////////////////////////////////////////////////////////////////////

class CPoltergeist :	public CBaseMonster ,
						public CTelekinesis,
						public CEnergyHolder {
	
	typedef		CBaseMonster	inherited;
	typedef		CEnergyHolder	Energy;

	friend class CPoltergeisMovementManager;

	f32					m_height;
	bool					m_disable_hide;

	SMotionVel				invisible_vel;

	CPolterSpecialAbility	*m_flame;
	CPolterSpecialAbility	*m_tele;

public:
					CPoltergeist		();
	virtual			~CPoltergeist		();	

	virtual void	Load				(pcstr section);
	virtual void	reload				(pcstr section);
	virtual void	reinit				();

	virtual BOOL	net_Spawn			(CSE_Abstract* DC);
	virtual void	net_Destroy			();
	virtual void	net_Relcase			(CObject *O);

	virtual void	UpdateCL			();
	virtual	void	shedule_Update		(u32 dt);

	virtual void	Die					(CObject* who);

	virtual CMovementManager *create_movement_manager();
	
	virtual void	ForceFinalAnimation	();

	virtual	void	on_activate			();
	virtual	void	on_deactivate		();
	virtual	void	Hit					(SHit* pHDS);

	IC		CPolterSpecialAbility		*ability() {return (m_flame ? m_flame : m_tele);}	
	
	IC		bool	is_hidden			() {return state_invisible;}
	
	// Poltergeist ability
			void	PhysicalImpulse		(const fVector3& position);
			void	StrangeSounds		(const fVector3& position);
			
			ref_sound m_strange_sound;
	
	// Movement
			fVector3 m_current_position;		// Позиция на ноде

	// Dynamic Height
			u32		time_height_updated;
			f32	target_height;

			void	UpdateHeight			();

	// Invisibility
			void	EnableHide				(){m_disable_hide = false;}
			void	DisableHide				(){m_disable_hide = true;}
	
private:
			void	Hide					();
			void	Show					();

public:
#ifdef DEBUG
			virtual CBaseMonster::SDebugInfo show_debug_info();
#endif

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPoltergeist)
#undef script_type_list
#define script_type_list save_type_list(CPoltergeist)

//////////////////////////////////////////////////////////////////////////
// Interface
//////////////////////////////////////////////////////////////////////////

class CPolterSpecialAbility {

	CParticlesObject	*m_particles_object;
	CParticlesObject	*m_particles_object_electro;

	pcstr				m_particles_hidden;
	pcstr				m_particles_damage;
	pcstr				m_particles_death;
	pcstr				m_particles_idle;

	ref_sound			m_sound_base;
	u32					m_last_hit_frame;

protected:
	CPoltergeist		*m_object;	

public:			
					CPolterSpecialAbility		(CPoltergeist *polter);
	virtual			~CPolterSpecialAbility		();

	virtual void	load						(pcstr section);
	virtual void	update_schedule				();
	virtual void	update_frame				();
	virtual void	on_hide						();
	virtual void	on_show						();
	virtual void	on_destroy					(){}
	virtual void	on_die						();
	virtual void	on_hit						(SHit* pHDS);
};

//////////////////////////////////////////////////////////////////////////
// Flame
//////////////////////////////////////////////////////////////////////////
class CPolterFlame : public CPolterSpecialAbility {

	typedef CPolterSpecialAbility inherited;

	ref_sound				m_sound;
	pcstr					m_particles_prepare;
	pcstr					m_particles_fire;
	pcstr					m_particles_stop;
	u32						m_time_fire_delay;
	u32						m_time_fire_play;

	f32					m_length;
	f32					m_hit_value;
	u32						m_hit_delay;

	u32						m_count;
	u32						m_delay;	// between 2 flames

	u32						m_time_flame_started;

	f32					m_min_flame_dist;
	f32					m_max_flame_dist;
	f32					m_min_flame_height;
	f32					m_max_flame_height;

	f32					m_pmt_aura_radius;
	
	// Scanner
	f32					m_scan_radius;
	u32						m_scan_delay_min;
	u32						m_scan_delay_max;
	
	SPPInfo					m_scan_effector_info;
	f32					m_scan_effector_time;
	f32					m_scan_effector_time_attack;
	f32					m_scan_effector_time_release;
	ref_sound				m_scan_sound;

	bool					m_state_scanning;
	u32						m_scan_next_time;

	enum EFlameState {
		ePrepare,
		eFire,
		eStop
	};

public:
	struct SFlameElement {
		const CObject		*target_object;
		fVector3				position;
		fVector3				target_dir;
		u32					time_started;
		ref_sound			sound;
		CParticlesObject	*particles_object;
		EFlameState			state;
		u32					time_last_hit;
	};

private:
	DEFINE_VECTOR			(SFlameElement*, FLAME_ELEMS_VEC, FLAME_ELEMS_IT);
	FLAME_ELEMS_VEC			m_flames;

public:	
					CPolterFlame				(CPoltergeist *polter);
	virtual			~CPolterFlame				();

	virtual void	load						(pcstr section);
	virtual void	update_schedule				();
	virtual void	on_destroy					();
	virtual void	on_die						();

private:
			void	select_state				(SFlameElement *elem, EFlameState state);
			bool	get_valid_flame_position	(const CObject *target_object, fVector3& res_pos);
			void	create_flame				(const CObject *target_object);
};

//////////////////////////////////////////////////////////////////////////
// TELE
//////////////////////////////////////////////////////////////////////////
class CPolterTele : public CPolterSpecialAbility {
	typedef CPolterSpecialAbility inherited;

	xr_vector<CObject*>	m_nearest;

	// external params
	f32				m_pmt_radius;
	f32				m_pmt_object_min_mass;
	f32				m_pmt_object_max_mass;
	u32					m_pmt_object_count;
	u32					m_pmt_time_to_hold;
	u32					m_pmt_time_to_wait;
	u32					m_pmt_time_to_wait_in_objects;
	u32					m_pmt_raise_time_to_wait_in_objects;
	f32				m_pmt_distance;
	f32				m_pmt_object_height;
	u32					m_pmt_time_object_keep;
	f32				m_pmt_raise_speed;
	f32				m_pmt_fly_velocity;

	ref_sound			m_sound_tele_hold;
	ref_sound			m_sound_tele_throw;

	enum ETeleState {
		eStartRaiseObjects,
		eRaisingObjects,
		eFireObjects,
		eWait
	} m_state;

	u32					m_time;
	u32					m_time_next;

public:	
					CPolterTele					(CPoltergeist *polter);
	virtual			~CPolterTele				();

	virtual void	load						(pcstr section);
	virtual void	update_schedule				();

private:
			void	tele_find_objects			(xr_vector<CObject*> &objects, const fVector3& pos);
			bool	tele_raise_objects			();
			void	tele_fire_objects			();

			bool	trace_object				(CObject *obj, const fVector3& target);
};
