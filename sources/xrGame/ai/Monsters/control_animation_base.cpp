#include "stdafx.h"
#include "control_animation_base.h"
#include "control_direction_base.h"
#include "control_movement_base.h"
#include "BaseMonster/base_monster.h"
#include "../../PHMovementControl.h"
#include "anim_triple.h"
#include "../../..\XR_3DA\skeletonanimated.h"
#include "../../detail_path_manager.h"
#include "monster_velocity_space.h"
#include "monster_event_manager.h"
#include "control_jump.h"
#include "../../sound_player.h"

// DEBUG purpose only
pstr dbg_action_name_table[] = {
	"ACT_STAND_IDLE",
		"ACT_SIT_IDLE",
		"ACT_LIE_IDLE",
		"ACT_WALK_FWD",
		"ACT_WALK_BKWD",
		"ACT_RUN",
		"ACT_EAT",
		"ACT_SLEEP",
		"ACT_REST",
		"ACT_DRAG",
		"ACT_ATTACK",
		"ACT_STEAL",
		"ACT_LOOK_AROUND",
		"ACT_JUMP"
};	

CControlAnimationBase::CControlAnimationBase()
{
	init_anim_storage	();
}

CControlAnimationBase::~CControlAnimationBase()
{
	free_anim_storage	();
}

void CControlAnimationBase::reinit()
{
	inherited::reinit		();

	m_tAction				= ACT_STAND_IDLE;
	spec_params				= 0;

	fx_time_last_play		= 0;

	accel_init				();

	aa_time_last_attack		= 0;

	// �������� ���������� ��������
	m_anim_motion_map.clear	();
	UpdateAnimCount			();

	// ������������� ���������� � ������� ��������
	m_cur_anim.motion			= eAnimStandIdle;
	m_cur_anim.index			= 0;
	m_cur_anim.time_started		= 0;
	m_cur_anim.speed._set_current	(-1.f);
	m_cur_anim.speed._set_target	(-1.f);
	m_cur_anim.blend			= 0;
	m_cur_anim.speed_change_vel	= 1.f;

	prev_motion					= cur_anim_info().motion; 

	m_prev_character_velocity	= 0.01f;

	spec_anim					= eAnimUndefined;

	// test
	m_man->capture				(this, ControlCom::eControlAnimation);
	m_man->subscribe			(this, ControlCom::eventAnimationSignal);

	AA_reload					(pSettings->r_string(*(m_object->cNameSect()), "attack_params"));

	braking_mode				= false;

	m_state_attack				= false;
}

void CControlAnimationBase::on_start_control(ControlCom::EControlType type)
{
	switch (type) {
	case ControlCom::eControlAnimation: 
		m_man->subscribe	(this, ControlCom::eventAnimationEnd);	
		m_state_attack		= false;
		select_animation	();
		break;
	}
}

void CControlAnimationBase::on_stop_control	(ControlCom::EControlType type)
{
	switch (type) {
	case ControlCom::eControlAnimation: 
		m_man->unsubscribe	(this, ControlCom::eventAnimationEnd); 
		m_state_attack		= false;
		break;
	}
}

void CControlAnimationBase::on_event(ControlCom::EEventType type, ControlCom::IEventData *data)
{
	switch (type) {
	case ControlCom::eventAnimationEnd:		select_animation(true);	m_state_attack = false; break;
	case ControlCom::eventAnimationSignal:	
		{
			SAnimationSignalEventData *event_data = (SAnimationSignalEventData *)data;
			if (event_data->event_id == CControlAnimation::eAnimationHit) check_hit(event_data->motion,event_data->time_perc);	break;
		}
	}
}

