#pragma once

#include "control_combase.h"
#include "ai_monster_defs.h"

struct SEventVelocityBounce : public ControlCom::IEventData {
	F32	m_ratio;

	IC		SEventVelocityBounce(F32 ratio) : m_ratio(ratio) {}
};


//////////////////////////////////////////////////////////////////////////
class CControlAnimationBase : public CControl_ComBase {
		typedef CControl_ComBase inherited;
protected:

		REPLACED_ANIM			m_tReplacedAnims;	// анимации подмены

		// сохранённые анимации 
		EMotionAnim				prev_motion; 

		// исправления сосояния 'бега на месте'
		TTime					time_start_stand;

		// работа с анимациями атаки
		TTime					aa_time_last_attack;	// время последнего нанесения хита

		// -------------------------------------------------------------------------
		u32						spec_params;			// дополнительные параметры

		TTime					fx_time_last_play;

		// -------------------------------------------------------------------------------------
		// Acceleration

		struct {
			bool					active;				
			bool					enable_braking;	// не использовать при торможении

			EAccelType				type;	

			F32					calm;
			F32					aggressive;

			VELOCITY_CHAIN_VEC		chain;
		} m_accel;

		// ---------------------------------------------------------------------------------------

		EMotionAnim				spec_anim; 

		MOTION_ITEM_MAP			m_tMotions;			// карта соответсвий EAction к SMotionItem
		TRANSITION_ANIM_VECTOR	m_tTransitions;		// вектор переходов из одной анимации в другую

		t_fx_index				default_fx_indexes;
		FX_MAP_STRING			fx_map_string;
		FX_MAP_U16				fx_map_u16;
		bool					map_converted;

		AA_VECTOR				m_attack_anims;


		bool					m_state_attack;

protected:

	ANIM_TO_MOTION_MAP			m_anim_motion_map;
	
	ANIM_ITEM_VECTOR			m_anim_storage;			
	void						init_anim_storage	();
	void						free_anim_storage	();

public:

	EAction					m_tAction;

	F32					m_prev_character_velocity;

public:
				CControlAnimationBase	();
	virtual		~CControlAnimationBase	();

	// Control_ComBase interface
	virtual void		reinit			();
	virtual void		on_event		(ControlCom::EEventType, ControlCom::IEventData*);	
	virtual void		on_start_control(ControlCom::EControlType type);
	virtual void		on_stop_control	(ControlCom::EControlType type);
	virtual void		update_frame	();

			void		ScheduledInit	();

	// создание карты анимаций (выполнять на Monster::Load)
	void		AddAnim					(EMotionAnim ma, pcstr tn, int s_id, SVelocityParam *vel, EPState p_s);
	void		AddAnim					(EMotionAnim ma, pcstr tn, int s_id, SVelocityParam *vel, EPState p_s, pcstr fx_front, pcstr fx_back, pcstr fx_left, pcstr fx_right);

	// -------------------------------------

	// добавить анимацию перехода (A - Animation, S - Position)
	void		AddTransition			(EMotionAnim from,	EMotionAnim to, EMotionAnim trans, bool chain, bool skip_aggressive = false);
	void		AddTransition			(EMotionAnim from,	EPState to,		EMotionAnim trans, bool chain, bool skip_aggressive = false);
	void		AddTransition			(EPState from,		EMotionAnim to, EMotionAnim trans, bool chain, bool skip_aggressive = false);
	void		AddTransition			(EPState from,		EPState to,		EMotionAnim trans, bool chain, bool skip_aggressive = false);

	// -------------------------------------

	void		LinkAction				(EAction act, EMotionAnim pmt_motion, EMotionAnim pmt_left, EMotionAnim pmt_right, F32 pmt_angle);
	void		LinkAction				(EAction act, EMotionAnim pmt_motion);

	// -------------------------------------

	void		AddReplacedAnim			(bool *b_flag, EMotionAnim pmt_cur_anim, EMotionAnim pmt_new_anim);

	// -------------------------------------
	bool		CheckTransition			(EMotionAnim from, EMotionAnim to);

	void		SetSpecParams			(u32 param) {spec_params |= param;}
	void		SetCurAnim				(EMotionAnim a) {cur_anim_info().motion = a;}
	EMotionAnim	GetCurAnim				() {return  cur_anim_info().motion;} 

	// работа с анимациями атак
	void		AA_reload				(pcstr section);
	SAAParam	&AA_GetParams			(pcstr anim_name);
	SAAParam	&AA_GetParams			(MotionID motion, F32 time_perc);

	// FX's
	void		FX_Play					(EHitSide side, F32 amount);

	MotionID	get_motion_id			(EMotionAnim a, u32 index = u32(-1));

protected:	

	void		UpdateAnimCount			();

	// работа с анимациями атак
	void		AA_Clear				(); 
	void		AA_SwitchAnimation		(EMotionAnim a, u32 i3);

	// дополнительные функции
	EPState		GetState				(EMotionAnim a);
	void		CheckReplacedAnim		();

	CMotionDef	*get_motion_def			(SAnimItem *it, u32 index);

public:
	F32		GetAnimSpeed			(EMotionAnim anim);
	bool		IsStandCurAnim			();
	bool		IsTurningCurAnim		();
	void		ValidateAnimation		();
	
	//////////////////////////////////////////////////////////////////////////
protected:
	void		update					();

protected:
	void		SelectAnimation			();
	void		SelectVelocities		();

	EAction		GetActionFromPath		();
	EAction		VelocityIndex2Action	(u32 velocity_index);


	void		stop_now				();

	//////////////////////////////////////////////////////////////////////////
	// DEBUG

protected:
	pcstr		GetAnimationName		(EMotionAnim anim);
	pcstr		GetActionName			(EAction action);

	// end DEBUG
	//////////////////////////////////////////////////////////////////////////

public:
	//-------------------------------------------------------------------------------
	// Acceleration

	void	accel_init				();
	void	accel_load				(pcstr section);

	void	accel_activate			(EAccelType type);
	IC	void	accel_deactivate		() {m_accel.active = false;	m_accel.enable_braking = false;}
	IC	void	accel_set_braking		(bool val = true) {m_accel.enable_braking = val;}

	F32	accel_get				(EAccelValue val = eAV_Accel);

	IC	bool	accel_active			(EAccelValue val = eAV_Accel) {return (val == eAV_Accel) ? m_accel.active : m_accel.enable_braking;}

	void	accel_chain_add			(EMotionAnim anim1, EMotionAnim anim2);
	bool	accel_chain_get			(F32 cur_speed, EMotionAnim target_anim, EMotionAnim &new_anim, F32& a_speed);
	bool	accel_chain_test		();

	bool	accel_check_braking		(F32 before_interval, F32 nominal_speed);
	bool	braking_mode;

	// --------------------------------------------------------------------------------	

	void	CheckVelocityBounce		();

	// Other
	void	SetTurnAnimation		();

	// MotionDef to animation name translation
	void		AddAnimTranslation		(const MotionID &motion, pcstr str);
	shared_str	GetAnimTranslation		(const MotionID &motion);
public:

	// информация о текущей анимации
	SCurrentAnimationInfo	m_cur_anim;
	SCurrentAnimationInfo	&cur_anim_info() {return m_cur_anim;}

	void					select_animation	(bool anim_end = false);
	void					set_animation_speed	();

	void					check_hit			(MotionID motion, F32 time_perc);
};

