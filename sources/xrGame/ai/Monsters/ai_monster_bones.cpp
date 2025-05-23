#include "stdafx.h"
#include "ai_monster_bones.h"
#include "../../..\XR_3DA\skeletoncustom.h"


//****************************************************************************************************
// class bonesBone
//****************************************************************************************************
void bonesBone::Set(CBoneInstance *b, u8 a, f32 ty, f32 cy, f32 r_s)
{
	bone			= b; 
	axis			= a;

	params.target_yaw	= ty; 
	params.cur_yaw	= cy; 
	params.r_speed	= r_s;
	params.dist_yaw	= _abs(ty-cy);
}


bool bonesBone::NeedTurn()
{
	if (!fsimilar(params.cur_yaw, params.target_yaw, EPSILON_3)) return true;
	return false;
}

void bonesBone::Turn(u32 dt)
{
	f32 PI_DIV_2m		= 8 * PI_DIV_6 / 3;
	f32 PIm			= PI_DIV_2m * 2;

	f32 cur_speed = params.r_speed * _cos(PI_DIV_2m - PIm * _abs(params.target_yaw - params.cur_yaw) / params.dist_yaw);

	f32 dy;
	dy =  cur_speed * dt / 1000;  // ��������� ������� � ��������� ����

	if (_abs(params.target_yaw - params.cur_yaw) < dy) params.cur_yaw = params.target_yaw;
	else params.cur_yaw += ((params.target_yaw > params.cur_yaw) ? dy : -dy);
}

void bonesBone::Apply()
{
	f32 x = 0.f;
	f32 y = 0.f;
	f32 z = 0.f;

	if ((axis & AXIS_X) == AXIS_X) x = params.cur_yaw;
	if ((axis & AXIS_Y) == AXIS_Y) y = params.cur_yaw;
	if ((axis & AXIS_Z) == AXIS_Z) z = params.cur_yaw;

	// ������� ������� �������� � �������� �� mTransform ����
	fMatrix4x4 M;
	M.setHPB (-y, -x, -z);
	bone->mTransform.mulB_43	(M);
}

//****************************************************************************************************
// class bonesManipulation
//****************************************************************************************************

void bonesManipulation::Reset()
{
	time_started		= 0;
	time_last_update	= 0;
	in_return_state		= false;
	bActive				= false;
	freeze_time			= 0;
	time_last_delta		= 1;
}

void bonesManipulation::AddBone (CBoneInstance *bone, u8 axis_used)
{
	bonesBone tempB;

	tempB.Set(bone, axis_used,0.f,0.f,1.f);

	m_Bones.push_back(tempB);
}

void bonesManipulation::SetMotion(CBoneInstance *bone, u8 axis, f32 target_yaw, f32 r_speed, u32 t)
{
	s32 index = -1;
	// ����� ���� bone � m_Bones
	for (u32 i=0; i<m_Bones.size(); ++i)  {
		if ((m_Bones[i].bone == bone) && (m_Bones[i].axis == axis)) {
			index = i;
			break;
		}
	}
	R_ASSERT(-1 != index);

	m_Bones[index].params.target_yaw	= target_yaw;
	m_Bones[index].params.r_speed		= r_speed;
	m_Bones[index].params.dist_yaw		= angle_difference(target_yaw,m_Bones[index].params.cur_yaw);
	if (t > freeze_time) freeze_time = t;

	bActive				= true;
	in_return_state		= false;
	time_started	= 0;
}



void bonesManipulation::Update(CBoneInstance *bone, u32 cur_time)
{
	// �������� ��������� ���� ������
	bool bones_were_turned = false;

	// ���������� dt
	u32 dt;
	if (cur_time == time_last_update) {
		dt = time_last_delta;
	} else dt = cur_time - time_last_update;
	time_last_delta = dt;
	time_last_update = cur_time;

	for (u32 i=0; i<m_Bones.size(); ++i) {
		if (m_Bones[i].NeedTurn()){
			if (m_Bones[i].bone == bone) m_Bones[i].Turn(dt);				
			bones_were_turned = true;
		}
	}

	// ���� ������� �������� ��������
	if (!bones_were_turned && in_return_state) {
		Reset();
		return;
	}

	// ���� ������ �� ��������� - �����
	if (!bActive && !bones_were_turned) return;

	// ���� ����������� ����������� ���� � �� ���� ����� �� ����������� (�������� �������...)
	if (!bones_were_turned && !in_return_state) {
		if ((0 == time_started) && (freeze_time > 0)) { // �������� �����
			time_started = cur_time;
		}

		if ((0 != time_started) && (time_started + freeze_time < cur_time)) { // ����� �����?
			time_started	= 0;

			// ������ �������
			in_return_state	= true;
			// ���������� � ���� ������ � m_Bone ������� � 0
			for (u32 i = 0; i<m_Bones.size(); ++i) {
				m_Bones[i].params.target_yaw	= 0.f;
				m_Bones[i].params.dist_yaw		= _abs(m_Bones[i].params.target_yaw - m_Bones[i].params.cur_yaw);
			}
			bActive = false;
		} 
	}

	// ���������� ��������� �� m_Bones
	for (u32 i = 0; i<m_Bones.size(); ++i) {
		if (m_Bones[i].bone == bone) m_Bones[i].Apply();
	}
}

bonesAxis &bonesManipulation::GetBoneParams(CBoneInstance *bone, u8 axis_used)
{
	// ����� ���� bone � m_Bones
	for (u32 i=0; i<m_Bones.size(); ++i)  {
		if ((m_Bones[i].bone == bone) && (m_Bones[i].axis == axis_used)) {
			return m_Bones[i].params;
		}
	}
	VERIFY(FALSE);
	return m_Bones[0].params;
}