void CControlAnimationBase::select_animation(bool anim_end)
{
	// start new animation
	SControlAnimationData		*ctrl_data = (SControlAnimationData*)m_man->data(this, ControlCom::eControlAnimation); 
	if (!ctrl_data) return;

	if (m_state_attack && !anim_end) return;
	
	if (cur_anim_info().motion == eAnimAttack) m_state_attack = true;
	else m_state_attack = false;

	
	// ��������� ��� ����������� � ��������� ��������
	m_object->ForceFinalAnimation();

	// �������� ������� SAnimItem, ��������������� ������� ��������
	SAnimItem *anim_it = m_anim_storage[cur_anim_info().motion];
	VERIFY(anim_it);

	// ���������� ����������� ������
	s32 index;
	if (-1 != anim_it->spec_id) index = anim_it->spec_id;
	else {
		VERIFY(anim_it->count != 0);
		index = ::Random.randI(anim_it->count);
	}

	// ���������� ��������	
	string128	s1,s2;
	MotionID	cur_anim		= smart_cast<CKinematicsAnimated*>(m_object->Visual())->ID_Cycle_Safe(strconcat(sizeof(s2),s2,*anim_it->target_name,itoa(index,s1,10)));

	// Setup Com
	ctrl_data->global.motion	= cur_anim;
	ctrl_data->global.actual	= false;
	ctrl_data->set_speed		(m_cur_anim.speed._get_target());

	// ��������� ������� ��������
	string64	st,tmp;
	strconcat	(sizeof(st),st,*anim_it->target_name,itoa(index,tmp,10));
	//	sprintf_s		(st, "%s%d", *anim_it->second.target_name, index);
	m_cur_anim.name				= st; 
	m_cur_anim.index			= u8(index);
	m_cur_anim.time_started		= Device.dwTimeGlobal;
	m_cur_anim.speed._set_current	(1.f);
	m_cur_anim.speed._set_target	(-1.f);
}

// ��������� ���������� �� ������� �� �������� from � to
bool CControlAnimationBase::CheckTransition(EMotionAnim from, EMotionAnim to)
{
	if (!m_man->check_start_conditions(ControlCom::eControlSequencer)) return false;

	// ����� ���������������� ��������
	bool		b_activated	= false;
	EMotionAnim cur_from = from; 
	EPState		state_from	= GetState(cur_from);
	EPState		state_to	= GetState(to);

	TRANSITION_ANIM_VECTOR_IT I = m_tTransitions.begin();
	bool bVectEmpty = m_tTransitions.empty();

	while (!bVectEmpty) {		// ���� � ����, ���� ������ ��������� �� ������

		bool from_is_good	= ((I->from.state_used) ? (I->from.state == state_from) : (I->from.anim == cur_from));
		bool target_is_good = ((I->target.state_used) ? (I->target.state == state_to) : (I->target.anim == to));

		if (from_is_good && target_is_good) {

			//if (I->skip_if_aggressive && m_object->m_bAggressive) return;

			// ������� �������
			if (!b_activated) {
				m_object->com_man().seq_init();
			}

			m_object->com_man().seq_add(get_motion_id(I->anim_transition));
			b_activated	= true;	


			if (I->chain) {
				cur_from	= I->anim_transition;
				state_from	= GetState(cur_from);
				I = m_tTransitions.begin();			// ������ �������
				continue;
			} else break;
		}
		if (m_tTransitions.end() == ++I) break;
	}

	if (b_activated) {
		m_object->com_man().seq_switch		();
		return true;
	}

	return false;
}

void CControlAnimationBase::CheckReplacedAnim()
{
	for (REPLACED_ANIM_IT it=m_tReplacedAnims.begin(); m_tReplacedAnims.end()!=it ;++it) 
		if ((cur_anim_info().motion == it->cur_anim) && (*(it->flag) == true)) { 
			cur_anim_info().motion = it->new_anim;
			return;
		}
}

SAAParam &CControlAnimationBase::AA_GetParams(pcstr anim_name)
{
	// ������ ������� �������� � AA_VECTOR
	MotionID motion = smart_cast<CKinematicsAnimated*>(m_object->Visual())->LL_MotionID(anim_name);

	for (AA_VECTOR_IT it = m_attack_anims.begin(); it != m_attack_anims.end(); it++) {
		if (it->motion == motion) return (*it);
	}
	
	VERIFY3(FALSE, "Error! No animation in AA_VECTOR! Animation = ", anim_name);
	return (*(m_attack_anims.begin()));
}

SAAParam &CControlAnimationBase::AA_GetParams(MotionID motion, f32 time_perc)
{
	// ������ ������� �������� � AA_VECTOR
	for (AA_VECTOR_IT it = m_attack_anims.begin(); it != m_attack_anims.end(); it++) {
		if ((it->motion == motion) && (it->time == time_perc)) return (*it);
	}

	VERIFY2(FALSE, "Error! No animation in AA_VECTOR! Animation = [UNKNOWN]");
	return (*(m_attack_anims.begin()));
}


EPState	CControlAnimationBase::GetState (EMotionAnim a)
{
	// ����� �������� 
	SAnimItem *item_it = m_anim_storage[a];
	VERIFY(item_it);

	return item_it->pos_state;
}

#define FX_CAN_PLAY_MIN_INTERVAL	50

