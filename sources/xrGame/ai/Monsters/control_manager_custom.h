#pragma once
#include "control_combase.h"

#include "anim_triple.h"
#include "control_jump.h"
#include "control_rotation_jump.h"
#include "control_melee_jump.h"


class CAnimationSequencer;
class CControlRotationJump;
class CControlRunAttack;
class CControlThreaten;
class CControlCriticalWound;

class CControlManagerCustom : public CControl_ComBase {
	typedef					CControl_ComBase	inherited;

	xr_vector<CObject*>		m_nearest;

	CAnimationSequencer		*m_sequencer;
	CAnimationTriple		*m_triple_anim;

	CControlRotationJump	*m_rotation_jump;
	CControlJump			*m_jump;
	CControlRunAttack		*m_run_attack;
	CControlThreaten		*m_threaten;
	CControlMeleeJump		*m_melee_jump;
	CControlCriticalWound	*m_critical_wound;

	DEFINE_VECTOR			(SControlRotationJumpData, ROT_JUMP_DATA_VEC, ROT_JUMP_DATA_VEC_IT);
	ROT_JUMP_DATA_VEC		m_rot_jump_data;
	
	SControlMeleeJumpData	m_melee_jump_data;

	pcstr					m_threaten_anim;
	f32					m_threaten_time;

public:
					CControlManagerCustom	();
					~CControlManagerCustom	();

	virtual void	reinit					();
	virtual void	on_event				(ControlCom::EEventType, ControlCom::IEventData*);
	virtual void	on_start_control		(ControlCom::EControlType type);
	virtual void	on_stop_control			(ControlCom::EControlType type);
	virtual void	update_frame			();
	virtual void	update_schedule			();

			void	add_ability				(ControlCom::EControlType);

	//-------------------------------------------------------------------------------
	// Sequencer
	void		seq_init				();
	void		seq_add					(MotionID motion);
	void		seq_switch				();					// ������� � ��������� ���������, ���� �������� �� ������� - ���������
	void		seq_run					(MotionID motion);

	//-------------------------------------------------------------------------------
	// Triple Animation
	void		ta_activate				(const SAnimationTripleData &data);
	void		ta_pointbreak			();
	void		ta_fill_data			(SAnimationTripleData &data, pcstr s1, pcstr s2, pcstr s3, bool execute_once, bool skip_prep, u32 capture_type = ControlCom::eCaptureDir | ControlCom::eCapturePath | ControlCom::eCaptureMovement);
	bool		ta_is_active			();
	bool		ta_is_active			(const SAnimationTripleData &data);
	void		ta_deactivate			();
	
	//-------------------------------------------------------------------------------
	// Jump
	void		jump					(CObject *obj, const SControlJumpData &ta);
	void		jump					(const SControlJumpData &ta);
	void		jump(const fVector3& position);
	void		load_jump_data			(pcstr s1, pcstr s2, pcstr s3, pcstr s4, u32 vel_mask_prepare, u32 vel_mask_ground, u32 flags);
	
	void		script_jump(const fVector3& position, f32 factor);

	//-------------------------------------------------------------------------------
	// Rotation Jump
	void		add_rotation_jump_data	(pcstr left1, pcstr left2, pcstr right1, pcstr right2, f32 angle, u32 flags = 0);
	void		add_melee_jump_data		(pcstr left, pcstr right);

	//-------------------------------------------------------------------------------
	// Threaten Animation
	void		set_threaten_data		(pcstr anim, f32 time) {m_threaten_anim = anim; m_threaten_time = time;}

	void		critical_wound			(pcstr anim);

private:
	void		check_attack_jump		();
	void		check_jump_over_physics	();
	void		check_rotation_jump		();
	void		check_melee_jump		();
	void		check_run_attack		();
	void		check_threaten			();

	void		fill_rotation_data		(SControlRotationJumpData &data, pcstr left1, pcstr left2, pcstr right1, pcstr right2, f32 angle, u32 flags);
};
