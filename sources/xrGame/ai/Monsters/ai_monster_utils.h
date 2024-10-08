#pragma once

// проверить, находится ли объект entity на ноде
// возвращает позицию объекта, если он находится на ноде, или центр его ноды
class CEntity;
extern Fvector get_valid_position(const CEntity *entity, const Fvector &actual_position);

// возвращает true, если объект entity находится на ноде
extern bool object_position_valid(const CEntity *entity);

IC Fvector random_position(const Fvector &center, F32 R)
{
	Fvector v;
	v = center;
	v.x += ::Random.randF(-R,R);
	v.z += ::Random.randF(-R,R);

	return v;
}

IC bool	from_right(F32 ty, F32 cy)
{
	return ((angle_normalize_signed(ty - cy) > 0));
}

IC bool	is_angle_between(F32 yaw, F32 yaw_from, F32 yaw_to)
{
	F32 diff = angle_difference(yaw_from,yaw_to);
	R_ASSERT(diff < PI);

	if ((angle_difference(yaw,yaw_from) < diff) && (angle_difference(yaw,yaw_to)<diff)) return true;
	else return false;
}

IC void velocity_lerp(F32& _cur, F32 _target, F32 _accel, F32 _dt)
{
	if (fsimilar(_cur, _target)) return;

	if (_target > _cur) {
		_cur += _accel * _dt;
		if (_cur > _target) _cur = _target;
	} else {
		_cur -= _accel * _dt;
		if (_cur < 0) _cur = 0.f;
	}
}

IC void def_lerp(F32& _cur, F32 _target, F32 _vel, F32 _dt)
{
	if (fsimilar(_cur, _target)) return;

	if (_target > _cur) {
		_cur += _vel * _dt;
		if (_cur > _target) _cur = _target;
	} else {
		_cur -= _vel * _dt;
		if (_cur < _target) _cur = _target;
	}
}

IC u32	time() 
{
	return Device.dwTimeGlobal;
}

//////////////////////////////////////////////////////////////////////////
// bone routines
//////////////////////////////////////////////////////////////////////////
extern	Fvector get_bone_position	(CObject *object, pcstr bone_name);

IC Fvector get_head_position(CObject *object) 
{
	return get_bone_position(object, "bip01_head");
}

//////////////////////////////////////////////////////////////////////////
// LTX routines
//////////////////////////////////////////////////////////////////////////
IC void read_delay(pcstr section, pcstr name, u32 &delay_min, u32 &delay_max)
{
	pcstr	delay	= pSettings->r_string(section,name);
	string128 tempst;	

	if (_GetItemCount(delay) == 2) {
		delay_min = u32(atoi(_GetItem(delay,0,tempst)));
		delay_max = u32(atoi(_GetItem(delay,1,tempst)));
	} else {
		delay_min	= 0;
		delay_max	= u32(atoi(delay));
	}
}

IC void read_distance(pcstr section, pcstr name, F32& dist_min, F32& dist_max)
{
	pcstr	dist	= pSettings->r_string(section,name);
	string128 tempst;

	VERIFY			(_GetItemCount(dist) == 2);
	
	dist_min		= F32(atof(_GetItem(dist,0,tempst)));
	dist_max		= F32(atof(_GetItem(dist,1,tempst)));
}