void CControlAnimationBase::FX_Play(EHitSide side, f32 amount)
{
	if (fx_time_last_play + FX_CAN_PLAY_MIN_INTERVAL > m_object->m_dwCurrentTime) return;

	SAnimItem *anim_it = m_anim_storage[cur_anim_info().motion];
	VERIFY(anim_it);

	clamp(amount,0.f,1.f);

	shared_str	*p_str = 0;
	switch (side) {
		case eSideFront:	p_str = &anim_it->fxs.front;	break;
		case eSideBack:		p_str = &anim_it->fxs.back;	break;
		case eSideLeft:		p_str = &anim_it->fxs.left;	break;
		case eSideRight:	p_str = &anim_it->fxs.right;	break;
	}

	if (p_str && p_str->size()) smart_cast<CKinematicsAnimated*>(m_object->Visual())->PlayFX(*(*p_str), amount);

	fx_time_last_play = m_object->m_dwCurrentTime;
}

f32 CControlAnimationBase::GetAnimSpeed(EMotionAnim anim)
{
	SAnimItem *anim_it = m_anim_storage[anim];
	VERIFY(anim_it);

	CMotionDef			*def = get_motion_def(anim_it, 0);

	return				(def->Dequantize(def->speed));
}

bool CControlAnimationBase::IsTurningCurAnim()
{
	SAnimItem *item_it = m_anim_storage[cur_anim_info().motion];
	VERIFY(item_it);

	if (!fis_zero(item_it->velocity.velocity.angular_real)) return true;
	return false;
}

bool CControlAnimationBase::IsStandCurAnim()
{
	SAnimItem *item_it = m_anim_storage[cur_anim_info().motion];
	VERIFY(item_it);

	if (fis_zero(item_it->velocity.velocity.linear)) return true;
	return false;
}


EAction CControlAnimationBase::VelocityIndex2Action(u32 velocity_index)
{
	switch (velocity_index) {
		case MonsterMovement::eMP_STAND:			return ACT_STAND_IDLE;
		case MonsterMovement::eVelocityParameterWalkNormal:		return ACT_WALK_FWD;
		case MonsterMovement::eVelocityParameterRunNormal:		return ACT_RUN;
		case MonsterMovement::eVelocityParameterWalkDamaged:	return ACT_WALK_FWD;
		case MonsterMovement::eVelocityParameterRunDamaged:		return ACT_RUN;
		case MonsterMovement::eVelocityParameterSteal:			return ACT_STEAL;
		case MonsterMovement::eVelocityParameterDrag:			return ACT_DRAG;
		case MonsterMovement::eVelocityParameterInvisible:		return ACT_RUN;
	}

	return m_object->CustomVelocityIndex2Action(velocity_index);
}

EAction CControlAnimationBase::GetActionFromPath()
{
	EAction action;

	u32 cur_point_velocity_index = m_object->movement().detail().path()[m_object->movement().detail().curr_travel_point_index()].velocity;
	action = VelocityIndex2Action(cur_point_velocity_index);

	u32 next_point_velocity_index = u32(-1);
	if (m_object->movement().detail().path().size() > m_object->movement().detail().curr_travel_point_index() + 1) 
		next_point_velocity_index = m_object->movement().detail().path()[m_object->movement().detail().curr_travel_point_index() + 1].velocity;

	if ((cur_point_velocity_index == MonsterMovement::eMP_STAND) && (next_point_velocity_index != u32(-1))) {
		if (!m_object->control().direction().is_turning(deg(1))) 
			action = VelocityIndex2Action(next_point_velocity_index);
	}

	return action;
}



//////////////////////////////////////////////////////////////////////////
// Debug

pcstr CControlAnimationBase::GetAnimationName(EMotionAnim anim)
{
	SAnimItem *item_it = m_anim_storage[anim];
	VERIFY(item_it);

	return *item_it->target_name;
}

pcstr CControlAnimationBase::GetActionName(EAction action)
{
	return dbg_action_name_table[action];
}

///////////////////////////////////////////////////////////////////////////////////////

