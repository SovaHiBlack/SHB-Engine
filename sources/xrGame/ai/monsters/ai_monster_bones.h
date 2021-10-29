#pragma once

#define AXIS_X	(1 << 0)
#define AXIS_Y	(1 << 1)
#define AXIS_Z	(1 << 2)

// параметры движения характерные для конкретной оси в боне
struct bonesAxis
{
	float			cur_yaw;
	float			target_yaw;
	float			r_speed;
	float			dist_yaw;		// необходимо лишь для определения текущей скорости по оси
};

// бона с параметрами движения по осям
struct bonesBone
{
	CBoneInstance* bone;
	bonesAxis		params;
	unsigned char				axis;

	bonesBone( )
	{
		bone = 0;
	}
	void	Set(CBoneInstance* b, unsigned char a, float ty, float cy, float r_s);
	bool	NeedTurn( );					// необходим поворот по оси p_axis?
	void	Turn(unsigned int dt);			// выполнить поворот по оси p_axis
	void	Apply( );					// установить углы у боны
};

// управление движениями костей
class bonesManipulation
{
	xr_vector<bonesBone>	m_Bones;
	unsigned int		freeze_time;

	bool	in_return_state;				// если идёт возврат к исходному положению
	unsigned int		time_started;
	unsigned int		time_last_update;
	unsigned int		time_last_delta;

	bool	bActive;

public:
	void 		Reset( );

	void 		AddBone(CBoneInstance* bone, unsigned char axis_used);
	void 		SetMotion(CBoneInstance* bone, unsigned char axis_used, float target_yaw, float r_speed, unsigned int t);

	void 		Update(CBoneInstance* bone, unsigned int cur_time);
	bool 		IsActive( )
	{
		return bActive;
	}
	bool 		IsReturn( )
	{
		return in_return_state;
	}

	bonesAxis& GetBoneParams(CBoneInstance* bone, unsigned char axis_used);
};