void CControlAnimationBase::ValidateAnimation()
{
	SAnimItem *item_it = m_anim_storage[cur_anim_info().motion];

	bool is_moving_anim		= !fis_zero(item_it->velocity.velocity.linear);
	bool is_moving_on_path	= m_object->control().path_builder().is_moving_on_path();

	if (is_moving_on_path && is_moving_anim) {
		m_object->dir().use_path_direction(cur_anim_info().motion == eAnimDragCorpse);
		return;
	}

	if (!is_moving_on_path && is_moving_anim) {
		cur_anim_info().motion				= eAnimStandIdle;
		m_object->move().stop				();
		return;
	}

	if (is_moving_on_path && !is_moving_anim) {
		m_object->move().stop				();
		return;
	}

	if (!m_object->control().direction().is_turning() && ((cur_anim_info().motion == eAnimStandTurnLeft) || (cur_anim_info().motion == eAnimStandTurnRight))) {
		cur_anim_info().motion		= eAnimStandIdle;
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
void CControlAnimationBase::UpdateAnimCount()
{
	CKinematicsAnimated *skel = smart_cast<CKinematicsAnimated*>(m_object->Visual());

	for (ANIM_ITEM_VECTOR_IT it = m_anim_storage.begin(); it != m_anim_storage.end(); it++)	{
		if (!(*it)) continue;

		// ���������, ���� �� ��� ��������� ������
		if ((*it)->count != 0) return;

		string128	s, s_temp; 
		u8 count = 0;

		for (s32 i=0; ; ++i) {
			strconcat	(sizeof(s_temp),s_temp, *((*it)->target_name),itoa(i,s,10));
			pcstr		name	= s_temp;
			MotionID	id		= skel->ID_Cycle_Safe(name);

			if (id.valid())  {
				count++;
				AddAnimTranslation(id,name);
			}
			else break;
		}

		if (count != 0) (*it)->count = count;
		else {
			sprintf_s(s, "Error! No animation: %s for monster %s", *((*it)->target_name), *m_object->cName());
			R_ASSERT2(count != 0, s);
		} 
	}
}

CMotionDef *CControlAnimationBase::get_motion_def(SAnimItem *it, u32 index)
{
	string128			s1,s2;
	CKinematicsAnimated	*skeleton_animated = smart_cast<CKinematicsAnimated*>(m_object->Visual());
	const MotionID		&motion_id = skeleton_animated->ID_Cycle_Safe(strconcat(sizeof(s2),s2,*it->target_name,itoa(index,s1,10)));
	return				(skeleton_animated->LL_GetMotionDef(motion_id));
}

void CControlAnimationBase::AddAnimTranslation(const MotionID &motion, pcstr str)
{
	m_anim_motion_map.insert(mk_pair(motion, str));	
}
shared_str CControlAnimationBase::GetAnimTranslation(const MotionID &motion)
{
	shared_str				ret_value;

	ANIM_TO_MOTION_MAP_IT	anim_it = m_anim_motion_map.find(motion);
	if (anim_it != m_anim_motion_map.end()) ret_value = anim_it->second;

	return ret_value;
}

MotionID CControlAnimationBase::get_motion_id(EMotionAnim a, u32 index)
{
	// �������� ������� SAnimItem, ��������������� ������� ��������
	SAnimItem *anim_it = m_anim_storage[a];
	VERIFY(anim_it);

	// ���������� ����������� ������
	if (index == u32(-1)) {
		if (-1 != anim_it->spec_id) index = anim_it->spec_id;
		else {
			VERIFY(anim_it->count != 0);
			index = ::Random.randI(anim_it->count);
		}
	}

	string128			s1,s2;
	return				(smart_cast<CKinematicsAnimated*>(m_object->Visual())->ID_Cycle_Safe(strconcat(sizeof(s2),s2,*anim_it->target_name,itoa(index,s1,10))));
}

void CControlAnimationBase::stop_now()
{
	m_object->move().stop			();
//	m_object->path().disable_path	();
}

void CControlAnimationBase::set_animation_speed()
{
	// Setup Com
	SControlAnimationData		*ctrl_data = (SControlAnimationData*)m_man->data(this, ControlCom::eControlAnimation); 
	if (!ctrl_data) return;
	ctrl_data->set_speed		(m_cur_anim.speed._get_target() );
}

void CControlAnimationBase::check_hit(MotionID motion, f32 time_perc)
{
	if (!m_object->EnemyMan.get_enemy()) return;
	const CEntityAlive *enemy = m_object->EnemyMan.get_enemy();

	SAAParam &params		= AA_GetParams(motion,time_perc);
	
	m_object->sound().play	(MonsterSound::eMonsterSoundAttackHit);

	bool should_hit = true;
	// ���������� ��������� �� �����
	fVector3 d;
	d.sub(enemy->Position(),m_object->Position());
	if (d.magnitude() > params.dist) should_hit = false;
	
	// �������� ��  Field-Of-Hit
	f32 my_h;
	f32 my_p;
	f32 h;
	f32 p;

	m_object->Direction().getHP(my_h,my_p);
	d.getHP(h,p);
	
	f32 from	= angle_normalize(my_h + params.foh.from_yaw);
	f32 to	= angle_normalize(my_h + params.foh.to_yaw);
	
	if (!is_angle_between(h, from, to)) should_hit = false;

	from	= angle_normalize(my_p + params.foh.from_pitch);
	to		= angle_normalize(my_p + params.foh.to_pitch);

	if (!is_angle_between(p, from, to)) should_hit = false;

	if (!m_object->EnemyMan.see_enemy_now()) should_hit = false;

	if (should_hit) m_object->HitEntity(enemy, params.hit_power, params.impulse, params.impulse_dir);

	m_object->MeleeChecker.on_hit_attempt(should_hit);
}

void parse_anim_params(pcstr val, SAAParam &anim)
{
	string16			cur_elem;

	_GetItem	(val,0,cur_elem);		anim.time			= f32(atof(cur_elem));
	_GetItem	(val,1,cur_elem);		anim.hit_power		= f32(atof(cur_elem));
	_GetItem	(val,2,cur_elem);		anim.impulse		= f32(atof(cur_elem));
	_GetItem	(val,3,cur_elem);		anim.impulse_dir.x	= f32(atof(cur_elem));
	_GetItem	(val,4,cur_elem);		anim.impulse_dir.y	= f32(atof(cur_elem));
	_GetItem	(val,5,cur_elem);		anim.impulse_dir.z	= f32(atof(cur_elem));
	_GetItem	(val,6,cur_elem);		anim.foh.from_yaw	= f32(atof(cur_elem));
	_GetItem	(val,7,cur_elem);		anim.foh.to_yaw		= f32(atof(cur_elem));
	_GetItem	(val,8,cur_elem);		anim.foh.from_pitch	= f32(atof(cur_elem));
	_GetItem	(val,9,cur_elem);		anim.foh.to_pitch	= f32(atof(cur_elem));
	_GetItem	(val,10,cur_elem);		anim.dist			= f32(atof(cur_elem));

	anim.impulse_dir.normalize();

	f32 clamp_val = PI_DIV_2 - EPSILON_3;
	clamp(anim.foh.from_yaw,	-clamp_val, clamp_val);
	clamp(anim.foh.to_yaw,		-clamp_val, clamp_val);
	clamp(anim.foh.from_pitch,	-clamp_val, clamp_val);
	clamp(anim.foh.to_pitch,	-clamp_val, clamp_val);
}

void CControlAnimationBase::AA_reload(pcstr section)
{
	if (!pSettings->section_exist(section)) return;

	m_attack_anims.clear();
	
	SAAParam			anim;
	pcstr				anim_name;
	pcstr				val;

	CKinematicsAnimated	*skel_animated = smart_cast<CKinematicsAnimated*>(m_object->Visual());

	for (u32 i=0; pSettings->r_line(section,i,&anim_name,&val); ++i) {
		

		anim.motion				= skel_animated->LL_MotionID(anim_name);
		if (!anim.motion.valid())	continue;

		// check if it is compound (if there is one item, mean it as a section)
		if (_GetItemCount(val) == 1) {
			pcstr compound_section = val;
			pcstr unused_line_name;

			for (u32 k=0; pSettings->r_line(compound_section,k,&unused_line_name,&val); ++k) {
				parse_anim_params	(val, anim);
				
				m_attack_anims.push_back(anim);
				m_man->animation().add_anim_event(anim.motion, anim.time, CControlAnimation::eAnimationHit);
			}
		} else {
			parse_anim_params(val, anim);
			
			m_attack_anims.push_back(anim);
			m_man->animation().add_anim_event(anim.motion, anim.time, CControlAnimation::eAnimationHit);
		}
	}
}

void CControlAnimationBase::init_anim_storage()
{
	m_anim_storage.reserve(eAnimCount);
	for (u32 i=0; i<eAnimCount; i++)
		m_anim_storage.push_back((SAnimItem *)0);
}

void CControlAnimationBase::free_anim_storage()
{
	for (u32 i=0; i<eAnimCount; i++) {
		SAnimItem *item = m_anim_storage[i];
		if (item) {
			xr_delete			(item);
			m_anim_storage[i]	= 0;
		}
	}
}
